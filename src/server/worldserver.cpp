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
    c->build(15);
    mChunks[index] = static_cast<Chunk*>(c);
    // TODO: Update chunk pointer cache
    // TODO: Update chunk pointer array
    // Return pointer
    return mChunks[index];
}

void WorldServer::loadUnloadChunks()
{
    for (int i = 0; i < mChunkLoadCount; i++)
    {
        // TODO: Try to read in file
        static_cast<ChunkServer*>(addChunk(mChunkLoadList[i]))->build(getDaylightBrightness());
    }
    mChunkLoadCount = 0;
    for (int i = 0; i < mChunkUnloadCount; i++)
    {
        // TODO: Save chunk
        deleteChunk(mChunkUnloadList[i].first->getPosition());
    }
    mChunkUnloadCount = 0;
}

void WorldServer::pendingLoadChunk(const Vec3i& chunkPos)
{
    if (mChunkLoadCount < MaxChunkLoadCount)
        mChunkLoadList[mChunkLoadCount++] = chunkPos;
}

void WorldServer::updateChunkLoadStatus()
{
    for (size_t i = 0; i < mChunkCount; ++i)
    {
        auto c = static_cast<ChunkServer*>(getChunkPointerArray().get(i));
        if (c)
        {
            c->decreaseStrongRef();
            c->decreaseWeakRef();
            if (c->checkReleaseable() && (mChunkUnloadCount < MaxChunkUnloadCount))
                mChunkUnloadList[mChunkUnloadCount++] = std::pair<Chunk *, int>(c, i);
        }
        loadUnloadChunks();
        infostream << "Chunk Release Triggered. Size is now:" << mChunkCount;
    }
}
