#ifndef RAKNET_H_
#define RAKNET_H_

#include <raknet/RakPeerInterface.h>
#include <thread>
#include "gateway.h"

class RaknetGateway : public Gateway
{
public:
    RaknetGateway(NetworkManager &network);
    ~RaknetGateway();
    void close();
    /**
     * Try to start up an server gateway and start listenning on it.
     * @throw std::runtime_error if start up failed
     * @return `true` if start up successfully,`false` if already running,otherwise throws an exception
     */
    bool run(const char *addr,unsigned short port);
private:
    void loop();
    RakNet::RakPeerInterface *m_peer;
    std::thread m_thread;
};

#endif