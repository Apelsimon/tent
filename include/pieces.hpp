#ifndef PIECES_HPP_
#define PIECES_HPP_

#include "byte_buffer.hpp"

#include <cstdint>
#include <unordered_set>

namespace tent
{

using piece_index = uint32_t;

class piece_request
{
public:
    piece_request(piece_index index, uint32_t begin, uint32_t len) :
        index_(index),
        begin_(begin),
        length_(len)
    {}

    piece_index index_;
    uint32_t begin_;
    uint32_t length_;
};

class piece_received
{
public:
    uint32_t index_;
    uint32_t begin_;
    byte_buffer block_;

    bool operator==(const piece_received& other) const 
    {
        return index_ == other.index_ && begin_ == other.begin_;
    }
};

class piece_received_hash
{
public:
    size_t operator()(const piece_received& piece) const
    {
        auto h1 = std::hash<uint32_t>{}(piece.index_);
        auto h2 = std::hash<uint32_t>{}(piece.begin_);

        return h1 ^ (h2 << 1); // TODO: use boost::hash_combine
    }
};

using received_piece_map = std::unordered_set<piece_received, piece_received_hash>;

}

#endif