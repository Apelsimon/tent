#include "tcp_socket.hpp"

#include "mul/byte_buffer.hpp"

#include <cstring>
#include <fcntl.h>
#include <sstream>
#include <stdexcept>
#include <unistd.h>

#include <iostream>

namespace tent
{

tcp_socket::tcp_socket() : 
    fd_(socket(AF_INET, SOCK_STREAM, 0))
{
    // TODO: reasonable to do this on creation?
    if(!valid())
    {
        std::stringstream ss;
        ss << "Failed to create socket, errno: " << std::strerror(errno);
        throw std::runtime_error{ss.str()};
    }
}


tcp_socket::~tcp_socket()
{
    close();
}

bool tcp_socket::bind(const endpoint& ep)
{
    auto sockaddr = ep.sockaddr();
    auto addrlen = sockaddr->sa_family == AF_INET ? sizeof(struct sockaddr_in) :
        sizeof(struct sockaddr_in6);
    return ::bind(fd_, sockaddr, addrlen) == 0;
}

bool tcp_socket::connect(const endpoint& ep)
{
    auto sockaddr = ep.sockaddr();
    auto addrlen = sockaddr->sa_family == AF_INET ? sizeof(struct sockaddr_in) :
        sizeof(struct sockaddr_in6);;
    return ::connect(fd_, sockaddr, addrlen) == 0;
}

bool tcp_socket::set_blocking(bool block)
{
    auto flags = fcntl(fd_, F_GETFL, 0);
    if(flags == -1)
    {
        return false;
    }

    flags = block ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    return fcntl(fd_, F_SETFL, flags) == 0;
}

bool tcp_socket::getsockopt(int level, int optname, void *optval, socklen_t *optlen)
{
    return ::getsockopt(fd_, level, optname, optval, optlen) == 0;
}

ssize_t tcp_socket::write(mul::byte_buffer& buffer)
{
    const auto res =  ::write(fd_, buffer.get_read(), buffer.get_read_available());
    if(res > 0)
    {
        buffer.inc_read(res);
    }
    return res;
}

ssize_t tcp_socket::read(mul::byte_buffer& buffer)
{
    const auto res = ::read(fd_, buffer.get_write(), buffer.get_write_available());
    if(res > 0)
    {
        buffer.inc_write(res);
    }
    return res;
}

void tcp_socket::close()
{
    if(valid())
    {
        ::close(fd_);
        fd_ = -1;
    }
}

void tcp_socket::reset()
{
    close();

    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if(!valid())
    {
        std::stringstream ss;
        ss << "Failed to create socket, errno: " << std::strerror(errno);
        throw std::runtime_error{ss.str()};
    }
}

}