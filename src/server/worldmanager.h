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

#include <world/world.h>
#include <plugin/pluginmanager.h>
#include <world/nwblock.h>

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

    void clear()
    {
        mWorlds.clear();
    }

    World* addWorld(const std::string& name)
    {
        mWorlds.emplace_back(new World(name, mPlugins, mBlocks));
        return mWorlds[mWorlds.size() - 1].get();
    }

    typedef World*(WorldCreator)(const char*, BlockManager*, PluginManager*);
    World* addWorld(const std::string& name, void* w)
    {
        mWorlds.emplace_back(reinterpret_cast<WorldCreator*>(w)(name.c_str(), &mBlocks, &mPlugins));
        return mWorlds[mWorlds.size() - 1].get();
    }

    std::vector<std::unique_ptr<World>>::iterator begin()
    {
        return mWorlds.begin();
    }

    std::vector<std::unique_ptr<World>>::iterator end()
    {
        return mWorlds.end();
    }

    World* getWorld(const std::string& name)
    {
        for(auto&& world : *this)
        {
            if (world->getWorldName() == name) return world.get();
        }
        return nullptr;
    }

    World* getWorld(size_t id)
    {
        for (auto&& world : *this)
        {
            if (world->getWorldID() == id) return world.get();
        }
        return nullptr;
    }

private:
    std::vector<std::unique_ptr<World>> mWorlds;
    PluginManager& mPlugins;
    BlockManager& mBlocks;
};

#endif
