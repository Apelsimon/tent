#ifndef HTTP_MESSAGES_HPP_
#define HTTP_MESSAGES_HPP_

#include <curlpp/Easy.hpp>

#include <unordered_map>

namespace tent
{
namespace http
{

enum class status_code
{
    OK = 200,
    MOVED_PERMANENTLY = 301
};

enum class header_field
{
    LOCATION
};

class imessage
{
public:
    virtual ~imessage() = default;
    virtual status_code status() const = 0;
    virtual std::string body() const = 0;
    virtual std::string field(header_field hfield) = 0;
};

class response : public imessage
{
public:
    status_code status() const override;
    std::string body() const override { return body_; }
    std::string field(header_field hfield);

    size_t header_cb(char* buffer, size_t size, size_t nitems);

    std::unordered_map<header_field, std::string> header_fields_;   
    std::string body_;
    curlpp::Easy curl_;    
};

}
}

#endif