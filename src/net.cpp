#include "net.hpp"

#include <arpa/inet.h>

namespace tent
{
namespace net
{

std::string saddr_to_str(uint32_t saddr)
{
    struct sockaddr_in addr{};
    addr.sin_addr.s_addr = htonl(saddr);

    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr.sin_addr), ipstr, INET_ADDRSTRLEN);

    return {ipstr};
}

} // namespace net
} // namespace tent