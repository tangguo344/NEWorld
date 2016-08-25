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

#ifndef PLAYER_H_
#define PLAYER_H_

#include <playerobject.h>
#include "mat4.h"

class Player : public PlayerObject
{
public:
    void accelerate(const Vec3d& acceleration)
    {
        m_speed += acceleration;
    }

    void setSpeed(const Vec3d& speed)
    {
        m_speed = speed;
    }

    void update() override
    {
        move();
    }

private:
    Vec3d m_speed;

    void move()
    {
        // TODO: Hit test
        //m_speed.normalize();
        //m.speed *= PlayerSpeed;
        m_position +=
            (
                Mat4d::rotation(m_rotation.x, Vec3d(1.0, 0.0, 0.0)) *
                Mat4d::rotation(m_rotation.y, Vec3d(0.0, 1.0, 0.0)) *
                Mat4d::rotation(m_rotation.z, Vec3d(0.0, 0.0, 1.0))
            )
            .transformVec3(m_speed);
        m_speed *= 0.96;
    }
};

#endif // !PLAYER_H_
