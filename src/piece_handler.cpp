#include "piece_handler.hpp"

#include "byte_buffer.hpp"
#include "session.hpp"

#include "libtorrent/hasher.hpp"

#include <bitset>
#include <iostream>
#include <unordered_set>

/* TODO: 
    - create helpers to retreive e.g piece and block sizes
    - look over redundancy
*/

namespace tent
{

static void build_dir_tree(const lt::file_storage& file_storage);

piece_handler::piece_handler(session& session, const lt::torrent_info& info) :
    session_(session),
    torrent_info_(info),
    have_map_(),
    request_map_(),
    received_pieces_(),
    file_handlers_()
{
    const auto& file_storage = info.files();

    build_dir_tree(file_storage);

    // init file handlers
    for(auto i : file_storage.file_range())
    {
        const std::string file_name{file_storage.file_name(i)};
        file_handlers_.emplace(std::piecewise_construct, 
            std::forward_as_tuple(file_name), std::forward_as_tuple(i, info));
    }
}

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
            std::cout << "wrote valid piece: " << key.index_ << std::endl;
            if(is_done()) 
            {
                session_.stop();
            }
        }
    }
}

void piece_handler::print_left()
{
    std::cout << "unreceived pieces: " << std::endl;
    for(const auto& pair : received_pieces_)
    {
        if(!pair.second.received_)
        {
            std::cout << " ind: " << pair.first.index_ <<", beg: " << pair.first.begin_ <<  std::endl;
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

            const auto file_slice = torrent_info_.map_block(key.index_, key.begin_, BLOCK_LEN);
            assert(file_slice.size() == 1);

            const auto file_index = file_slice[0].file_index;
            const std::string file_name{torrent_info_.files().file_name(file_index)};

            auto it = file_handlers_.find(file_name);
            if(it != file_handlers_.end())
            {
                it->second.write(key, block.block_);
            }
            else
            {
                std::cout << "no file handler found for file: " << file_name << std::endl;
            }
            
        } 
    }

    return piece_valid_for_print;
}

static void build_dirs(std::unordered_set<std::string>& dirs, std::string& path)
{
    std::stringstream current_dir{""};
    auto it = path.find("/");

    while(it != std::string::npos)
    {
        current_dir << path.substr(0, it);
        if(std::find(dirs.begin(), dirs.end(), current_dir.str()) == dirs.end())
        {
            dirs.insert(current_dir.str());
            mkdir(current_dir.str().c_str(), 0777);
        }
        current_dir << "/";

        path = path.substr(it + 1);
        it = path.find("/");
    }
}

void build_dir_tree(const lt::file_storage& file_storage)
{
    std::unordered_set<std::string> dirs;
    for(auto index : file_storage.file_range())
    {
        std::string file_path{file_storage.file_path(index)};
        build_dirs(dirs, file_path);
    }
}

} // namespace tent