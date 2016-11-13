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

#ifndef NWAPICALL
    #ifdef _MSC_VER
        #define NWAPICALL __cdecl
    #else
        #define NWAPICALL __attribute__((__cdecl__))
    #endif
#endif

// NEWorld constants

const int NWChunkSize = 32;
const int32_t NWAirID = 0;

// NEWorld structures

struct NWvec3i
{
    int32_t x, y, z;
};

enum
{
    nwPluginTypeClientOnly,
    nwPluginTypeServerOnly,
    nwPluginTypeShared
};

struct NWplugindata
{
    const char* pluginName;
    const char* authorName;
    const char* internalName;
    int32_t pluginType;
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
NWAPIENTRY int NWAPICALL nwSetBlock(const NWvec3i* pos, NWblockdata block);
NWAPIENTRY size_t NWAPICALL nwRegisterBlock(const NWblocktype*);

#ifdef NEWORLD_PLUGIN_CLIENT_SIDE
    // Client-only APIs

    typedef size_t NWtextureid;
    typedef void(*NWblockrenderfunc)(void* cthis, NWblockdata data, int x, int y, int z);

    struct NWblocktexture
    {
        NWtextureid right, left, top, bottom, front, back;
    };

    NWAPIENTRY NWtextureid NWAPICALL nwRegisterTexture(const char* filename);
    NWAPIENTRY void NWAPICALL nwSetBlockRenderFunc(size_t id, NWblockrenderfunc func);
    NWAPIENTRY void NWAPICALL nwUseDefaultBlockRenderFunc(size_t id, void* data);

#endif

#ifdef NEWORLD_PLUGIN_SERVER_SIDE
    // Server-only APIs

    typedef void NWAPICALL NWchunkgenerator(const NWvec3i*, NWblockdata*, int32_t);
    NWAPIENTRY size_t NWAPICALL nwRegisterChunkGenerator(NWchunkgenerator* const generator);

#endif

// Plugin exported functions
NWAPIENTRY NWplugindata* NWAPICALL getInfo();
NWAPIENTRY void NWAPICALL init();
NWAPIENTRY void NWAPICALL unload();

#ifdef __cplusplus
}
#endif

#endif // !NWAPI_H_
