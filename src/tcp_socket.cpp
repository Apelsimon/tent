#include "tcp_socket.hpp"

#include "byte_buffer.hpp"

#include <cstring>
#include <fcntl.h>
#include <sstream>
#include <stdexcept>
#include <unistd.h>

#include <iostream>

namespace tent
{

tcp_socket::tcp_socket(const endpoint& ep) : 
    fd_(socket(AF_INET, SOCK_STREAM, 0)),
    endpoint_(ep)
{
    if(!valid())
    {
        std::stringstream ss;
        ss << "Failed to create socket, errno: " << std::strerror(errno);
        throw std::runtime_error{ss.str()};
    }

    if(!set_blocking(false))
    {
        std::stringstream ss;
        ss << "Failed to set O_NONBLOCK, errno: " << std::strerror(errno);
        throw std::runtime_error{ss.str()};
    }
}


tcp_socket::~tcp_socket()
{
    if(valid())
    {
        close();
    }
}

bool tcp_socket::bind()
{
    auto sockaddr = endpoint_.sockaddr();
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

ssize_t tcp_socket::write(byte_buffer& buffer)
{
    return ::write(fd_, buffer.get_read(), buffer.read_available());
}

ssize_t tcp_socket::read(byte_buffer& buffer)
{
    return ::read(fd_, buffer.get_write(), buffer.write_available());;
}

void tcp_socket::close()
{
    ::close(fd_);
    fd_ = -1;
}

}