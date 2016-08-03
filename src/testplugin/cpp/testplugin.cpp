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
#include <cmath>

NWplugindata* TestPlugin = nullptr;

// NEWorld constants
constexpr int ChunkSize = 32;
constexpr int32_t AirID = 0;

// Block IDs
int32_t RockID;

// Export functions
extern "C"
{
    NWAPIEXPORT NWplugindata* NWAPICALL init();
    NWAPIEXPORT void NWAPICALL unload();
}

// Perlin Noise 2D, copied from old NEWorld
namespace WorldGen
{
    int seed = 3404;
    double NoiseScaleX = 64;
    double NoiseScaleZ = 64;

    inline double Noise(int x, int y)
    {
        long long xx = x + y * 13258953287;
        xx = xx >> 13 ^ xx;
        return (xx*(xx*xx * 15731 + 789221) + 1376312589 & 0x7fffffff) / 16777216.0;
    }

    inline double Interpolate(double a, double b, double x)
    {
        return a*(1.0 - x) + b*x;
    }

    double InterpolatedNoise(double x, double y)
    {
        int int_X, int_Y;
        double fractional_X, fractional_Y, v1, v2, v3, v4, i1, i2;
        int_X = int(floor(x));
        fractional_X = x - int_X;
        int_Y = int(floor(y));
        fractional_Y = y - int_Y;
        v1 = Noise(int_X, int_Y);
        v2 = Noise(int_X + 1, int_Y);
        v3 = Noise(int_X, int_Y + 1);
        v4 = Noise(int_X + 1, int_Y + 1);
        i1 = Interpolate(v1, v2, fractional_X);
        i2 = Interpolate(v3, v4, fractional_X);
        return Interpolate(i1, i2, fractional_Y);
    }

    double PerlinNoise2D(double x, double y)
    {
        double total = 0, frequency = 1, amplitude = 1;
        for (int i = 0; i <= 4; i++)
        {
            total += InterpolatedNoise(x*frequency, y*frequency)*amplitude;
            frequency *= 2;
            amplitude /= 2.0;
        }
        return total;
    }

    inline int getHeight(int x, int y)
    {
        return int(PerlinNoise2D(x / NoiseScaleX, y / NoiseScaleZ)) / 4 - 32;
    }
}

// Chunk generator
void NWAPICALL generator(const NWvec3i* pos, NWblockdata* blocks, int daylightBrightness)
{
    /*
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
    */
    for (int x = 0; x < ChunkSize; x++)
        for (int z = 0; z < ChunkSize; z++)
        {
            int height = WorldGen::getHeight(x, z);
            for (int y = 0; y < ChunkSize; y++)
            {
                NWblockdata &block = blocks[x*ChunkSize*ChunkSize + y*ChunkSize + z];
                if (y <= height)
                {
                    block.id = RockID;
                    block.brightness = block.state = 0;
                }
                else
                {
                    block.id = AirID;
                    block.brightness = daylightBrightness;
                    block.state = 0;
                }
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
    RockID = nwRegisterBlock(&rock);
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
