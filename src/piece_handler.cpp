#include "piece_handler.hpp"

#include "byte_buffer.hpp"
#include "session.hpp"

#include <bitset>
#include <iostream>

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
    static uint64_t received = 0;
    static uint64_t duplicates = 0;

    piece_received_key key{piece.read_32(), piece.read_32()};
    if(!received_pieces_[key])
    {
        ++received;
        received_pieces_[key] = true;
        file_handler_.write(key, piece);

        if(is_done())
        {
            session_.stop();
        }
    }
    else
    {
        std::cout << "Received pieces: " << received << std::endl;
        std::cout << "received duplicate, count: " << ++duplicates << std::endl;
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
        }
        else
        {
            std::cout << "NOTHING MORE TO REBUILD!" << std::endl;
            return {false, msg::request{0, 0, 0}};
        }
    }

    msg::request piece_req;
    piece_received_key key;
    do
    {
        piece_req = queue.front();
        queue.pop();

        key = {piece_req.index_, piece_req.begin_};
    } while (received_pieces_[key] && !queue.empty());   

    return {true, piece_req};
}

void piece_handler::add_to_queue(const std::string& peer_id, uint32_t index)
{    
    const auto piece_size = torrent_info_.piece_size(index);
    const auto blocks_per_piece = std::ceil(piece_size / static_cast<double>(BLOCK_LEN));

    auto& queue = request_map_[peer_id];
    for(uint32_t block_ind = 0; block_ind < blocks_per_piece; ++block_ind)
    {
        piece_received_key key{index, block_ind * BLOCK_LEN};
        if(!received_pieces_[key])
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
    for(auto piece_ind = 0; piece_ind < torrent_info_.num_pieces(); ++piece_ind)
    {
        const auto piece_size = torrent_info_.piece_size(piece_ind);
        const auto blocks_per_piece = std::ceil(piece_size / static_cast<double>(BLOCK_LEN));
        for(auto block_ind = 0; block_ind < blocks_per_piece; ++block_ind)
        {
            if(!received_pieces_[{piece_ind, block_ind * BLOCK_LEN}])
            {
                return false;
            }
        }
    }

    return true;
}

}