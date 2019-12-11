#ifndef MSG_FACTORY_HPP_
#define MSG_FACTORY_HPP_

#include <string>

namespace tent
{

class byte_buffer;

class msg_factory
{
public:
    static void handshake(byte_buffer& buffer, const std::string& peer_id, 
        const std::string& info_hash);
};

}

#endif