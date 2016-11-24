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
#include <world/nwchunk.h>

Chunk* WorldServer::addChunk(const Vec3i& chunkPos)
{
    size_t index = getChunkIndex(chunkPos);
    if (index < getChunkCount() && mChunks[index]->getPosition() == chunkPos)
    {
        Assert(false);
        return nullptr;
    }
    newChunkPtr(index);
    mChunks[index].reset(new Chunk(chunkPos));
	mChunks[index]->build(15);//getDaylightBrightness());
    return mChunks[index].get();
}

void WorldServer::updateChunkLoadStatus()
{
    for (size_t i = 0; i < getChunkCount(); ++i)
    {
        auto c = getChunkPtr(i);
        if (c)
        {
            c->decreaseWeakRef();
            if (c->checkReleaseable())
                deleteChunk(i);
            else
                ++i;
        }
    }
}
