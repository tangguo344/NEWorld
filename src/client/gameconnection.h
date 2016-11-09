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
#include "network.h"
#include <common.h>
#include <functional>
#include <vec3.h>
#include "worldclient.h"

class World;
class Chunk;

class GameConnection
{
public:
    virtual ~GameConnection() = default;
    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual void waitForConnected() = 0;

    // Functions
    virtual void login(const char* username, const char* password) = 0;
    using ChunkCallback = std::function<void(Chunk*)>;
    virtual void getChunk(size_t worldID, Vec3i pos) = 0;
    virtual World* getWorld(size_t id) = 0;

    // Callbacks
    virtual void setChunkCallback(ChunkCallback callback) = 0;
};

class MultiplayerConnection : public GameConnection
{
public:
    MultiplayerConnection(std::string host,unsigned int port, WorldClient* worldNow)
        :mHost(host),mPort(port),mConn([this](Identifier id, unsigned char* data)
    {
        handleReceivedData(id, data);
    }),mWorld(worldNow) {}
    void connect() override;
    void disconnect() override;
    World* getWorld(size_t id) override;
    void waitForConnected() override;

    // Functions
    void login(const char* username, const char* password) override;
    void getChunk(size_t worldID, Vec3i pos) override;

    // Callbacks
    void setChunkCallback(ChunkCallback callback) override
    {
        mChunkCallback = callback;
    };

private:
    void handleReceivedData(Identifier id, unsigned char* data);
    Connection mConn;
    std::string mHost;
    unsigned int mPort;
    flatbuffers::FlatBufferBuilder mFbb;
    ChunkCallback mChunkCallback;
    WorldClient* mWorld;
};

#endif
