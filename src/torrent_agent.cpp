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

static size_t get_msg_len(const std::vector<uint8_t>& buffer, bool handshake_received);
static bool set_peer_id_and_validate_handshake(const tent::byte_buffer& msg, tent::peer_info& peer_info);
static tent::byte_buffer chop(std::vector<uint8_t>& buffer, size_t slice_len);

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
    io_buffer_(1 << 15),
    msg_buffer_(),
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
            std::cout << "Disconnected with " << *peer_info_ << std::endl;
            sm_.on_event(session_event::DISCONNECTED);
        }        
    }

    if(errno && errno != EAGAIN && errno != EWOULDBLOCK)
    {
        std::cerr << "read error: " << std::strerror(errno) << std::endl;
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
    request_pieces();
}

void torrent_agent::disconnected()
{
    connected_ = false;
    socket_.close();
}

void torrent_agent::execute()
{
    if(connected_ && !choked_)
    {
        request_pieces();
    }
}

void torrent_agent::on_read(const byte_buffer& buffer)
{
    msg_buffer_.insert(msg_buffer_.end(), buffer.get_read(), 
        buffer.get_read() + buffer.read_available());
    
    auto msg_len = get_msg_len(msg_buffer_, sm_.handshake_received());
    while(msg_buffer_.size() >= 4 && msg_buffer_.size() >= msg_len)
    {   
        const auto slice = chop(msg_buffer_, msg_len);
        if(!sm_.handshake_received())
        {
            // TODO: close connection if peer_id doesnt match
            if(set_peer_id_and_validate_handshake(slice, *peer_info_))
            {
                sm_.on_event(session_event::HANDSHAKE);
            }
        }
        else
        {
            auto msg = message(slice);
            handle_msg(msg);
        }

        msg_len = get_msg_len(msg_buffer_, sm_.handshake_received());
    }
}

void torrent_agent::send()
{
    errno = 0;
    while(connected_ && io_buffer_.read_available() > 0 && !errno)
    {
        const auto res = socket_.write(io_buffer_);
        if(res > 0)
        {
            io_buffer_.inc_read(res);
        }
    }

    if(errno && errno != EAGAIN && errno != EWOULDBLOCK)
    {
        std::cerr << "write error: " << std::strerror(errno) << std::endl;
    }
}

void torrent_agent::handle_msg(message& msg)
{
    switch (msg.id_)
    {
    case message::id::KEEP_ALIVE:
        // TODO
        break;
    case message::id::CHOKE:
        std::cout << "Choked by peer: " << *peer_info_ << std::endl;
        sm_.on_event(session_event::CHOKE);
        break;
    case message::id::UNCHOKE:
        std::cout << "Unchoked by peer: " << *peer_info_ << std::endl;
        sm_.on_event(session_event::UNCHOKE);        
        break;
    case message::id::INTERESTED:
        // TODO
        break;
    case message::id::NOT_INTERESTED:
        // TODO
        break;
    case message::id::HAVE:
        // TODO
        break;
    case message::id::BITFIELD:
        piece_handler_.have(peer_info_->id_, msg.payload_);
        break;
    case message::id::REQUEST:
        // TODO
        break;
    case message::id::PIECE:
        piece_handler_.received(msg.payload_);
        request_pieces();
        break;
    case message::id::CANCEL:
        // TODO
        break;
    case message::id::PORT:
        // TODO
        break;
    default:
        std::cerr << "Unknow message id" << std::endl;  
        break;
    }
}

void torrent_agent::request_pieces()
{
    if(!choked_)
    {
        constexpr auto MAX_BURST = 15; // TODO: increase when multiple receive is available

        auto send_count = 0;
        while(send_count < MAX_BURST)
        {   
            auto result = piece_handler_.get_piece_request(peer_info_->id_);
            if(!result.first)
            {
                break;
            }
            
            io_buffer_.reset();
            msg_factory::request(io_buffer_, result.second);
            send();
            
            ++send_count;
        }
        
    }
}

} // namespace tent

size_t get_msg_len(const std::vector<uint8_t>& buffer, bool handshake_received)
{
    if(!handshake_received)
    {
        return buffer[0] + 49;
    }
    return ntohl(*reinterpret_cast<const uint32_t*>(buffer.data())) + 4;
}

bool set_peer_id_and_validate_handshake(const tent::byte_buffer& msg, tent::peer_info& peer_info)
{
    const auto proto_str_len = static_cast<uint8_t>(msg.peek_8());
    const auto proto_str = std::string{reinterpret_cast<const char*>(msg.get_read() + 1), proto_str_len};
    const auto peer_id = std::string{reinterpret_cast<const char*>(msg.get_read() + 48), 20};
    
    if(peer_info.id_ != "")
    {
        peer_info.id_ = peer_id;
    }

    return msg.read_available() == static_cast<size_t>(proto_str_len + 49) && 
        proto_str == tent::protocol::V1;    
}

tent::byte_buffer chop(std::vector<uint8_t>& buffer, size_t slice_len)
{
    tent::byte_buffer slice;
    slice.write(buffer.data(), slice_len);
    buffer.erase(buffer.begin(), buffer.begin() + slice_len);

    return slice;
}