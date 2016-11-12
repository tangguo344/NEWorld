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
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
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

// NEWorld constants
const int ChunkSize = 32;
const int32_t AirID = 0;

// NEWorld structures

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

// NEWorld APIs

NWAPIENTRY NWblockdata NWAPICALL nwGetBlock(const NWvec3i* pos);
NWAPIENTRY size_t NWAPICALL nwSetBlock(const NWvec3i* pos, NWblockdata block);
NWAPIENTRY size_t NWAPICALL nwRegisterBlock(const NWblocktype*);

#ifdef NEWORLD_PLUGIN_CLIENT_SIDE

#define NWRENDERFUNCSTDFULLBLOCKSAMEFACE  0x0001
#define NWRENDERFUNCSTDFULLBLOCKROUNDFACE 0x0002
#define NWRENDERFUNCSTDFULLBLOCKDIFFFACE  0x0003
#define NWRENDERFUNCSTDHALFBLOCKSAMEFACE  0x0004
#define NWRENDERFUNCSTDHALFBLOCKROUNDFACE 0x0005
#define NWRENDERFUNCSTDHALFBLOCKDIFFFACE  0x0006

struct NWSTDSameFaceTexGroup
{
    size_t tex;
};

struct NWSTDRoundFaceTexGroup
{
    size_t texTop, texBottom, texRound;
};

struct NWSTDDiffFaceTexGroup
{
    size_t texTop, texBottom, texLeft, texRight, texFront, texBack;
};

typedef void(*nwBlockRenderFunc)(void* cthis, NWblockdata data, int32_t x, int32_t y, int32_t z);
NWAPIEXPORT void NWAPICALL nwSetBlockRenderFunc(int32_t id, nwBlockRenderFunc func);
NWAPIEXPORT void NWAPICALL nwUseStandardRenderFunc(int32_t id, int32_t func, void* data);

#endif

#ifdef NEWORLD_PLUGIN_SERVER_SIDE
// Server-only APIs

typedef void NWAPICALL NWchunkgenerator(const NWvec3i*, NWblockdata*, int32_t);

NWAPIENTRY int32_t NWAPICALL nwRegisterChunkGenerator(NWchunkgenerator* const generator);

#endif

// Plugin exported functions
NWAPIEXPORT NWplugindata* NWAPICALL getInfo();
NWAPIEXPORT void NWAPICALL init();
NWAPIEXPORT void NWAPICALL unload();

#ifdef __cplusplus
}
#endif

#endif // !NWAPI_H_
