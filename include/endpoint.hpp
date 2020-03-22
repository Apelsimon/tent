#ifndef ENDPOINT_HPP_
#define ENDPOINT_HPP_

#include <arpa/inet.h>
#include <netinet/in.h>
#include <ostream>
#include <string>

namespace tent
{

class endpoint
{
public: 
    endpoint() = default;
    endpoint(const std::string& ip, uint16_t port);
    endpoint(const struct sockaddr* addr);

    const struct sockaddr* sockaddr()  const
    { 
        return reinterpret_cast<const struct sockaddr*>(&endpoint_);
    }
    struct sockaddr* sockaddr() { return reinterpret_cast<struct sockaddr*>(&endpoint_);  }

private:
    struct sockaddr_in endpoint_; // TODO: support IPv6
};

std::ostream& operator<<(std::ostream& os, const tent::endpoint& info);

}



#endif