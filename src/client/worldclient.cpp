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

Chunk* WorldClient::addChunk(const Vec3i& chunkPos)
{
    size_t index = getChunkIndex(chunkPos);
    if (index < mChunkCount && mChunks[index]->getPosition() == chunkPos)
    {
        assert(false);
        return nullptr;
    }
    newChunkPtr(index);
    mChunks[index] = static_cast<Chunk*>(new ChunkClient(chunkPos,*this));
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
            if (pr < MaxChunkRenderCount) pr++;
        }
        else
        {
            if (!p->isRenderBuilt())
                continue;

            p->destroyVertexArray();
        }
    }

    for (int i = 0; i < pr; i++)
    {
        mChunkRenderList[i].first->buildVertexArray();
    }

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
