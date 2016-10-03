/*
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
*/

#ifndef NWAPI_H_
#define NWAPI_H_

#include <stdint.h>

#ifndef __cplusplus
    #include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#if defined _WIN32 || defined __CYGWIN__
#ifdef _MSC_VER
#define NWAPIENTRY __declspec(dllimport)
#define NWAPIEXPORT __declspec(dllexport)
#else
#define NWAPIENTRY __attribute__((dllimport))
#define NWAPIEXPORT __attribute__((dllexport))
#endif
#else
#define NWAPIENTRY __attribute__((visibility("default")))
#define NWAPIEXPORT __attribute__((visibility("default")))
#endif

#ifdef _MSC_VER
#define NWAPICALL __cdecl
#else
#define NWAPICALL __attribute__((__cdecl__))
#endif

struct NWvec3i
{
    int32_t x, y, z;
};

struct NWplugindata
{
    const char* pluginName;
    const char* authorName;
    const char* internalName;
    bool isClientPlugin;
};

struct NWblockdata
{
    uint32_t id : 12;
    uint32_t brightness : 4;
    uint32_t state : 16;
};

struct NWblocktype
{
    const char* blockname;
    bool solid;
    bool translucent;
    bool opaque;
    int32_t explodePower;
    int32_t hardness;
};

NWAPIENTRY NWblockdata NWAPICALL nwGetBlock(const NWvec3i* pos);
NWAPIENTRY int32_t NWAPICALL nwSetBlock(const NWvec3i* pos, NWblockdata block);
NWAPIENTRY int32_t NWAPICALL nwRegisterBlock(const NWblocktype*);

#ifdef NEWORLD_PLUGIN_CLIENT_SIDE
// Client-only APIs

#endif

#ifdef NEWORLD_PLUGIN_SERVER_SIDE
// Server-only APIs

typedef void NWAPICALL NWchunkgenerator(const NWvec3i*, NWblockdata*, int32_t);

NWAPIENTRY int32_t NWAPICALL nwRegisterChunkGenerator(NWchunkgenerator* const generator);

#endif

#ifdef __cplusplus
}
#endif

// NEWorld constants
constexpr int ChunkSize = 32;
constexpr int32_t AirID = 0;

// Export functions
extern "C"
{
    NWAPIEXPORT NWplugindata* NWAPICALL getInfo();
    NWAPIEXPORT void NWAPICALL init();
    NWAPIEXPORT void NWAPICALL unload();
}

#endif // !NWAPI_H_

