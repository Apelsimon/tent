#include "file_reader.hpp"
#include "net_reactor.hpp"
#include "session.hpp"
#include "signal_handler.hpp"

#include "libtorrent/torrent_info.hpp"

#include <iostream>
#include <vector>


int main(int argc, char* argv[])
{
    tent::signal_handler::init();

    auto file_buffer = tent::file_reader::to_buffer(argv[1]);
    lt::error_code error;
    
    const lt::torrent_info torrent_info{file_buffer.data(), 
        static_cast<int>(file_buffer.size()), error};
    
    if(error.value() != 0)
    {
        std::cerr << "Failed to read torrent: " << error.message() << std::endl;
        return EXIT_FAILURE;
    }

    tent::net_reactor reactor;
    tent::session session{reactor, torrent_info};

    std::cout << "start reactor" << std::endl;
    reactor.start();
    std::cout << "reactor stopped" << std::endl;

    return EXIT_SUCCESS;
}