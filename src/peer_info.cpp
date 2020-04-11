#include "peer_info.hpp"

#include <sstream>

namespace tent
{

std::string peer_info::to_string() const
{
    std::stringstream ss;
    ss << *this;
    return ss.str();
}

std::ostream& operator<<(std::ostream& os, const peer_info& info) 
{ 
    return os << "peer: " << info.id_ << ", endpoint: " << info.endpoint_;
}



}