#include "session.hpp"

#include "net_reactor.hpp"
#include "peer_connection.hpp"
#include "peer_info.hpp"
#include "tracker_client.hpp"

#include <iostream>

namespace tent
{

const std::string session::PEER_ID = "-tent0001-0123456789";

session::session(net_reactor& reactor, const lt::torrent_info& info) : 
    reactor_(reactor),
    tracker_client_(std::make_unique<tracker_client>(info)),
    torrent_info_(info)
{
    start();
}

void session::start()
{
    std::vector<std::unique_ptr<peer_info>> received_peers;
    if(!tracker_client_->announce(PEER_ID, PORT, received_peers))
    {
        return;
    }

    // setup peer connections
    for(auto& peer : received_peers)
    {
        auto connection = std::make_unique<peer_connection>(reactor_, std::move(peer));
        connections_.push_back(std::move(connection));
        connections_.back()->connect_to_peer();
    }
}

session::~session() {}


}