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
#ifndef COMMANDCONTROLLER_H_
#define COMMANDCONTROLLER_H_

#include <command.h>
#include <thread>
#include <atomic>
#include <future>

class CommandManager
{
public:
    CommandManager()
        : mMainloop(std::async([this] { inputLoop(); }))
    {
    }
    ~CommandManager()
    {
        mThreadRunning.store(false, std::memory_order_release);
        if (!mWaitingForInputing.load(std::memory_order_acquire))
        {
            mMainloop.wait();
        }
    }

    CommandManager(const CommandManager&) = delete;
    CommandManager& operator=(const CommandManager&) = delete;

    CommandMap& getCommandMap() { return mCommandMap; }

    void inputLoop();
    void setRunningStatus(bool s);

    void addCommand(std::string name, CommandInfo info, CommandHandleFunction func)
    {
        mCommandMap.insert({name, {info, func}});
    }

private:
    CommandExecuteStat handleCommand(Command cmd);

    std::future<void> mMainloop;
    std::atomic_bool mThreadRunning{true};
    std::atomic_bool mWaitingForInputing{false};
    CommandMap mCommandMap;
};

#endif //!COMMANDCONTROLLER_H_
