#include "session.hpp"

#include "net_reactor.hpp"
#include "torrent_agent.hpp"
#include "peer_info.hpp"
#include "tracker_client.hpp"

#include <iostream>
#include <random>

static std::string create_local_peer_id();

namespace tent
{

session::session(net_reactor& reactor, const lt::torrent_info& info) : 
    reactor_(reactor),
    tracker_client_(std::make_unique<tracker_client>(info)),
    torrent_info_(info),
    piece_handler_(info),
    local_peer_id_(create_local_peer_id()),
    running_(false)
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

    while(running_)
    {
        const auto poll_res = reactor_.poll();

        if(poll_res < 0) 
        {
            stop();
        }
        else if(poll_res == 0)// timeout
        {
            // TODO: do stuff on timeout
        }
    }
}

session::~session() {}

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