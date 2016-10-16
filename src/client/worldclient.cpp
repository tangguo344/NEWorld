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
    if (index < m_chunkCount && m_chunks[index]->getPosition() == chunkPos)
    {
        assert(false);
        return nullptr;
    }
    newChunkPtr(index);
    m_chunks[index] = static_cast<Chunk*>(new ChunkClient(chunkPos,*this));
    // TODO: Update chunk pointer cache
    // TODO: Update chunk pointer array
    // Return pointer
    return m_chunks[index];
}

void WorldClient::renderUpdate(const Vec3i& position)
{
    Vec3i chunkpos = getChunkPos(position);
    int pr = 0;

    for (size_t i = 0; i < getChunkCount(); i++)
    {
        ChunkClient* p = static_cast<ChunkClient*>(getChunkPtr(i));

        // In render range, pending to render
        if (chunkpos.chebyshevDistance(p->getPosition()) <= m_renderDist)
        {
            if (p->isRenderBuilt()/* || p->isEmpty()*/)
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
                if (distsqr < m_chunkRenderList[middle].second)
                    last = middle - 1;
                else
                    first = middle + 1;
            }

            // Not very near, don't render now
            if (first > pr || first >= MaxChunkRenderCount)
                continue;

            // Move elements to make prace
            for (int j = MaxChunkRenderCount - 1; j > first; j--)
                m_chunkRenderList[j] = m_chunkRenderList[j - 1];

            // Insert into list
            m_chunkRenderList[first] = { p, distsqr };

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
        m_chunkRenderList[i].first->buildVertexArray();
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
        if (chunkpos.chebyshevDistance(p->getPosition()) > m_renderDist)
            continue;
        Renderer::translate(Vec3f(p->getPosition() * ChunkSize));
        p->render();
        Renderer::translate(Vec3f(-p->getPosition() * ChunkSize));
        renderedChunks++;
    }
    return renderedChunks;
}

