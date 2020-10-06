#include "udp_tracker_client.hpp"

#include "log_ctrl.hpp"
#include "msg_factory.hpp"
#include "net.hpp"
#include "net_reactor.hpp"
#include "peer_info.hpp"
#include "random.hpp"

#include <iostream>
#include <sys/epoll.h>

namespace tent
{

udp_tracker_client::udp_tracker_client(const lt::torrent_info& info, const std::string& peer_id, uint16_t local_port,
    std::vector<std::unique_ptr<peer_info>>& received_peers) :
    buffer_(2048),
    current_tracker_ep_(),
    reactor_(200),
    received_peers_(received_peers),
    socket_(),
    info_(info),
    peer_id_(peer_id),
    info_hash_(),
    local_port_(local_port),
    current_transaction_id_()
{
    std::stringstream ss;
    ss <<  info_.info_hash();
    info_hash_ = ss.str();

    reactor_.reg(*this, EPOLLIN);
}

udp_tracker_client::~udp_tracker_client()
{
    reactor_.unreg(*this);
}

void udp_tracker_client::read()
{
    tent::endpoint src_ep;
    buffer_.reset();
    if(socket_.recvfrom(buffer_, src_ep) > 0) // TODO: verify src with current_tracker_ep
    {
        // TODO: verify recv length
        const auto action = buffer_.read_32();
        const auto transaction_id = buffer_.read_32(); 
        
        //TODO: make action enum 
        if(transaction_id == current_transaction_id_ && action == 0) // connect rsp
        {
            const auto connection_id = buffer_.read_64(); 
            const auto key = rnd::uint32();
            current_transaction_id_ = rnd::transaction_id();

            buffer_.reset();   
            msg_factory::announce(buffer_, connection_id, current_transaction_id_, info_hash_, 
                peer_id_, info_.total_size(), key, local_port_);
            
            if(socket_.sendto(buffer_, current_tracker_ep_) < 0)
            {
                std::cerr << "fail to send udp announce: " << std::strerror(errno) << std::endl;   
            }
        }
        else if(transaction_id == current_transaction_id_ && action == 1) // announce rsp
        {
            // TODO: handle interval
            /*const auto interval = */buffer_.read_32();
            /*const auto leechers = */buffer_.read_32();
            /*const auto seeders = */buffer_.read_32();

            while(buffer_.get_read_available() >= 6)
            {
                const auto ipaddr = buffer_.read_32();
                const auto port = buffer_.read_16();

                const auto peer_ip = net::saddr_to_str(ipaddr);
                const auto peer_port = port;

                auto it = std::find_if(received_peers_.begin(), received_peers_.end(), [&peer_ip, peer_port](const auto& peer){
                    auto addr = reinterpret_cast<sockaddr_in*>(peer->endpoint_.sockaddr());
                    return net::saddr_to_str(ntohl(addr->sin_addr.s_addr)) == peer_ip && ntohs(addr->sin_port) == peer_port;
                });

                if(it == received_peers_.end())
                {
                    received_peers_.push_back(std::make_unique<tent::peer_info>("", peer_ip, port)); // empty peer id for now, will be set on handshake
                }
            }
        }
    }
    else 
    {
        std::cout << "recv fail: " << std::strerror(errno) << std::endl;
    }
}

void udp_tracker_client::write()
{
}

bool udp_tracker_client::announce(const std::string& url)
{
    auto it = url.find("udp://");
    if(it != std::string::npos)
    {
        auto rest = url.substr(it + 6);
        const auto peername_delim = rest.find(":");
        const auto port_delim = rest.find("/");

        if(peername_delim != std::string::npos && port_delim != std::string::npos)
        {
            const auto peername = rest.substr(0, peername_delim);
            const auto peer_port = rest.substr(peername_delim + 1, 
                port_delim - peername_delim - 1);

            struct addrinfo hints;
            struct addrinfo* result;

            std::memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_UNSPEC;
            hints.ai_socktype = SOCK_DGRAM;
            hints.ai_flags = AI_PASSIVE;
            hints.ai_protocol = 0;
            hints.ai_canonname = nullptr;
            hints.ai_addr = nullptr;
            hints.ai_next = nullptr;

            const auto get_ai_res = getaddrinfo(peername.c_str(), nullptr,
                &hints, &result);
            if(get_ai_res != 0)
            {
                std::cerr << "Couldn't get addrinfo: " << gai_strerror(get_ai_res) << std::endl;
                return false;
            }

            for(auto res = result; res; res = res->ai_next)
            {
                if(res->ai_addr->sa_family == AF_INET) // TODO: IPv6 not supported for udp tracker?
                {
                    auto peer_addr = reinterpret_cast<sockaddr_in*>(res->ai_addr);
                    peer_addr->sin_port = htons(std::stoi(peer_port));
                    current_tracker_ep_ = tent::endpoint{res->ai_addr};
                    current_transaction_id_ = rnd::transaction_id();

                    buffer_.reset();
                    tent::msg_factory::connect(buffer_, current_transaction_id_);
                    
                    if(socket_.sendto(buffer_, current_tracker_ep_) > 0)
                    {
                        while(reactor_.poll() > 0);
                    }
                    else 
                    {
                        std::cout << "failed to send udp connect: " << strerror(errno) << std::endl;
                    }
                }
            }

            freeaddrinfo(result);
        }
    }
    return !received_peers_.empty();
}
} // namespace tent