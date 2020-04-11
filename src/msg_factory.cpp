#include "msg_factory.hpp"

#include "bittorrent_protocol.hpp"
#include "byte_buffer.hpp"
#include "message.hpp"
#include "messages.hpp"
#include "peer_info.hpp"
#include "pieces.hpp"

static std::vector<uint8_t> hex_str_to_byte_buff(const std::string& hex);

namespace tent
{

void msg_factory::handshake(byte_buffer& buffer, const std::string& peer_id, 
    const std::string& info_hash)
{
    const uint8_t STR_LEN = protocol::V1.size();

    buffer.write_8(STR_LEN);
    buffer.write(reinterpret_cast<const uint8_t*>(protocol::V1.data()), STR_LEN);
    buffer.write_64(0);
    buffer.write(hex_str_to_byte_buff(info_hash).data(), 20);
    buffer.write(reinterpret_cast<const uint8_t*>(peer_id.data()), 20);
}

byte_buffer msg_factory::handshake(const std::string& peer_id, const std::string& info_hash)
{
    byte_buffer bb;
    handshake(bb, peer_id, info_hash);
    return bb;
}

void msg_factory::keep_alive(byte_buffer& buffer)
{
    buffer.write_32(0);
}

void msg_factory::choke(byte_buffer& buffer)
{
    buffer.write_32(1);
    buffer.write_8(message::id::CHOKE);
}

void msg_factory::unchoke(byte_buffer& buffer)
{
    buffer.write_32(1);
    buffer.write_8(message::id::UNCHOKE);
}

void msg_factory::interested(byte_buffer& buffer)
{
    buffer.write_32(1);
    buffer.write_8(message::id::INTERESTED);
}

byte_buffer msg_factory::interested()
{
    byte_buffer bb;
    interested(bb);
    return bb;
}

void msg_factory::not_interested(byte_buffer& buffer)
{
    buffer.write_32(1);
    buffer.write_8(message::id::NOT_INTERESTED);
}

void msg_factory::request(byte_buffer& buffer, const msg::request& req)
{
    buffer.write_32(13);
    buffer.write_8(message::id::REQUEST);
    buffer.write_32(req.index_);
    buffer.write_32(req.begin_);
    buffer.write_32(req.length_);
}

byte_buffer msg_factory::request(const msg::request& req)
{
    byte_buffer bb;
    request(bb, req);
    return bb;
}

void msg_factory::piece(byte_buffer& buffer, const msg::piece& piece)
{
    buffer.write_32(9 + piece.block_.read_available());
    buffer.write_8(message::id::PIECE);
    buffer.write_32(piece.index_);
    buffer.write_32(piece.begin_);
    buffer.write(piece.block_.get_read(), piece.block_.read_available());
}

void msg_factory::connect(byte_buffer& buffer, uint32_t transaction_id)
{
    constexpr uint64_t CONNECTION_ID = 0x41727101980;
    constexpr uint32_t CONNECT_ACTION = 0; // TODO: replace with action enum

    buffer.write_64(CONNECTION_ID);
    buffer.write_32(CONNECT_ACTION);
    buffer.write_32(transaction_id);
}

byte_buffer msg_factory::connect(uint32_t transaction_id)
{
    byte_buffer bb;
    connect(bb, transaction_id);
    return bb;
}

void msg_factory::announce(byte_buffer& buffer, uint64_t connection_id, 
        uint32_t transaction_id, const std::string& info_hash, 
        const std::string& peer_id, uint64_t left, uint32_t key, uint16_t port)
{ 
    constexpr uint32_t ANNOUNCE_ACTION = 1; // TODO: replace with action enum

    buffer.write_64(connection_id);
    buffer.write_32(ANNOUNCE_ACTION); 
    buffer.write_32(transaction_id);
    buffer.write(hex_str_to_byte_buff(info_hash).data(), 20); 
    buffer.write(reinterpret_cast<const uint8_t*>(peer_id.c_str()), 20);
    buffer.write_64(0); // downloaded
    buffer.write_64(left);
    buffer.write_64(0); // uploaded
    buffer.write_32(0); // event
    buffer.write_32(0); // default ip
    buffer.write_32(key);
    buffer.write_32(-1); // num want TODO: cant write -1 (default) to byte buffer. 
    buffer.write_16(port);
}

} // namespace tent

static int char_to_int(char input)
{
    if('0' <= input && input <= '9')
    return input - '0';
    if('A' <= input && input <= 'F')
    return input - 'A' + 10;
    if('a' <= input && input <= 'f')
    return input - 'a' + 10;

    return 0;
}

std::vector<uint8_t> hex_str_to_byte_buff(const std::string& hex)
{
    std::vector<uint8_t> buffer(hex.size() / 2);

    auto i = 0;
    for(auto& entry : buffer)
    {
        entry = 16 * char_to_int(hex[i]) + char_to_int(hex[i+1]);
        i += 2;
    }

    return buffer;
}