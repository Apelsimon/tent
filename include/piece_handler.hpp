#ifndef PIECE_HANDLER_HPP_
#define PIECE_HANDLER_HPP_

#include "file_handler.hpp"
#include "messages.hpp"
#include "pieces.hpp"

#include "libtorrent/torrent_info.hpp"

#include <queue>
#include <string>

namespace tent
{

using piece_index = uint32_t;

class session;

class piece_handler
{
public:
    piece_handler(session& session, const lt::torrent_info& info);

    void have(const std::string& peer_id, uint32_t index);
    void have(const std::string& peer_id, byte_buffer& bitfield);
    void received(byte_buffer& piece);
    std::pair<bool, msg::request> get_piece_request(const std::string& peer_id);

    static constexpr uint16_t BLOCK_LEN = 1 << 14;
private:
    void add_to_queue(const std::string& peer_id, uint32_t index);
    bool rebuild_queue(const std::string& peer_id);
    bool is_done();

    session& session_;
    const lt::torrent_info& torrent_info_;

    std::unordered_map<std::string, std::unordered_set<piece_index>> have_map_;
    std::unordered_map<std::string, std::queue<msg::request>> request_map_;
    received_pieces_map received_pieces_;
    file_handler file_handler_;
};

}

#endif