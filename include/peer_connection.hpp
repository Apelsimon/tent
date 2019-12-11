#ifndef NET_REACTOR_CLIENT_HPP_
#define NET_REACTOR_CLIENT_HPP_

#include "byte_buffer.hpp"
#include "inet_reactor_client.hpp"
#include "session_sm.hpp"
#include "tcp_socket.hpp"

#include "libtorrent/torrent_info.hpp"

#include <memory>

namespace tent 
{

class net_reactor;
class peer_info;

class peer_connection : public inet_reactor_client, public ism_client 
{
public:
    peer_connection(net_reactor& reactor, lt::torrent_info& torrent_info, 
        std::unique_ptr<peer_info> info, const std::string& local_peer_id);
    ~peer_connection();

    void do_read() override;
    void do_write() override;
    int fd() const override { return socket_.fd(); }

    void start() override;
    void connect() override;
    void handshake() override;

private:
    net_reactor& reactor_;
    std::unique_ptr<peer_info> peer_info_;
    tcp_socket socket_;
    session_sm sm_;
    byte_buffer buffer_;

    lt::torrent_info& torrent_info_;

    const std::string& local_peer_id_;
    bool connected_;
};

}


#endif