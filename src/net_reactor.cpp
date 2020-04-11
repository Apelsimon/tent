#include "net_reactor.hpp"

#include "spdlog/spdlog.h"

#include <iostream>
#include <cstring>

namespace tent
{

static bool ctl_add(int efd, int fd, uint32_t events);
static bool ctl_mod(int efd, int fd, uint32_t events);

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
    auto it = clients_.find(client.fd());
    if(it == clients_.end())
    {   
        if(!ctl_add(efd_, client.fd(), events))
        {
            return false;
        }
        
        client.set_events(events);
        clients_[client.fd()] = &client;
    }
    else
    {
        auto flags = client.get_events();
        client.set_events(flags | events);

        return ctl_mod(efd_, client.fd(), client.get_events());
    }
    

    return true;
}

bool net_reactor::unreg(inet_reactor_client& client, uint32_t events)
{
    if(events == 0)
    {
        epoll_ctl(efd_, EPOLL_CTL_DEL, client.fd(), nullptr);
        return clients_.erase(client.fd()) > 0;
    }

    auto it = clients_.find(client.fd());
    if(it == clients_.end())
    {
        return false;    
    }

    auto flags = client.get_events();
    client.set_events(flags & ~events);

    return ctl_mod(efd_, client.fd(), client.get_events());
}

bool ctl_add(int efd, int fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;

    if (epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev) == -1) 
    {
        std::cerr << "epoll_ctl error, failed to add client" << std::endl;
        return false;
    }

    return true;
}

bool ctl_mod(int efd, int fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;

    if (epoll_ctl(efd, EPOLL_CTL_MOD, fd, &ev) == -1) 
    {
        std::cerr << "epoll_ctl error, failed to add client" << std::endl;
        return false;
    }

    return true;
}

}