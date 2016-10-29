#ifndef NETWORK_H__
#define NETWORK_H__

#include <raknet/RakPeerInterface.h>
#include <thread>
#include "../protocol/gen/protocol.h"

class Connection
{
public:
    Connection();
    ~Connection();
    bool connect(const char *addr,unsigned short port);
    template<class ProtocolType>
    void send(const flatbuffers::Offset<ProtocolType>& data, PacketPriority priority, PacketReliability reliability)
    {
        sendRawData(reinterpret_cast<const char*>(&data), sizeof(data), priority, reliability);
    }
private:
    void loop();
    void sendRawData(const char *data, int len, PacketPriority priority, PacketReliability reliability);
    RakNet::RakPeerInterface *mPeer;
    std::thread mThread;
};

#endif // NETWORK_H__
