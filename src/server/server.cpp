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

#include "server.h"
#include <jsonhelper.h>
#include "commandmanager.h"

Server::Server(std::vector<std::string> args)
    : mWorlds(mPlugins, mBlocks), mPlugins(false), mArgs(args), mNetwork(mWorlds)
{
    using namespace std::chrono;
    auto startTime = steady_clock::now();

    // Plugin
    PluginAPI::Blocks = &mBlocks;

    infostream << "Initializing plugins...";
    mPlugins.loadPlugins();

    // World
    mWorlds.addWorld("main_world");

    // Network
    mNetwork.run(getJsonValue<std::string>(getSettings()["server"]["ip"], "127.0.0.1").c_str(),
                 getJsonValue<unsigned short>(getSettings()["server"]["port"], 31111));

    // Builtin Commands
    initBuiltinCommands();

    // Done
    auto doneTime = steady_clock::now();
    infostream << "Initialization done in " << duration_cast<milliseconds>(doneTime - startTime).count() << "ms!";
}

void Server::run()
{
    mNetwork.loop();
}

void Server::stop()
{
    mNetwork.close();
    mCommands.setRunningStatus(false);
}

Server::~Server()
{
    // TODO: Terminate here
}
