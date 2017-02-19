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

#ifndef PLUGINMANAGER_H_
#define PLUGINMANAGER_H_

#include <string>
#include <vector>
#include "library.h"
#include "plugin.h"

// Plugin system
class PluginManager {
public:
    PluginManager(bool isClient = true);
    ~PluginManager() { unloadPlugins(); }

    // Load single plugin
    void loadPlugin(const std::string& filename);
    // Load plugins
    void loadPlugins();
    // Unload plugins
    void unloadPlugins();

private:
    std::vector<Plugin> mPlugins;
    bool mIsClient;
};

#endif // !PLUGINMANAGER_H_
