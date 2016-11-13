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
#include <flatfactory.h>

MultiplayerConnection::MultiplayerConnection(const std::string& host, unsigned short port)
    :mConn([this](Identifier id, unsigned char* data, size_t len)
{
    flatbuffers::Verifier v(data,len);
    switch (id)
    {
    case Identifier::s2cChunk:
    {
        if(!s2c::VerifyChunkBuffer(v)) break;
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
    mConn.stop();
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

void MultiplayerConnection::login(const char *username, const char *password)
{
    mConn.send<c2s::Login>(FlatFactory::c2s::Login(username,password),PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE_ORDERED_WITH_ACK_RECEIPT);
}

void MultiplayerConnection::getChunk(Vec3i pos)
{
    mConn.send<c2s::RequestChunk>(FlatFactory::c2s::RequestChunk(pos), PacketPriority::MEDIUM_PRIORITY, PacketReliability::UNRELIABLE);
}

LocalConnectionByNetWork::LocalConnectionByNetWork(const std::string& host, unsigned short port)
    :MultiplayerConnection(host, port),
    mPath(getJsonValue<std::string>(getSettings()["server"]["file"], "nwserver").c_str()),
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
        bool opened = mLib.get<bool NWAPICALL(int, char**, bool)>("nwInitServer")(sizeof(argv) / sizeof(argv[0]), const_cast<char**>(argv),true);
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
    if (mLocalServerThread.joinable())
    {
        debugstream << "Call nwStopServer";
        mLib.get<void NWAPICALL()>("nwStopServer")();
        debugstream << "Waiting for local server thread...";
        mLocalServerThread.join();
        debugstream << "Local server thread exited!";
    }
}

