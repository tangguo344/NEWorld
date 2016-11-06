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

#include "common.h"
#include "utils.h"
#include "pluginmanager.h"
#include "logger.h"
#include <fs.h>

PluginManager::PluginManager(bool isClient)
    : mIsClient(isClient)
{
}

void PluginManager::loadPlugin(const std::string& filename)
{
    mPlugins.emplace_back(Plugin(filename));
    Plugin& plugin = mPlugins[mPlugins.size() - 1];
    if (plugin.getData().isClientPlugin != mIsClient)
    {
        mPlugins.pop_back();
        return;
    }

    plugin.init();

    if (!plugin.isLoaded())
    {
        mPlugins.pop_back();
        warningstream << "Failed to load plugin from \"" << filename << "\", skipping";
        return;
    }

    infostream << "Loaded plugin \"" << plugin.getData().pluginName << "\"["
               << plugin.getData().internalName
               << "], authored by \"" << plugin.getData().authorName << "\"";
}

void PluginManager::loadPlugins()
{
    using namespace filesystem;
    std::string path = "./plugins/";
    if (exists(path))
    {
        files_in_dir(path, [this](std::string filename)
        {
            std::string suffix = filename.substr(filename.size() - std::string(LibSuffix).size());
            strtolower(suffix);
            if (suffix != LibSuffix) return; //TODO: FIXME: may ignore linux plugins
            loadPlugin(filename);
        });
    }
}

void PluginManager::unloadPlugins()
{
    mPlugins.clear();
}
