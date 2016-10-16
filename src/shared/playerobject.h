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

#ifndef PLAYEROBJECT_H_
#define PLAYEROBJECT_H_

#include "object.h"

const double PlayerHeight = 1.6;
const double PlayerWidth = 0.6;
const Vec3d PlayerHitboxSize = Vec3d(PlayerWidth, PlayerHeight, PlayerWidth);

class PlayerObject : public Object
{
public:
    PlayerObject(const World* world)
        : Object(world, Vec3d(), Vec3d(), Vec3d(1.0), AABB(-PlayerHitboxSize / 2.0, PlayerHitboxSize / 2.0))
    {
    }

    PlayerObject(const Object& obj) : Object(obj)
    {
    }

    ~PlayerObject()
    {
    }

    void rotate(const Vec3d& rotation)
    {
        m_rotation += rotation;
    }

    void setDirection(const Vec3d& direction)
    {
        m_rotation = direction;
    }

    Vec3d getDirection() const
    {
        return m_rotation;
    }

    void setSpeed(double speed)
    {
        m_speed = speed;
    }

    double getSpeed() const
    {
        return m_speed;
    }

private:
    Vec3d m_direction; // Body direction, head direction is `m_rotation` in class Object
    double m_speed = 0.2;
};

#endif
