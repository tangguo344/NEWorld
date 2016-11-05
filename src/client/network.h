#ifndef NETWORK_H__
#define NETWORK_H__

#include <raknet/RakPeerInterface.h>
#include <thread>
#include <mutex>
#include "../protocol/gen/protocol.h"
#include <raknet/MessageIdentifiers.h>
#include <future>
#include <limits.h>
#include <logger.h>

class Connection
{
public:
    Connection(std::function<void(Identifier, unsigned char*)> userDataCallback);
    ~Connection();
    bool connect(const char *addr,unsigned short port);
    template<class ProtocolType>
    void send(const flatbuffers::FlatBufferBuilder& fbb, const flatbuffers::Offset<ProtocolType>& data, PacketPriority priority, PacketReliability reliability)
    {
        debugstream << "Sent a packet!"<<static_cast<int>(packetType2Id<ProtocolType>());
        sendRawData(packetType2Id<ProtocolType>(),fbb.GetBufferPointer() , fbb.GetSize()*CHAR_BIT, priority, reliability);
    }
    void waitForConnected()
    {
        auto future = mConnected.get_future();
        future.wait();
    }

private:
    void loop();
    void sendRawData(Identifier id, const unsigned char *data, int len, PacketPriority priority, PacketReliability reliability);
    RakNet::RakPeerInterface *mPeer;
    std::thread mThread;
    std::promise<void> mConnected;
    RakNet::SystemAddress mAddr;
    std::function<void(Identifier, unsigned char*)> mUserDataCallback;
};

#endif // NETWORK_H__
