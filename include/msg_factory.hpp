#ifndef MSG_FACTORY_HPP_
#define MSG_FACTORY_HPP_

#include <string>

namespace mul
{
    class byte_buffer;
} // namespace mul

namespace tent
{

namespace msg 
{
    struct piece;
    struct request;
}

class piece_request;

// TODO: return buffers instead?
class msg_factory
{
public:
    static void handshake(mul::byte_buffer& buffer, const std::string& peer_id, 
        const std::string& info_hash);
    static mul::byte_buffer handshake(const std::string& peer_id, const std::string& info_hash);
    static void keep_alive(mul::byte_buffer& buffer);
    static void choke(mul::byte_buffer& buffer);
    static void unchoke(mul::byte_buffer& buffer);
    static void interested(mul::byte_buffer& buffer);
    static mul::byte_buffer interested();
    static void not_interested(mul::byte_buffer& buffer);
    static void request(mul::byte_buffer& buffer, const msg::request& req);
    static mul::byte_buffer request(const msg::request& req);
    static void piece(mul::byte_buffer& buffer, const msg::piece& piece);

    // udp tracker client msgs
    static void connect(mul::byte_buffer& buffer, uint32_t transaction_id);
    static mul::byte_buffer connect(uint32_t transaction_id);
    static void announce(mul::byte_buffer& buffer, uint64_t connection_id, 
        uint32_t transaction_id, const std::string& info_hash, 
        const std::string& peer_id, uint64_t left, uint32_t key, uint16_t port);
};

}

#endif