#ifndef GATEWAY_H
#define GATEWAY_H

#include <stdexcept>
#include "networkmanager.h"

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
    inline NetworkManager &getNetworkManager() {return m_network;}
    /// @return `true` if this gateway is running,
    inline bool isAlive() {return m_alive;}
protected:
    /// Set the gateway status as `alive` or not.
    void setAlive(bool alive) {m_alive = alive;}
private:
    NetworkManager &m_network;
    bool m_alive;
};

/** The base class of any Connection class to it's Gateway
 *  @see Gateway
 *  @see NetworkManager
 */
class Connection
{
public:
    virtual void sendData(const char *data,size_t len) = 0;
};

#endif
