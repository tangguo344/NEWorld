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

#include "common.h"
#include "vec3.h"
#include "blockdata.h"

constexpr int ChunkSizeLog2 = 5, ChunkSize = 1 << ChunkSizeLog2; // 2 ^ ChunkSizeLog2 == 32

class Chunk
{
public:
    virtual ~Chunk() {}
    /// Get chunk position
    const Vec3i& getPosition() const
    {
        return m_position;
    }

    /// Get chunk updated flag
    bool isUpdated() const
    {
        return m_updated;
    }

    /// Set chunk updated flag
    void setUpdated(bool updated)
    {
        m_updated = updated;
    }

    /// Get block data in this chunk
    BlockData getBlock(const Vec3i& pos) const
    {
        assert(pos.x >= 0 && pos.x < ChunkSize && pos.y >= 0 && pos.y < ChunkSize && pos.z >= 0 && pos.z < ChunkSize);
        return m_blocks[pos.x * ChunkSize * ChunkSize + pos.y * ChunkSize + pos.z];
    }

    /// Get block reference in this chunk
    BlockData& getBlock(const Vec3i& pos)
    {
        assert(pos.x >= 0 && pos.x < ChunkSize && pos.y >= 0 && pos.y < ChunkSize && pos.z >= 0 && pos.z < ChunkSize);
        return m_blocks[pos.x * ChunkSize * ChunkSize + pos.y * ChunkSize + pos.z];
    }

    /// Get block pointer
    BlockData* getBlocks() { return m_blocks; }
    const BlockData* getBlocks() const { return m_blocks; }

    /// Set block data in this chunk
    void setBlock(const Vec3i& pos, BlockData block)
    {
        assert(pos.x >= 0 && pos.x < ChunkSize && pos.y >= 0 && pos.y < ChunkSize && pos.z >= 0 && pos.z < ChunkSize);
        m_blocks[pos.x * ChunkSize * ChunkSize + pos.y * ChunkSize + pos.z] = block;
        m_updated = true;
    }
protected:
    explicit Chunk(const Vec3i& position) : m_position(position) {}

private:
    Vec3i m_position;
    BlockData m_blocks[ChunkSize * ChunkSize * ChunkSize];
    bool m_updated = false;

};

#endif // !CHUNK_H_
