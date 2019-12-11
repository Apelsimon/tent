#ifndef TCP_SOCKET_HPP_
#define TCP_SOCKET_HPP_

#include "endpoint.hpp"

namespace tent 
{

class byte_buffer;

class tcp_socket
{
public:
    tcp_socket(const endpoint& ep);
    ~tcp_socket();

    bool bind();
    bool connect(const endpoint& ep);
    bool set_blocking(bool block);
    bool getsockopt(int level, int optname, void *optval, socklen_t *optlen);
    ssize_t write(byte_buffer& buffer);
    ssize_t read(byte_buffer& buffer);
    void close();
    bool valid() const { return fd_ != -1; }
    int fd() const { return fd_; }

private:
    int fd_;
    endpoint endpoint_;
};


}

#endif