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
#include "common/nwconcepts.hpp"

class PluginManager;

class World : public NonCopyable
{
public:
    World(const std::string& name, const PluginManager& plugins, const BlockManager& blocks)
        : mName(name), mID(0), mPlugins(plugins), mBlocks(blocks), mDaylightBrightness(15), mChunks(1024)
    {
    }

    World(World&& rhs) noexcept
        : mName(std::move(rhs.mName)), mID(rhs.mID), mPlugins(rhs.mPlugins), mBlocks(rhs.mBlocks),
        mDaylightBrightness(rhs.mDaylightBrightness), mChunks(std::move(rhs.mChunks))
    {
    }
    virtual ~World() = default;

    ////////////////////////////////////////
    // World Properties
    ////////////////////////////////////////
    const std::string& getWorldName() const noexcept { return mName; }
    size_t getWorldID() const noexcept { return mID; }
    int getDaylightBrightness() const noexcept { return mDaylightBrightness; }

    ////////////////////////////////////////
    // Chunk Management
    ////////////////////////////////////////
    using ChunkIterator = ChunkManager::iterator;
    using ChunkReference = ChunkManager::reference;
    // Raw Access
    ChunkManager& getChunks() noexcept { return mChunks; }
    const ChunkManager& getChunks() const noexcept { return mChunks; }
    // Alias declearations for Chunk management
    size_t getChunkCount() const { return mChunks.size(); }
    size_t getReservedChunkCount() const { return mChunks.capacity(); }
    ChunkReference getChunk(size_t index) { return mChunks[index]; }
    ChunkReference getChunk(const Vec3i& ChunkPos) { return mChunks[ChunkPos]; }
    bool isChunkLoaded(const Vec3i& ChunkPos) const noexcept { return mChunks.isLoaded(ChunkPos); }
    ChunkIterator deleteChunk(size_t index) { return mChunks.erase(index); }
    ChunkIterator deleteChunk(const Vec3i& ChunkPos) { return mChunks.erase(ChunkPos); }
    static int getChunkAxisPos(int pos) { return ChunkManager::getAxisPos(pos); }
    static Vec3i getChunkPos(const Vec3i& pos) { return ChunkManager::getPos(pos); }
    static int getBlockAxisPos(int pos) { return ChunkManager::getBlockAxisPos(pos); }
    static Vec3i getBlockPos(const Vec3i& pos) { return ChunkManager::getBlockPos(pos); }
    BlockData getBlock(const Vec3i& pos) const { return mChunks.getBlock(pos); }
    BlockData& getBlock(const Vec3i& pos) { return mChunks.getBlock(pos); }
    void setBlock(const Vec3i& pos, BlockData block) const { mChunks.setBlock(pos, block); }
    size_t getChunkIndex(const Vec3i& ChunkPos) const { return mChunks.getIndex(ChunkPos); }
    auto insertChunk(size_t index, ChunkHDC<Chunk>&& ptr) { return mChunks.insert(index, std::move(ptr)); }
    auto insertChunk(const Vec3i& pos, ChunkHDC<Chunk>&& ptr) { return mChunks.insert(pos, std::move(ptr)); }
    auto resetChunk(size_t index, Chunk* ptr) { return mChunks.reset(index, ptr); }
    auto resetChunk(const Vec3i& pos, Chunk* ptr) { return mChunks.reset(pos, ptr); }
    template <typename... ArgType, typename Func>
    void doIfChunkLoaded(const Vec3i& ChunkPos, Func func, ArgType&&... args)
    {
        mChunks.doIfLoaded(ChunkPos, func, std::forward<ArgType>(args)...);
    };

    // Add Chunk
    virtual Chunk* addChunk(const Vec3i& chunkPos, ChunkOnReleaseBehavior::Behavior behv = ChunkOnReleaseBehavior::Behavior::Release) 
    {
        return insertChunk(chunkPos, std::move(ChunkHDC<Chunk>(new Chunk(chunkPos, *this), ChunkOnReleaseBehavior(behv))))->get();
    }


    ////////////////////////////////////////
    // BlockType Management
    ////////////////////////////////////////
    const BlockManager& getBlockTypes() const { return mBlocks; }
    const BlockType& getType(int id) const { return mBlocks[id]; }

    NWCOREAPI std::vector<AABB> getHitboxes(const AABB& range) const;

    // Main update
    NWCOREAPI void update();

    NWCOREAPI void updateChunkLoadStatus();

protected:

    // World name
    std::string mName;
    // World ID
    size_t mID;
    static size_t IDCount;
    // Loaded plugins
    const PluginManager& mPlugins;
    // Loaded blocks
    const BlockManager& mBlocks;
    // All Chunks (Chunk array)
    ChunkManager mChunks;

    int mDaylightBrightness;
};

#endif // !WORLD_H_
