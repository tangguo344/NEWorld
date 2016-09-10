#include "networkmanager.h"
#include "logger.h"

// TODO:Implement them!!!
// TODO:Delete all debug log before merge to `renew`

NetworkManager::NetworkManager(Server &s)
    : m_server(s)
{
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
