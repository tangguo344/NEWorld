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

#include "neworld.h"
#include "texture.h"
#include <pluginapi.h>
#include "window.h"
#include "gamescene.h"

NEWorld::NEWorld() : m_plugins(true)
{
    // Initialize
    Logger::init("client");
    infostream << "Initializing...";
    Texture::init();
    PluginAPI::Blocks = &m_blocks;
    PluginAPI::Plugins = &m_plugins;
    m_plugins.loadPlugins();

    // Run
    GameScene game(m_plugins, m_blocks);
    while(true)
    {
        game.render();
    }

    // Terminate
    infostream << "Terminating...";
    m_plugins.unloadPlugins();
    Texture::free();
}
