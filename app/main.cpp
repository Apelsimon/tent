#include "file_reader.hpp"
#include "log_ctrl.hpp"
#include "net_reactor.hpp"
#include "session.hpp"
#include "signal_handler.hpp"

#include "libtorrent/torrent_info.hpp"

#include <chrono>
#include <iostream>
#include <vector>

#include <inttypes.h>

int main(int argc, char* argv[])
{
    tent::log_ctrl log_ctrl;

    if(argc < 2)
    {
        spdlog::get("console")->info( "usage: tent <torrent file>");
        return EXIT_FAILURE;
    }

    auto begin = std::chrono::steady_clock::now();

    tent::signal_handler::init();

    auto file_buffer = tent::file_reader::to_buffer(argv[1]);
    lt::error_code error;
    
    const lt::torrent_info torrent_info{file_buffer.data(), 
        static_cast<int>(file_buffer.size()), error};
    
    if(error.value() != 0)
    {
        spdlog::get("console")->error("Failed to read torrent: {}", error.message());
        return EXIT_FAILURE;
    }

    tent::net_reactor reactor;
    tent::session session{reactor, torrent_info};
    tent::signal_handler::set_cb( [&session]() { session.print_left(); });
    session.start();

    auto end = std::chrono::steady_clock::now();
    spdlog::get("console")->info("Time difference: {}[s]", std::chrono::duration_cast<std::chrono::seconds>(end - begin).count());

    return EXIT_SUCCESS;
}