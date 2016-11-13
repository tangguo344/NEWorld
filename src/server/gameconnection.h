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

#ifndef GAME_CONNECTION_SERVER_H_
#define GAME_CONNECTION_SERVER_H_

#include "networkmanager.h"
#include "connection.h"
#include "worldmanager.h"

class World;
class Chunk;

class GameConnection
{
public:
    virtual ~GameConnection() = default;

    // Functions
    virtual void sendChunk(Chunk* chunk) = 0;
    virtual void handleReceivedData(Identifier id, unsigned char* data, size_t len) = 0;
};

class MultiplayerConnection : public GameConnection
{
public:
    MultiplayerConnection(WorldManager& wm,NetworkManager &network, RakNet::RakPeerInterface *peer, RakNet::SystemAddress addr)
        :mConn(network,peer,addr), mWorlds(wm) {}

    void sendChunk(Chunk* chunk) override;
private:
    void handleReceivedData(Identifier id, unsigned char* data, size_t len) override;
    Connection mConn;
    WorldManager& mWorlds;
};
#endif
