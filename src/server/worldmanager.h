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

#ifndef WORLDMANAGER_H_
#define WORLDMANAGER_H_

#include <vector>

#include "worldserver.h"
#include <pluginmanager.h>
#include <nwblock.h>

// Multi-world
class WorldManager
{
public:
    WorldManager(PluginManager& plugins, BlockManager& blocks) :
        mPlugins(plugins), mBlocks(blocks)
    {
    }

    ~WorldManager()
    {
        mWorlds.clear();
    }

    WorldServer* addWorld(const std::string& name)
    {
        mWorlds.emplace_back(new WorldServer(name, mPlugins, mBlocks));
        return mWorlds[mWorlds.size() - 1];
    }

    std::vector<WorldServer*>::iterator begin()
    {
        return mWorlds.begin();
    }

    std::vector<WorldServer*>::iterator end()
    {
        return mWorlds.end();
    }

    WorldServer* getWorld(const std::string& name)
    {
        for(WorldServer* world : *this)
        {
            if (world->getWorldName() == name) return world;
        }
        return nullptr;
    }

    WorldServer* getWorld(size_t id)
    {
        for (WorldServer* world : *this)
        {
            if (world->getWorldID() == id) return world;
        }
        return nullptr;
    }

private:
    std::vector<WorldServer*> mWorlds;
    PluginManager& mPlugins;
    BlockManager& mBlocks;
};

#endif
