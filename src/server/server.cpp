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
#include "settings.h"
#include "commandmanager.h"

Server::Server(std::vector<std::string> args)
    : m_worlds(m_plugins, m_blocks), m_plugins(false), m_args(args), m_network(*this), m_raknet(m_network)
{
    using namespace std::chrono;
    auto start_time = steady_clock::now();
    
    // Plugin
    PluginAPI::Blocks = &m_blocks;
    infostream << "Initializing plugins...";
    m_plugins.loadPlugins();
    
    // World
    World* world = m_worlds.addWorld("main_world");

    m_worldLoaders.insert({ "main_world", WorldLoader(*world, 16) }); //TODO: get the range by players' settings
    
    // Network
    m_raknet.run("127.0.0.1",9887); // TODO: get address and port to bind from settingsmanager.
    warningstream << "TODO: The RakNet gateway is not completed yet!";
    
    // Done
    auto done_time = steady_clock::now();
    infostream << "Done!(in " << duration_cast<milliseconds>(done_time - start_time).count() << "ms)!";

    // Process input
    initBuiltinCommands();
    m_commands.processInput(); // This will block the main thread
}

Server::~Server()
{
    // TODO: Terminate here
}
