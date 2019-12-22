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
    file_handler(const lt::torrent_info& info);

    void write(const piece_received_key& pos, byte_buffer& data);

private:
    const lt::torrent_info& torrent_info_;

    std::ofstream file;
};

} // namespace tent

#endif
