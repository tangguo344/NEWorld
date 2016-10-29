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

#ifndef PLAYER_MANAGER_H_
#define PLAYER_MANAGER_H_

#include <vec3.h>
#include <aabb.h>
#include <logger.h>

#include <unordered_map>
#include <string>

using PlayerID = std::string; // Player ID is player's name

struct PlayerData
{
    PlayerID m_uid;
    bool m_online;
    std::string m_key;

    Vec3d m_speed, m_direction, m_position, m_rotation, m_scale;
    AABB m_hitbox;
};

class PlayerManager
{
public:
    PlayerData* getPlayer(const PlayerID& uid)
    {
        auto player = m_players.find(uid);
        if (player == m_players.end())
        {
            warningstream << "Failed to find player:" << uid;
            return nullptr;
        }
        return &player->second;
    }

    PlayerData* loginOrRegister(const PlayerID& uid, const std::string& key)
    {
        auto& player = m_players[uid];
        if (player.m_uid.empty())   // new user
        {
            player.m_uid = uid;
            // init player data
        }
        else
        {
            if (player.m_key != key) //wrong password
                return nullptr;
        }

        player.m_online = true;
        return &player;
    }

private:
    std::unordered_map<PlayerID, PlayerData> m_players;
};

#endif // !PLAYER_MANAGER_H_
