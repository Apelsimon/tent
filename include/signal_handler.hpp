#ifndef SIGNAL_HANDLER_HPP_
#define SIGNAL_HANDLER_HPP_

namespace tent
{

class signal_handler
{
public:
    static void init();

private:
    static void handle(int signum);
};

}

#endif