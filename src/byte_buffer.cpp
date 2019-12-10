#include "byte_buffer.hpp"

#include <stdexcept>

namespace tent
{

byte_buffer::byte_buffer(size_t size) : 
    buff_(size),
    read_(0),
    write_(0)
{

}

const uint8_t* byte_buffer::read() const
{
    if(!invariant())
    {
        std::runtime_error{"Invariant violated"};
    }

    return buff_.data() + read_;
}

uint8_t* byte_buffer::write()
{
    if(!invariant())
    {
        std::runtime_error{"Invariant violated"};
    }

    return buff_.data() + write_;
}

void byte_buffer::inc_read(size_t inc)
{
    read_ += inc;

    if(!invariant())
    {
        std::runtime_error{"Invariant violated"};
    }
}

void byte_buffer::inc_write(size_t inc)
{
    write_ += inc;

    if(!invariant())
    {
        std::runtime_error{"Invariant violated"};
    }
}

size_t byte_buffer::read_available() const
{
    if(!invariant())
    {
        std::runtime_error{"Invariant violated"};
    }

    return write_ - read_;
}

size_t byte_buffer::write_available() const
{
    if(!invariant())
    {
        std::runtime_error{"Invariant violated"};
    }
    
    return buff_.size() - write_;
}

bool byte_buffer::invariant() const
{
    return read_ <= write_ && read_ <= buff_.size() && write_ <= buff_.size();
}


} 
