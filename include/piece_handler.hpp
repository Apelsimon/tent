#ifndef PIECE_HANDLER_HPP_
#define PIECE_HANDLER_HPP_

#include "messages.hpp"
#include "pieces.hpp"

#include "libtorrent/torrent_info.hpp"

#include <queue>
#include <string>

namespace tent
{

using piece_index = uint32_t;

class piece_handler
{
public:
    piece_handler(const lt::torrent_info& info);

    void have(const std::string& peer_id, uint32_t index);
    void have(const std::string& peer_id, byte_buffer& bitfield);
    void received(byte_buffer& piece);
    std::pair<bool, msg::request> get_piece_request(const std::string& peer_id);

private:
    void add_to_queue(const std::string& peer_id, uint32_t index);
    bool rebuild_queue(const std::string& peer_id);

    const lt::torrent_info& torrent_info_;

    std::unordered_map<std::string, std::unordered_set<piece_index>> have_map_;
    std::unordered_map<std::string, std::queue<msg::request>> request_map_;
    received_pieces_map received_pieces_;
};

}

#endif