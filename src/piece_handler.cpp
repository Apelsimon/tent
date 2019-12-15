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
    }
}

void piece_handler::have(byte_buffer& bitfield)
{
    auto index_offset = 0;
    while(bitfield.read_available() > 0)
    {
        std::bitset<8> bits{bitfield.read_8()};
        for(auto it = 0; it < bits.size(); ++it)
        {
            if(bits[it])
            {
                have(index_offset * 8 + it);
            }
        }
        ++index_offset;
    }
}

piece get_piece()
{
    return {};
}

}