#include "http_client.hpp"

#include "http_messages.hpp"

#include <curlpp/Options.hpp>

#include <sstream>

namespace tent
{
namespace http
{

std::unique_ptr<imessage> client::get(const std::string& url)
{
    auto resp = std::make_unique<response>();

    try 
	{
        std::ostringstream oss;
        resp->curl_.setOpt(curlpp::options::WriteStream{&oss});
        resp->curl_.setOpt(curlpp::Options::Url{url});
        resp->curl_.perform();
        resp->body_ = oss.str();
	}
	catch( curlpp::RuntimeError &e )
	{
		std::cout << e.what() << std::endl;
	}
	catch( curlpp::LogicError &e )
	{
		std::cout << e.what() << std::endl;
	}
    
    return resp;
}

}
}