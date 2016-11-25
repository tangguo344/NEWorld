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

#include <atomic>
#include <chrono>
#include <memory>
#include <vector>
#include "common/vec3.h"
#include "common/debug.h"
#include "common/nwexport.h"
#include "nwblock.h"

using ChunkGenerator = void NWAPICALL(const Vec3i*, BlockData*, int);


class NWCOREAPI Chunk
{
public:
	// Chunk size
    static bool ChunkGeneratorLoaded;
    static ChunkGenerator *ChunkGen;
	static constexpr int SizeLog2() { return 5; }
	static constexpr int Size(){ return 0b100000; };

    explicit Chunk(const Vec3i& position) : mPosition(position) {}
    virtual ~Chunk() {}

    // Get chunk position
    const Vec3i& getPosition() const
    {
        return mPosition;
    }

    // Get chunk updated flag
    bool isUpdated() const
    {
        return mUpdated;
    }

    // Set chunk updated flag
    void setUpdated(bool updated)
    {
        mUpdated = updated;
    }

    // Get block data in this chunk
    BlockData getBlock(const Vec3i& pos) const
    {
        Assert(pos.x >= 0 && pos.x < Size() && pos.y >= 0 && pos.y < Size() && pos.z >= 0 && pos.z < Size());
        return mBlocks[pos.x * Size() * Size() + pos.y * Size() + pos.z];
    }

    // Get block reference in this chunk
    BlockData& getBlock(const Vec3i& pos)
    {
        Assert(pos.x >= 0 && pos.x < Size() && pos.y >= 0 && pos.y < Size() && pos.z >= 0 && pos.z < Size());
        return mBlocks[pos.x * Size() * Size() + pos.y * Size() + pos.z];
    }

    // Get block pointer
    BlockData* getBlocks()
    {
        return mBlocks;
    }
    const BlockData* getBlocks() const
    {
        return mBlocks;
    }

    // Set block data in this chunk
    void setBlock(const Vec3i& pos, BlockData block)
    {
        Assert(pos.x >= 0 && pos.x < Size() && pos.y >= 0 && pos.y < Size() && pos.z >= 0 && pos.z < Size());
        mBlocks[pos.x * Size() * Size() + pos.y * Size() + pos.z] = block;
        mUpdated = true;
    }

    // Build chunk
    void build(int daylightBrightness);

    // Reference Counting
    void increaseWeakRef();
    void decreaseWeakRef();
    void increaseStrongRef();
    void decreaseStrongRef();
    bool checkReleaseable() const;

private:
    Vec3i mPosition;
    BlockData mBlocks[0b1'00000'00000'00000];
    bool mUpdated = false;
	// For Garbage Collection
	long long mReferenceCount;
	std::chrono::steady_clock::time_point mLastRequestTime;
    std::atomic<int> mRefrenceCount{0}, mWeakRefrenceCount{0};
};

template <template<typename>class prtT>
class ChunkManager
{
public:
	using data_t = prtT<Chunk>;
	using array_t = std::vector<data_t>;
	using iterator = array_t::iterator;
    using const_iterator = array_t::const_iterator;
    using reverse_iterator = array_t::reverse_iterator;
    using const_reverse_iterator = array_t::const_reverse_iterator;
    using reference = Chunk&;
    using const_reference = const reference;
    ChunkManager() = default;
    ChunkManager(size_t size) { mChunks.reserve(size); }
    ChunkManager(const ChunkManager&) = delete;
    ChunkManager(ChunkManager&& rhs) : mChunks(std::move(rhs.mChunks)) {}
    ~ChunkManager() = default;
    // Access and modifiers
    size_t size() const noexcept { return mChunks.size(); }
    size_t capacity() const noexcept { return mChunks.capacity(); }
    iterator begin() noexcept { return mChunks.begin(); }
    const_iterator begin() const noexcept { return mChunks.begin(); }
    iterator end() noexcept { return mChunks.end(); }
    const_iterator end() const noexcept { return mChunks.end(); }
    reverse_iteratorr rbegin() noexcept { return mChunks.rbegin(); }
    const_reverse_iterator rbegin() const noexcept { return mChunks.rbegin(); }
    reverse_iterator rend() noexcept { return mChunks.rend(); }
    const_reverse_iterator rend() const noexcept { return mChunks.rend(); }
    
    const_iterator cbegin() const noexcept { return mChunks.cbegin(); }
    const_iterator cend() const noexcept { return mChunks.cend(); }
    const_reverse_iterator crbegin() const noexcept { return mChunks.crbegin(); }
    const_reverse_iterator crend() const noexcept { return mChunks.crend(); }
    
    reference at(size_t index) { return *mChunks[index]; }
    const_reference at(size_t index) const { return *mChunks[index]; }
    reference at(const Vec3i& chunkPos) { return at(getIndex(chunkPos)); }
    const_reference at(const Vec3i& chunkPos) const { return at(getIndex(chunkPos)); }
    
    reference operator[](size_t id) { return at(id); }
    const_reference operator[](size_t id) const { return at(id); }
    reference operator[](const Vec3i& chunkPos) { return at(chunkPos); }
    const_reference operator[](const Vec3i& chunkPos) const { return at(chunkPos); }
    
    iterator insert(iterator it, data_t&& chunk) { return mChunks.insert(it, std::move(chunk)); }
    iterator insert(size_t id, data_t&& chunk) { return insert(mChunks.begin() + id, std::move(chunk)); }
    iterator insert(const Vec3i& chunkPos, data_t&& chunk) { return insert(getIndex(chunkPos), std::move(chunk)); }
  
    iterator erase(iterator it) { return mChunks.erase(it); }
    iterator erase(size_t id) { return erase(mChunks.begin() + id); }
    iterator erase(const Vec3i& chunkPos) { return erase(getIndex(chunkPos)); }
    
    // Chunk Only Functions
    size_t getIndex(const Vec3i& chunkPos) const
    {
        int first = 0, last = static_cast<int>(size()) - 1;
        while (first <= last)
        {
            int mid = (first + last) / 2;
            if (mChunks[mid]->getPosition() < pos)
                first = mid + 1;
            else
                last = mid - 1;
        }
        return first;
    }
    
    bool isLoaded(const Vec3i& chunkPos) const
    {
        size_t index = getIndex(chunkPos);
        return (index <= getCount() || mChunks[index]->getPosition() == chunkPos);
    }
    
    // Convert world position to chunk coordinate (one axis)
    static int getPos(int pos) noexcept
    {
#ifdef NEWORLD_COMPILER_RSHIFT_ARITH
        return pos >> Chunk::SizeLog2();
#else
        return ((pos >= 0) ? pos : (pos - Chunk::Size() + 1)) / Chunk::Size();
#endif
    }

    // Convert world position to chunk coordinate (all axes)
    static Vec3i getPos(const Vec3i& pos) noexcept
    {
        return Vec3i(getPos(pos.x), getPos(pos.y), getPos(pos.z));
    }

    // Convert world position to block coordinate in chunk (one axis)
    static int getBlockPos(int pos) noexcept
    {
        return pos & (Chunk::Size() - 1);
    }

    // Convert world position to block coordinate in chunk (all axes)
    static Vec3i getBlockPos(const Vec3i& pos) noexcept
    {
        return Vec3i(getBlockPos(pos.x), getBlockPos(pos.y), getBlockPos(pos.z));
    }

    // Get block data
    BlockData getBlock(const Vec3i& pos) const
    {
        return at(getPos(pos)).getBlock(getBlockPos(pos));
    }

    // Get block reference
    BlockData& getBlock(const Vec3i& pos)
    {
        return at(getPos(pos)).getBlock(getBlockPos(pos));
    }

    // Set block data
    void setBlock(const Vec3i& pos, BlockData block) const
    {
        at(getPos(pos)).setBlock(getBlockPos(pos), block);
    }
private:
    array_t mChunks;
};

#endif // !CHUNK_H_
