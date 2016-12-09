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

#include <mutex>
#include <atomic>
#include <chrono>
#include <memory>
#include <vector>
#include "common/vec3.h"
#include "common/debug.h"
#include "common/nwexport.h"
#include "common/nwconcepts.hpp"
#include "nwblock.h"

using ChunkGenerator = void NWAPICALL(const Vec3i*, BlockData*, int);

class NWCOREAPI Chunk : public NonCopyable
{
public:
    // Chunk size
    static bool ChunkGeneratorLoaded;
    static ChunkGenerator *ChunkGen;
    static constexpr int SizeLog2() { return 5; }
    static constexpr int Size(){ return 0b100000; };

    explicit Chunk(const Vec3i& position, class World& world);
    virtual ~Chunk() {}

    // Get chunk position
    const Vec3i& getPosition()
    {
        return mPosition;
    }

    // Get chunk updated flag
    bool isUpdated() const
    {
        return mUpdated;
    }

    bool isModified() const
    {
        return mModified;
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
    void markRequest() noexcept { std::unique_lock<std::mutex> lock(mMutex); mLastRequestTime = std::chrono::steady_clock::now(); }
    void increaseRef() noexcept { std::unique_lock<std::mutex> lock(mMutex); ++mReferenceCount; }
    void decreaseRef() noexcept { std::unique_lock<std::mutex> lock(mMutex); --mReferenceCount; }
    bool checkReleaseable() noexcept
    {
        std::unique_lock<std::mutex> lock(mMutex);
        using namespace std::chrono;
        return (((steady_clock::now() - mLastRequestTime) > 10s) && mReferenceCount <= 0);
    }

    World* getWorld() noexcept { return mWorld; }
protected:
    class World* mWorld;
private:
    Vec3i mPosition;
    std::mutex mMutex;
    BlockData mBlocks[0b1000000000000000];
    bool mUpdated = false, mModified = false;
    // For Garbage Collection
    int mReferenceCount{0};
    std::chrono::steady_clock::time_point mLastRequestTime;
};


struct NWCOREAPI ChunkOnReleaseBehavior
{
    enum class Behavior : size_t
    {
        Release, DeReference
    } status;
    void operator()(Chunk* target)
    {
        switch (status)
        {
        case ChunkOnReleaseBehavior::Behavior::Release:
            delete target;
            break;
        case ChunkOnReleaseBehavior::Behavior::DeReference:
            target->decreaseRef();
            break;
        }
    }
    ChunkOnReleaseBehavior(Behavior b) : status(b) {}
};

//I Dont Know Whats The Best Way...
template <class T>
using ChunkHDC = std::unique_ptr<T, ChunkOnReleaseBehavior>;

template <template<typename>class prtT>
class ChunkManagerBase: public NonCopyable
{
public:
    using data_t = prtT<Chunk>;
    using array_t = std::vector<data_t>;
    using iterator = typename array_t::iterator;
    using const_iterator = typename array_t::const_iterator;
    using reverse_iterator = typename array_t::reverse_iterator;
    using const_reverse_iterator = typename array_t::const_reverse_iterator;
    using reference = Chunk&;
    using const_reference = const Chunk&;
    ChunkManagerBase() = default;
    ChunkManagerBase(size_t size) { mChunks.reserve(size); }
    ChunkManagerBase(ChunkManagerBase&& rhs) : mChunks(std::move(rhs.mChunks)) {}
    ~ChunkManagerBase() = default;
    // Access and modifiers
    size_t size() const noexcept { return mChunks.size(); }
    size_t capacity() const noexcept { return mChunks.capacity(); }
    iterator begin() noexcept { return mChunks.begin(); }
    const_iterator begin() const noexcept { return mChunks.begin(); }
    iterator end() noexcept { return mChunks.end(); }
    const_iterator end() const noexcept { return mChunks.end(); }
    reverse_iterator rbegin() noexcept { return mChunks.rbegin(); }
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
    
    iterator insert(iterator it, data_t&& chunk) { std::unique_lock<std::mutex> lock(mMutex); return mChunks.insert(it, std::move(chunk)); }
    iterator insert(size_t id, data_t&& chunk) { return insert(mChunks.begin() + id, std::move(chunk)); }
    iterator insert(const Vec3i& chunkPos, data_t&& chunk) { return insert(getIndex(chunkPos), std::move(chunk)); }
  
    iterator erase(iterator it) { std::unique_lock<std::mutex> lock(mMutex); return mChunks.erase(it); }
    iterator erase(size_t id) { return erase(mChunks.begin() + id); }
    iterator erase(const Vec3i& chunkPos) { return erase(getIndex(chunkPos)); }

    iterator reset(iterator it, Chunk* chunk) { it->reset(chunk); return it; }
    iterator reset(size_t id, Chunk* chunk) { return reset(mChunks.begin() + id, chunk); }
    iterator reset(const Vec3i& chunkPos, Chunk* chunk) { return reset(getIndex(chunkPos), chunk); }
    
    // Chunk Only Functions
    size_t getIndex(const Vec3i& chunkPos) const
    {
        int first = 0, last = static_cast<int>(size()) - 1;
        while (first <= last)
        {
            int mid = (first + last) / 2;
            if (mChunks[mid]->getPosition() < chunkPos)
                first = mid + 1;
            else
                last = mid - 1;
        }
        return first;
    }

    template <typename... ArgType, typename Func>
    void doIfLoaded(const Vec3i& chunkPos, Func func, ArgType&&... args)
    {
        size_t index = getIndex(chunkPos);
        if ((index < size()) && (mChunks[index]->getPosition() == chunkPos))
            func(at(index), std::forward<ArgType>(args)...);
    };

    bool isLoaded(const Vec3i& chunkPos) const noexcept
    {
        size_t index = getIndex(chunkPos);
        return (index < size()) && (chunkPos == mChunks[index]->getPosition());
    }
    
    // Convert world position to chunk coordinate (one axis)
    static int getAxisPos(int pos) noexcept
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
        return Vec3i(getAxisPos(pos.x), getAxisPos(pos.y), getAxisPos(pos.z));
    }

    // Convert world position to block coordinate in chunk (one axis)
    static int getBlockAxisPos(int pos) noexcept
    {
        return pos & (Chunk::Size() - 1);
    }

    // Convert world position to block coordinate in chunk (all axes)
    static Vec3i getBlockPos(const Vec3i& pos) noexcept
    {
        return Vec3i(getBlockAxisPos(pos.x), getBlockAxisPos(pos.y), getBlockAxisPos(pos.z));
    }

    // Get block data
    BlockData getBlock(const Vec3i& pos) const
    {
        return at(getPos(pos)).getBlock(getBlockPos(pos));
    }

    // Set block data
    void setBlock(const Vec3i& pos, BlockData block)
    {
        at(getPos(pos)).setBlock(getBlockPos(pos), block);
    }

private:
    array_t mChunks;
    std::mutex mMutex;
};

using ChunkManager = ChunkManagerBase<ChunkHDC>;

#endif // !CHUNK_H_
