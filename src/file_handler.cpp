#include "file_handler.hpp"

#include "byte_buffer.hpp"
#include "pieces.hpp"
#include "piece_handler.hpp"

#include <iostream>

namespace tent
{

file_handler::file_handler(const lt::torrent_info& info) :
    torrent_info_(info),
    file(info.name(), std::ios::binary | std::ios::out)
{
    file.seekp(info.total_size() - 1);
    file.write("", 1);
    file.seekp(0);
}

void file_handler::write(const piece_received_key& pos, byte_buffer& data)
{
    const auto piece_len = torrent_info_.piece_length();
    const auto file_len = torrent_info_.total_size();
    const auto offset = piece_len * pos.index_ + pos.begin_ * piece_handler::BLOCK_LEN;
    const auto write_len = data.read_available() % (file_len - offset);
    std::cout << "write index: " << pos.index_ << ", begin: " << pos.begin_ << ", read avail: " <<
        data.read_available() << std::endl;
    file.seekp(offset, std::ios_base::beg);
    file.write(reinterpret_cast<const char*>(data.get_read()), write_len);
    file.seekp(0, std::ios_base::beg);
}

} // namespace tent