#include "tracker_client.hpp"

#include "http_client.hpp"
#include "http_messages.hpp"
#include "peer_info.hpp"

static std::string percent_encode(const lt::sha1_hash& hash, std::size_t num = 2, char separator = '%');
static void parse_peers(std::vector<std::unique_ptr<tent::peer_info>>& peers, const std::string& tracker_rsp);

namespace tent
{

tracker_client::tracker_client(const lt::torrent_info& info) : 
    http_client_(std::make_unique<http::client>()),
    torrent_info_(info)
{
}

tracker_client::~tracker_client() {}

bool tracker_client::announce(const std::string& peer_id, uint16_t port, std::vector<std::unique_ptr<peer_info>>& received_peers)
{
    if(torrent_info_.trackers().empty())
    {
        std::cout << "No trackers available" << std::endl;
        return false;
    }

    // TODO: announce to multiple trackers
    auto& tracker = torrent_info_.trackers()[0]; 

    std::stringstream ss;
    ss << tracker.url << "?info_hash=" << percent_encode(torrent_info_.info_hash()) << "&peer_id=" 
        << peer_id << "&port=" << port << "&downloaded=0&uploaded=0&left="
        << torrent_info_.total_size() << "&event=started";

    auto resp = http_client_->get(ss.str());

    if(resp->status() == http::status_code::MOVED_PERMANENTLY)
    {
        resp = http_client_->get(resp->field(http::header_field::LOCATION));
    }

    if(resp->status() != http::status_code::OK)
    {
        std::cout << "Announce to trackers failed" << std::endl;
        return false;
    }

    parse_peers(received_peers, resp->body());
    return true;
}

}

std::string percent_encode(const lt::sha1_hash& hash, std::size_t num, char separator)
{
    std::stringstream ss;
    ss << separator << hash;
    std::string result = ss.str();

    for (auto it = result.begin() + 1; (num+1) <= std::distance(it, result.end()); ++it )
    {
        std::advance(it,num);
        it = result.insert(it,separator);
    }

    return result;
}

void parse_peers(std::vector<std::unique_ptr<tent::peer_info>>& peers, const std::string& tracker_rsp)
{
    lt::error_code error;
    auto decoded = lt::bdecode(tracker_rsp, error);
    
    if(error.value() != 0)
    {
        std::cerr << "Failed to decode: '" << error.message() << std::endl;
        return;
    }

    peers.clear();

    auto peer_list = decoded.dict_find_list("peers");
    for(auto i = 0; i < peer_list.list_size(); ++i)
    {
        const auto peer_dict = peer_list.list_at(i);
        const auto id = std::string{peer_dict.dict_find_string_value("peer id")};
        const auto ip = std::string{peer_dict.dict_find_string_value("ip")};
        const auto port = peer_dict.dict_find_int_value("port");
        peers.push_back(std::make_unique<tent::peer_info>(id, ip, port));
    }    
}