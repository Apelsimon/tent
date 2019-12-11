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

const uint8_t* byte_buffer::get_read() const
{
    if(!invariant())
    {
        std::runtime_error{"Invariant violated"};
    }

    return buff_.data() + read_;
}

uint8_t* byte_buffer::get_write()
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

void byte_buffer::append(const uint8_t* data, size_t size)
{
    write_ += size;

    if(!invariant())
    {
        std::runtime_error{"Invariant violated"};
    }

    std::copy(data, data + size, buff_.data() + (write_ - size));    
}

void byte_buffer::write(uint8_t* data, size_t size)
{
    write_ += size;
    if(!invariant())
    {
        std::runtime_error{"Invariant violated"};
    }
    std::copy(data, data + size, buff_.data() + (write_ - size));
}

void byte_buffer::write_8(uint8_t data)
{
    write(&data, 1);
}

void byte_buffer::write_32(uint32_t data)
{
    write(reinterpret_cast<uint8_t*>(&data), 4);
}

void byte_buffer::write_64(uint64_t data)
{
    write(reinterpret_cast<uint8_t*>(&data), 8);
}

bool byte_buffer::invariant() const
{
    return read_ <= write_ && read_ <= buff_.size() && write_ <= buff_.size();
}


} 