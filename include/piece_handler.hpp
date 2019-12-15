#ifndef PIECE_HANDLER_HPP_
#define PIECE_HANDLER_HPP_

#include "libtorrent/torrent_info.hpp"

#include <cstdint>
#include <set>

namespace tent
{

class byte_buffer;

using piece_index = uint32_t;

class piece
{
public:
    piece_index index_;
    uint32_t begin_;
    uint32_t length_;
};

class piece_handler
{
public:
    piece_handler(const lt::torrent_info& info);

    void have(uint32_t index);
    void have(byte_buffer& bitfield);
    piece get_piece();

private:
    const lt::torrent_info& torrent_info_;

    std::set<piece_index> have_set_;
};

}

#endif