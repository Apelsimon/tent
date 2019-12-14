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

class torrent_agent : public inet_reactor_client, public ism_client 
{
public:
    torrent_agent(net_reactor& reactor, lt::torrent_info& torrent_info, 
        std::unique_ptr<peer_info> info, const std::string& local_peer_id);
    ~torrent_agent();

    void read() override;
    void write() override;
    int fd() const override { return socket_.fd(); }

    void start() override;
    void connect() override;
    void handshake() override;
    void interested() override;

private:
    void on_read(const byte_buffer& buffer);
    void send();

    net_reactor& reactor_;
    std::unique_ptr<peer_info> peer_info_;
    tcp_socket socket_;
    session_sm sm_;
    byte_buffer io_buffer_;
    byte_buffer msg_buffer_;

    lt::torrent_info& torrent_info_;

    const std::string& local_peer_id_;
    bool connected_;
    bool handshake_received_;
};

}


#endif