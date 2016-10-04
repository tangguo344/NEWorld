#ifndef NETWORK_H__
#define NETWORK_H__

#include <raknet/RakPeerInterface.h>
#include <thread>

class Connection
{
public:
    Connection();
    ~Connection();
    bool connect(const char *addr,unsigned short port);
private:
    void loop();
    RakNet::RakPeerInterface *mPeer;
    std::thread mThread;
};

#endif // NETWORK_H__
