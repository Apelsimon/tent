#ifndef PIECES_HPP_
#define PIECES_HPP_

#include "byte_buffer.hpp"

#include <cstdint>
#include <unordered_set>
#include <unordered_map>

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

class piece_received_key
{
public:
    uint32_t index_;
    uint32_t begin_;

    bool operator==(const piece_received_key& other) const 
    {
        return index_ == other.index_ && begin_ == other.begin_;
    }
};

using received_pieces_map = std::unordered_map<piece_received_key, byte_buffer>;

}

namespace std 
{

  template <>
  struct hash<tent::piece_received_key>
  {
    size_t operator()(const tent::piece_received_key& key) const
    {
        // TODO: use boost::hash_combine  
        const auto h1 = std::hash<uint32_t>{}(key.index_);
        const auto h2 = std::hash<uint32_t>{}(key.begin_);

        return h1 ^ (h2 << 1);
    }
  };

}

#endif