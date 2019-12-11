#include "peer_connection.hpp"

#include "msg_factory.hpp"
#include "net_reactor.hpp"
#include "peer_info.hpp"

#include <sys/epoll.h>

#include <iostream>
#include <sstream>
#include <cstring>

namespace tent 
{

peer_connection::peer_connection(net_reactor& reactor, lt::torrent_info& torrent_info,
    std::unique_ptr<peer_info> info, const std::string& local_peer_id) : 
    reactor_(reactor),
    peer_info_(std::move(info)),
    socket_(endpoint{"0.0.0.0", 0}),
    sm_(*this),
    buffer_(2048),
    torrent_info_(torrent_info),
    local_peer_id_(local_peer_id),
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
    buffer_.reset();
    
    errno = 0;
    while(connected_ && buffer_.write_available() > 0 && !errno)
    {
        const auto res = socket_.read(buffer_);
        if(res > 0)
        {
            buffer_.inc_write(res);
        }
        else if(res == 0)
        {
            connected_ = false;
        }
        
    }
}

void peer_connection::do_write()
{
    if(!connected_)
    {
        int error;
        socklen_t len = sizeof(error);
        if(socket_.getsockopt(SOL_SOCKET, SO_ERROR, &error, &len))
        {
            connected_ = error == 0; // TODO: retry connection on fail
            if(connected_)
            {
                sm_.on_event(session_event::CONNECTED);
            }
        }
    }
}

void peer_connection::start()
{
    sm_.on_event(session_event::START);
}

void peer_connection::connect()
{
    connected_ = socket_.connect(peer_info_->endpoint_);
    if(connected_)
    {
        sm_.on_event(session_event::CONNECTED);
    }
}

void peer_connection::handshake() 
{
    buffer_.reset();

    std::stringstream info_hash;
    info_hash << torrent_info_.info_hash();
    
    msg_factory::handshake(buffer_, local_peer_id_, info_hash.str());

    errno = 0;
    while(buffer_.read_available() > 0 && !errno)
    {
        const auto res = socket_.write(buffer_);
        if(res > 0)
        {
            buffer_.inc_read(res);
        }
    } 
}

}