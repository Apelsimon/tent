#ifndef HTTP_MESSAGES_HPP_
#define HTTP_MESSAGES_HPP_

#include <curlpp/Easy.hpp>

namespace tent
{
namespace http
{
 
class imessage
{
public:
    virtual ~imessage() = default;
    virtual int status() = 0;
};

class response : public imessage
{
public:
    int status() override;

    std::string body_;
    curlpp::Easy curl_;
};

}
}

#endif