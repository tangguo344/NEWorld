/'
* NEWorld: A free game with similar rules to Minecraft.
* Copyright (C) 2016 NEWorld Team
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
'/

#ifndef NWAPI_BI_
#define NWAPI_BI_

#ifdef NEWORLD_PLUGIN_SERVER_SIDE
    #inclib "../../../Bin/NEWorldServer"
#else
    #inclib "../../../Bin/NEWorld"
#endif

#define NWAPICALL cdecl

type int32_t as long
type uint32_t as ulong

type NWvec3i
    x as uint32_t
    y as uint32_t
    z as uint32_t
end type

type NWplugindata
    pluginName as zstring ptr = 0
    authorName as zstring ptr = 0
    internalName as zstring ptr = 0
end type

type NWblockdata
    id : 12 as uint32_t
    brightness : 4 as uint32_t
    state : 16 as uint32_t
end type

type NWblocktype
    blockname as zstring ptr = 0
    solid as byte
    translucent as byte
    opaque as byte
    explodePower as int32_t
    hardness as int32_t
end type

declare function nwGetBlock NWAPICALL alias "nwGetBlock" (byval as const NWvec3i ptr) as NWblockdata
declare sub nwSetBlock NWAPICALL alias "nwSetBlock" (byval as const NWvec3i ptr, byval as NWblockdata)
declare function nwRegisterBlock NWAPICALL alias "nwRegisterBlock" (byval as const NWblocktype ptr) as int32_t

#endif ' !NWAPI_BI_
