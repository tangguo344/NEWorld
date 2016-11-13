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

#include <atomic>
#include <memory>
#include <chunk.h>
#include <world.h>
#include "renderer.h"
#include "blockrenderer.h"

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
    }

    ~ChunkClient()
    {
        if (mRenderBuilt)
            destroyVertexArray();
    }

    // Is render built
    bool isRenderBuilt() const
    {
        return mRenderBuilt;
    }

    bool needRenderRebuilt() const
    {
        return (!isRenderBuilt()) || isUpdated();
    }

    // Build VBO
    void buildVertexArray();

    // Destroy VBO
    void destroyVertexArray()
    {
        mBuffer.destroy();
        mBufferTrans.destroy();
        mRenderBuilt = false;
    }

    // Draw call
    void render() const
    {
        mBuffer.render();
        mBufferTrans.render();
    }

    static Chunk* getFromFlatbuffers(const s2c::Chunk* fbChunk, WorldClient& worlds);

    // Render default block
    void renderBlock(BlockTexCoord coord[], const Vec3i& pos);

private:
    // Target world
    World& mWorld;
    // Vertex buffer object
    VertexBuffer mBuffer, mBufferTrans;
    // Render built
    bool mRenderBuilt = false;
    // Vertex array
    static VertexArray va0, va1;
    // Merge face rendering
    static bool mergeFace;

    bool adjacentTest(BlockData a, BlockData b) const
    {
        return a.getID() != 0 && !mWorld.getType(b.getID()).isOpaque() && !(a.getID() == b.getID());
    }
};


#endif // !CHUNKCLIENT_H_
