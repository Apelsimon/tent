#include "piece_handler.hpp"

#include "byte_buffer.hpp"
#include "session.hpp"

#include "libtorrent/hasher.hpp"

#include <bitset>
#include <iostream>

/* TODO: 
    - create helpers to retreive e.g piece and block sizes
    - look over redundancy
*/

namespace tent
{

piece_handler::piece_handler(session& session, const lt::torrent_info& info) :
    session_(session),
    torrent_info_(info),
    have_map_(),
    request_map_(),
    received_pieces_(),
    file_handler_(info)
{}

void piece_handler::have(const std::string& peer_id, uint32_t index)
{
    auto& have_set = have_map_[peer_id];
    if(have_set.find(index) == have_set.end())
    {   
        have_set.emplace(index);
        add_to_queue(peer_id, index);
    }
}

void piece_handler::have(const std::string& peer_id, byte_buffer& bitfield)
{
    auto index_offset = 0;
    while(bitfield.read_available() > 0)
    {
        std::bitset<8> bits{bitfield.read_8()};
        for(int i = bits.size() - 1; i >= 0; --i)
        {
            if(bits[i])
            {
                have(peer_id, index_offset * 8 + (bits.size() - i - 1));
            }
        }
        ++index_offset;
    }
}

void piece_handler::received(byte_buffer& piece)
{
    piece_received_key key{piece.read_32(), piece.read_32()};
    if(!received_pieces_[key].received_)
    {
        received_pieces_[key].block_.write(piece.get_read(), piece.read_available());
        received_pieces_[key].received_ = true;

        if(write_if_valid(key.index_))
        {
            std::cout << "Valid piece " << key.index_ << " written to file" << std::endl;
            if(is_done()) 
            {
                session_.stop();
            }
        }
    }
}

// TODO: return unique_ptr or struct to signal success
std::pair<bool, msg::request> piece_handler::get_piece_request(const std::string& peer_id)
{
    auto& queue = request_map_[peer_id];
    if(queue.empty() && !rebuild_queue(peer_id))
    {
        std::cout << "Nothing more to rebuild" << std::endl;
        return {false, msg::request{0, 0, 0}};
    }

    msg::request piece_req;
    auto unreceived_piece_requested = false;
    do
    {
        piece_req = queue.front();
        queue.pop();

        unreceived_piece_requested = received_pieces_[{piece_req.index_, piece_req.begin_}].received_;
    } while (unreceived_piece_requested && !queue.empty());   

    return {!unreceived_piece_requested, piece_req};
}

void piece_handler::add_to_queue(const std::string& peer_id, uint32_t index)
{    
    const auto piece_size = torrent_info_.piece_size(index);
    const auto blocks_per_piece = std::ceil(piece_size / static_cast<double>(BLOCK_LEN));

    auto& queue = request_map_[peer_id];
    for(uint32_t block_ind = 0; block_ind < blocks_per_piece; ++block_ind)
    {
        piece_received_key key{index, block_ind * BLOCK_LEN};
        if(!received_pieces_[key].received_)
        {
            const auto block_len = (block_ind < (blocks_per_piece - 1)  || 
                piece_size % BLOCK_LEN == 0) ? BLOCK_LEN : piece_size % BLOCK_LEN;

            queue.emplace(index, block_ind * BLOCK_LEN, block_len);
        }
    }
}

bool piece_handler::rebuild_queue(const std::string& peer_id)
{
    auto& have_set = have_map_[peer_id];
    for(auto have_index : have_set)
    {
        add_to_queue(peer_id, have_index);
    }

    return !request_map_[peer_id].empty();
}

bool piece_handler::is_done()
{
    const uint32_t num_pieces = torrent_info_.num_pieces();

    for(uint32_t piece_ind = 0; piece_ind < num_pieces; ++piece_ind)
    {
        const auto piece_size = torrent_info_.piece_size(piece_ind);
        const auto blocks_per_piece = std::ceil(piece_size / static_cast<double>(BLOCK_LEN));
        for(uint32_t block_ind = 0; block_ind < blocks_per_piece; ++block_ind)
        {
            if(!received_pieces_[{piece_ind, block_ind * BLOCK_LEN}].received_)
            {
                return false;
            }
        }
    }

    return true;
}

bool piece_handler::write_if_valid(uint32_t index)
{
    const auto piece_size = torrent_info_.piece_size(index);
    const auto blocks_per_piece = std::ceil(piece_size / static_cast<double>(BLOCK_LEN));

    lt::hasher hasher;
    for(uint32_t block_ind = 0; block_ind < blocks_per_piece; ++block_ind)
    {
        auto& block = received_pieces_[{index, block_ind * BLOCK_LEN}];
        if(!block.received_)
        {
            return false;
        }
        
        hasher.update(reinterpret_cast<const char*>(block.block_.get_read()), 
            block.block_.read_available());
    }

    const auto piece_valid_for_print = hasher.final() == torrent_info_.hash_for_piece(index);
    if(piece_valid_for_print)
    {
        for(uint32_t block_ind = 0; block_ind < blocks_per_piece; ++block_ind)
        {
            piece_received_key key{index, block_ind * BLOCK_LEN};
            auto& block = received_pieces_[key];
            file_handler_.write(key, block.block_);
        } 
    }

    return piece_valid_for_print;
}

}