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
    if (index < m_chunkCount && m_chunks[index]->getPosition() == chunkPos)
    {
        assert(false);
        return nullptr;
    }
    newChunkPtr(index);
    m_chunks[index] = static_cast<Chunk*>(new ChunkServer(chunkPos));
    // TODO: Update chunk pointer cache
    // TODO: Update chunk pointer array
    // Return pointer
    return m_chunks[index];
}

void WorldServer::sortChunkLoadUnloadList(const Vec3i& centerPos)
{
    //Vec3i centerCPos;
    int pl = 0, pu = 0;
    int distsqr, first, middle, last;

    // centerPos to chunk coords
    //centerCPos = m_world->getChunkPos(centerPos);

    for (size_t ci = 0; ci < getChunkCount(); ci++)
    {
        Vec3i curPos = getChunkPtr(ci)->getPosition();
        // Get chunk center pos
        curPos.for_each([](int& x)
        {
            x = x * ChunkSize + ChunkSize / 2 - 1;
        });

        // Out of load range, pending to unload
        if (centerPos.chebyshevDistance(curPos) > m_loadRange)
        {
            // Distance from centerPos
            distsqr = (curPos - centerPos).lengthSqr();

            // Binary search in unload list
            first = 0;
            last = pl - 1;
            while (first <= last)
            {
                middle = (first + last) / 2;
                if (distsqr > m_chunkUnloadList[middle].second)
                    last = middle - 1;
                else
                    first = middle + 1;
            }

            // Not very far, don't unload now
            if (first > pl || first >= MaxChunkUnloadCount) continue;

            // Move elements to make place
            for (int j = MaxChunkUnloadCount - 1; j > first; j--)
                m_chunkUnloadList[j] = m_chunkUnloadList[j - 1];

            // Insert into list
            m_chunkUnloadList[first] = { getChunkPtr(ci), distsqr };

            // Add counter
            if (pl < MaxChunkUnloadCount) pl++;
        }
    }
    m_chunkUnloadCount = pl;

    for (int x = centerPos.x - m_loadRange; x <= centerPos.x + m_loadRange; x++)
        for (int y = centerPos.y - m_loadRange; y <= centerPos.y + m_loadRange; y++)
            for (int z = centerPos.z - m_loadRange; z <= centerPos.z + m_loadRange; z++)
                // In load range, pending to load
                if (!isChunkLoaded(Vec3i(x, y, z))) // if (m_cpa.get(Vec3i(x, y, z)) == nullptr)
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
                        if (distsqr < m_chunkLoadList[middle].second)
                            last = middle - 1;
                        else
                            first = middle + 1;
                    }

                    // Not very near, don't load now
                    if (first > pu || first >= MaxChunkLoadCount) continue;

                    // Move elements to make place
                    for (int j = MaxChunkLoadCount - 1; j > first; j--)
                        m_chunkLoadList[j] = m_chunkLoadList[j - 1];

                    // Insert into list
                    m_chunkLoadList[first] = { Vec3i(x, y, z),distsqr };

                    // Add counter
                    if (pu < MaxChunkLoadCount) pu++;
                }
    m_chunkLoadCount = pu;
}

void WorldServer::loadUnloadChunks()
{
    for (int i = 0; i < m_chunkLoadCount; i++)
    {
        // TODO: Try to read in file
        static_cast<ChunkServer*>(addChunk(m_chunkLoadList[i].first))->build(getDaylightBrightness());
    }
    for (int i = 0; i < m_chunkUnloadCount; i++)
    {
        // TODO: Save chunk
        deleteChunk(m_chunkUnloadList[i].first->getPosition());
    }
}
