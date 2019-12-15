#include "piece_handler.hpp"

#include "byte_buffer.hpp"

#include <bitset>
#include <iostream>

namespace tent
{

piece_handler::piece_handler(const lt::torrent_info& info) :
    torrent_info_(info)
{}

void piece_handler::have(uint32_t index)
{
    if(have_set_.find(index) == have_set_.end())
    {   
        have_set_.emplace(index);
        add_to_queue(index);
    }
}

void piece_handler::have(byte_buffer& bitfield)
{
    auto index_offset = 0;
    while(bitfield.read_available() > 0)
    {
        std::bitset<8> bits{bitfield.read_8()};
        for(auto i = 0; i < bits.size(); ++i)
        {
            if(bits[i])
            {
                have(index_offset * 8 + i);
            }
        }
        ++index_offset;
    }
}

void piece_handler::received(byte_buffer& piece)
{
    piece_received pr{piece.read_32(), piece.read_32(), piece};
    if(received_pieces_.find(pr) == received_pieces_.end())
    {
        received_pieces_.insert(pr);
    }
}

std::pair<bool, piece_request> piece_handler::get_piece_request()
{
    if(request_queue_.empty())
    {
        rebuild_queue(); // TODO
        std::cout << "REBUILD QUEUE!" << std::endl;
        return {false, piece_request{0, 0, 0}};
    }

    auto piece = request_queue_.front();
    request_queue_.pop();

    return {true, piece};
}

void piece_handler::add_to_queue(uint32_t index)
{
    static constexpr uint16_t BLOCK_LEN = 1 << 14;
    
    const auto piece_size = torrent_info_.piece_size(index);
    const auto blocks_per_piece = std::ceil(piece_size / static_cast<double>(BLOCK_LEN));

    for(auto block_ind = 0; block_ind < blocks_per_piece; ++block_ind)
    {
        const auto block_len = block_ind < (blocks_per_piece - 1) ? 
            BLOCK_LEN : piece_size % BLOCK_LEN;

        request_queue_.emplace(index, block_ind, block_len);
    }
}

void piece_handler::rebuild_queue()
{
    // TODO
}

}