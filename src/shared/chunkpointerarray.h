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

#ifndef CHUNKPOINTERARRAY_H_
#define CHUNKPOINTERARRAY_H_

#include <cstring>
#include <algorithm>
#include <boost/core/noncopyable.hpp>
#include "chunkbase.h"

class ChunkPointerArray : boost::noncopyable
{
private:
    /// Array
    ChunkBase** m_array;
    /// Array size
    int m_size, m_size2, m_size3;
    /// Origin
    Vec3i m_org;

public:
    explicit ChunkPointerArray(int size) : m_org(-size / 2, -size / 2, -size / 2)
    {
        m_size = size;
        m_size2 = size * size;
        m_size3 = size * size * size;
        m_array = new ChunkBase*[m_size3];
        memset(m_array, 0, m_size3 * sizeof(ChunkBase*));
    }

    ChunkPointerArray(ChunkPointerArray&& rhs) : m_size(rhs.m_size), m_size2(rhs.m_size2), m_size3(rhs.m_size3), m_org(rhs.m_org)
    {
        std::swap(m_array, rhs.m_array);
    }

    ~ChunkPointerArray()
    {
        delete[] m_array;
    }

    /// Move array by delta
    void move(const Vec3i& delta)
    {
        ChunkBase** arr = new ChunkBase*[m_size3];
        Vec3i::for_range(0, m_size, [this, arr, delta](const Vec3i& pos)
        {
            arr[pos.x * m_size2 + pos.y * m_size + pos.z] =
                exist(pos + delta) ?
                m_array[(pos.x + delta.x) * m_size2 + (pos.y + delta.y) * m_size + (pos.z + delta.z)] :
                nullptr;
        });
        delete[] m_array;
        m_array = arr;
        m_org += delta;
    }

    /// Move array to pos
    void moveTo(const Vec3i& pos)
    {
        move(pos - m_org);
    }

    /// Check if specific element is inside array range
    bool exist(const Vec3i& pos) const
    {
        return pos.x >= 0 && pos.x < m_size && pos.z >= 0 && pos.z < m_size && pos.y >= 0 && pos.y < m_size;
    }

    /// Get chunk pointer from array
    ChunkBase* get(Vec3i pos) const
    {
        pos -= m_org;
        return exist(pos) ? m_array[pos.x * m_size2 + pos.y * m_size + pos.z] : nullptr;
    }

    /// Update chunk pointer in array
    void set(Vec3i pos, ChunkBase* c) const
    {
        pos -= m_org;
        if (exist(pos))
            m_array[pos.x * m_size2 + pos.y * m_size + pos.z] = c;
    }
};

#endif
