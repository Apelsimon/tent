#include "file_handler.hpp"

#include "byte_buffer.hpp"
#include "pieces.hpp"
#include "piece_handler.hpp"

namespace tent
{

file_handler::file_handler(lt::file_index_t index, const lt::torrent_info& info) :
    index_(index),
    torrent_info_(info),
    file_(info.files().file_path(index), std::ios::binary | std::ios::out)
{
    file_.seekp(info.files().file_size(index) - 1);
    file_.write("", 1);
}

void file_handler::write(const piece_received_key& pos, byte_buffer& data)
{
    const auto piece_len = torrent_info_.piece_length();
    const auto file_len = torrent_info_.total_size();
    const auto offset = piece_len * pos.index_ + pos.begin_;
    const auto write_len = data.read_available() % (file_len - offset);

    file_.seekp(offset);
    file_.write(reinterpret_cast<const char*>(data.get_read()), write_len);
}

} // namespace tent