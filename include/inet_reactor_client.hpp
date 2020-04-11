#ifndef INET_REACTOR_CLIENT_HPP_
#define INET_REACTOR_CLIENT_HPP_

#include <cstdint>

namespace tent
{

class inet_reactor_client
{
public:
    virtual ~inet_reactor_client() = default;
    virtual void read() = 0;
    virtual void write() = 0;
    virtual int fd() const = 0;
    virtual uint32_t get_events() const { return events_; }
    virtual void set_events(uint32_t events) { events_ = events; }

private:
    uint32_t events_{0};
};

}

#endif