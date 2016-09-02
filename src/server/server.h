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
#include <session.h>
#include <worldmanager.h>
#include <blockmanager.h>
#include <pluginmanager.h>
#include <pluginapi.h>
#include <ratemeter.h>
#include "worldloader.h"
#include <unordered_map>
#include <thread>
#include "commandcontroller.h"

constexpr int UpdateInterval = 1000/60, GlobalUpdateInterval = 1000/60; // unit: ms

extern unsigned short globalPort;

class Server
{
public:
    Server(unsigned short port);

    ~Server();

    void run() { m_ioService.run(); }

    //void sendToAllSessions(Packet packet);

private:
    void doAccept();
    void doGlobalUpdate();

    void initCommands();

    boost::asio::io_service m_ioService;
    boost::asio::ip::tcp::acceptor m_acceptor;
    boost::asio::ip::tcp::socket m_socket;
    std::vector<std::weak_ptr<Session>> m_sessions;

    boost::asio::deadline_timer m_updateTimer;
    RateMeter m_ups;

    WorldManager m_worlds;
    BlockManager m_blocks;
    PluginManager m_plugins; // Loaded plugins

    std::unordered_map<std::string, WorldLoader> m_worldLoaders;

    CommandController m_commandController;
};

#endif // SERVER_H__
