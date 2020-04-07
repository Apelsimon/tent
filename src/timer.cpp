#include "timer.hpp"

#include "itimer_client.hpp"

#include <algorithm>

namespace tent
{

void timer::tick()
{
    start_ = std::chrono::high_resolution_clock::now();
}

void timer::tock()
{
    auto end_ = std::chrono::high_resolution_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_ - start_).count();

    for(auto client : clients_)
    {
        client->elapsed_ += elapsed_ms;
        if(client->timeout_ms_ <= client->elapsed_)
        {
            client->on_timeout();
            client->elapsed_ = 0;
        }
    }
}

void timer::reg(itimer_client& client, float timeout_ms)
{
    client.elapsed_ = 0;
    client.timeout_ms_ = timeout_ms;
    clients_.push_back(&client);
}

void timer::unreg(itimer_client& client)
{
    clients_.erase(std::remove(clients_.begin(), clients_.end(),
        &client), clients_.end());
}


}