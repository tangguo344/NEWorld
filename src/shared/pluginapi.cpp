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

#include "pluginapi.h"

namespace PluginAPI
{
    // See [1] in pluginapi.h
    BlockManager* Blocks;
    PluginManager* Plugins;
    World* CurrWorld;

    BlockData convertBlockData(const NWblockdata& src)
    {
        return BlockData(src.id, src.brightness, src.state);
    }

    NWblockdata convertBlockData(const BlockData& src)
    {
        NWblockdata res;
        res.id = src.getID();
        res.brightness = src.getBrightness();
        res.state = src.getState();
        return res;
    }

    BlockType convertBlockType(const NWblocktype& src)
    {
        return BlockType(src.blockname, src.solid, src.translucent, src.opaque, src.explodePower, src.hardness);
    }
}

// Export APIs for plugins

extern "C"
{
    // Please don't put `using namespace` in header files (This is a source file 2333)
    using namespace PluginAPI;

    NWAPIEXPORT NWblockdata NWAPICALL nwGetBlock(const NWvec3i* pos)
    {
        return convertBlockData(CurrWorld->getBlock(*pos));
    }

    NWAPIEXPORT int32_t NWAPICALL nwSetBlock(const NWvec3i* pos, NWblockdata block)
    {
        CurrWorld->setBlock(*pos, convertBlockData(block));
        return 0;
    }

    NWAPIEXPORT int32_t NWAPICALL nwRegisterBlock(const NWblocktype* block)
    {
        return Blocks->registerBlock(convertBlockType(*block));
    }

    NWAPIEXPORT void NWAPICALL nwLog(char* str, Logger::Level level)
    {
        Logger("", "", 0, level) << str;
    }
}
