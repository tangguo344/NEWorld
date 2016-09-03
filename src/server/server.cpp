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

Server::Server(std::vector<std::string> args)
    : m_worlds(m_plugins, m_blocks), m_updateTimer(m_ioService), m_plugins(false), m_args(args)
{
    // Initialization
    PluginAPI::Blocks = &m_blocks;
    infostream << "Initializing plugins...";
    m_plugins.loadPlugins();
    World* world = m_worlds.addWorld("main_world");
    //  m_worldLoaders.insert({ "main_world", WorldLoader(*world, 16) }); //TODO: get the range by players' settings
    // Start server
    infostream << "Server started!";
    initCommands();
}

Server::~Server()
{
    // TODO: Terminate here
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

void Server::run()
{
    // TODO:Initilizal NetworkManager
}