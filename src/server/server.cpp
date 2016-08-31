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
using namespace boost::asio;
using namespace boost::system;

unsigned short globalPort;

void errorHandle(const tcp::socket& m_socket, error_code ec)
{
    infostream << m_socket.remote_endpoint().address().to_string() << " disconnected, code: " << ec.value();
}

void Session::doUpdate()
{
    auto self(shared_from_this());
    m_updateTimer.expires_from_now(boost::posix_time::microseconds(UpdateInterval));
    m_updateTimer.async_wait([this, self](error_code)
    {
        // TODO: Process client actions here
        doWrite();
    });
}

//void Server::sendToAllSessions(Packet packet)
//{
//TODO: fix it
//for (auto iter = m_sessions.begin(); iter != m_sessions.end();)
//{
//    auto session = iter->lock();
//    if (session)
//    {
//        session->addRequest(packet);
//        ++iter;
//    }
//    else
//    {
//        iter = m_sessions.erase(iter);
//    }
//}
//}

Server::Server(boost::asio::io_service & ioservice, unsigned short port)
    : m_acceptor(ioservice, boost::asio::ip::tcp::endpoint(tcp::v4(), port)), m_socket(ioservice),
      m_worlds(m_plugins, m_blocks), m_updateTimer(m_socket.get_io_service())
{
    // Initialization
    PluginAPI::Blocks = &m_blocks;
    infostream << "Initializing plugins...";
    //        m_plugins.loadPlugins(base);
    World* world = m_worlds.addWorld("main_world");
    m_worldLoaders.insert({ "main_world", WorldLoader(*world, 16) }); //TODO: get the range by players' settings
    // Start server
    infostream << "Server started!";
    doGlobalUpdate();
    doAccept();
}

Server::~Server()
{
    // TODO: Terminate here
}

void Server::doAccept()
{
    m_acceptor.async_accept(m_socket, [this](boost::system::error_code ec)
    {
        if (!ec)
        {
            infostream << m_socket.remote_endpoint().address().to_string() << " connected to the server";
            auto session = std::make_shared<Session>(std::move(m_socket));
            session->start();
            m_sessions.push_back(session);
        }
        doAccept();
    });
}
void Server::doGlobalUpdate()
{
    m_updateTimer.expires_from_now(boost::posix_time::microseconds(GlobalUpdateInterval));
    m_updateTimer.async_wait([this](boost::system::error_code)
    {
        // Update worlds
        for (auto& worldLoader : m_worldLoaders)
        {
            worldLoader.second.sortChunkLoadUnloadList({0,0,0}); //TODO: players' position;
            worldLoader.second.loadUnloadChunks();
        }
        for (auto world : m_worlds) world->update();
        doGlobalUpdate();

        m_ups.refresh();
    });
}
