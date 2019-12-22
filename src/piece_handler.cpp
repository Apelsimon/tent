#include "piece_handler.hpp"

#include "byte_buffer.hpp"

#include <bitset>
#include <iostream>

namespace tent
{

piece_handler::piece_handler(const lt::torrent_info& info) :
    torrent_info_(info)
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
    static uint64_t received = 0;

    piece_received_key key{piece.read_32(), piece.read_32()};
    if(received_pieces_.find(key) == received_pieces_.end())
    {
        ++received;
        std::cout << "Received pieces: " << received << std::endl;
        received_pieces_[key] = piece;
    }
}

// TODO: return unique_ptr or struct to signal success
std::pair<bool, msg::request> piece_handler::get_piece_request(const std::string& peer_id)
{
    auto& queue = request_map_[peer_id];
    if(queue.empty())
    {
        if(rebuild_queue(peer_id))
        {
            std::cout << "SUCCESSFULLY REBUILT QUEUE! size: " << queue.size() 
                << " for peer: " << peer_id << std::endl;
        }
        else
        {
            std::cout << "NOTHING MORE TO REBUILD!" << std::endl;
            return {false, msg::request{0, 0, 0}};
        }
    }

    auto piece = queue.front();
    queue.pop();

    return {true, piece};
}

void piece_handler::add_to_queue(const std::string& peer_id, uint32_t index)
{
    static constexpr uint16_t BLOCK_LEN = 1 << 14;
    
    const auto piece_size = torrent_info_.piece_size(index);
    const auto blocks_per_piece = std::ceil(piece_size / static_cast<double>(BLOCK_LEN));

    auto& queue = request_map_[peer_id];
    for(uint32_t block_ind = 0; block_ind < blocks_per_piece; ++block_ind)
    {
        piece_received_key key{index, block_ind};
        if(received_pieces_.find(key) == received_pieces_.end())
        {
            const auto block_len = (block_ind < (blocks_per_piece - 1)  || 
                piece_size % BLOCK_LEN == 0) ? BLOCK_LEN : piece_size % BLOCK_LEN;
                
            queue.emplace(index, block_ind, block_len);
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

}