#include "tracker_client.hpp"

#include "byte_buffer.hpp"
#include "endpoint.hpp"
#include "http_client.hpp"
#include "http_messages.hpp"
#include "msg_factory.hpp"
#include "peer_info.hpp"
#include "random.hpp"
#include "udp_socket.hpp"
#include "udp_tracker_client.hpp"

#include "spdlog/spdlog.h"

static std::string percent_encode(const lt::sha1_hash& hash, ssize_t num = 2, char separator = '%');
static void parse_peers(std::vector<std::unique_ptr<tent::peer_info>>& peers, const std::string& tracker_rsp);

namespace tent
{

tracker_client::tracker_client(const lt::torrent_info& info) :
    torrent_info_(info)
{
}

tracker_client::~tracker_client() {}

// TODO: return received peers instead (if possible?)
// TODO: change to peer_info set
bool tracker_client::announce(uint16_t port, const std::string& peer_id,
    std::vector<std::unique_ptr<peer_info>>& received_peers)
{
    if(torrent_info_.trackers().empty())
    {
        spdlog::info("No trackers available");
        return false;
    }

    received_peers.clear();

    http::client http_client;
    udp_tracker_client udp_tracker{torrent_info_, peer_id, port, received_peers};

    for(auto& tracker : torrent_info_.trackers())
    {
        if(tracker.url.find("udp") != std::string::npos)
        {
            udp_tracker.announce(tracker.url);
        }
        else // http
        {
            std::stringstream ss;
            ss << tracker.url << "?info_hash=" << percent_encode(torrent_info_.info_hash()) << "&peer_id=" 
                << peer_id << "&port=" << port << "&downloaded=0&uploaded=0&left="
                << torrent_info_.total_size() << "&event=started";
            
            auto resp = http_client.get(ss.str());

            if(resp->status() == http::status_code::MOVED_PERMANENTLY)
            {
                resp = http_client.get(resp->field(http::header_field::LOCATION));
            }

            if(resp->status() == http::status_code::OK)
            {
                parse_peers(received_peers, resp->body());
            }
        }
    }

    spdlog::info("Available peers: {}", received_peers.size());
    return !received_peers.empty();
}

} // namespace tent

std::string percent_encode(const lt::sha1_hash& hash, ssize_t num, char separator)
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
        spdlog::error("Failed to decode: '{}'", error.message());
        return;
    }

    auto peer_list = decoded.dict_find_list("peers");
    if(peer_list.type() != lt::bdecode_node::list_t)
    {
        spdlog::error("Couldn't find peer list", error.message());
        return;
    }

    for(auto i = 0; i < peer_list.list_size(); ++i)
    {
        const auto peer_dict = peer_list.list_at(i);
        const auto id = std::string{peer_dict.dict_find_string_value("peer id")};
        const auto peer_ip = std::string{peer_dict.dict_find_string_value("ip")};
        const auto peer_port = peer_dict.dict_find_int_value("port");


        auto it = std::find_if(peers.begin(), peers.end(), [&peer_ip, peer_port](const auto& peer){
            auto addr = reinterpret_cast<sockaddr_in*>(peer->endpoint_.sockaddr());
            char ipstr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &addr->sin_addr, ipstr, INET_ADDRSTRLEN); // TODO: support IPv6
            return std::string{ipstr} == peer_ip && ntohs(addr->sin_port) == peer_port;
        });

        if(it == peers.end())
        {
            peers.push_back(std::make_unique<tent::peer_info>(id, peer_ip, peer_port));
        }
    }
}