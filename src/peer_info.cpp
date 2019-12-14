#include "peer_info.hpp"

namespace tent
{

std::ostream &operator<<(std::ostream& os, const peer_info& info) 
{ 
    return os << "peer: " << info.id_ << ", endpoint: " << info.endpoint_;
}

}