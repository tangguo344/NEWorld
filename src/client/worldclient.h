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

#ifndef WORLDCLIENT_H_
#define WORLDCLIENT_H_

#include <world.h>
#include "chunkclient.h"

const int MaxChunkRenderCount = 4;

class WorldClient : public World
{
public:
    WorldClient(const std::string& name, const PluginManager& plugins, const BlockManager& blocks)
        : World(name, plugins, blocks)
    {
    }

    WorldClient(World&& world) : World(std::move(world))
    {
    }

    Chunk* addChunk(const Vec3i& chunkPos) override;

    void setRenderDistance(int x)
    {
        m_renderDist = x;
    }

    // Build/Destroy VBO
    void renderUpdate(const Vec3i& position);

    // Render all chunks
    size_t render(const Vec3i& position) const;

private:
    // Render distance
    int m_renderDist = 0;
    // Render build list
    std::pair<ChunkClient*, int> m_chunkRenderList[MaxChunkRenderCount];

};

#endif // !WORLDCLIENT_H_
