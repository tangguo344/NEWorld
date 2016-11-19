/*
* NEWorld: A free game with similar rules to Minecraft.
* Copyright (C) 2016 NEWorld Team
*
* This file is part of NEWorld.
* NEWorld is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* NEWorld is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef NETWORK_H__
#define NETWORK_H__

#include <raknet/RakPeerInterface.h>
#include <thread>
#include <mutex>
#include "../protocol/gen/protocol.h"
#include <future>
#include <climits>
#include <common/nwsafety.hpp>

class Connection
{
public:
    Connection(std::function<void(Identifier, unsigned char*, size_t)> userDataCallback);
    ~Connection();
    bool connect(const char *addr,unsigned short port);
    template<class ProtocolType>
    void send(const flatbuffers::FlatBufferBuilder& fbb, PacketPriority priority, PacketReliability reliability)
    {
        sendRawData(packetType2Id<ProtocolType>(),fbb.GetBufferPointer() , fbb.GetSize(), priority, reliability);
    }
    void waitForConnected()
    {
        auto future = mConnected.get_future();
        future.wait();
    }
    void stop()
    {
        verbosestream << "Disconnecting...";
        mUserClosed.store(true);
        mPeer->Shutdown(5000, 0, PacketPriority::HIGH_PRIORITY);
        if (mThread.joinable())
            mThread.join();
        RakNet::RakPeerInterface::DestroyInstance(mPeer);
        debugstream << "Network thread exited.";
    }
private:
    void loop();
    void sendRawData(Identifier id, const unsigned char *data, uint32_t len, PacketPriority priority, PacketReliability reliability);
    RakNet::RakPeerInterface *mPeer;
    std::thread mThread;
    std::promise<void> mConnected;
    RakNet::SystemAddress mAddr;
    std::function<void(Identifier, unsigned char*, size_t)> mUserDataCallback;
    std::atomic<bool> mUserClosed{false};
};

#endif // NETWORK_H__
