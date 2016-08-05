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

#ifndef PLAYERCONTROL_H_
#define PLAYERCONTROL_H_

#include <object.h>

class PlayerControl : public Object
{
public:
    PlayerControl() : Object(), m_direction(0.0, 0.0, 0.0)
    {
    }

    ~PlayerControl()
    {
    }

    void accelerate(const Vec3d& acceleration)
    {
        m_speed += acceleration;
    }

    void setSpeed(const Vec3d& speed)
    {
        m_speed = speed;
    }

    void rotate(const Vec3d& rotate)
    {
        m_direction += rotate;
    }

    void setDirection(const Vec3d& direction)
    {
        m_direction = direction;
    }

    const Vec3d& getDirection() const
    {
        return m_direction;
    }

    void move()
    {

    }

    void update()
    {
        move();
    }

private:
    Vec3d m_speed, m_direction;

};

#endif // !PLAYERCONTROL_H_
