#include "networkmanager.h"
#include "logger.h"

// TODO:Implement them!!!
// TODO:Delete all debug log before merge to `renew`

NetworkManager::NetworkManager(Server &s)
    : mServer(s),mGateway(*this)
{
    mGateway.run("127.0.0.1",9887); // TODO: get address and port to bind from settingsmanager.
}

void NetworkManager::stop()
{
    mGateway.close();
}

void NetworkManager::onConnStart(Connection *conn)
{
    debugstream << "onConnStart:";
}

void NetworkManager::onConnStop(Connection *conn)
{
    debugstream << "onConnStop:";
}

void NetworkManager::onReciveData(Connection *conn, const char *data, size_t len)
{
    debugstream << "onReciveData[" << len << "]:" << data;
}
