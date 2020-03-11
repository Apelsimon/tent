#ifndef SIGNAL_HANDLER_HPP_
#define SIGNAL_HANDLER_HPP_

#include <functional>

namespace tent
{

class signal_handler
{
public:
    static void init();
    static void set_cb(std::function<void()> cb);
private:
    static void handle(int signum);
};

}

#endif