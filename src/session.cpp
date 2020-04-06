#include "session.hpp"

#include "net_reactor.hpp"
#include "torrent_agent.hpp"
#include "peer_info.hpp"
#include "tracker_client.hpp"

#include <iomanip>
#include <iostream>
#include <random>

static std::string create_local_peer_id();

namespace tent
{

session::session(net_reactor& reactor, const lt::torrent_info& info) : 
    reactor_(reactor),
    tracker_client_(std::make_unique<tracker_client>(info)),
    torrent_info_(info),
    piece_handler_(*this, info),
    local_peer_id_(create_local_peer_id()),
    notified_(false),
    running_(false),
    mutex_(),
    cv_()
{
}

session::~session() 
{
}

void session::start()
{
    std::vector<std::unique_ptr<peer_info>> received_peers;
    if(!tracker_client_->announce(PORT, local_peer_id_, received_peers))
    {
        std::cerr << "Failed to announce to tracker" << std::endl;
        return;
    }

    for(auto& peer : received_peers)
    {
        auto agent = std::make_unique<torrent_agent>(*this, reactor_, 
            torrent_info_, std::move(peer), piece_handler_);
        agents_.push_back(std::move(agent));
    }

    running_ = true;
    std::string progress_str{""};

    std::thread engine_thread{&session::engine, this};
    std::unique_lock<std::mutex> lock(mutex_);
    while(running_)
    {
        cv_.wait(lock, [this]{ return notified_; });
        print_progress(progress_str);
        notified_ = false;
    }
    
    engine_thread.join();
}

void session::stop()
{ 
    running_ = false; 
    notified_ = true;
}

float session::completed() 
{
    return 100.f * piece_handler_.written() / static_cast<float>(torrent_info_.num_pieces());
}

void session::engine()
{
    while(running_)
    {
        std::unique_lock<std::mutex> lg{mutex_};

        const auto poll_res = reactor_.poll();

        if(poll_res < 0) 
        {
            stop();
        }
        else if(poll_res == 0)// timeout
        {
            // std::cout << "session poll timeout" << std::endl;
            // TODO: do stuff on timeout
        }
        notified_ = true;
        cv_.notify_one();
    }
}

void session::print_progress(std::string& progress_str)
{
    
    const auto progress_percentage = completed();
    const auto done = progress_percentage >= 100.f;
    if(done || progress_percentage > progress_str.size())
    {
        progress_str += "#";
        std::cout << "\r" << std::fixed << std::setprecision(2) << progress_percentage 
            << "% completed [" << progress_str << std::setw(101 - progress_str.size()) 
            << std::setfill(' ') << "]" << std::flush;
        if(done)
        {
            std::cout << std::endl;
        }
    }
}

} // namespace tent

std::string create_local_peer_id()
{
    static const std::string client_name = "-tent0001-";
    static const std::string alphanum = "0123456789"
                                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                        "abcdefghijklmnopqrstuvwxyz";
    std::random_device dev;
    std::stringstream ss;
    
    ss << client_name;
    for(auto i = 0; i < 10; ++i)
    {
      std::mt19937 rng(dev());
      std::uniform_int_distribution<std::mt19937::result_type> dist(
        0, alphanum.size() - 1);

      ss << alphanum[dist(rng)];
    }
    return ss.str();
}