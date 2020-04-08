#ifndef SESSION_HPP_
#define SESSION_HPP_

#include "piece_handler.hpp"
#include "timer.hpp"

#include "libtorrent/torrent_info.hpp"

#include <condition_variable>
#include <mutex>
#include <thread>

namespace tent
{

class net_reactor;
class torrent_agent;
class tracker_client;

class session
{
public:
    session(net_reactor& reactor, const lt::torrent_info& info);
    ~session();

    void start();
    void stop(); 
    void print_left() { piece_handler_.print_left();}
    const std::string& peer_id() const { return local_peer_id_; }
    float completed();

private:
    void engine();
    void print_progress(std::string& progress_str);
    
    static constexpr uint16_t PORT = 6881; 

    net_reactor& reactor_;
    std::unique_ptr<tracker_client> tracker_client_;
    std::vector<std::unique_ptr<torrent_agent>> agents_;
    timer timer_;

    lt::torrent_info torrent_info_;

    piece_handler piece_handler_;
    const std::string local_peer_id_;
    bool notified_;
    bool running_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

}

#endif