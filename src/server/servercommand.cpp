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

#include "servercommand.h"
#include <logger.h>
#include <consolecolor.h>
#include <chrono>
#include <command.h>
#include "server.h"
#include <utils.h>
#include "settings.h"

bool inputThreadRunning = true;

CommandMap commandMap;
#define CommandDefine(commandName, commandAuthor, commandHelp) commandMap.insert({commandName, std::pair<CommandInfo,CommandHandleFunction>({commandAuthor, commandHelp},[](Command cmd)->CommandExecuteStat
#define EndCommandDefine )})

void stopInputThreadRunning()
{
    inputThreadRunning = false;
}

void initCommands()
{
    CommandDefine("help", "Internel", "Help")
    {
        std::string helpString = "\nAvailable commands:\n";
        for (const auto& command : commandMap)
        {
            helpString += command.first + " - " + command.second.first.author + " : " + command.second.first.help + "\n";
        }
        return{ true, helpString };
    }
    EndCommandDefine;

    CommandDefine("test.hello", "Internal", "Say hello")
    {
        return{ true, "Hello!" };
    }
    EndCommandDefine;

    CommandDefine("test.fatal", "Internal", "test fatal log.")
    {
        fatalstream << "execute test.fatal!";
        return { true , "" };
    }
    EndCommandDefine;

    CommandDefine("test.error", "Internal", "test error log.")
    {
        errorstream << "execute test.error!";
        return{ true , "" };
    }
    EndCommandDefine;

    CommandDefine("test.warning", "Internal", "do NOT use it :-)")
    {
        warningstream << "Your computer will explode in three seconds!!!";
        return{ true , "" };
    }
    EndCommandDefine;

    CommandDefine("test.rainbow", "Internal", "test colors.")
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
    }
    EndCommandDefine;

    CommandDefine("server.stop", "Internal", "Stop the server")
    {
        ioService.stop();
        stopInputThreadRunning();
        return{ true, "" };
    }
    EndCommandDefine;

    CommandDefine("conf.get", "Internal", "Get one configuration item. Usage: conf.get <confname>")
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

            if(exist)
                return{ true, cmd.args[0] + " = " + now.dump() };
            else
                return{ false, "The configuration item does not exist." };
        }
        else
        {
            return{ false, "Usage: conf.get <confname>" };
        }
    }
    EndCommandDefine;

    CommandDefine("conf.show", "Internal", "Show the configuration.")
    {
        return{ true, getSettings().dump() };
    }
    EndCommandDefine;

    CommandDefine("conf.save", "Internal", "Save the configuration.")
    {
        saveSettings();
        return{ true,"Done!" };
    }
    EndCommandDefine;
}

CommandExecuteStat handleCommand(Command cmd)
{
    strtolower(cmd.name);
    auto result = commandMap.find(cmd.name);
    if (result != commandMap.end())
        return (*result).second.second(cmd);
    else
        return{ false,"Failed to execute the command: The command does not exist, type help for available commands." };
}

void inputThreadFunc()
{
    initCommands();
    while (inputThreadRunning)
    {
        std::string input;
        std::cout << LColorFunc::white << "$> " << LColorFunc::lwhite;
        std::getline(std::cin, input);
        auto result = handleCommand(Command(input));
        if (result.info != "")
            infostream << result.info;
    }
}
