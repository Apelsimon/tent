#include "msg_factory.hpp"

#include "bittorrent_protocol.hpp"
#include "byte_buffer.hpp"
#include "message.hpp"
#include "messages.hpp"
#include "peer_info.hpp"
#include "pieces.hpp"

static std::vector<std::uint8_t> hex_str_to_byte_buff(const std::string& hex);

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

void msg_factory::piece(byte_buffer& buffer, const msg::piece& piece)
{
    buffer.write_32(9 + piece.block_.read_available());
    buffer.write_8(message::id::PIECE);
    buffer.write_32(piece.index_);
    buffer.write_32(piece.begin_);
    buffer.write(piece.block_.get_read(), piece.block_.read_available());
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

std::vector<std::uint8_t> hex_str_to_byte_buff(const std::string& hex)
{
    std::vector<std::uint8_t> buffer(hex.size() / 2);

    auto i = 0;
    for(auto& entry : buffer)
    {
        entry = 16 * char_to_int(hex[i]) + char_to_int(hex[i+1]);
        i += 2;
    }

    return buffer;
}