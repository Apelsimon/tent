#ifndef FILE_HANDLER_HPP_
#define FILE_HANDLER_HPP_

#include "libtorrent/torrent_info.hpp"

namespace tent
{

class byte_buffer;
class piece_received_key;

class file_handler
{
public:
    file_handler(lt::file_index_t index, const lt::torrent_info& info);

    void write(const piece_received_key& pos, byte_buffer& data);

private:
    const lt::file_index_t index_;
    const lt::torrent_info& torrent_info_;

    std::ofstream file_;
};

} // namespace tent

#endif
