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

#include "connection.h"
#include <common/nwsafety.hpp>

Connection::Connection(NetworkManager &network, RakNet::RakPeerInterface *peer, RakNet::SystemAddress addr)
    :mNetwork(network), mPeer(peer), mAddr(addr)
{
    verbosestream << inet_ntoa(mAddr.address.addr4.sin_addr) << ':' << mAddr.address.addr4.sin_port << " connected.";
}

Connection::~Connection()
{
    verbosestream << inet_ntoa(mAddr.address.addr4.sin_addr) << ':' << mAddr.address.addr4.sin_port << " disconnected.";
}

void Connection::sendRawData(Identifier id, const unsigned char *data, uint32_t len, PacketPriority priority, PacketReliability reliability)
{
    RakNet::BitStream bsOut;
    bsOut.Write(static_cast<RakNet::MessageID>(ID_USER_PACKET_ENUM));
    bsOut.Write(static_cast<int16_t>(id));
    bsOut.WriteBits(data, len * CHAR_BIT);
    mPeer->Send(&bsOut, priority, reliability, 0, mAddr, false);
}
