#include "tracker_client.hpp"

#include "http_client.hpp"
#include "http_messages.hpp"

static std::string percent_encode(const lt::sha1_hash& hash, std::size_t num = 2, char separator = '%');

namespace tent
{

tracker_client::tracker_client(const lt::torrent_info& info) : 
    http_client_(std::make_unique<http::client>()),
    torrent_info_(info)
{
}

tracker_client::~tracker_client() {}

void tracker_client::announce(uint16_t port)
{
    if(torrent_info_.trackers().empty())
    {
        return;
    }

    // TODO: announce to multiple trackers
    auto& tracker = torrent_info_.trackers()[0]; 

    std::stringstream ss;
    ss << tracker.url << "?info_hash=" << percent_encode(torrent_info_.info_hash()) << "&peer_id=" 
        << "-tent0001-0123456789" << "&port=" << port << "&downloaded=0&uploaded=0&left="
        << torrent_info_.total_size() << "&event=started";

    auto resp = http_client_->get(ss.str());

    if(resp->status() == http::status_code::MOVED_PERMANENTLY)
    {
        resp = http_client_->get(resp->field(http::header_field::LOCATION));
    }
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