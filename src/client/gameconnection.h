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

class World;

class GameConnection
{
public:
    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual World* getWorld(size_t id) = 0;
    virtual ~GameConnection() = default;
};

class MultiplayerConnection : public GameConnection
{
public:
    void connect() override;
    void disconnect() override;
    World* getWorld(size_t id) override;
};
#endif