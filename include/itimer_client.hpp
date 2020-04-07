#ifndef ITIMER_CLIENT_HPP_
#define ITIMER_CLIENT_HPP_

namespace tent
{

class itimer_client
{
public: 
    virtual ~itimer_client() = default;
    virtual void on_timeout() = 0;

    float timeout_ms_{0};
    float elapsed_{0};
};

}

#endif