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

#ifndef PLUGINAPI_H_
#define PLUGINAPI_H_

#include "common.h"
#include "logger.h"
#include "vec3.h"
#include "blockdata.h"
#include "blocktype.h"
#include "blockmanager.h"
#include "world.h"

namespace PluginAPI {
    /*
        [1]
        这里用了几个指针，请不要介意，因为理论上Plugin API是可以直接控制NEWorld和Server的，
        包括他们中的很多东西比如World、BlockManager啦，然后我就给了个指针让他操作。。。如果
        不这么弄其实也可以，只是结构会比较奇怪，NEWorld或Server类要组合到Plugin API里。。。
        这个结构找个时间确定一下，我先暂时这么写吧。。。
        -- qiaozhanrong
    */

    extern BlockManager* Blocks;
    extern PluginManager* Plugins;
    extern World* CurrWorld;

    // Structures for plugin interface
    // Aliases cannot be used when structure definitions in NEWorld and in Plugin API are different

    using NWvec3i = Vec3i;

    struct NWblockdata {
        uint32_t id : 12;
        uint32_t brightness : 4;
        uint32_t state : 16;
    };

    struct NWblocktype {
        char* blockname = nullptr;
        bool solid;
        bool translucent;
        bool opaque;
        int32_t explodePower;
        int32_t hardness;
    };

    // Conversions between plugin structures and NEWorld structures
    // This is used when structure definitions in NEWorld and in Plugin API are different

    BlockData convertBlockData(const NWblockdata& src);
    NWblockdata convertBlockData(const BlockData& src);
    BlockType convertBlockType(const NWblocktype& src);

}

#endif
