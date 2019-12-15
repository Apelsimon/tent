#include "torrent_agent.hpp"

#include "bittorrent_protocol.hpp"
#include "message.hpp"
#include "msg_factory.hpp"
#include "net_reactor.hpp"
#include "peer_info.hpp"
#include "piece_handler.hpp"
#include "session.hpp"

#include <sys/epoll.h>

#include <iostream>
#include <sstream>
#include <cstring>

static size_t get_msg_len(const tent::byte_buffer& buffer, bool handshake_received);
static bool msg_is_valid_handshake(const uint8_t* msg, size_t msg_len, 
    const std::string& expected_peer_id);

namespace tent 
{

torrent_agent::torrent_agent(session& session, net_reactor& reactor, 
    lt::torrent_info& torrent_info, std::unique_ptr<peer_info> info,
    piece_handler& handler) :
    session_(session),
    reactor_(reactor),
    peer_info_(std::move(info)),
    socket_(endpoint{"0.0.0.0", 0}),
    sm_(*this),
    io_buffer_(2048),
    msg_buffer_(2048),
    piece_handler_(handler),
    torrent_info_(torrent_info),
    connected_(false),
    choked_(true)
{
    socket_.bind();

    reactor_.reg(*this, EPOLLIN | EPOLLOUT);
}

torrent_agent::~torrent_agent()
{
    reactor_.unreg(*this);
}

void torrent_agent::read()
{
    io_buffer_.reset();
    
    errno = 0;
    while(connected_ && io_buffer_.write_available() > 0 && !errno)
    {
        const auto res = socket_.read(io_buffer_);
        if(res > 0)
        {
            io_buffer_.inc_write(res);
        }
        else if(res == 0)
        {
            connected_ = false;
        }
        
    }

    if(io_buffer_.read_available() > 0)
    {
        on_read(io_buffer_);
    }
}

void torrent_agent::write()
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

void torrent_agent::start()
{
    sm_.on_event(session_event::START);
}

void torrent_agent::connect()
{
    connected_ = socket_.connect(peer_info_->endpoint_);
    if(connected_)
    {
        sm_.on_event(session_event::CONNECTED);
    }
}

void torrent_agent::handshake() 
{
    io_buffer_.reset();

    std::stringstream info_hash;
    info_hash << torrent_info_.info_hash();
    msg_factory::handshake(io_buffer_, session_.peer_id(), info_hash.str());

    send(); 
}

void torrent_agent::interested()
{
    io_buffer_.reset();
    msg_factory::interested(io_buffer_);

    send();
}

void torrent_agent::choked()
{
    choked_ = true;
}

void torrent_agent::unchoked() 
{
    choked_ = false;
    // request_piece();
}

void torrent_agent::on_read(const byte_buffer& buffer)
{
    msg_buffer_.write(buffer.get_read(), buffer.read_available());
    
    while(msg_buffer_.read_available() >= 4 && 
        msg_buffer_.read_available() >= get_msg_len(msg_buffer_, sm_.handshake_received()))
    {
        const auto msg_len = get_msg_len(msg_buffer_, sm_.handshake_received());
        
        if(!sm_.handshake_received())
        {
            const auto msg = msg_buffer_.read(msg_len);
            // TODO: close connection if peer_id doesnt match
            if(msg_is_valid_handshake(msg, msg_len, peer_info_->id_))
            {
                sm_.on_event(session_event::HANDSHAKE);
            }
        }
        else
        {
            auto msg = message(msg_buffer_);
            msg_buffer_.inc_read(msg_len);

            handle_msg(msg);
        }
    }
    
    if(msg_buffer_.read_available() == 0)
    {
        msg_buffer_.reset();
    }
}

void torrent_agent::send()
{
    errno = 0;
    while(io_buffer_.read_available() > 0 && !errno)
    {
        const auto res = socket_.write(io_buffer_);
        if(res > 0)
        {
            io_buffer_.inc_read(res);
        }
    }
}

void torrent_agent::handle_msg(message& msg)
{
    switch (msg.id_)
    {
    case message::id::KEEP_ALIVE:
        // TODO
        std::cout << "KEEP_ALIVE" << std::endl;
        break;
    case message::id::CHOKE:
        sm_.on_event(session_event::CHOKE);
        std::cout << "CHOKE" << std::endl;
        break;
    case message::id::UNCHOKE:
        std::cout << "UNCHOKE" << std::endl;
        sm_.on_event(session_event::UNCHOKE);        
        break;
    case message::id::INTERESTED:
        // TODO
        std::cout << "INTERESTED" << std::endl;
        break;
    case message::id::NOT_INTERESTED:
        // TODO
        std::cout << "NOT_INTERESTED" << std::endl;
        break;
    case message::id::HAVE:
        // TODO
        std::cout << "HAVE" << std::endl;
        break;
    case message::id::BITFIELD:
        // TODO
        std::cout << "BITFIELD" << std::endl;
        piece_handler_.have(msg.payload_);
        break;
    case message::id::REQUEST:
        // TODO
        std::cout << "REQUEST" << std::endl;
        break;
    case message::id::PIECE:
        // TODO
        std::cout << "PIECE" << std::endl;
        break;
    case message::id::CANCEL:
        // TODO
        std::cout << "CANCEL" << std::endl;
        break;
    case message::id::PORT:
        // TODO
        std::cout << "PORT" << std::endl;
        break;
    default:
        std::cerr << "Unknow message id" << std::endl;  
        break;
    }
}

} // namespace tent

size_t get_msg_len(const tent::byte_buffer& buffer, bool handshake_received)
{
    if(!handshake_received)
    {
        return buffer.peek_8() + 49;
    }
    return buffer.peek_32() + 4;
}

bool msg_is_valid_handshake(const uint8_t* msg, size_t msg_len, const std::string& expected_peer_id)
{
    const auto proto_str_len = *msg;
    const auto proto_str = std::string{reinterpret_cast<const char*>(msg + 1), proto_str_len};
    const auto peer_id = std::string{reinterpret_cast<const char*>(msg + 48), 20};
    
    return msg_len == (proto_str_len + 49) && proto_str == tent::protocol::V1 && peer_id == expected_peer_id;
}