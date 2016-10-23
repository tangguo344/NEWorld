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

#ifndef GAME_H_
#define GAME_H_

#include <thread>
#include "vec3.h"
#include "renderer.h"

#include <world.h>
#include <logger.h>
#include <pluginapi.h>
#include <blockmanager.h>
#include <pluginmanager.h>
#include "worldclient.h"
#include "texture.h"
#include "player.h"
#include "network.h"

class Game
{
public:
    Game(const PluginManager& pm, const BlockManager& bm);

    ~Game()
    {
        m_connection.disconnect();
        if (m_localServerThread.joinable()) m_localServerThread.join();
    }

    void render();

private:
    // Window size
    int windowWidth = 852, windowHeight = 480;

    // Texture test
    Texture m_texture;
    // Blocks
    const BlockManager& m_blocks;
    // Loaded plugins
    const PluginManager& m_plugins;
    // World
    WorldClient m_world;
    // Player test
    Player m_player;
    // Network connection
    Connection m_connection;
    // Local server thread
    std::thread m_localServerThread;
};

#endif // !GAME_H_
