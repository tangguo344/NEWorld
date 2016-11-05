#ifndef NetworkManager_H
#define NetworkManager_H

#include <stdexcept>
#include <thread>
#include <raknet/RakPeerInterface.h>
#include <raknet/MessageIdentifiers.h>
#include "../protocol/gen/protocol.h"
#include <functional>
class Connection;

class NetworkManager
{
public:
    NetworkManager(std::function<void(Identifier, unsigned char*)> userDataCallback);
    ~NetworkManager();
    void close();
    /**
     * Try to start up an server NetworkManager and start listenning on it.
     * @throw std::runtime_error if start up failed
     * @return `true` if start up successfully,`false` if something unexcepted happened
     */
    bool run(const char *addr,unsigned short port);
private:
    void loop();
    Connection *newConnection(RakNet::SystemAddress addr);
    void deleteConnection(RakNet::SystemAddress addr);
    RakNet::RakPeerInterface *mPeer;
    std::thread mThread;
    std::vector<Connection*> mConns;
    std::function<void(Identifier, unsigned char*)> mUserDataCallback;
};

/** The base class of any Connection class to it's NetworkManager
 *  @see NetworkManager
 *  @see NetworkManager
 */
class Connection
{
public:
    friend class NetworkManager;
    template<class ProtocolType>
    void send(const flatbuffers::Offset<ProtocolType>& data, PacketPriority priority, PacketReliability reliability)
    {
        sendRawData(static_cast<RakNet::MessageID>(ID_USER_PACKET_ENUM), reinterpret_cast<const unsigned char*>(&data), sizeof(data), priority, reliability);
    }
private:
    Connection(NetworkManager &network,RakNet::RakPeerInterface *peer,RakNet::SystemAddress addr);
    ~Connection();
    void sendRawData(RakNet::MessageID id, const unsigned char *data, int len, PacketPriority priority, PacketReliability reliability);
    NetworkManager &mNetwork;
    RakNet::RakPeerInterface *mPeer;
    RakNet::SystemAddress mAddr;
};

#endif
