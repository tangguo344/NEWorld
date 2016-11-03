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

#ifndef SIGNLE_PLAYER_MANAGER_H_
#define SIGNLE_PLAYER_MANAGER_H_

#include <thread>
#include <logger.h>
#include <pluginmanager.h>
#include <jsonhelper.h>
#include <atomic>
#include <chrono>
#include <functional>

class SinglePlayManager
{
public:
    explicit SinglePlayManager(std::function<void(bool)> callback) :
        mPath(getJsonValue<std::string>(getSettings()["server"]["file"], "nwserver").c_str()),
        mTimeout(getJsonValue<int>(getSettings()["client"]["server_start_timeout"], 30)),
        mCallback(callback)
    {

    }
    ~SinglePlayManager()
    {
        if (m_localServerThread.joinable())
        {
            mLib.get<void NWAPICALL()>("nwStopServer")();
            m_localServerThread.join();
        }
    }

    void run()
    {
        mLib = std::move(Library(mPath));
        m_localServerThread = std::thread([this]
        {
            const char *argv[] = { mPath.c_str(),"-single-player-mode" };
            bool opened = mLib.get<bool NWAPICALL(int, char**)>("nwInitServer")(sizeof(argv) / sizeof(argv[0]), const_cast<char**>(argv));
            if (opened)
            {
                mReady.store(true);
                mLib.get<void NWAPICALL()>("nwRunServer")();
                mLib.get<void NWAPICALL()>("nwFreeServer")();
            }
            else
            {
                fatalstream << "Failed to start local server";
            }
        });

        mStartTime = std::chrono::system_clock::now();

        while (!mReady)
        {
            std::this_thread::yield();
            if (mTimeout != -1 && std::chrono::system_clock::now() - mStartTime > std::chrono::seconds(mTimeout))
            {
                fatalstream << "Local server timeout!";
                m_localServerThread.detach();
                mCallback(false);
                return;
            }
        }
        mCallback(true);
    }

private:
    // Library
    Library mLib;
    // Library Path
    std::string mPath;
    // Local Server Thread
    std::thread m_localServerThread;
    // Is Ready
    std::atomic_bool mReady = false;
    // Timeout Time (sec)
    int mTimeout;
    // The Start Time
    std::chrono::system_clock::time_point mStartTime;
    // Finish Callback
    std::function<void(bool)> mCallback;
};

#endif // !SIGNLE_PLAYER_MANAGER_H_
