#ifndef SESSION_SM_HPP_
#define SESSION_SM_HPP_

namespace tent
{

enum class session_event
{
    START,
    CONNECTED,
    HANDSHAKE,
    CHOKE,
    UNCHOKE
};

class ism_client
{
public:
    virtual ~ism_client() = default;
    virtual void start() = 0;
    virtual void connect() = 0;
    virtual void handshake() = 0;
    virtual void interested() = 0;
    virtual void choked() = 0;
    virtual void unchoked() = 0;
};

class session_sm
{
public:
    session_sm(ism_client& client);
    void on_event(session_event ev);
    bool handshake_received();

private:
    enum state
    {
        STARTED,
        CONNECTED,
        HANDSHAKE_RECEIVED,
        CHOKED,
        UNCHOKED,
        IDLE
    };

    ism_client& client_;
    state state_;
};

}

#endif