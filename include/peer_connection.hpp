#ifndef NET_REACTOR_CLIENT_HPP_
#define NET_REACTOR_CLIENT_HPP_

#include "inet_reactor_client.hpp"
#include "tcp_socket.hpp"

#include <memory>

namespace tent 
{

class net_reactor;
class peer_info;

class peer_connection : public inet_reactor_client 
{
public:
    peer_connection(net_reactor& reactor, std::unique_ptr<peer_info> info);
    ~peer_connection();

    void do_read() override;
    void do_write() override;
    int fd() const override { return socket_.fd(); }

    void connect_to_peer();

private:
    net_reactor& reactor_;
    std::unique_ptr<peer_info> peer_info_;
    tcp_socket socket_;

    bool connected_;
};

}


#endif