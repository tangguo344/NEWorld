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
#include "chunkrenderer.h"
#include <cmath>
#include <SDL2/SDL.h>

size_t BlockTextureBuilder::mPixelPerTexture = 32;
size_t BlockTextureBuilder::mTexturePerLine = 8;
std::vector<Texture::RawTexture> BlockTextureBuilder::mRawTexs;
std::vector<std::shared_ptr<BlockRenderer>> BlockRendererManager::mBlockRenderers;

void BlockRendererManager::render(size_t id, Chunk* chunk, const Vec3i& pos)
{
    if (mBlockRenderers[id]) mBlockRenderers[id]->render(chunk, pos);
}

void BlockRendererManager::setBlockRenderer(size_t pos, std::shared_ptr<BlockRenderer>&& blockRenderer)
{
    if (pos >= mBlockRenderers.size()) 
        mBlockRenderers.resize(pos + 1);
    mBlockRenderers[pos] = std::move(blockRenderer);
}

void BlockRendererManager::flushTextures()
{
    for (auto& x : mBlockRenderers)
        if (x) x->flushTexture();
}

void DefaultBlockRenderer::flushTexture()
{
    for (auto i = 0; i < 6; ++i)
        BlockTextureBuilder::getTexturePos(tex[i].d, tex[i].pos);
}

void DefaultBlockRenderer::render(Chunk* chunk, const Vec3i& pos)
{
    ChunkRenderer::renderBlock(chunk, tex, pos);
}

DefaultBlockRenderer::DefaultBlockRenderer(size_t data[])
{
    for (auto i = 0; i < 6; ++i)
        tex[i].pos = data[i];
}

size_t BlockTextureBuilder::capacity()
{
    size_t w = capacityRaw() / mPixelPerTexture;
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
    mPixelPerTexture = wid;
}

size_t BlockTextureBuilder::getWidthPerTex()
{
    return mPixelPerTexture;
}

size_t BlockTextureBuilder::addTexture(const Texture::RawTexture& rawTexture)
{
    mRawTexs.push_back(rawTexture);
    return mRawTexs.size() - 1;
}

Texture BlockTextureBuilder::buildAndFlush()
{
    size_t count = mRawTexs.size();
    Assert(count <= capacity());
    mTexturePerLine = (1 << static_cast<int>(ceil(log2(ceil(sqrt(count))))));
    auto wid = mTexturePerLine * mPixelPerTexture;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    Uint32 masks[] = { 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff };
#else
    Uint32 masks[] = { 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 };
#endif
    auto s = SDL_CreateRGBSurface(0, wid, wid, 32, masks[0], masks[1], masks[2], masks[3]);
    for (size_t i = 0; i < count; ++i)
    {
        auto x = i % mTexturePerLine;
        auto y = i / mTexturePerLine;
        SDL_Rect r;
        r.x = x * mPixelPerTexture;
        r.y = y * mPixelPerTexture;
        r.w = r.h = mPixelPerTexture;
        SDL_BlitScaled(mRawTexs[i].getSurface(), nullptr, s, &r);
    }
    mRawTexs.clear();
    TextureID ret;
    glGenTextures(1, &ret);
    glBindTexture(GL_TEXTURE_2D, ret);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    Texture::build2DMipmaps(GL_RGBA, s->w, s->h, static_cast<int>(log2(mPixelPerTexture)), static_cast<const ubyte*>(s->pixels));
    return Texture(ret);
}

size_t BlockTextureBuilder::addTexture(const char* path)
{
    return addTexture(Texture::RawTexture(std::string(path)));
}

size_t BlockTextureBuilder::getTexturePerLine()
{
    return mTexturePerLine;
}

void BlockTextureBuilder::getTexturePos(float *pos, size_t id)
{
    float percentagePerTexture = 1.0f / mTexturePerLine;
    auto x = id % mTexturePerLine;
    auto y = id / mTexturePerLine;
    pos[0] = percentagePerTexture * x;
    pos[1] = percentagePerTexture * y;
    pos[2] = percentagePerTexture * (x + 1);
    pos[3] = percentagePerTexture * (y + 1);
}
