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

#ifndef WORLDRENDERER_H_
#define WORLDRENDERER_H_

#include <world.h>
#include "chunkrenderer.h"

class WorldRenderer
{
public:
    WorldRenderer(World& world) : m_world(world)
    {
    }

    // Build/Destroy VBO
    void update();

    // Render all chunks
    void render() const
    {
        for(auto& chunkRenderer: m_chunkRenderers)
            chunkRenderer.render();
    }

private:
    // Target world
    World& m_world;
    vector<ChunkRenderer> m_chunkRenderers;
};

#endif // !WORLDRENDERER_H_
