#ifndef NET_REACTOR_HPP_
#define NET_REACTOR_HPP_

#include "inet_reactor_client.hpp"

#include <unordered_map>

namespace tent
{

class net_reactor
{
public:
    net_reactor();
    ~net_reactor();

    int poll();
    void stop();
    bool reg(inet_reactor_client& client, uint32_t events);
    bool unreg(inet_reactor_client& client);

private:
    int efd_;
    std::unordered_map<int, inet_reactor_client*> clients_;
};

}



#endif