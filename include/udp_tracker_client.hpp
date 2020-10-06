#ifndef UDP_TRACKER_CLIENT_HPP_
#define UDP_TRACKER_CLIENT_HPP_

#include "endpoint.hpp"
#include "inet_reactor_client.hpp"
#include "net_reactor.hpp"
#include "udp_socket.hpp"

#include "libtorrent/torrent_info.hpp"

#include "mul/byte_buffer.hpp"

#include <string>

namespace tent
{

class peer_info;

class udp_tracker_client : public inet_reactor_client
{
public: 
    udp_tracker_client(const lt::torrent_info& info, const std::string& peer_id, uint16_t local_port, 
        std::vector<std::unique_ptr<peer_info>>& received_peers);
    ~udp_tracker_client();

    void read() override;
    void write() override;
    int fd() const override { return socket_.fd(); }

    bool announce(const std::string& url);

private:
    mul::byte_buffer buffer_;
    endpoint current_tracker_ep_;
    net_reactor reactor_;
    std::vector<std::unique_ptr<peer_info>>& received_peers_;
    udp_socket socket_;
    
    const lt::torrent_info& info_;

    std::string peer_id_;
    std::string info_hash_;
    uint16_t local_port_;
    uint32_t current_transaction_id_;
};

} // namespace tent

#endif