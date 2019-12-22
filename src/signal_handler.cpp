#include "signal_handler.hpp"

#include <csignal>
#include <iostream>

namespace tent
{

void signal_handler::init()
{
    signal(SIGABRT, handle);
    signal(SIGFPE, handle);
    signal(SIGILL, handle);
    signal(SIGINT, handle);
    signal(SIGSEGV, handle);
    signal(SIGTERM, handle);
    signal(SIGPIPE, handle);
}

void signal_handler::handle(int signum)
{
    switch(signum)
    {
    case SIGABRT:
        std::cout << "RECEIVED SIGABRT" << std::endl;
        break;
    case SIGFPE:
        std::cout << "RECEIVED SIGFPE" << std::endl;
        break;
    case SIGILL:
        std::cout << "RECEIVED SIGILL" << std::endl;   
        break;
    case SIGINT:
        std::cout << "RECEIVED SIGINT" << std::endl;
        break;
    case SIGSEGV:
        std::cout << "RECEIVED SIGSEGV" << std::endl;
        break;
    case SIGTERM:
        std::cout << "RECEIVED SIGTERM" << std::endl;
        break;
    case SIGPIPE:
        std::cout << "RECEIVED SIGPIPE" << std::endl;
        return;
        break;
    }

    exit(signum);
}


}