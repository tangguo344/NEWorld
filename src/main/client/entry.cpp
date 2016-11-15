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

// The client version

#define NEWORLD_PLUGIN_CLIENT_SIDE
#include "../shared/shared.h"
NWplugindata* MainPlugin = nullptr;

NWAPIEXPORT NWplugindata* NWAPICALL getInfo()
{
    return getMyInfo(true);
}

// Main function
void NWAPICALL init()
{
    sharedInit();
    NWtextureid id[] =
    {
        nwRegisterTexture("./res/blocks/grass_top.png"),
        nwRegisterTexture("./res/blocks/grass_round.png"),
        nwRegisterTexture("./res/blocks/dirt.png"),
        nwRegisterTexture("./res/blocks/rock.png"),
        nwRegisterTexture("./res/blocks/sand.png")
    };
    NWblocktexture grass{ id[1], id[1], id[0], id[2], id[1], id[1] };
    NWblocktexture rock{ id[3], id[3], id[3], id[3], id[3], id[3] };
    NWblocktexture dirt{ id[2], id[2], id[2], id[2], id[2], id[2] };
    NWblocktexture sand{ id[4], id[4], id[4], id[4], id[4], id[4] };
    nwUseDefaultBlockRenderFunc(GrassID, &grass);
    nwUseDefaultBlockRenderFunc(RockID, &rock);
    nwUseDefaultBlockRenderFunc(DirtID, &dirt);
    nwUseDefaultBlockRenderFunc(SandID, &sand);
}

// Unload function
void NWAPICALL unload()
{
    if (MainPlugin != nullptr) delete MainPlugin;
}
