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
    template <class T>
    using ServerHDC = std::unique_ptr<T>;
    using chunks_t = ChunkManager<ServerHDC>;
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

    // Alias declearations for chunk management
    auto getChunkCount() const { return mChunks.size(); }
    auto getReservedChunkCount() const { return mChunks.capacity(); }
    auto& getChunk(size_t index) { return mChunks[index]; }
    auto& getChunk(const Vec3i& chunkPos) { return mChunks[chunkPos]; }
    auto isChunkLoaded(const Vec3i& chunkPos) const noexcept { return mChunks.isLoaded(chunkPos); }
    auto deleteChunk(size_t index) { return mChunks.erase(index); }
    auto deleteChunk(const Vec3i& chunkPos) { return mChunks.erase(chunkPos); }
    static auto getChunkAxisPos(int pos) { return chunks_t::getAxisPos(pos); }
    static auto getChunkPos(const Vec3i& pos) { return chunks_t::getPos(pos); }
    static auto getBlockAxisPos(int pos) { return chunks_t::getBlockAxisPos(pos); }
    static auto getBlockPos(const Vec3i& pos) { return chunks_t::getBlockPos(pos); }
    auto getBlock(const Vec3i& pos) const { return mChunks.getBlock(pos); }
    auto& getBlock(const Vec3i& pos) { return mChunks.getBlock(pos); }
    void setBlock(const Vec3i& pos, BlockData block) const { mChunks.setBlock(pos, block); }
    auto getChunkIndex(const Vec3i& chunkPos) const { return mChunks.getIndex(chunkPos); }
    template <typename... ArgType, typename Func>
    void doIfChunkLoaded(const Vec3i& chunkPos, Func func, ArgType&&... args)
    {
        mChunks.doIfLoaded(chunkPos, func, std::forward<ArgType>(args)...);
    };

    // Add chunk
    virtual Chunk* addChunk(const Vec3i& chunkPos) = 0;

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
        : mName(name), mID(0),mPlugins(plugins), mBlocks(blocks), mDaylightBrightness(15), mChunks(1024)
    {
    }

    World(World&& rhs) noexcept
        : mName(std::move(rhs.mName)), mID(rhs.mID), mPlugins(rhs.mPlugins), mBlocks(rhs.mBlocks),
           mDaylightBrightness(rhs.mDaylightBrightness), mChunks(std::move(rhs.mChunks))
    {
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
    chunks_t mChunks;

    int mDaylightBrightness;

    // New pointer at mChunks[index]
    auto newChunk(size_t index, ServerHDC<Chunk>&& ptr) { return mChunks.insert(index, std::move(ptr)); }
};

#endif // !WORLD_H_
