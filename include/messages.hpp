#ifndef MESSAGES_HPP_
#define MESSAGES_HPP_

#include "mul/byte_buffer.hpp"

#include <cstdint>

namespace tent
{

namespace msg 
{

struct request
{
    request() = default;
    request(uint32_t index, uint32_t begin, uint32_t len) :
        index_(index),
        begin_(begin),
        length_(len)
    {}

    uint32_t index_;
    uint32_t begin_;
    uint32_t length_;
};

struct piece
{
    piece(uint32_t index, uint32_t begin, const mul::byte_buffer& block) :
        index_(index),
        begin_(begin),
        block_(block)
    {}

    uint32_t index_;
    uint32_t begin_;
    mul::byte_buffer block_;
};

} // namespace msg
} // namespace tent

#endif