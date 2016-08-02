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

#include "../../../api/cpp/nwapi.h"

NWplugindata* TestPlugin = nullptr;

// NEWorld constants
constexpr int ChunkSize = 32;

// Block IDs
int32_t rockID;

// Export functions
extern "C"
{
    NWAPIEXPORT NWplugindata* NWAPICALL init();
    NWAPIEXPORT void NWAPICALL unload();
}

// Chunk generator
void NWAPICALL generator(const NWvec3i* pos, NWblockdata* blocks, int daylightBrightness)
{
    if (pos->y >= 2)
    {
        for (int x = 0; x < ChunkSize; x++)
            for (int z = 0; z < ChunkSize; z++)
                for (int y = 0; y < ChunkSize; y++)
                {
                    NWblockdata &block = blocks[x*ChunkSize*ChunkSize + y*ChunkSize + z];
                    block.id = block.state = 0;
                    block.brightness = daylightBrightness;
                }
    }
    else
    {
        for (int x = 0; x < ChunkSize; x++)
            for (int z = 0; z < ChunkSize; z++)
                for (int y = 0; y < ChunkSize; y++)
                {
                    NWblockdata &block = blocks[x*ChunkSize*ChunkSize + y*ChunkSize + z];
                    block.id = rockID;
                    block.brightness = block.state = 0;
                }
    }
}

// Main function
NWplugindata* NWAPICALL init()
{
    NWblocktype rock;
    rock.blockname = "Rock";
    rock.solid = 1;
    rock.translucent = 0;
    rock.opaque = 1;
    rock.explodePower = 0;
    rock.hardness = 2;
    rockID = nwRegisterBlock(&rock);

    nwRegisterChunkGenerator(generator);

    TestPlugin = new NWplugindata();
    TestPlugin->pluginName = "Test Plugin";
    TestPlugin->authorName = "INFINIDEAS";
    TestPlugin->internalName = "infinideas.testplugin_cpp";
    return TestPlugin;
}

// Unload function
void NWAPICALL unload()
{
    if (TestPlugin != nullptr) delete TestPlugin;
}
