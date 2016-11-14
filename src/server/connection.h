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

#ifndef CONNETCION_SERVER_H_
#define CONNETCION_SERVER_H_

#include "networkmanager.h"

/*
* The base class of any Connection class to it's NetworkManager
*  @see NetworkManager
*  @see NetworkManager
*/
class Connection
{
public:
    friend class MultiplayerConnection;
    template<class ProtocolType>
    void send(const flatbuffers::FlatBufferBuilder& fbb, const flatbuffers::Offset<ProtocolType>&, PacketPriority priority, PacketReliability reliability)
    {
        sendRawData(packetType2Id<ProtocolType>(), fbb.GetBufferPointer(), fbb.GetSize()*CHAR_BIT, priority, reliability);
    }
private:
    Connection(NetworkManager &network, RakNet::RakPeerInterface *peer, RakNet::SystemAddress addr);
    ~Connection();
    void sendRawData(Identifier id, const unsigned char *data, int len, PacketPriority priority, PacketReliability reliability) const;
    NetworkManager &mNetwork;
    RakNet::RakPeerInterface *mPeer;
    RakNet::SystemAddress mAddr;
};

#endif

