#ifndef NET_REACTOR_CLIENT_HPP_
#define NET_REACTOR_CLIENT_HPP_

#include "byte_buffer.hpp"
#include "inet_reactor_client.hpp"
#include "session_sm.hpp"
#include "tcp_socket.hpp"

#include "libtorrent/torrent_info.hpp"

#include <memory>
#include <vector>

namespace tent 
{

class message;
class net_reactor;
class peer_info;
class piece_handler;
class session;

class torrent_agent : public inet_reactor_client, public ism_client 
{
public:
    torrent_agent(session& session, net_reactor& reactor, lt::torrent_info& torrent_info, 
        std::unique_ptr<peer_info> info, piece_handler& handler);
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

    void execute();
    
private:
    void on_read(const byte_buffer& buffer);
    void send();
    void handle_msg(message& msg);
    void request_pieces();

    session& session_;
    net_reactor& reactor_;
    std::unique_ptr<peer_info> peer_info_;
    tcp_socket socket_;
    session_sm sm_;
    byte_buffer io_buffer_;
    std::vector<uint8_t> msg_buffer_;
    piece_handler& piece_handler_;

    lt::torrent_info& torrent_info_;

    bool connected_;
    bool choked_;
};

}


#endif