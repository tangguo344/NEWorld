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
#include "chunk.h"

class ChunkPointerArray
{
private:
    // Array
    Chunk** mArray;
    // Array size
    int mSize, mSize2, mSize3;
    // Origin
    Vec3i mOrg;

public:
    explicit ChunkPointerArray(int size)
        : mSize(size), mSize2(size * size), mSize3(size * size * size), mOrg(-size / 2, -size / 2, -size / 2)
    {
        mArray = new Chunk*[mSize3];
        //memset(mArray, 0, mSize3 * sizeof(Chunk*));
    }

    ChunkPointerArray(ChunkPointerArray&& rhs)
        : mSize(rhs.mSize), mSize2(rhs.mSize2), mSize3(rhs.mSize3), mOrg(rhs.mOrg)
    {
        std::swap(mArray, rhs.mArray);
    }

    ~ChunkPointerArray()
    {
        delete[] mArray;
    }

    ChunkPointerArray(const ChunkPointerArray&) = delete;
    ChunkPointerArray& operator=(const ChunkPointerArray&) = delete;

    // Move array by delta
    void move(const Vec3i& delta)
    {
        Chunk** arr = new Chunk*[mSize3];
        Vec3i::for_range(0, mSize, [this, arr, delta](const Vec3i& pos)
        {
            arr[pos.x * mSize2 + pos.y * mSize + pos.z] =
                exist(pos + delta) ?
                mArray[(pos.x + delta.x) * mSize2 + (pos.y + delta.y) * mSize + (pos.z + delta.z)] :
                nullptr;
        });
        delete[] mArray;
        mArray = arr;
        mOrg += delta;
    }

    // Move array to pos
    void moveTo(const Vec3i& pos)
    {
        move(pos - mOrg);
    }

    // Check if specific element is inside array range
    bool exist(const Vec3i& pos) const
    {
        return pos.x >= 0 && pos.x < mSize && pos.z >= 0 && pos.z < mSize && pos.y >= 0 && pos.y < mSize;
    }

    // Get chunk pointer from array
    Chunk* get(Vec3i pos) const
    {
        pos -= mOrg;
        return exist(pos) ? mArray[pos.x * mSize2 + pos.y * mSize + pos.z] : nullptr;
    }

    // Update chunk pointer in array
    void set(Vec3i pos, Chunk* c) const
    {
        pos -= mOrg;
        if (exist(pos))
            mArray[pos.x * mSize2 + pos.y * mSize + pos.z] = c;
    }
};

#endif
