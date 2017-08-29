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

#include <world/world.h>
#include <world/playerobject.h>
#include <renderer/renderer.h>

class Player : public PlayerObject
{
public:
    Player(const World* world) : PlayerObject(world)
    {
    }

    void accelerate(const Vec3d& acceleration)
    {
        mSpeed += acceleration;
    }

    void accelerateRotation(const Vec3d& acceleration)
    {
        mRotationSpeed += acceleration;
    }

    void setSpeed(const Vec3d& speed)
    {
        mSpeed = speed;
    }

    Vec3d getPositionDelta() const
    {
        return mPositionDelta;
    }

    Vec3d getRotationDelta() const
    {
        return mRotationDelta;
    }

    void render() override;

    void update() override
    {
        move();
        rotationMove();
    }

private:
    Vec3d mSpeed, mRotationSpeed;
    Vec3d mPositionDelta, mRotationDelta;

    void move();
    void rotationMove();
};

#endif // !PLAYER_H_
