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
#include <event.h>
#include "worldclient.h"
#include "texture.h"
#include "player.h"
#include "network.h"
#include "widgetmanager.h"
#include "localserverloader.h"
#include <ratemeter.h>
#include "gameconnection.h"

class Game
{
public:
    Game(PluginManager& pm, const BlockManager& bm);
    ~Game();

    void update();
    void multiUpdate();
    void render();

    Event::EventBus& getEventBus();

private:
    // Window size
    int windowWidth = 852, windowHeight = 480;

    // Texture test
    Texture mTexture;
    // Blocks
    const BlockManager& mBlocks;
    // Loaded plugins
    PluginManager& mPlugins;
    // World
    WorldClient mWorld;
    // Player test
    Player mPlayer;
    // Local server
    LocalServerLoader mSinglePlayManager;
    // Widget manager
    WidgetManager mWidgetManager;
    // Main event bus
    Event::EventBus mEventBus;
    // Update scheduler
    RateMeter mUpdateScheduler{60};
    // Game connection
    std::shared_ptr<GameConnection> mConnection;
};

#endif // !GAME_H_
