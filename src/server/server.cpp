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
    : mWorlds(mPlugins, mBlocks), mPlugins(false), mArgs(args),
      mNetwork([this](Identifier id, unsigned char* data) {handleData(id, data); })
{
    using namespace std::chrono;
    auto start_time = steady_clock::now();

    // Plugin
    PluginAPI::Blocks = &mBlocks;

    infostream << "Initializing plugins...";
    mPlugins.loadPlugins();

    // World
    mWorlds.addWorld("main_world");

    // Network
    mNetwork.run(getJsonValue<std::string>(getSettings()["server"]["ip"], "127.0.0.1").c_str(),
                 getJsonValue<unsigned short>(getSettings()["server"]["port"], 9887));

    // Builtin Commands
    initBuiltinCommands();

    // Done
    auto done_time = steady_clock::now();
    infostream << "Done!(in " << duration_cast<milliseconds>(done_time - start_time).count() << "ms)!";
}

void Server::run()
{
    // Process input
    mCommands.inputLoop(); // This will block the main thread
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

void Server::handleData(Identifier id, unsigned char* data)
{
    switch(id)
    {
    case Identifier::c2sLogin:
    {
        auto login=c2s::GetLogin(data);
        bool success = true;
        std::string reason = "";
        if (login->versionId() != NEWorldVersion)
        {
            success = false;
            reason = "Mismatched version. Server:" + std::to_string(NEWorldVersion) + " Player:" + std::to_string(login->versionId());
        }

        //TODO: validate password

        if (success)
            infostream << "Player " << login->username()->str() << " login!";
        else
            warningstream << "Player " << login->username()->str() << " failed to login: " << reason;
    }
    }
}
