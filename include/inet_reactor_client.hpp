#ifndef INET_REACTOR_CLIENT_HPP_
#define INET_REACTOR_CLIENT_HPP_

namespace tent
{

class inet_reactor_client
{
public:
    virtual ~inet_reactor_client() = default;
    virtual void read() = 0;
    virtual void write() = 0;
    virtual int fd() const = 0;
};

}

#endif