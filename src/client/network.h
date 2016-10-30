#ifndef NETWORK_H__
#define NETWORK_H__

#include <raknet/RakPeerInterface.h>
#include <thread>
#include "../protocol/gen/protocol.h"
#include <raknet/MessageIdentifiers.h>

class Connection
{
public:
    Connection();
    ~Connection();
    bool connect(const char *addr,unsigned short port);
    template<class ProtocolType>
    void send(const flatbuffers::Offset<ProtocolType>& data, PacketPriority priority, PacketReliability reliability)
    {
        sendRawData(ID_USER_PACKET_ENUM, reinterpret_cast<const unsigned char*>(&data), sizeof(data), priority, reliability);
    }
    void waitForConnected()
    {
        while (!mConnected);
    }
private:
    void loop();
    void Connection::sendRawData(RakNet::MessageID id, const unsigned char *data, int len, PacketPriority priority, PacketReliability reliability);
    RakNet::RakPeerInterface *mPeer;
    std::thread mThread;
    volatile bool mConnected = false;
    RakNet::SystemAddress mAddr;
};

#endif // NETWORK_H__
