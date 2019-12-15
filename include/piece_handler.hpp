#ifndef PIECE_HANDLER_HPP_
#define PIECE_HANDLER_HPP_

#include "pieces.hpp"

#include "libtorrent/torrent_info.hpp"

#include <queue>

namespace tent
{

class piece_handler
{
public:
    piece_handler(const lt::torrent_info& info);

    void have(uint32_t index);
    void have(byte_buffer& bitfield);
    void received(byte_buffer& piece);
    std::pair<bool, piece_request> get_piece_request();

private:
    void add_to_queue(uint32_t index);
    void rebuild_queue();

    const lt::torrent_info& torrent_info_;

    std::unordered_set<piece_index> have_set_;
    std::queue<piece_request> request_queue_;
    received_piece_map received_pieces_;
};

}

#endif