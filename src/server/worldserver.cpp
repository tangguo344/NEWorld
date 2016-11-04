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
    mChunks[index] = static_cast<Chunk*>(new ChunkServer(chunkPos));
    // TODO: Update chunk pointer cache
    // TODO: Update chunk pointer array
    // Return pointer
    return mChunks[index];
}

void WorldServer::sortChunkLoadUnloadList(const Vec3i& centerPos)
{
    //Vec3i centerCPos;
    int pl = 0, pu = 0;
    int distsqr, first, middle, last;

    // centerPos to chunk coords
    //centerCPos = mWorld->getChunkPos(centerPos);

    for (size_t ci = 0; ci < getChunkCount(); ci++)
    {
        Vec3i curPos = getChunkPtr(ci)->getPosition();
        // Get chunk center pos
        curPos.for_each([](int& x)
        {
            x = x * ChunkSize + ChunkSize / 2 - 1;
        });

        // Out of load range, pending to unload
        if (centerPos.chebyshevDistance(curPos) > mLoadRange)
        {
            // Distance from centerPos
            distsqr = (curPos - centerPos).lengthSqr();

            // Binary search in unload list
            first = 0;
            last = pl - 1;
            while (first <= last)
            {
                middle = (first + last) / 2;
                if (distsqr > mChunkUnloadList[middle].second)
                    last = middle - 1;
                else
                    first = middle + 1;
            }

            // Not very far, don't unload now
            if (first > pl || first >= MaxChunkUnloadCount) continue;

            // Move elements to make place
            for (int j = MaxChunkUnloadCount - 1; j > first; j--)
                mChunkUnloadList[j] = mChunkUnloadList[j - 1];

            // Insert into list
            mChunkUnloadList[first] = { getChunkPtr(ci), distsqr };

            // Add counter
            if (pl < MaxChunkUnloadCount) pl++;
        }
    }
    mChunkUnloadCount = pl;

    for (int x = centerPos.x - mLoadRange; x <= centerPos.x + mLoadRange; x++)
        for (int y = centerPos.y - mLoadRange; y <= centerPos.y + mLoadRange; y++)
            for (int z = centerPos.z - mLoadRange; z <= centerPos.z + mLoadRange; z++)
                // In load range, pending to load
                if (!isChunkLoaded(Vec3i(x, y, z))) // if (mCpa.get(Vec3i(x, y, z)) == nullptr)
                {
                    Vec3i curPos(x, y, z);
                    // Get chunk center pos
                    curPos.for_each([](int& x)
                    {
                        x = x * ChunkSize + ChunkSize / 2 - 1;
                    });

                    // Distance from centerPos
                    distsqr = (curPos - centerPos).lengthSqr();

                    // Binary search in load list
                    first = 0;
                    last = pu - 1;
                    while (first <= last)
                    {
                        middle = (first + last) >> 1;
                        if (distsqr < mChunkLoadList[middle].second)
                            last = middle - 1;
                        else
                            first = middle + 1;
                    }

                    // Not very near, don't load now
                    if (first > pu || first >= MaxChunkLoadCount) continue;

                    // Move elements to make place
                    for (int j = MaxChunkLoadCount - 1; j > first; j--)
                        mChunkLoadList[j] = mChunkLoadList[j - 1];

                    // Insert into list
                    mChunkLoadList[first] = { Vec3i(x, y, z),distsqr };

                    // Add counter
                    if (pu < MaxChunkLoadCount) pu++;
                }
    mChunkLoadCount = pu;
}

void WorldServer::loadUnloadChunks()
{
    for (int i = 0; i < mChunkLoadCount; i++)
    {
        // TODO: Try to read in file
        static_cast<ChunkServer*>(addChunk(mChunkLoadList[i].first))->build(getDaylightBrightness());
    }
    for (int i = 0; i < mChunkUnloadCount; i++)
    {
        // TODO: Save chunk
        deleteChunk(mChunkUnloadList[i].first->getPosition());
    }
}
