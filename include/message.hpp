#ifndef MESSAGE_HPP_
#define MESSAGE_HPP_

#include "byte_buffer.hpp"

namespace tent
{

class message
{
public:

    enum id 
    {
        KEEP_ALIVE = -1,
        CHOKE = 0,
        UNCHOKE = 1,
        INTERESTED = 2,
        NOT_INTERESTED = 3,
        HAVE = 4,
        BITFIELD = 5,
        REQUEST = 6,
        PIECE = 7,
        CANCEL = 8,
        PORT = 9      
    };

    message(const byte_buffer& buffer) :
        id_(buffer.peek_32() == 0 ? KEEP_ALIVE : 
            static_cast<id>(buffer.peek_8(4))),
        payload_()
    {
        if(id_ != KEEP_ALIVE)
        { 
            payload_.write(buffer.get_read() + 5, buffer.peek_32() - 1);
        }
    }

    id id_;
    byte_buffer payload_;
};

}

#endif