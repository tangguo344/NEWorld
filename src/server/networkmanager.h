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
#ifndef NETWORKMANAGER_H_
#define NETWORKMANAGER_H_
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdexcept>
#include <thread>
#include <raknet/RakPeerInterface.h>
#include <raknet/MessageIdentifiers.h>
#include <raknet/BitStream.h>
#include "../protocol/gen/protocol.h"
#include <functional>
#include <limits.h>
#include "worldmanager.h"
class GameConnection;

class NetworkManager
{
public:
    NetworkManager(WorldManager& wm);
    ~NetworkManager();
    void close();
    /**
     * Try to start up an server NetworkManager and start listenning on it.
     * @throw std::runtime_error if start up failed
     * @return `true` if start up successfully,`false` if something unexcepted happened
     */
    bool run(const char *addr, unsigned short port);
    void loop();
private:
    GameConnection* newConnection(RakNet::SystemAddress addr);
    void deleteConnection(RakNet::SystemAddress addr);
    RakNet::RakPeerInterface *mPeer;
    std::vector<GameConnection*> mConns;
    WorldManager& mWorlds;//TODO: maybe use event instead later.
};


#endif
