#ifndef NETWORKMGR_H
#define NETWORKMGR_H

#include <cstddef> //for size_t
#include "gateway.h"

/** Manage the network connections.
 *  Use Gateway to send and recive data,unpack them and call the protocol callback.
 *  @see Gateway
 */
class Gateway;
class Connection;
class Server;

class NetworkManager
{
public:
    NetworkManager(Server &s);
    void onReciveData(Connection *conn,const char *data,size_t len);
    void onConnStart(Connection *conn);
    void onConnStop(Connection *conn);
private:
    Server &m_server;
};

#endif
