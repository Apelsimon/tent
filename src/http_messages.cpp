#include "http_messages.hpp"

#include <curlpp/Infos.hpp>

#include <iterator>
#include <sstream>

namespace tent
{
namespace http
{

status_code response::status() const
{
    return static_cast<status_code>(curlpp::infos::ResponseCode::get(curl_));
}


std::string response::field(header_field hfield)
{
    return header_fields_[hfield];
}

size_t response::header_cb(char* buffer, size_t size, size_t nitems)
{
    static const std::string location_match = "Location: ";

    const std::string field{buffer, size * nitems};
    
    auto start = field.find(location_match);
    if(start != std::string::npos)
    {
        start += location_match.size();
        header_fields_[header_field::LOCATION] = field.substr(start, field.size() - start - 2);
    }

    return size * nitems;
}

}
}