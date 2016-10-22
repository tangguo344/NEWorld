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

#include "commandcontroller.h"
#include <logger.h>
#include <consolecolor.h>

CommandExecuteStat CommandController::handleCommand(Command cmd)
{
    strtolower(cmd.name);
    auto result = m_commandMap.find(cmd.name);
    if (result != m_commandMap.end())
        return (*result).second.second(cmd);
    return{ false,"Failed to execute the command: The command does not exist, type help for available commands." };
}

void CommandController::mainLoop()
{
    while (m_threadRunning.load(std::memory_order_acquire))
    {
        std::string input;
        //std::cout << LColorFunc::white << "$> " << LColorFunc::lwhite;
        m_waitingForInputing.store(true, std::memory_order_release);
        getline(std::cin, input);
        m_waitingForInputing.store(false, std::memory_order_release);
        auto result = handleCommand(Command(input));
        if (result.info != "")
            infostream << result.info;
    }
}
