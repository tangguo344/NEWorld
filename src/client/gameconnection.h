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

#ifndef GAME_CONNECTION_H_
#define GAME_CONNECTION_H_
#include <string>
#include "network.h"
#include <common.h>

class World;

class GameConnection
{
public:
    virtual ~GameConnection() = default;
    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual void waitForConnected() = 0;

    // Functions
    virtual void login(const char* username, const char* password) = 0;
    virtual World* getWorld(size_t id) = 0;
};

class MultiplayerConnection : public GameConnection
{
public:
    MultiplayerConnection(std::string host,unsigned int port):mHost(host),mPort(port) {}
    void connect() override;
    void disconnect() override;
    World* getWorld(size_t id) override;
    void waitForConnected() override;

    // Functions
    void login(const char* username, const char* password) override;
private:
    Connection mConn;
    std::string mHost;
    unsigned int mPort;
    flatbuffers::FlatBufferBuilder mFbb;
};
#endif
