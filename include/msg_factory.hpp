#ifndef MSG_FACTORY_HPP_
#define MSG_FACTORY_HPP_

#include <string>

namespace tent
{

class byte_buffer;
class piece_request;

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
    static void request(byte_buffer& buffer, const piece_request& req);
};

}

#endif