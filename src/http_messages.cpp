#include "http_messages.hpp"

#include <curlpp/Infos.hpp>

namespace tent
{
namespace http
{

int response::status()
{
    return curlpp::infos::ResponseCode::get(curl_);
}

}
}