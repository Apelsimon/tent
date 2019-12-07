#ifndef NET_REACTOR_CLIENT_HPP_
#define NET_REACTOR_CLIENT_HPP_

#include "inet_reactor_client.hpp"

namespace tent 
{

class net_reactor;

class net_reactor_client : public inet_reactor_client 
{
public:
    net_reactor_client(net_reactor& reactor);

    void do_read() override;
    void do_write() override;
    int fd() const override { return fd_; }

private:
    int fd_;
    net_reactor& reactor_;
};

}


#endif