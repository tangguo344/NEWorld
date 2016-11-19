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

#ifndef WORLDSERVER_H_
#define WORLDSERVER_H_

#include <utility>
#include <world/world.h>

class WorldServer : public World
{
public:
    WorldServer(const std::string& name, PluginManager& plugins, BlockManager& blocks)
        : World(name,plugins,blocks)
    {
    }

    Chunk* addChunk(const Vec3i& chunkPos) override;

    void updateChunkLoadStatus();
};

#endif // !WORLDSERVER_H_
