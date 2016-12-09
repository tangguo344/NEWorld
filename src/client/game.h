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
#include <mutex>
#include "common/vec3.h"
#include "renderer.h"

#include <world/world.h>
#include <common/nwsafety.hpp>
#include <world/nwblock.h>
#include <plugin/pluginmanager.h>
#include "worldclient.h"
#include "texture.h"
#include "player.h"
#include "network.h"
#include "widgetmanager.h"
#include <common/ratemeter.h>
#include "gameconnection.h"
#include "window.h"

class Game
{
public:
    // Game update frequency
    static constexpr int UpdateFrequency = 30;

    Game(const std::string& name, std::shared_ptr<GameConnection> connection,
         const Window& window);
    ~Game();

    void update();
    void multiUpdate();
    void render();
private:
    const Window& mWindow;
    // Texture test
    Texture mTexture;
    // World
    WorldClient mWorld;
    // Player
    Player mPlayer;
    // Widget manager
    WidgetManager mGUIWidgets;
    // Update scheduler
    RateMeter mUpdateScheduler{UpdateFrequency};
    // Rate counters
    int mFpsCounter, mUpsCounter, mFpsLatest, mUpsLatest;
    RateMeter mRateCounterScheduler{1};
    // Game connection
    std::shared_ptr<GameConnection> mConnection;

    std::mutex mMutex;
};

#endif // !GAME_H_
