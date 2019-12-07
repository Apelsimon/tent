#include "file_reader.hpp"
#include "tracker_client.hpp"

#include "libtorrent/torrent_info.hpp"

#include <iostream>
#include <vector>


int main(int argc, char* argv[])
{
    auto file_buffer = tent::file_reader::to_buffer(argv[1]);
    lt::error_code error;
    
    const lt::torrent_info torrent_info{file_buffer.data(), static_cast<int>(file_buffer.size()), error};

    // std::printf("\n\n----- torrent file info -----\n\n"
    // "nodes:\n");
    // for (auto const& i : torrent_info.nodes())
    //     std::printf("%s: %d\n", i.first.c_str(), i.second);

    // puts("trackers:\n");
    // for (auto const& i : torrent_info.trackers())
    //     std::printf("%2d: %s\n", i.tier, i.url.c_str());

    // std::stringstream ih;
    // ih << torrent_info.info_hash();
    // std::printf("number of pieces: %d\n"
    //     "piece length: %d\n"
    //     "info hash: %s\n"
    //     "comment: %s\n"
    //     "created by: %s\n"
    //     "name: %s\n"
    //     "number of files: %d\n"
    //     "files:\n"
    //     , torrent_info.num_pieces()
    //     , torrent_info.piece_length()
    //     , ih.str().c_str()
    //     , torrent_info.comment().c_str()
    //     , torrent_info.creator().c_str()
    //     , torrent_info.name().c_str()
    //     , torrent_info.num_files());

    tent::tracker_client tracker_client{torrent_info};
    tracker_client.announce(12345);
  
}