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

#include "player.h"

void Player::move()
{
    //mSpeed.normalize();
    //m.speed *= PlayerSpeed;
    Vec3d delta = Mat4d::rotation(mRotation.y, Vec3d(0.0, 1.0, 0.0)).transformVec3(mSpeed);
    std::vector<AABB> hitboxes = getWorldPtr()->getHitboxes(getHitbox().expand(delta));

    for (auto& curr : hitboxes)
        delta.x = getHitbox().maxMoveOnXclip(curr, delta.x);
    mPosition.x += delta.x;
    moveHitbox(Vec3d(delta.x, 0.0, 0.0));

    for (auto& curr : hitboxes)
        delta.z = getHitbox().maxMoveOnZclip(curr, delta.z);
    mPosition.z += delta.z;
    moveHitbox(Vec3d(0.0, 0.0, delta.z));

    for (auto& curr : hitboxes)
        delta.y = getHitbox().maxMoveOnYclip(curr, delta.y);
    mPosition.y += delta.y;
    moveHitbox(Vec3d(0.0, delta.y, 0.0));

    mSpeed *= 0.8;
    //mSpeed += Vec3d(0.0, -0.05, 0.0);
}
