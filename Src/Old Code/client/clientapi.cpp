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
#include "renderer/blockrenderer.h"
#include <api/nwapi.h>

extern "C"
{
    NWAPIEXPORT void NWAPICALL nwSetBlockRenderFunc(size_t, NWblockrenderfunc)
    {

    }

    NWAPIEXPORT void NWAPICALL nwUseDefaultBlockRenderFunc(size_t id, void *data)
    {
        NWblocktexture* ptr = reinterpret_cast<NWblocktexture*>(data);
        size_t array[] = { ptr->right, ptr->left, ptr->top, ptr->bottom, ptr->front, ptr->back };
        BlockRendererManager::setBlockRenderer(id, std::make_shared<DefaultBlockRenderer>(array));
    }

    NWAPIEXPORT NWtextureid NWAPICALL nwRegisterTexture(const char* filename)
    {
        return BlockTextureBuilder::addTexture(filename);
    }
}
