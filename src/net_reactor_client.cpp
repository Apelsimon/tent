#include "net_reactor_client.hpp"

#include "net_reactor.hpp"

#include <sys/epoll.h>

namespace tent 
{

net_reactor_client::net_reactor_client(net_reactor& reactor) : 
    reactor_(reactor),
    fd_(-1)
{
    reactor_.reg(*this, EPOLLIN | EPOLLOUT);
}

net_reactor_client::~net_reactor_client()
{
    reactor_.unreg(*this);
}

void net_reactor_client::do_read()
{

}

void net_reactor_client::do_write()
{

}

}