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

#ifndef CHUNKCLIENT_H_
#define CHUNKCLIENT_H_

#include <boost/core/noncopyable.hpp>
#include <chunk.h>
#include <world.h>
#include "renderer.h"

class ChunkClient : public Chunk
{
public:
    ChunkClient(const Vec3i& position, World& world) : Chunk(position), m_world(world)
    {
    }

    // Is render built
    bool isRenderBuilt()
    {
        return m_renderBuilt;
    }

    // Build VBO
    void buildVertexArray();

    // Destroy VBO
    void destroyVertexArray()
    {
        m_buffer.destroy();
        m_renderBuilt = false;
    }

    // Draw call
    void render() const
    {
        m_buffer.render();
    }

private:
    // Target world
    World& m_world;
    // Vertex buffer object
    VertexBuffer m_buffer;
    // Render built
    bool m_renderBuilt = false;

    // Vertex array
    static VertexArray va;
    // Merge face rendering
    static bool mergeFace;

    bool adjacentTest(BlockData a, BlockData b) const
    {
        if (a.getID() == 0) return false;
        if (m_world.getBlockTypes().getType(b.getID()).isOpaque()) return false;
        return true;
    }
};

#endif // !CHUNKCLIENT_H_
