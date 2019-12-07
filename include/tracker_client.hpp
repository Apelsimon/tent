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

class tracker_client
{
public:
    tracker_client(const lt::torrent_info& info);
    ~tracker_client();

    void announce(uint16_t port);
    
private:
    std::unique_ptr<http::client> http_client_;
    const lt::torrent_info torrent_info_;
};

}

#endif