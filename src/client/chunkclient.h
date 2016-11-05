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

#include <chunk.h>
#include <world.h>
#include "renderer.h"

class WorldClient;

namespace s2c
{
    struct Chunk;
}

class ChunkClient : public Chunk
{
public:
    ChunkClient(const Vec3i& position, World& world) : Chunk(position), mWorld(world)
    {
        // TEMP CODE
        // Generate terrain at client side to test rendering
        if (position.y < 0 && position.x >= -4 && position.x < 4 && position.z >= -4 && position.z < 4)
        {
            Vec3i::for_range(0, ChunkSize, [&](const Vec3i& curr)
            {
                setBlock(curr, BlockData(1, 0, 0));
            });
        }
        // END TEMP CODE
    }

    // Is render built
    bool isRenderBuilt() const
    {
        return mRenderBuilt;
    }

    // Build VBO
    void buildVertexArray();

    // Destroy VBO
    void destroyVertexArray()
    {
        mBuffer.destroy();
        mRenderBuilt = false;
    }

    // Draw call
    void render() const
    {
        mBuffer.render();
    }

    static Chunk* getFromFlatbuffers(const s2c::Chunk* fbChunk, WorldClient& worlds);


private:
    // Target world
    World& mWorld;
    // Vertex buffer object
    VertexBuffer mBuffer;
    // Render built
    bool mRenderBuilt = false;

    // Vertex array
    static VertexArray va;
    // Merge face rendering
    static bool mergeFace;

    bool adjacentTest(BlockData a, BlockData b) const
    {
        if (a.getID() == 0) return false;
        if (mWorld.getBlockTypes().getType(b.getID()).isOpaque()) return false;
        return true;
    }
};

#endif // !CHUNKCLIENT_H_
