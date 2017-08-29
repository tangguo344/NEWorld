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
    mPositionDelta = Mat4d::rotation(mRotation.y, Vec3d(0.0, 1.0, 0.0)).transformVec3(mSpeed);
    Vec3d originalDelta = mPositionDelta;
    std::vector<AABB> hitboxes = getWorldPtr()->getHitboxes(getHitbox().expand(mPositionDelta));

    for (auto& curr : hitboxes)
        mPositionDelta.x = getHitbox().maxMoveOnXclip(curr, mPositionDelta.x);
    moveHitbox(Vec3d(mPositionDelta.x, 0.0, 0.0));
    if (mPositionDelta.x != originalDelta.x) mSpeed.x = 0.0;

    for (auto& curr : hitboxes)
        mPositionDelta.z = getHitbox().maxMoveOnZclip(curr, mPositionDelta.z);
    moveHitbox(Vec3d(0.0, 0.0, mPositionDelta.z));
    if (mPositionDelta.z != originalDelta.z) mSpeed.z = 0.0;

    for (auto& curr : hitboxes)
        mPositionDelta.y = getHitbox().maxMoveOnYclip(curr, mPositionDelta.y);
    moveHitbox(Vec3d(0.0, mPositionDelta.y, 0.0));
    if (mPositionDelta.y != originalDelta.y) mSpeed.y = 0.0;

    mPosition += mPositionDelta;

    mSpeed *= 0.8;
    //mSpeed += Vec3d(0.0, -0.05, 0.0);
}

void Player::rotationMove()
{
    if (mRotation.x + mRotationSpeed.x > 90.0)
        mRotationSpeed.x = 90.0 - mRotation.x;
    if (mRotation.x + mRotationSpeed.x < -90.0)
        mRotationSpeed.x = -90.0 - mRotation.x;
    mRotation += mRotationSpeed;
    mRotationDelta = mRotationSpeed;
    mRotationSpeed *= 0.6;
}

void Player::render()
{
    // Player model not finished yet
    /*
    glDisable(GL_CULL_FACE);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    // X
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(mPosition.x - 2.0f, mPosition.y, mPosition.z);
    glVertex3f(mPosition.x + 2.0f, mPosition.y, mPosition.z);
    glEnd();
    //Y
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(mPosition.x, mPosition.y - 2.0f, mPosition.z);
    glVertex3f(mPosition.x, mPosition.y + 2.0f, mPosition.z);
    glEnd();
    //Z
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(mPosition.x, mPosition.y, mPosition.z - 2.0f);
    glVertex3f(mPosition.x, mPosition.y, mPosition.z + 2.0f);
    glEnd();
    glEnable(GL_CULL_FACE);
    */
}