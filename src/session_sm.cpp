#include "session_sm.hpp"

namespace tent
{

session_sm::session_sm(ism_client& client) :
    client_(client),
    state_(IDLE)
{
    client_.start();
}

void session_sm::on_event(session_event ev)
{
    switch (ev)
    {
    case session_event::START:
    {
        state_ = STARTED;
        client_.connect();
        break;   
    }
    case session_event::CONNECTED: 
    {
        state_ = CONNECTED;
        client_.handshake();
        break;
    }
    case session_event::HANDSHAKE: 
    {
        state_ = HANDSHAKE_RECEIVED;
        client_.interested();
        break;
    }
    case session_event::CHOKE: 
    {
        state_ = CHOKED;
        client_.choked();
        break;
    }
    case session_event::UNCHOKE: 
    {
        state_ = UNCHOKED;
        client_.unchoked();
        break;
    }
    default:
        break;
    }
}

bool session_sm::handshake_received()
{
    return state_ >= HANDSHAKE_RECEIVED;
}

}