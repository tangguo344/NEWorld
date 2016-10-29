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
#ifndef IDENTIFIER_H_
#define IDENTIFIER_H_

#include <cinttypes>
#include "../protocol/gen/protocol.h"

enum Identifier :uint32_t
{
    //Client to server (0 ~ 2^30-1)
    Login = 0,
    //Server to client (2^30 ~ 2*2^30-1)
    Close = 1u << 30,
    //Common (2*2^30 ~ 3*2^30-1)
    Chat = (1u << 30) * 2
};

#endif // !IDENTIFIER_H_
