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
#include "commandcontroller.h"

#include "settings.h"
#include <consolecolor.h>
#include <functional>
using namespace boost::asio;
using namespace boost::system;

std::function<void()> errorHook;

void errorHandle(const boost::asio::ip::tcp::socket& m_socket, error_code ec)
{
    infostream << m_socket.remote_endpoint().address().to_string() << " disconnected, code: " << ec.value();
    if (errorHook) errorHook();
}

void Session::doUpdate()
{
    auto self(shared_from_this());
    m_updateTimer.expires_from_now(boost::posix_time::milliseconds(UpdateInterval));
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

Server::Server(std::vector<std::string> args)
    : m_acceptor(m_ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), getJsonValue(getSettings()["server"]["port"], 8090))), m_socket(m_ioService),
      m_worlds(m_plugins, m_blocks), m_updateTimer(m_socket.get_io_service()), m_plugins(false), m_args(args)
{
    // Initialization
    PluginAPI::Blocks = &m_blocks;
    infostream << "Initializing plugins...";
    m_plugins.loadPlugins();
    World* world = m_worlds.addWorld("main_world");
    m_worldLoaders.insert({ "main_world", WorldLoader(*world, 16) }); //TODO: get the range by players' settings
    // Start server
    infostream << "Server started!";
    doGlobalUpdate();
    doAccept();
    initCommands();

    if (std::find(args.begin(), args.end(), "-single-player-mode") != args.end())
        errorHook = [this] {m_ioService.stop(); };
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
    m_updateTimer.expires_from_now(boost::posix_time::milliseconds(GlobalUpdateInterval));
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

void Server::initCommands()
{
    m_commandController.addCommand("help", {"Internel","Help"}, [this](Command cmd)->CommandExecuteStat
    {
        std::string helpString = "\nAvailable commands:\n";
        for (const auto& command : m_commandController.getCommandMap())
        {
            helpString += command.first + " - " + command.second.first.author + " : " + command.second.first.help + "\n";
        }
        return {true, helpString};
    });
    m_commandController.addCommand("test.hello", { "Internel","Say hello" }, [this](Command cmd)->CommandExecuteStat
    {
        return{ true, "Hello!" };
    });
    m_commandController.addCommand("test.log", { "Internel","test the logger." }, [this](Command cmd)->CommandExecuteStat
    {
        fatalstream << "execute test.fatal!";
        errorstream << "execute test.error!";
        warningstream << "Your computer will explode in three seconds!!!";
        return{ true , "" };
    });
    m_commandController.addCommand("test.rainbow", { "Internel","test colors." }, [this](Command cmd)->CommandExecuteStat
    {
        infostream << "Grayscales:";
        infostream << LColor::black << "2333333333 [0%]";
        infostream << LColor::lblack << "2333333333 [50%]";
        infostream << LColor::white << "2333333333 [75%]";
        infostream << LColor::lwhite << "2333333333 [100%]";
        infostream << "Dark colors:";
        infostream << LColor::red << "2333333333 [dark red]";
        infostream << LColor::yellow << "2333333333 [dark yellow]";
        infostream << LColor::green << "2333333333 [dark green]";
        infostream << LColor::cyan << "2333333333 [dark cyan]";
        infostream << LColor::blue << "2333333333 [dark blue]";
        infostream << LColor::magenta << "2333333333 [dark magenta]";
        infostream << "Bright colors:";
        infostream << LColor::lred << "2333333333 [red]";
        infostream << LColor::lyellow << "2333333333 [yellow]";
        infostream << LColor::lgreen << "2333333333 [green]";
        infostream << LColor::lcyan << "2333333333 [cyan]";
        infostream << LColor::lblue << "2333333333 [blue]";
        infostream << LColor::lmagenta << "2333333333 [magenta]";
        return{ true , "" };
    });
    m_commandController.addCommand("server.stop", { "Internel","Stop the server." }, [this](Command cmd)->CommandExecuteStat
    {
        m_ioService.stop();
        return{ true, "" };
    });
    m_commandController.addCommand("conf.get", { "Internel","Get one configuration item. Usage: conf.get <confname>" }, [this](Command cmd)->CommandExecuteStat
    {
        if (cmd.args.size() == 1)
        {
            auto keys = split(cmd.args[0], ".");
            Json now = getSettings();
            bool exist = true;
            for (auto key : keys)
            {
                auto iter = now.find(key);
                if (iter == now.end())
                {
                    exist = false;
                    break;
                }
                now = iter.value();
            }

            if (exist)
                return{ true, cmd.args[0] + " = " + now.dump() };
            else
                return{ false, "The configuration item does not exist." };
        }
        else
        {
            return{ false, "Usage: conf.get <confname>" };
        }
    });
    m_commandController.addCommand("conf.show", { "Internel","Show the configuration." }, [this](Command cmd)->CommandExecuteStat
    {
        return{ true, getSettings().dump() };
    });
    m_commandController.addCommand("conf.save", { "Internel","Save the configuration." }, [this](Command cmd)->CommandExecuteStat
    {
        saveSettings();
        return{ true,"Done!" };
    });
    m_commandController.addCommand("server.ups", { "Internel","Show the ups." }, [this](Command cmd)->CommandExecuteStat
    {
        return{ true,"Ups: " + std::to_string(m_ups.getRate()) };
    });
}
