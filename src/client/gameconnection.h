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

#ifndef GAME_CONNECTION_CLIENT_H_
#define GAME_CONNECTION_CLIENT_H_

#include <string>
#include <functional>
#include "network.h"
#include "worldclient.h"
#include <common/vec3.h>
#include <common/common.h>
#include <common/library.h>

class World;
class Chunk;

class ClientGameConnection
{
public:
    virtual ~ClientGameConnection() = default;
    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual void waitForConnected() = 0;
    virtual void login(const char* username, const char* password) = 0;

    // Functions
    virtual void getChunk(Vec3i pos) = 0;
    virtual World* getWorld(size_t id) = 0;

    // Callbacks
    void setWorld(WorldClient* w){ mWorld = w; }

protected:
    WorldClient* mWorld;
};

class MultiplayerConnection : public ClientGameConnection
{
public:
    MultiplayerConnection(const std::string& host, unsigned short port);
    virtual ~MultiplayerConnection() = default;
    void connect() override;
    void disconnect() override;
    void waitForConnected() override;
    void login(const char* username, const char* password) override;

    // Functions
    void getChunk(Vec3i pos) override;
    World* getWorld(size_t id) override;
protected:
    ClientConnection mConn;

private:
    std::string mHost;
    unsigned short mPort;
    flatbuffers::FlatBufferBuilder mFbb;
};

// Tunnel Connection : Client Side
class LocalConnection : public ClientGameConnection
{
public:
    LocalConnection();
    ~LocalConnection()
    {
        LocalConnection::disconnect();
    }
    void connect() override;
    void disconnect() override;
    void waitForConnected() override;
    void login(const char* username, const char* password) override;

    // Functions
    void getChunk(Vec3i pos) override;
    World* getWorld(size_t id) override;
private:
    std::thread mLocalServerThread;
    std::atomic_bool mReady{ false };
    int mTimeout;
    std::chrono::system_clock::time_point mStartTime;
};

#endif
