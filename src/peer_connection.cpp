#include "peer_connection.hpp"

#include "bittorrent_protocol.hpp"
#include "msg_factory.hpp"
#include "net_reactor.hpp"
#include "peer_info.hpp"

#include <sys/epoll.h>

#include <iostream>
#include <sstream>
#include <cstring>

static bool get_msg_len(const tent::byte_buffer& buffer, bool handshake_received);
static bool msg_is_handshake(const uint8_t* msg, size_t msg_len);

namespace tent 
{

peer_connection::peer_connection(net_reactor& reactor, lt::torrent_info& torrent_info,
    std::unique_ptr<peer_info> info, const std::string& local_peer_id) : 
    reactor_(reactor),
    peer_info_(std::move(info)),
    socket_(endpoint{"0.0.0.0", 0}),
    sm_(*this),
    rcv_buffer_(2048),
    msg_buffer_(2048),
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

void peer_connection::read()
{
    rcv_buffer_.reset();
    
    errno = 0;
    while(connected_ && rcv_buffer_.write_available() > 0 && !errno)
    {
        const auto res = socket_.read(rcv_buffer_);
        if(res > 0)
        {
            rcv_buffer_.inc_write(res);
        }
        else if(res == 0)
        {
            connected_ = false;
        }
        
    }

    if(rcv_buffer_.read_available() > 0)
    {
        on_read(rcv_buffer_);
    }
}

void peer_connection::write()
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
    rcv_buffer_.reset();

    std::stringstream info_hash;
    info_hash << torrent_info_.info_hash();
    
    msg_factory::handshake(rcv_buffer_, local_peer_id_, info_hash.str());

    errno = 0;
    while(rcv_buffer_.read_available() > 0 && !errno)
    {
        const auto res = socket_.write(rcv_buffer_);
        if(res > 0)
        {
            rcv_buffer_.inc_read(res);
        }
    } 
}

void peer_connection::on_read(const byte_buffer& buffer)
{
    msg_buffer_.write(buffer.get_read(), buffer.read_available());
    const auto handshake_received = sm_.handshake_received();
    
    while(msg_buffer_.read_available() >= 4 && 
        msg_buffer_.read_available() >= get_msg_len(msg_buffer_, sm_.handshake_received()))
    {
        const auto msg_len = get_msg_len(msg_buffer_, sm_.handshake_received());
        const auto msg = msg_buffer_.read(msg_len);

        if(!sm_.handshake_received() && msg_is_handshake(msg, msg_len))
        {
            sm_.on_event(session_event::HANDSHAKE);
        }
        else
        {
            // handle msg..
        }
    }
    
    if(msg_buffer_.read_available() == 0)
    {
        msg_buffer_.reset();
    }
}

} // namespace tent

bool get_msg_len(const tent::byte_buffer& buffer, bool handshake_received)
{
    if(!handshake_received)
    {
        return buffer.peek_8() + 49;
    }

    return buffer.peek_32() + 4;
}

bool msg_is_handshake(const uint8_t* msg, size_t msg_len)
{
    const auto proto_str_len = *msg;
    const auto proto_str = std::string{reinterpret_cast<const char*>(msg + 1), proto_str_len};
    
    return msg_len == proto_str_len + 49 && proto_str == tent::protocol::V1;
}