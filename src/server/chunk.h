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

#include <common.h>
#include <chunkbase.h>

using ChunkGenerator = void NWAPICALL(const Vec3i*, BlockData*, int);

extern bool ChunkGeneratorLoaded;
extern ChunkGenerator *ChunkGen;

class Chunk : public ChunkBase
{
public:
    explicit Chunk(ChunkBase& chunk) : ChunkBase(chunk) {}

    // Build chunk
    void build(int daylightBrightness);
};

#endif // !CHUNK_H_
