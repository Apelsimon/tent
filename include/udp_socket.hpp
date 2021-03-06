#ifndef UDP_SOCKET_HPP_
#define UDP_SOCKET_HPP_

#include "endpoint.hpp"

namespace mul
{
    class byte_buffer;
}

namespace tent
{

// TODO: sync with tcp socket to remove duplicate code
class udp_socket
{
public:
    udp_socket();
    ~udp_socket();
 
    ssize_t sendto(mul::byte_buffer& buffer, const endpoint& ep, int flags = 0);
    ssize_t recvfrom(mul::byte_buffer& buffer, endpoint& ep, int flags = 0);
    void close();
    bool valid() const { return fd_ != -1; }
    int fd() const { return fd_; }

private:
    int fd_;
};

} // namespace tent

#endif