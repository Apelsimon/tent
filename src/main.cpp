#include "file_reader.hpp"
#include "tracker_client.hpp"

#include "libtorrent/torrent_info.hpp"

#include <iostream>
#include <vector>


int main(int argc, char* argv[])
{
    auto file_buffer = tent::file_reader::to_buffer(argv[1]);
    lt::error_code error;
    
    const lt::torrent_info torrent_info{file_buffer.data(), 
        static_cast<int>(file_buffer.size()), error};
    
    if(error.value() != 0)
    {
        std::cerr << "Failed to read torrent: " << error.message() << std::endl;
        return EXIT_FAILURE;
    }

    tent::tracker_client tracker_client{torrent_info};
    tracker_client.announce(12345);  

    return EXIT_SUCCESS;
}