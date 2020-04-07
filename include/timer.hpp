#ifndef TIMER_HPP_
#define TIMER_HPP_

#include <chrono>
#include <vector>

namespace tent
{

class itimer_client;

using time_point = std::chrono::high_resolution_clock::time_point;
using clients = std::vector<itimer_client*>;


class timer
{
public:
    void tick();
    void tock();
    void reg(itimer_client& client, float timeout_ms);
    void unreg(itimer_client& client);

private:
    time_point start_;
    clients clients_;
};

}

#endif