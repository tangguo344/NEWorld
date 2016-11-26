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

class NWCOREAPI World : public nwNonCopyable
{
public:
    template <class T>
    using ServerHDC = std::unique_ptr<T>;

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
    using chunks_t = ChunkManager<ServerHDC>;
	using chunkIterator = chunks_t::iterator;
	using chunkReference = chunks_t::reference;
	// Raw Access
	chunks_t& getChunks() noexcept { return mChunks; }
	const chunks_t& getChunks() const noexcept { return mChunks; }
    // Alias declearations for chunk management
    size_t getChunkCount() const { return mChunks.size(); }
	size_t getReservedChunkCount() const { return mChunks.capacity(); }
	chunkReference getChunk(size_t index) { return mChunks[index]; }
	chunkReference getChunk(const Vec3i& chunkPos) { return mChunks[chunkPos]; }
    bool isChunkLoaded(const Vec3i& chunkPos) const noexcept { return mChunks.isLoaded(chunkPos); }
	chunkIterator deleteChunk(size_t index) { return mChunks.erase(index); }
	chunkIterator deleteChunk(const Vec3i& chunkPos) { return mChunks.erase(chunkPos); }
    static int getChunkAxisPos(int pos) { return chunks_t::getAxisPos(pos); }
    static Vec3i getChunkPos(const Vec3i& pos) { return chunks_t::getPos(pos); }
    static int getBlockAxisPos(int pos) { return chunks_t::getBlockAxisPos(pos); }
    static Vec3i getBlockPos(const Vec3i& pos) { return chunks_t::getBlockPos(pos); }
    BlockData getBlock(const Vec3i& pos) const { return mChunks.getBlock(pos); }
    BlockData& getBlock(const Vec3i& pos) { return mChunks.getBlock(pos); }
    void setBlock(const Vec3i& pos, BlockData block) const { mChunks.setBlock(pos, block); }
    size_t getChunkIndex(const Vec3i& chunkPos) const { return mChunks.getIndex(chunkPos); }
	auto insertChunk(size_t index, ServerHDC<Chunk>&& ptr) { return mChunks.insert(index, std::move(ptr)); }
	auto insertChunk(const Vec3i& pos, ServerHDC<Chunk>&& ptr) { return mChunks.insert(pos, std::move(ptr)); }
    template <typename... ArgType, typename Func>
    void doIfChunkLoaded(const Vec3i& chunkPos, Func func, ArgType&&... args)
    {
        mChunks.doIfLoaded(chunkPos, func, std::forward<ArgType>(args)...);
    };

    // Add chunk
    virtual Chunk* addChunk(const Vec3i& chunkPos) = 0;

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
};

#endif // !WORLD_H_
