#include "endpoint.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <sstream>
#include <stdexcept>

namespace tent
{

endpoint::endpoint(const std::string& ip, uint16_t port)
{
    std::memset(&endpoint_, 0, sizeof(endpoint_));
    endpoint_.sin_family = AF_INET;
    endpoint_.sin_port = htons(port);

    if(inet_pton(AF_INET, ip.c_str(), &endpoint_.sin_addr) != 1)
    {
        std::stringstream ss;
        ss << "Invalid network address, errno: " << std::strerror(errno);
        throw std::runtime_error{ss.str()};
    }
}

endpoint::endpoint(const struct sockaddr* addr)
{
    std::memcpy(&endpoint_, addr, sizeof(sockaddr_in)); // TODO: support IPv6
}

std::ostream& operator<<(std::ostream& os, const tent::endpoint& info) 
{ 
    char ip[INET_ADDRSTRLEN];
    const auto addr = reinterpret_cast<const struct sockaddr_in*>(info.sockaddr());
    inet_ntop(AF_INET, &addr->sin_addr, ip, INET_ADDRSTRLEN);

    return os << '[' << ip << ':' << ntohs(addr->sin_port) << ']';
}

}