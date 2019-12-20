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

void byte_buffer::write_16(uint16_t data)
{
    data = htons(data);
    write(reinterpret_cast<uint8_t*>(&data), 2);
}

void byte_buffer::write_32(uint32_t data)
{
    data = htonl(data);
    write(reinterpret_cast<uint8_t*>(&data), 4);
}

void byte_buffer::write_64(uint64_t data)
{
    const uint32_t high = htonl(static_cast<uint32_t>(data >> 32));
    const uint32_t low = htonl(static_cast<uint32_t>(data & 0xffffffff));

    data = (static_cast<uint64_t>(high) << 32) | low;
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

uint16_t byte_buffer::read_16()
{
    uint16_t res;
    auto data = read(2);
    std::copy(data, data + 2, reinterpret_cast<uint8_t*>(&res));

    return ntohs(res);
}

uint32_t byte_buffer::read_32() 
{
    auto data = read(4);

    uint32_t res;
    std::copy(data, data + 4, reinterpret_cast<uint8_t*>(&res));

    return ntohl(res);
}

uint64_t byte_buffer::read_64()
{
    auto data = read(8);
    uint64_t res;
    std::copy(data, data + 8, reinterpret_cast<uint8_t*>(&res));

    const uint32_t high = ntohl(static_cast<uint32_t>(res >> 32));
    const uint32_t low = ntohl(static_cast<uint32_t>(res & 0xffffffff));

    return (static_cast<uint64_t>(high) << 32) | low;
}

uint8_t byte_buffer::peek_8(size_t offset) const
{
    return *(get_read() + offset);
}

uint16_t byte_buffer::peek_16(size_t offset) const
{
    uint16_t res;
    
    const auto read = get_read() + offset;
    std::copy(read, read + 4, reinterpret_cast<uint8_t*>(&res));

    return ntohs(res);
}

uint32_t byte_buffer::peek_32(size_t offset) const
{
    uint32_t res;
    const auto read = get_read() + offset;
    std::copy(read, read + 4, reinterpret_cast<uint8_t*>(&res));

    return ntohl(res);
}

uint64_t byte_buffer::peek_64(size_t offset) const
{
    const uint32_t high = peek_32(offset);
    const uint32_t low = peek_32(4 + offset);

    return (static_cast<uint64_t>(high) << 32) | low;
}

bool byte_buffer::operator==(const byte_buffer& other) const
{
    if(read_ != other.read_ || write_ != other.write_ || 
        buff_.size() != other.buff_.size())
    {
        return false;
    }

    for(auto i = 0; i < read_available(); ++i)
    {
        if(peek_8(i) != other.peek_8(i))
        {
            return false;
        }
    }

    return true;
}

bool byte_buffer::operator!=(const byte_buffer& other) const
{
    return !(*this == other);
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


void byte_buffer::print(std::ostream& os) const
{
    os << "[ read: " << read_ << ", write: " << write_ << ", size: " << 
        buff_.size() << " ]";
}

}  // namespace tent


std::ostream& operator<<(std::ostream& os, const tent::byte_buffer& buff)
{
    buff.print(os);
    return os;
}
