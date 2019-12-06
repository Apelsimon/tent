#ifndef HTTP_CLIENT_HPP_
#define HTTP_CLIENT_HPP_

#include "curlpp/cURLpp.hpp"

#include <memory>

namespace tent
{
namespace http
{

class imessage;

class client
{
public:
    std::unique_ptr<imessage> get(const std::string& url);
    
private:
    curlpp::Cleanup cleanup_;
};

}
}

#endif