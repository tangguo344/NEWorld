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
#include "gameconnection.h"
#include <logger.h>
#include <vec3.h>
#include "chunkclient.h"

void MultiplayerConnection::connect()
{
    mConn.connect(mHost.c_str(), mPort);
}

void MultiplayerConnection::disconnect()
{
    //not implement
    warningstream << "Function not implement";
    assert(false);
}

World* MultiplayerConnection::getWorld(size_t)
{
    //not implement
    warningstream << "Function not implement";
    assert(false);
    return nullptr;
}

void MultiplayerConnection::waitForConnected()
{
    mConn.waitForConnected();
}

void MultiplayerConnection::login(const char * username, const char * password)
{
    auto login = c2s::CreateLoginDirect(mFbb, username, password, NEWorldVersion);
    c2s::FinishLoginBuffer(mFbb, login);
    mConn.send(mFbb, login,PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE);
}

void MultiplayerConnection::getChunk(size_t worldID, Vec3i pos)
{
    auto req = c2s::CreateRequestChunk(mFbb, worldID, pos.x, pos.y, pos.z);
    c2s::FinishRequestChunkBuffer(mFbb, req);
    mConn.send(mFbb, req, PacketPriority::MEDIUM_PRIORITY, PacketReliability::UNRELIABLE);
}

void MultiplayerConnection::handleReceivedData(Identifier id, unsigned char* data)
{
    switch (id)
    {
    case Identifier::s2cChunk:
    {
        mChunkCallback(ChunkClient::getFromFlatbuffers(s2c::GetChunk(data), *mWorld));
        break;
    }
    default:
        warningstream << "Unsupported game packet received: " << static_cast<int>(id);
    }
}
