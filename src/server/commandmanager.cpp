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
    auto result = mCommandMap.find(cmd.name);
    if (result != mCommandMap.end())
        return (*result).second.second(cmd);
    return{ false,"Command not exists, type help for available commands." };
}

CommandManager::CommandManager()
    : mMainloop([this] { inputLoop(); })
{
}

CommandManager::~CommandManager()
{
    mThreadRunning.store(false, std::memory_order_release);
    if (!mWaitingForInput.load(std::memory_order_acquire))
    {
        mMainloop.join();
        debugstream << "Input thread exited.";
    }
    else
    {
        mMainloop.detach();
        debugstream << "Input thread detached.";
    }
}

void CommandManager::inputLoop()
{
    while (mThreadRunning.load(std::memory_order_acquire))
    {
        std::string input;
        //std::cout << LColorFunc::white << "$> " << LColorFunc::lwhite;
        mWaitingForInput.store(true, std::memory_order_release);
        getline(std::cin, input);
        mWaitingForInput.store(false, std::memory_order_release);
        auto result = handleCommand(Command(input));
        if (result.info != "")
            infostream << result.info;
    }
}

void CommandManager::setRunningStatus(bool s)
{
    mThreadRunning = s;
}
