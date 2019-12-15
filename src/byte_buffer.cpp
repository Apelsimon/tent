#include "byte_buffer.hpp"

#include <algorithm>
#include <arpa/inet.h>
#include <stdexcept>
#include <iostream>

namespace tent
{

byte_buffer::byte_buffer() : 
    buff_(),
    read_(0),
    write_(0)
{
}

byte_buffer::byte_buffer(size_t size) : 
    buff_(size, 0),
    read_(0),
    write_(0)
{
    if(!invariant())
    {
        throw std::length_error{"Buffer exceeded max limit"};
    }
}

const uint8_t* byte_buffer::get_read() const
{
    if(!invariant())
    {
        throw std::runtime_error{"Invariant violated"};
    }

    return buff_.data() + read_;
}

uint8_t* byte_buffer::get_write()
{
    if(!invariant())
    {
        throw std::runtime_error{"Invariant violated"};
    }

    return buff_.data() + write_;
}

void byte_buffer::inc_read(size_t inc)
{
    read_ += inc;

    if(!invariant())
    {
        throw std::range_error{"Increased read violates invariant"};
    }
}

void byte_buffer::inc_write(size_t inc)
{
    write_ += inc;

    if(!invariant())
    {
        throw std::range_error{"Increased write violates invariant"};
    }
}

size_t byte_buffer::read_available() const
{
    if(!invariant())
    {
        throw std::runtime_error{"Invariant violated"};
    }

    return write_ - read_;
}

size_t byte_buffer::write_available() const
{
    if(!invariant())
    {
        throw std::runtime_error{"Invariant violated"};
    }
    
    return buff_.size() - write_;
}

void byte_buffer::write(const uint8_t* data, size_t size)
{
    write_ += size;
    if(write_ > buff_.size())
    {
        expand(write_);
    }
    std::copy(data, data + size, buff_.data() + (write_ - size));
}

void byte_buffer::write_8(uint8_t data)
{
    write(&data, 1);
}

void byte_buffer::write_32(uint32_t data)
{
    data = htonl(data);
    write(reinterpret_cast<uint8_t*>(&data), 4);
}

void byte_buffer::write_64(uint64_t data)
{
    write(reinterpret_cast<uint8_t*>(&data), 8);
}

uint8_t* byte_buffer::read(size_t size)
{
    read_ += size;
    if(!invariant())
    { 
        throw std::range_error{"Read of size: " + std::to_string(size) + " violates invariant"};
    }

    return buff_.data() + (read_ - size);
}

uint8_t byte_buffer::read_8() 
{
    return *read(1);
}

uint32_t byte_buffer::read_32() 
{
    auto data = read(4);

    uint32_t res;
    std::copy(data, data + 4, &res);

    return res;
}

uint8_t byte_buffer::peek_8(size_t offset) const
{
    return *(get_read() + offset);
}

uint32_t byte_buffer::peek_32(size_t offset) const
{
    uint32_t res;
    const auto read = get_read() + offset;
    std::copy(read, read + 4, reinterpret_cast<uint8_t*>(&res));

    return ntohl(res);
}

byte_buffer byte_buffer::slice(size_t begin, size_t end) const
{
    const auto size = end - begin;
    if(size <= 0)
    {
        return {};
    }

    byte_buffer bb{size};
    bb.write(data() + begin, size);

    return bb;
}

bool byte_buffer::invariant() const
{
    return read_ <= write_ && read_ <= buff_.size() && write_ <= buff_.size() && buff_.size() <= MAX_SIZE;
}

void byte_buffer::expand(size_t size)
{
    const auto new_size = 2 * size;
    if(new_size > MAX_SIZE)
    {
        throw std::length_error{"Buffer exceeded max limit"};
    }

    buff_.resize(2 * size);
}


} 
