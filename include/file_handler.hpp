#ifndef FILE_HANDLER_HPP_
#define FILE_HANDLER_HPP_

#include "libtorrent/torrent_info.hpp"

namespace mul
{
    class byte_buffer;
}

namespace tent
{

class piece_received_key;

class file_handler
{
public:
    file_handler(lt::file_index_t index, const lt::torrent_info& info);

    void write(const piece_received_key& pos, mul::byte_buffer& data);
    void write(mul::byte_buffer& data, std::int64_t offset, std::int64_t size);

private:
    const lt::file_index_t index_;
    const lt::torrent_info& torrent_info_;

    std::ofstream file_;
};

} // namespace tent

#endif
