#ifndef PEER_HPP_
#define PEER_HPP_

#include <string>

namespace tent
{

class peer 
{
public:
    peer(const std::string& id, const std::string& ip, uint16_t port) :
        id_(id),
        ip_(ip),
        port_(port)
    {}

    std::string id_;
    std::string ip_;
    uint16_t port_;
};

}

#endif