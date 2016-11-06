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

#include "worldserver.h"
#include "chunkserver.h"

Chunk* WorldServer::addChunk(const Vec3i& chunkPos)
{
    size_t index = getChunkIndex(chunkPos);
    if (index < mChunkCount && mChunks[index]->getPosition() == chunkPos)
    {
        assert(false);
        return nullptr;
    }
    newChunkPtr(index);
    ChunkServer* c = new ChunkServer(chunkPos);
    c->build(15);//getDaylightBrightness());
    mChunks[index] = static_cast<Chunk*>(c);
    // TODO: Update chunk pointer cache
    // TODO: Update chunk pointer array
    // NOTE: The load/save process should be implemented in the ctor/dtor of ChunkServer
    return mChunks[index];
}

void WorldServer::updateChunkLoadStatus()
{
    for (size_t i = 0; i < mChunkCount; ++i)
    {
        auto c = static_cast<ChunkServer*>(getChunkPtr(i));
        if (c)
        {
            c->decreaseWeakRef();
            if (c->checkReleaseable())
                deleteChunk(c->getPosition());
            else
                ++i;
        }
    }
}
