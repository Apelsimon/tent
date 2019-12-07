#ifndef INET_REACTOR_CLIENT_HPP_
#define INET_REACTOR_CLIENT_HPP_

namespace tent
{

class inet_reactor_client
{
public:
    virtual ~inet_reactor_client() = default;
    virtual void do_read() = 0;
    virtual void do_write() = 0;
    virtual int fd() const = 0;
};

}

#endif