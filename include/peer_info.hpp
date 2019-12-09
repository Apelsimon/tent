#ifndef PEER_INFO_HPP_
#define PEER_INFO_HPP_

#include "endpoint.hpp"

#include <string>

namespace tent
{

class peer_info 
{
public:
    peer_info(const std::string& id, const std::string& ip, uint16_t port) :
        id_(id),
        endpoint_(ip, port)
    {}

    std::string id_;
    endpoint endpoint_;
};

}

#endif