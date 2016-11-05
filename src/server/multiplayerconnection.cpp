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

void MultiplayerConnection::sendChunk(Chunk* chunk)
{
    auto pos = chunk->getPosition().conv<s2c::Vec3>();
    std::vector<int> blocks;
    for (auto i = 0; i < ChunkSize*ChunkSize*ChunkSize; ++i)
        blocks.push_back(chunk->getBlocks()[i].getData()); //嘛，有空再优化吧
    auto c = s2c::CreateChunkDirect(mFbb, &pos, &blocks);
    s2c::FinishChunkBuffer(mFbb, c);
    mConn.send(mFbb, c, PacketPriority::MEDIUM_PRIORITY, PacketReliability::RELIABLE);
}

void MultiplayerConnection::handleReceivedData(Identifier id, unsigned char* data)
{
    switch (id)
    {
    case Identifier::c2sLogin:
    {
        auto login = c2s::GetLogin(data);
        bool success = true;
        std::string reason = "";
        if (login->versionId() != NEWorldVersion)
        {
            success = false;
            reason = "Mismatched version. Server:" + std::to_string(NEWorldVersion) + " Player:" + std::to_string(login->versionId());
        }

        // TODO: validate password

        if (success)
            infostream << "Player " << login->username()->str() << " login!";
        else
            warningstream << "Player " << login->username()->str() << " failed to login: " << reason;
        break;
    }
    case Identifier::c2sRequestChunk:
    {
        debugstream << "It seems that someone need a chunk!";
        auto req = c2s::GetRequestChunk(data);
        auto chunk = mWorlds.getWorld(req->worldID())->addChunk({ req->x(), req->y(), req->z() });
        sendChunk(chunk);
        debugstream << "Do not worry! I have sent a chunk for you!";
        break;
    }
    default:
        warningstream << "Unsupported game packet received: " << static_cast<int>(id);
    }
}
