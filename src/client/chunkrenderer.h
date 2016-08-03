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

#ifndef CHUNKRENDERER_H_
#define CHUNKRENDERER_H_

#include <boost/core/noncopyable.hpp>
#include "../shared/chunk.h"
#include "../shared/world.h"
#include "renderer.h"

class ChunkRenderer
    :boost::noncopyable
{
public:
    ChunkRenderer(World& world, Chunk& chunk) : m_world(world), m_chunk(chunk)
    {
    }

    // Build VBO
    void buildVertexArray();

    // Draw call
    void render() const
    {
        m_buffer.render();
    }

private:
    // Target world
    World& m_world;
    // Target chunk
    Chunk& m_chunk;
    // Vertex buffer object
    VertexBuffer m_buffer;
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

#endif // !CHUNKRENDERER_H_
