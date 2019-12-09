#ifndef SESSION_HPP_
#define SESSION_HPP_

#include "libtorrent/torrent_info.hpp"

namespace tent
{

class net_reactor;
class peer_connection;
class tracker_client;

class session
{
public:
    session(net_reactor& reactor, const lt::torrent_info& info);
    ~session();

private:
    void start();

    static constexpr uint16_t PORT = 12345;
    static const std::string PEER_ID;

    net_reactor& reactor_;
    std::unique_ptr<tracker_client> tracker_client_;
    std::vector<std::unique_ptr<peer_connection>> connections_;

    lt::torrent_info torrent_info_;
};

}

#endif