#ifndef PEER_INFO_HPP_
#define PEER_INFO_HPP_

#include "endpoint.hpp"

#include <ostream>
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

    std::string to_string() const;

    std::string id_;
    endpoint endpoint_;
};

std::ostream &operator<<(std::ostream& os, const peer_info& info);

}

#endif