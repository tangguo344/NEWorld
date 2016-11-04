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

#ifndef SERVER_H_
#define SERVER_H_

#include <memory>
#include <vector>
#include <logger.h>
#include "worldmanager.h"
#include <blockmanager.h>
#include <pluginmanager.h>
#include <pluginapi.h>
#include <ratemeter.h>
#include "worldserver.h"
#include <unordered_map>
#include <thread>
#include "commandmanager.h"
#include "networkmanager.h"
#include "networkmanager.h"
#include <boost/timer.hpp>

constexpr int UpdateInterval = 1000/60, GlobalUpdateInterval = 1000/60; // unit: ms

class Server
{
public:
    Server(std::vector<std::string> args);
    void run();
    void stop();
    ~Server();
private:

    void initBuiltinCommands();

    RateMeter mUps;

    boost::timer mUpdateTimer;

    // Managers
    WorldManager mWorlds;
    BlockManager mBlocks;
    PluginManager mPlugins;
    NetworkManager mNetwork;
    CommandManager mCommands;

    std::vector<std::string> mArgs;
};

#endif
