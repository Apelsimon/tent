#ifndef NET_REACTOR_HPP_
#define NET_REACTOR_HPP_

#include "inet_reactor_client.hpp"

#include <sys/epoll.h>
#include <unordered_map>

namespace tent
{

class net_reactor
{
public:
    net_reactor(int wait_ms = 5000);
    ~net_reactor();

    int poll();
    void stop();
    bool reg(inet_reactor_client& client, uint32_t events);
    bool unreg(inet_reactor_client& client, uint32_t events = 0);

private:
    int efd_;
    int wait_ms_;
    std::unordered_map<int, inet_reactor_client*> clients_;
};

}



#endif