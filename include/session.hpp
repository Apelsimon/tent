#ifndef SESSION_HPP_
#define SESSION_HPP_

#include "piece_handler.hpp"

#include "libtorrent/torrent_info.hpp"

namespace tent
{

class net_reactor;
class torrent_agent;
class tracker_client;

class session
{
public:
    session(net_reactor& reactor, const lt::torrent_info& info);
    ~session();

    void start();
    void stop() { running_ = false; }

    const std::string& peer_id() const { return local_peer_id_; }

private:
    static constexpr uint16_t PORT = 12345;

    net_reactor& reactor_;
    std::unique_ptr<tracker_client> tracker_client_;
    std::vector<std::unique_ptr<torrent_agent>> agents_;

    lt::torrent_info torrent_info_;

    piece_handler piece_handler_;
    const std::string local_peer_id_;
    bool running_;
};

}

#endif