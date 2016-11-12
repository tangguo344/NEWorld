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

#include "worldclient.h"
#include "gameconnection.h"

Chunk* WorldClient::addChunk(const Vec3i& chunkPos)
{
    size_t index = getChunkIndex(chunkPos);
    if (index < mChunkCount && mChunks[index]->getPosition() == chunkPos)
    {
        Assert(false);
        return nullptr;
    }
    newChunkPtr(index);
    mChunks[index] = static_cast<Chunk*>(new ChunkClient(chunkPos,*this));
    Assert(mChunks[index] != nullptr);
    // TODO: Update chunk pointer cache
    // TODO: Update chunk pointer array
    // Return pointer
    return mChunks[index];
}

void WorldClient::renderUpdate(const Vec3i& position)
{
    Vec3i chunkpos = getChunkPos(position);
    int pr = 0;

    for (size_t i = 0; i < getChunkCount(); i++)
    {
        ChunkClient* p = static_cast<ChunkClient*>(getChunkPtr(i));

        // In render range, pending to render
        if (chunkpos.chebyshevDistance(p->getPosition()) <= mRenderDist)
        {
            if (p->isRenderBuilt()/* || p->isEmpty()*/)
                continue;

            // Check neighbor chunks
            bool f = true;
            Vec3i::for_range(-1, 2, [&](Vec3i& neighbor)
            {
                if (neighbor == Vec3i(0, 0, 0))
                    return;
                if (!isChunkLoaded(p->getPosition() + neighbor))
                {
                    // Neighbor chunk not loaded
                    f = false;
                    neighbor = Vec3i(2, 2, 2); // Exit for_range loop
                }
            });
            if (!f)
                continue;

            // Get chunk center pos
            Vec3i curPos = p->getPosition();
            curPos.for_each([](int& x)
            {
                x = x * ChunkSize + ChunkSize / 2 - 1;
            });

            // Distance from center pos
            int distsqr = (curPos - position).lengthSqr();

            // Binary search in render list
            int first = 0, last = pr - 1;
            while (first <= last)
            {
                int middle = (first + last) / 2;
                if (distsqr < mChunkRenderList[middle].second)
                    last = middle - 1;
                else
                    first = middle + 1;
            }

            // Not very near, don't render now
            if (first > pr || first >= MaxChunkRenderCount)
                continue;

            // Move elements to make prace
            for (int j = MaxChunkRenderCount - 1; j > first; j--)
                mChunkRenderList[j] = mChunkRenderList[j - 1];

            // Insert into list
            mChunkRenderList[first] = { p, distsqr };

            // Add counter
            if (pr < MaxChunkRenderCount)
                pr++;
        }
        else
        {
            if (!p->isRenderBuilt())
                continue;

            p->destroyVertexArray();
        }
    }

    for (int i = 0; i < pr; i++)
        mChunkRenderList[i].first->buildVertexArray();

}

size_t WorldClient::render(const Vec3i& position) const
{
    Vec3i chunkpos = getChunkPos(position);
    size_t renderedChunks = 0;
    for (size_t i = 0; i < getChunkCount(); i++)
    {
        ChunkClient* p = static_cast<ChunkClient*>(getChunkPtr(i));
        if (!p->isRenderBuilt())
            continue;
        if (chunkpos.chebyshevDistance(p->getPosition()) > mRenderDist)
            continue;
        Renderer::translate(Vec3f(p->getPosition() * ChunkSize));
        p->render();
        Renderer::translate(Vec3f(-p->getPosition() * ChunkSize));
        renderedChunks++;
    }
    return renderedChunks;
}

void WorldClient::sortChunkLoadUnloadList(const Vec3i& centerPos)
{
    Vec3i centerCPos;
    int pl = 0, pu = 0;
    int distsqr, first, middle, last;

    // centerPos to chunk coords
    centerCPos = getChunkPos(centerPos);

    for (size_t ci = 0; ci < getChunkCount(); ci++)
    {
        Vec3i curPos = getChunkPtr(ci)->getPosition();

        // Out of load range, pending to unload
        if (centerCPos.chebyshevDistance(curPos) > mLoadRange)
        {
            // Get chunk center pos
            curPos.for_each([](int& x)
            {
                x = x * ChunkSize + ChunkSize / 2 - 1;
            });

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
            if (first > pl || first >= MaxChunkUnloadCount)
                continue;

            // Move elements to make place
            for (int j = MaxChunkUnloadCount - 1; j > first; j--)
                mChunkUnloadList[j] = mChunkUnloadList[j - 1];

            // Insert into list
            mChunkUnloadList[first] = { getChunkPtr(ci), distsqr };

            // Add counter
            if (pl < MaxChunkUnloadCount)
                pl++;
        }
    }
    mChunkUnloadCount = pl;

    for (int x = centerCPos.x - mLoadRange; x <= centerCPos.x + mLoadRange; x++)
        for (int y = centerCPos.y - mLoadRange; y <= centerCPos.y + mLoadRange; y++)
            for (int z = centerCPos.z - mLoadRange; z <= centerCPos.z + mLoadRange; z++)
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
                    if (first > pu || first >= MaxChunkLoadCount)
                        continue;

                    // Move elements to make place
                    for (int j = MaxChunkLoadCount - 1; j > first; j--)
                        mChunkLoadList[j] = mChunkLoadList[j - 1];

                    // Insert into list
                    mChunkLoadList[first] = { Vec3i(x, y, z), distsqr };

                    // Add counter
                    if (pu < MaxChunkLoadCount)
                        pu++;
                }
    mChunkLoadCount = pu;
}

void WorldClient::tryLoadChunks(GameConnection& conn)
{
    for (int i = 0; i < mChunkLoadCount; i++)
    {
        addChunk(mChunkLoadList[i].first);
        conn.getChunk(mID, mChunkLoadList[i].first);
    }
    for (int i = 0; i < mChunkUnloadCount; i++)
        deleteChunk(mChunkUnloadList[i].first->getPosition());
}


