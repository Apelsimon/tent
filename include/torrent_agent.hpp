#ifndef NET_REACTOR_CLIENT_HPP_
#define NET_REACTOR_CLIENT_HPP_

#include "inet_reactor_client.hpp"
#include "itimer_client.hpp"
#include "session_sm.hpp"
#include "tcp_socket.hpp"

#include "libtorrent/torrent_info.hpp"

#include "mul/byte_buffer.hpp"

#include <memory>
#include <queue>
#include <vector>

namespace tent 
{

class message;
class net_reactor;
class peer_info;
class piece_handler;
class session;
class timer;

using send_queue = std::queue<mul::byte_buffer>;

class torrent_agent : public inet_reactor_client, public ism_client, public itimer_client 
{
public:
    torrent_agent(session& session, net_reactor& reactor, lt::torrent_info& torrent_info, 
        std::unique_ptr<peer_info> info, piece_handler& handler, timer& timer);
    ~torrent_agent();

    void read() override;
    void write() override;
    int fd() const override { return socket_.fd(); }

    void start() override;
    void connect() override;
    void handshake() override;
    void interested() override;
    void choked() override;
    void unchoked() override;
    void disconnected() override;

    void on_timeout() override;

    void execute();
    
private:
    void on_read(const mul::byte_buffer& buffer);
    bool send(mul::byte_buffer& buffer);
    void handle_msg(message& msg);
    void request_pieces();

    session& session_;
    net_reactor& reactor_;
    std::unique_ptr<peer_info> peer_info_;
    tcp_socket socket_;
    session_sm sm_;
    mul::byte_buffer io_buffer_;
    std::vector<uint8_t> msg_buffer_;
    piece_handler& piece_handler_;
    timer& timer_;
    send_queue send_queue_;

    lt::torrent_info& torrent_info_;

    bool connected_;
    bool choked_;
    int connection_attempts_;
};

}


#endif