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

#include "blockrenderer.h"
#include "chunkclient.h"
#include <cmath>
#include <SDL2/SDL.h>

size_t BlockTextureBuilder::mPPT = 32;
size_t BlockTextureBuilder::mTPL = 8;
std::vector<Texture::RawTexture> BlockTextureBuilder::mRawTexs;
std::vector<std::shared_ptr<BlockRenderer>> BlockRenderer::funcs;

void StandardFullBlockRenderer::flushTex()
{
    infostream << "flushed";
    for (auto i = 0; i < 6; ++i)
        BlockTextureBuilder::getTexPos(tex[i].d, tex[i].pos);
}

void StandardFullBlockRenderer::render(ChunkClient* chunk, const Vec3i& pos)
{
    chunk->stdFullBlock(tex, pos);
}

StandardFullBlockRenderer::StandardFullBlockRenderer(size_t data[])
{
    for (auto i = 0; i < 6; ++i)
        tex[i].pos = data[i];
}

size_t BlockTextureBuilder::capacity()
{
    size_t w = capacityRaw() / mPPT;
    return w * w;
}

size_t BlockTextureBuilder::capacityRaw()
{
    int cap = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &cap);
    return cap;
}

void BlockTextureBuilder::setWidthPerTex(size_t wid)
{
    mPPT = wid;
}

size_t BlockTextureBuilder::getWidthPerTex()
{
    return mPPT;
}

size_t BlockTextureBuilder::push(const Texture::RawTexture& raw)
{
    mRawTexs.push_back(raw);
    return mRawTexs.size() - 1;
}

Texture BlockTextureBuilder::buildAndFlush()
{
    size_t count = mRawTexs.size();
    Assert(count <= capacity());
    int length = static_cast<int>(sqrt(count) + 0.8);
    int texsize = (1 << static_cast<int>(ceil(log2(length))));
    int wid = texsize * mPPT;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    Uint32 masks[] = { 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff };
#else
    Uint32 masks[] = { 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 };
#endif
    auto s = SDL_CreateRGBSurface(0, wid, wid, 32, masks[0], masks[1], masks[2], masks[3]);
    for (size_t i = 0; i < length; ++i)
    {
        auto x = i % texsize;
        auto y = i / texsize;
        SDL_Rect r;
        r.x = x * wid;
        r.y = y * wid;
        r.w = r.h = wid;
        SDL_BlitScaled(mRawTexs[i].getSurface(), NULL, s, &r);
    }
    mTPL = texsize;
    TextureID ret;
    glGenTextures(1, &ret);
    glBindTexture(GL_TEXTURE_2D, ret);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    Texture::build2DMipmaps(GL_RGBA, s->w, s->h, static_cast<int>(log2(s->w)), static_cast<const ubyte*>(s->pixels));
    return Texture(ret);
}

size_t BlockTextureBuilder::push(const char *s)
{
    return BlockTextureBuilder::push(Texture::RawTexture(std::string(s)));
}

size_t BlockTextureBuilder::getTexPerLine()
{
    return mTPL;
}

void BlockTextureBuilder::getTexPos(float *pos, size_t id)
{
    float pct = 1.0 / mTPL;
    auto x = id % mTPL;
    auto y = id / mTPL;
    pos[0] = pct * x;
    pos[1] = pct * y;
    pos[2] = pct * (x + 1);
    pos[3] = pct * (y + 1);
}
