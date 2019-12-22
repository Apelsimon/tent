#include "file_reader.hpp"
#include "net_reactor.hpp"
#include "session.hpp"
#include "signal_handler.hpp"

#include "libtorrent/torrent_info.hpp"

#include <chrono>
#include <iostream>
#include <vector>


int main(int argc, char* argv[])
{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

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

    session.start();

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[s]" << std::endl;

    return EXIT_SUCCESS;
}