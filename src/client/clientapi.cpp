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

#define NEWORLD_PLUGIN_CLIENT_SIDE

#include <memory>
#include <string>
#include "blockrenderer.h"
#include "../../api/c/nwapi.h"

void expandFuncGroup(int32_t size)
{
    if (size >= BlockRenderer::funcs.size())
        BlockRenderer::funcs.resize(size + 1);
}

extern "C"
{
NWAPIEXPORT void NWAPICALL nwSetBlockRenderFunc(int32_t id, nwBlockRenderFunc func)
{

}

NWAPIEXPORT void NWAPICALL nwUseStandardRenderFunc(int32_t id, int32_t func, void *data)
{
    expandFuncGroup(id);
    switch (func)
    {
        case NWRENDERFUNCSTDFULLBLOCKSAMEFACE:
        {
            auto t = reinterpret_cast<NWSTDSameFaceTexGroup*>(data)->tex;
            decltype(t) a[] = {t, t, t, t, t, t};
            BlockRenderer::funcs[id] =
                    std::make_shared<StandardFullBlockRenderer>(a);
            break;
        }
        default:
            break;
    };
}

NWAPIEXPORT int32_t NWAPICALL nwRegisterTexture(const char* path)
{
    return static_cast<int32_t>(BlockTextureBuilder::push(path));
}

}
