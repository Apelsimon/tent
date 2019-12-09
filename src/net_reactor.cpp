#include "net_reactor.hpp"

#include <iostream>
#include <sys/epoll.h>

namespace tent
{

net_reactor::net_reactor() : running_(false), efd_(epoll_create1(0))
{

}

net_reactor::~net_reactor() {}

void net_reactor::start()
{
    constexpr auto MAX_EVENTS = 10;
    struct epoll_event events[MAX_EVENTS];
    
    running_ = true;

    while(running_)
    {
        auto nfds = epoll_wait(efd_, events, MAX_EVENTS, -1);

        for(auto i = 0; i < nfds; ++i)
        {
            auto it = clients_.find(events[i].data.fd);
            if(it != clients_.end())
            {
                if(events[i].events & EPOLLIN)
                {
                    it->second->do_read();
                }
                if(events[i].events & EPOLLOUT)
                {
                    it->second->do_write();
                }
            }
        }
    }
}

void net_reactor::stop()
{
    running_ = false;
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
    return clients_.erase(client.fd()) > 0;
}

}