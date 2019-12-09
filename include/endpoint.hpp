#ifndef ENDPOINT_HPP_
#define ENDPOINT_HPP_

#include <netinet/in.h>
#include <string>

namespace tent
{

class endpoint
{
public: 
    endpoint(const std::string& ip, uint16_t port);

    const struct sockaddr* sockaddr()  const
    { 
        return reinterpret_cast<const struct sockaddr*>(&endpoint_);
    }

public:
    struct sockaddr_in endpoint_;
};

}

#endif