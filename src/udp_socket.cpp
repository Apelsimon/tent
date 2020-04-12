#include "udp_socket.hpp"

#include "byte_buffer.hpp"

#include <cstring>
#include <sstream>
#include <unistd.h>

namespace tent
{

udp_socket::udp_socket() :
    fd_(socket(AF_INET, SOCK_DGRAM, 0))
{
    if(!valid())
    {
        std::stringstream ss;
        ss << "Failed to create socket, errno: " << std::strerror(errno);
        throw std::runtime_error{ss.str()};
    }
}

udp_socket::~udp_socket()
{
    if(fd_ != -1)
    {
        close();
    }
}

ssize_t udp_socket::sendto(byte_buffer& buffer, const endpoint& ep, int flags)
{
    const auto res = ::sendto(fd_, buffer.get_read(), buffer.read_available(), flags, 
        ep.sockaddr(), sizeof(sockaddr_in));
    if(res > 0)
    {
        buffer.inc_read(res);
    }
    return res;
}

ssize_t udp_socket::recvfrom(byte_buffer& buffer, endpoint& ep, int flags)
{
    socklen_t addrlen = sizeof(sockaddr_in);
    const auto res = ::recvfrom(fd_, buffer.get_write(), buffer.write_available(), flags,
        ep.sockaddr(), &addrlen);
    if(res > 0)
    {
        buffer.inc_write(res); // TODO: do this for tcp as well
    }
    return res;
}

void udp_socket::close()
{
    ::close(fd_);
    fd_ = -1;
}

} // namespace tent