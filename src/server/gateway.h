#ifndef GATEWAY_H
#define GATEWAY_H

#include "networkmanager.h"

/** The base class of all Gateways.
 *  Network Gateway,can be implemented as TCP or UDP.
 *  Need to bind a NetworkManager.
 *  @see NetworkManager
 *  @see Connection
 */
class Gateway
{
public:
    /** The base class of any Connection class to it's Gateway
     *  @see Gateway
     *  @see NetworkManager
     */
    class Connection
    {
    public:
        virtual void sendData(const char *data,size_t len) = 0;
    };
    virtual Gateway(NetworkManager &network);
private:
    NetworkManager &m_network;
};
#endif
