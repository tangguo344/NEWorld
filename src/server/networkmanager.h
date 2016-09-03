#ifndef NETWORKMGR_H
#define NETWORKMGR_H

#include <cstddef> //for size_t
#include "gateway.h"
/** Manage the network connections.
 *  Use Gateway to send and recive data,unpack them and call the protocol callback.
 *  @see Gateway
 */
class NetworkManager
{
public:
    void onReciveData(Gateway::Connection *conn,const char *data,size_t len);
    void onConnStart(Gateway::Connection *conn);
    void onConnStop(Gateway::Connection *conn);
};

#endif
