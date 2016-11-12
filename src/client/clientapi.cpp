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
extern "C"
{
    NWAPIEXPORT void NWAPICALL nwSetBlockRenderFunc(size_t id, nwBlockRenderFunc func)
    {

    }

    NWAPIEXPORT void NWAPICALL nwUseStandardRenderFunc(size_t id, size_t func, void *data)
    {
        switch (func)
        {
        case nwRenderFuncStdFullBlockSameFace:
        {
            auto t = reinterpret_cast<NWSTDSameFaceTexGroup*>(data)->tex;
            size_t a[] = {t, t, t, t, t, t};
            BlockRendererManager::setBlockRenderer(id, std::make_shared<StandardFullBlockRenderer>(a));
            break;
        }
        case nwRenderFuncStdFullBlockRoundFace:
        {
            auto p = reinterpret_cast<NWSTDRoundFaceTexGroup*>(data);
            auto t = p->texTop, b = p->texBottom, r = p->texRound;
            size_t a[] = {r, r, t, b, r, r};
            BlockRendererManager::setBlockRenderer(id, std::make_shared<StandardFullBlockRenderer>(a));
            break;
        }
        case nwRenderFuncStdFullBlockDiffFace:
        {
            auto p = reinterpret_cast<NWSTDDiffFaceTexGroup*>(data);
            size_t a[] = {p->texRight, p->texLeft, p->texTop, p->texBottom, p->texFront, p->texBack};
            BlockRendererManager::setBlockRenderer(id, std::make_shared<StandardFullBlockRenderer>(a));
            break;
        }
        default:
            break;
        };
    }

    NWAPIEXPORT size_t NWAPICALL nwRegisterTexture(const char* path)
    {
        return BlockTextureBuilder::addTexture(path);
    }

}
