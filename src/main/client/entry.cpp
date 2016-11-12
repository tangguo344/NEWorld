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
#include <logger.h>
NWplugindata* MainPlugin = nullptr;

NWAPIEXPORT NWplugindata* NWAPICALL getInfo()
{
    return getInfo(true);
}

// Main function
void NWAPICALL init()
{
    sharedInit();
    NWSTDSameFaceTexGroup rock{nwRegisterTexture("./res/test.png")};
    nwUseStandardRenderFunc(1, NWRENDERFUNCSTDFULLBLOCKSAMEFACE, &rock);
}

// Unload function
void NWAPICALL unload()
{
    if (MainPlugin != nullptr) delete MainPlugin;
}
