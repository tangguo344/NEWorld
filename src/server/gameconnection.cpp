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
#include "chunkserver.h"
#include <nwsafety.hpp>
#include <vec3.h>
#include <flatfactory.h>

void MultiplayerConnection::sendChunk(Chunk* chunk)
{
    auto pos = chunk->getPosition().conv<s2c::Vec3>();
    // Be careful!
    Assert(sizeof(BlockData) == sizeof(uint32_t));
    std::vector<int> blocks(reinterpret_cast<uint32_t*>(chunk->getBlocks()),
                            reinterpret_cast<uint32_t*>(chunk->getBlocks()) + Chunk::Size*Chunk::Size*Chunk::Size);
    mConn.send<s2c::Chunk>(FlatFactory::s2c::Chunk(pos,blocks), PacketPriority::MEDIUM_PRIORITY, PacketReliability::RELIABLE);
}

void MultiplayerConnection::handleReceivedData(Identifier id, unsigned char* data, size_t len)
{
    flatbuffers::Verifier v(data,len);
    switch (id)
    {
    case Identifier::c2sLogin:
    {
        if(!c2s::VerifyLoginBuffer(v)) break;
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
        if(!c2s::VerifyRequestChunkBuffer(v)) break;
        auto req = c2s::GetRequestChunk(data);
        World *world = mWorlds.getWorld(0);// TODO:Current world of player.
        Chunk *chunk = world->getChunkPtr({req->x(), req->y(), req->z()});
        if (chunk == nullptr) chunk = world->addChunk({req->x(), req->y(), req->z()});
        static_cast<ChunkServer*>(chunk)->increaseWeakRef();
        sendChunk(chunk);
        break;
    }
    default:
        warningstream << "Unsupported game packet received: " << static_cast<int>(id);
    }
}
