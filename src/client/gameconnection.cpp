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

#include "chunkclient.h"
#include "gameconnection.h"
#include <vec3.h>
#include <logger.h>
#include <library.h>
#include <jsonhelper.h>

MultiplayerConnection::MultiplayerConnection(const std::string& host, unsigned short port) :
    mConn([this](Identifier id, unsigned char* data)
{
    switch (id)
    {
    case Identifier::s2cChunk:
    {
        mChunkCallback(ChunkClient::getFromFlatbuffers(s2c::GetChunk(data), *mWorld));
        break;
    }
    default:
        warningstream << "Unsupported game packet received: " << static_cast<int>(id);
    }
}), mHost(host),
mPort(port)
{
}

void MultiplayerConnection::connect()
{
    mConn.connect(mHost.c_str(), mPort);
}

void MultiplayerConnection::disconnect()
{
    //not implement
    warningstream << "Function not implemented";
    Assert(false);
}

World* MultiplayerConnection::getWorld(size_t)
{
    //not implement
    warningstream << "Function not implemented";
    Assert(false);
    return nullptr;
}

void MultiplayerConnection::waitForConnected()
{
    mConn.waitForConnected();
}

void MultiplayerConnection::login(const char * username, const char * password)
{
    mFbb.Clear();
    auto login = c2s::CreateLoginDirect(mFbb, username, password, NEWorldVersion);
    c2s::FinishLoginBuffer(mFbb, login);
    mConn.send(mFbb, login,PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE);
}

void MultiplayerConnection::getChunk(size_t worldID, Vec3i pos)
{
    mFbb.Clear();
    auto req = c2s::CreateRequestChunk(mFbb, pos.x, pos.y, pos.z, worldID);
    c2s::FinishRequestChunkBuffer(mFbb, req);
    mConn.send(mFbb, req, PacketPriority::MEDIUM_PRIORITY, PacketReliability::UNRELIABLE);
}

LocalConnectionByNetWork::LocalConnectionByNetWork(std::string host, unsigned short port):
    MultiplayerConnection(host, port),
    mPath(getJsonValue<std::string>(getSettings()["server"]["file"], "nwserver.dll").c_str()),
    mTimeout(getJsonValue<int>(getSettings()["client"]["server_start_timeout"], 30))
{
}

void LocalConnectionByNetWork::connect()
{
    mCallback = [this](bool success)
    {
        if (success) MultiplayerConnection::connect();
    };
    mLib = std::move(Library(mPath));
    mLocalServerThread = std::thread([this]()
    {
        const char *argv[] = { mPath.c_str(),"-single-player-mode" };
        bool opened = mLib.get<bool NWAPICALL(int, char**)>("nwInitServer")(sizeof(argv) / sizeof(argv[0]), const_cast<char**>(argv));
        if (opened)
        {
            mReady.store(true);
            mLib.get<void NWAPICALL()>("nwRunServer")();
        }
        else
            fatalstream << "Failed to start local server";
    });

    mStartTime = std::chrono::system_clock::now();

    while (!mReady)
    {
        std::this_thread::yield();
        if (mTimeout != -1 && std::chrono::system_clock::now() - mStartTime > std::chrono::seconds(mTimeout))
        {
            fatalstream << "Local server timeout!";
            mLocalServerThread.detach();
            mCallback(false);
            return;
        }
    }
    mCallback(true);
}

void LocalConnectionByNetWork::disconnect()
{
    MultiplayerConnection::disconnect();
    if (mLocalServerThread.joinable())
    {
        mLib.get<void NWAPICALL()>("nwStopServer")();
        mLocalServerThread.join();
    }
}

