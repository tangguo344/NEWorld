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

#ifndef CHUNKSERVER_H_
#define CHUNKSERVER_H_

#include <common.h>
#include <chunk.h>
#include <atomic>

using ChunkGenerator = void NWAPICALL(const Vec3i*, BlockData*, int);

extern bool ChunkGeneratorLoaded;
extern ChunkGenerator *ChunkGen;

class ChunkServer : public Chunk
{
public:
    explicit ChunkServer(const Vec3i& position) :
            Chunk(position), mRefrenceCount(0), mWeakRefrenceCount(0) {}

    ChunkServer(const ChunkServer&) = delete;
    ChunkServer& operator=(const ChunkServer&) = delete;

    // Build chunk
    void build(int daylightBrightness);
    //Reference Counting
    void increaseWeakRef();
    void decreaseWeakRef();
    void increaseStrongRef();
    void decreaseStrongRef();
    bool checkReleaseable() const;
protected:
    std::atomic<int16_t> mRefrenceCount, mWeakRefrenceCount;
};

#endif // !CHUNKSERVER_H_
