#ifndef GATEWAY_H
#define GATEWAY_H

#include <stdexcept>
#include <thread>
#include <raknet/RakPeerInterface.h>

/** The base class of all Gateways.
 *  Network Gateway,can be implemented as TCP or UDP.
 *  @see NetworkManager
 *  @see Connection
 */
class NetworkManager;

class Gateway
{
public:
    Gateway(NetworkManager &network);
    ~Gateway();
    void close();
    /**
     * Try to start up an server gateway and start listenning on it.
     * @throw std::runtime_error if start up failed
     * @return `true` if start up successfully,`false` if already running,otherwise throws an exception
     */
    bool run(const char *addr,unsigned short port);
private:
    void loop();
    NetworkManager &mNetwork;
    RakNet::RakPeerInterface *mPeer;
    std::thread mThread;
};

/** The base class of any Connection class to it's Gateway
 *  @see Gateway
 *  @see NetworkManager
 */
class Connection
{
public:
    void sendData(const char *data,size_t len);
};

#endif
