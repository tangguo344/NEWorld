#include "worldgen.h"
#include "../shared/shared.h"
int WorldGen::seed = 3404;
double WorldGen::NoiseScaleX = 64;
double WorldGen::NoiseScaleZ = 64;
// Chunk generator
void NWAPICALL generator(const NWvec3i * pos, NWblockdata * blocks, int daylightBrightness)
{
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

double WorldGen::InterpolatedNoise(double x, double y)
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

double WorldGen::PerlinNoise2D(double x, double y)
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
