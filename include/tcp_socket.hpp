#ifndef TCP_SOCKET_HPP_
#define TCP_SOCKET_HPP_

#include "endpoint.hpp"

namespace tent 
{

class tcp_socket
{
public:
    tcp_socket(const endpoint& ep);
    ~tcp_socket();

    bool bind();
    bool connect(const endpoint& ep);
    bool set_blocking(bool block);
    bool getsockopt(int level, int optname, void *optval, socklen_t *optlen);
    void close();
    bool valid() const { return fd_ != -1; }
    int fd() const { return fd_; }

private:
    int fd_;
    endpoint endpoint_;
};


}

#endif