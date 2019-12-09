#include "peer_connection.hpp"

#include "net_reactor.hpp"
#include "peer_info.hpp"

#include <sys/epoll.h>

#include <iostream>
#include <sstream>
#include <cstring>

namespace tent 
{

peer_connection::peer_connection(net_reactor& reactor, std::unique_ptr<peer_info> info) : 
    reactor_(reactor),
    peer_info_(std::move(info)),
    socket_(endpoint{"0.0.0.0", 0}),
    connected_(false)
{
    socket_.bind();

    reactor_.reg(*this, EPOLLIN | EPOLLOUT);
}

peer_connection::~peer_connection()
{
    reactor_.unreg(*this);
}

void peer_connection::do_read()
{
}

void peer_connection::do_write()
{
    if(!connected_)
    {
        int error;
        socklen_t len = sizeof(error);
        if(socket_.getsockopt(SOL_SOCKET, SO_ERROR, &error, &len))
        {
            std::cout << "fd: " << socket_.fd() << " successfull connect? " << error << ", " 
                <<  std::strerror(error) << std::endl;
            connected_ = error == 0; // TODO: retry connection on fail
        }
    }
}

void peer_connection::connect_to_peer()
{
    socket_.connect(peer_info_->endpoint_);
}

}