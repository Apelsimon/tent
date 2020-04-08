#include "net_reactor.hpp"

#include <iostream>
#include <sys/epoll.h>

#include <cstring>

namespace tent
{

net_reactor::net_reactor(int wait_ms) : 
    efd_(epoll_create1(0)),
    wait_ms_(wait_ms)
{

}

net_reactor::~net_reactor() {}

int net_reactor::poll()
{
    constexpr auto MAX_EVENTS = 50;

    struct epoll_event events[MAX_EVENTS];
    
    const auto nfds = epoll_wait(efd_, events, MAX_EVENTS, wait_ms_);
    if(nfds == -1)
    {
        std::cerr << "epoll_wait error: " << std::strerror(errno) << std::endl;
    }

    for(auto i = 0; i < nfds; ++i)
    {
        auto it = clients_.find(events[i].data.fd);
        if(it != clients_.end())
        {
            if(events[i].events & EPOLLIN)
            {
                it->second->read();
            }
            if(events[i].events & EPOLLOUT)
            {
                it->second->write();
            }
        }
    }

    return nfds;
}

bool net_reactor::reg(inet_reactor_client& client, uint32_t events)
{
    if(clients_.find(client.fd()) == clients_.end())
    {
        struct epoll_event ev;
        ev.events = events | EPOLLET;
        ev.data.fd = client.fd();
        if (epoll_ctl(efd_, EPOLL_CTL_ADD, client.fd(), &ev) == -1) 
        {
            std::cerr << "epoll_ctl error, failed to add client" << std::endl;
            return false;
        }
        clients_[client.fd()] = &client;
        
        return true;
    }

    return false;
}

bool net_reactor::unreg(inet_reactor_client& client)
{
    epoll_ctl(efd_, EPOLL_CTL_DEL, client.fd(), nullptr);
    return clients_.erase(client.fd()) > 0;
}

}