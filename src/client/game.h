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
#include "singleplaymanager.h"

class GameConnection
{
public:
    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual World* getWorld(size_t id) = 0;
    virtual ~GameConnection() = default;
};

class SinglePlayerConnection : public GameConnection
{
public:
    void connect() override;
    void disconnect() override;
    World* getWorld(size_t id) override;
};

class Game
{
public:
    Game(PluginManager& pm, const BlockManager& bm);
    ~Game();

    void update();
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
    // Local Server
    SinglePlayManager mSinglePlayManager;
    // Connection
    Connection mConn;
    // FlatBuffer Builder
    flatbuffers::FlatBufferBuilder mFbb;
    // Widget Manager
    WidgetManager mWidgetManager;
    // Main event bus
    Event::EventBus mEventBus;
};

#endif // !GAME_H_
