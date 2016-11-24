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

#ifndef WORLD_H_
#define WORLD_H_

#include <algorithm>
#include <string>
#include <cstdlib>
#include <memory>
#include <vector>
#include "common/aabb.h"
#include "nwchunk.h"
#include "nwblock.h"
#include "common/nwexport.h"

class PluginManager;

class NWCOREAPI World
{
public:
    World(const World&) = delete;
    World& operator=(const World&) = delete;
	virtual ~World() = default;

    // Get world name
    const std::string& getWorldName() const
    {
        return mName;
    }

    // Get world id
    size_t getWorldID() const
    {
        return mID;
    }

    // Get chunk count
    size_t getChunkCount() const
    {
        return mChunks.size();
    }

    // Get reserved chunk count
    size_t getReservedChunkCount() const
    {
        return mChunks.capacity();
    }

    // Get chunk pointer by index
    Chunk* getChunkPtr(size_t index) const
    {
        Assert(index < getChunkCount());
        return mChunks[index].get();
    }

    // Get chunk pointer by chunk coordinates
    // Optimized for clustered search
    Chunk* getChunkPtr(const Vec3i& chunkPos) const
    {
        // TODO: Try chunk pointer cache
        Chunk* res = getChunkPtrNonclustered(chunkPos);
        // TODO: Update chunk pointer array
        // TODO: Update chunk pointer cache
        return res;
    }

    // Non-clustered & thread-safe version of getChunkPtr()
    // Will not update CPA and CPC
    Chunk* getChunkPtrNonclustered(const Vec3i& chunkPos) const
    {
        // TODO: Try chunk pointer array
        size_t index = getChunkIndex(chunkPos);
        if (index >= getChunkCount() || mChunks[index]->getPosition() != chunkPos)
            return nullptr;
        return mChunks[index].get();
    }

    bool isChunkLoaded(const Vec3i& chunkPos) const
    {
        return getChunkPtr(chunkPos) != nullptr;
    }

    // Add chunk
    virtual Chunk* addChunk(const Vec3i& chunkPos) = 0;

    // Delete chunk
    int deleteChunk(size_t index);
    int deleteChunk(const Vec3i& chunkPos);

#ifdef NEWORLD_COMPILER_RSHIFT_ARITH
    // Convert world position to chunk coordinate (one axis)
    static int getChunkPos(int pos)
    {
        return pos >> Chunk::SizeLog2();
    }
#else
    // Convert world position to chunk coordinate (one axis)
    static int getChunkPos(int pos)
    {
        if (pos >= 0)
            return pos / Chunk::Size();
        return (pos - Chunk::Size() + 1) / Chunk::Size();
    }
#endif

    // Convert world position to chunk coordinate (all axes)
    static Vec3i getChunkPos(const Vec3i& pos)
    {
        return Vec3i(getChunkPos(pos.x), getChunkPos(pos.y), getChunkPos(pos.z));
    }

    // Convert world position to block coordinate in chunk (one axis)
    static int getBlockPos(int pos)
    {
        return pos & (Chunk::Size() - 1);
    }

    // Convert world position to block coordinate in chunk (all axes)
    static Vec3i getBlockPos(const Vec3i& pos)
    {
        return Vec3i(getBlockPos(pos.x), getBlockPos(pos.y), getBlockPos(pos.z));
    }

    // Get block data
    BlockData getBlock(const Vec3i& pos) const
    {
        Chunk* chunk = getChunkPtr(getChunkPos(pos));
        Assert(chunk != nullptr);
        return chunk->getBlock(getBlockPos(pos));
    }

    // Get block reference
    BlockData& getBlock(const Vec3i& pos)
    {
        Chunk* chunk = getChunkPtr(getChunkPos(pos));
        Assert(chunk != nullptr);
        return chunk->getBlock(getBlockPos(pos));
    }

    // Set block data
    void setBlock(const Vec3i& pos, BlockData block) const
    {
        Chunk* chunk = getChunkPtr(getChunkPos(pos));
        Assert(chunk != nullptr);
        chunk->setBlock(getBlockPos(pos), block);
    }

    int getDaylightBrightness() const
    {
        return mDaylightBrightness;
    }

    const BlockManager& getBlockTypes() const
    {
        return mBlocks;
    }

    const BlockType& getType(int id) const
    {
        return mBlocks[id];
    }

    std::vector<AABB> getHitboxes(const AABB& range) const;

    // Main update
    void update();

protected:
    World(const std::string& name, const PluginManager& plugins, const BlockManager& blocks)
        : mName(name), mID(0),mPlugins(plugins), mBlocks(blocks), mDaylightBrightness(15)
    {
		mChunks.reserve(1024);
    }

    World(World&& rhs) noexcept
        : mName(std::move(rhs.mName)), mID(rhs.mID), mPlugins(rhs.mPlugins), mBlocks(rhs.mBlocks),
           mDaylightBrightness(rhs.mDaylightBrightness)
    {
        std::swap(mChunks, rhs.mChunks);
    }

    // World name
    std::string mName;
    // World ID
    size_t mID;
    static size_t IDCount;
    // Loaded plugins
    const PluginManager& mPlugins;
    // Loaded blocks
    const BlockManager& mBlocks;
    // All chunks (chunk array)
    std::vector<std::unique_ptr<Chunk>> mChunks;

    int mDaylightBrightness;

    // New pointer at mChunks[index]
    void newChunkPtr(size_t index)
    {
		mChunks.insert(mChunks.begin() + index, nullptr);
    }

    // Erase pointer at mChunks[index]
    void eraseChunkPtr(size_t index)
    {
		mChunks.erase(mChunks.begin() + index);
    }

    // Search chunk index, or the index the chunk should insert into
    size_t getChunkIndex(const Vec3i& chunkPos) const;

};

#endif // !WORLD_H_
