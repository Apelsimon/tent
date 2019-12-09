#ifndef TRACKER_CLIENT_HPP_
#define TRACKER_CLIENT_HPP_

#include "libtorrent/torrent_info.hpp"

#include <memory>
#include <vector>

namespace tent
{

namespace http
{
class client;
}

class peer_info;

class tracker_client
{
public:
    tracker_client(const lt::torrent_info& info);
    ~tracker_client();

    bool announce(const std::string& peer_id, uint16_t port, std::vector<std::unique_ptr<peer_info>>& received_peers);
    
private:
    std::unique_ptr<http::client> http_client_;
    const lt::torrent_info torrent_info_;
};

}

#endif