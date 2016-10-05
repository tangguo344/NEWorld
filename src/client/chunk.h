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

#ifndef CHUNK_H_
#define CHUNK_H_

#include <boost/core/noncopyable.hpp>
#include <chunkbase.h>
#include <worldbase.h>
#include "renderer.h"

// Chunk (Client)
class Chunk
    : public ChunkBase, boost::noncopyable
{
public:
    Chunk(WorldBase& world, Vec3i& position): ChunkBase(position), m_world(world)
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
    // Current world
    WorldBase &m_world;
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

#endif // !CHUNK_H_
