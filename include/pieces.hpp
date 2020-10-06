#ifndef PIECES_HPP_
#define PIECES_HPP_

#include "mul/byte_buffer.hpp"

#include <cstdint>
#include <unordered_set>
#include <unordered_map>

namespace tent
{

struct piece_received_key
{
  uint32_t index_;
  uint32_t begin_;

  bool operator==(const piece_received_key& other) const 
  {
      return index_ == other.index_ && begin_ == other.begin_;
  }
};

struct received_block
{
  bool received_{false};
  mul::byte_buffer block_;
};

using received_pieces_map = std::unordered_map<piece_received_key, received_block>;

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