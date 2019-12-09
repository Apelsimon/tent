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

}