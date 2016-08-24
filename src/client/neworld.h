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

#ifndef NEWORLD_H_
#define NEWORLD_H_

#include <common.h>
#include "network.h"
#include <string>
#include <uilib.h>
#include <pluginmanager.h>
#include <worldmanager.h>
#include <blockmanager.h>

class Application : public UI::Core::Application
{
public:
    void beforeLaunch() override;
    void afterLaunch() override;
    void onTerminate() override;

    // TODO: change it later (or not)
    static Application& getApplication()
    {
        static Application app(852, 480, "NEWorld", "./");
        return app;
    }
    PluginManager& getPlugins() { return m_plugins; }
    WorldManager& getWorlds() { return m_worlds; }
    BlockManager& getBlocks() { return m_blocks; }

private:

    Application(int width, int height, const std::string& title, const std::string& path);
    int m_width, m_height;
    std::string m_title;

    PluginManager m_plugins;
    WorldManager m_worlds;
    BlockManager m_blocks;
};



#endif // !NEWORLD_H_
