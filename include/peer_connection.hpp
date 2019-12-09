#ifndef NET_REACTOR_CLIENT_HPP_
#define NET_REACTOR_CLIENT_HPP_

#include "inet_reactor_client.hpp"
#include "session_sm.hpp"
#include "tcp_socket.hpp"

#include <memory>

namespace tent 
{

class net_reactor;
class peer_info;

class peer_connection : public inet_reactor_client, public ism_client 
{
public:
    peer_connection(net_reactor& reactor, std::unique_ptr<peer_info> info);
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

    bool connected_;
};

}


#endif