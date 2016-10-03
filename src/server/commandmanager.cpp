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

#include "commandmanager.h"
#include <logger.h>
#include <consolecolor.h>

CommandExecuteStat CommandManager::handleCommand(Command cmd)
{
    strtolower(cmd.name);
    auto result = m_commandMap.find(cmd.name);
    if (result != m_commandMap.end())
        return (*result).second.second(cmd);
    else
        return {false,"Command not exists, type help for available commands." };
}

void CommandManager::inputLoop()
{
    while (m_threadRunning)
    {
        std::string input;
        //std::cout << LColorFunc::white << "> " << LColorFunc::lwhite;
        std::getline(std::cin, input);
        auto result = handleCommand(Command(input));
        if (result.info != "")
            infostream << result.info;
    }
}

void CommandManager::setStatus(bool s)
{
    m_threadRunning = s;
}