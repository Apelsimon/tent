#ifndef MSG_FACTORY_HPP_
#define MSG_FACTORY_HPP_

#include <string>

namespace tent
{

namespace msg 
{
    struct piece;
    struct request;
}

class byte_buffer;
class piece_request;

// TODO: return buffers instead?
class msg_factory
{
public:
    static void handshake(byte_buffer& buffer, const std::string& peer_id, 
        const std::string& info_hash);
    static void keep_alive(byte_buffer& buffer);
    static void choke(byte_buffer& buffer);
    static void unchoke(byte_buffer& buffer);
    static void interested(byte_buffer& buffer);
    static void not_interested(byte_buffer& buffer);
    static void request(byte_buffer& buffer, const msg::request& req);
    static void piece(byte_buffer& buffer, const msg::piece& piece);

    // udp tracker client msgs
    static void connect(byte_buffer& buffer, uint32_t transaction_id);
    static void announce(byte_buffer& buffer, uint64_t connection_id, 
        uint32_t transaction_id, const std::string& info_hash, 
        const std::string& peer_id, uint64_t left, uint32_t key, uint16_t port);
};

}

#endif