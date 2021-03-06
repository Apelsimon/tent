#ifndef TCP_SOCKET_HPP_
#define TCP_SOCKET_HPP_

#include "endpoint.hpp"

namespace mul
{
    class byte_buffer;
}

namespace tent 
{

class tcp_socket
{
public:
    tcp_socket();
    ~tcp_socket();

    bool bind(const endpoint& ep);
    bool connect(const endpoint& ep);
    bool set_blocking(bool block);
    bool getsockopt(int level, int optname, void *optval, socklen_t *optlen);
    ssize_t write(mul::byte_buffer& buffer);
    ssize_t read(mul::byte_buffer& buffer);
    void close();
    bool valid() const { return fd_ != -1; }
    int fd() const { return fd_; }
    void reset();
    
private:
    int fd_;
};


}

#endif