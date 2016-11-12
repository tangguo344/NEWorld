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

size_t BlockTextureBuilder::mPPT;
std::vector<Texture::RawTexture> BlockTextureBuilder::mRawTexs;
std::vector<std::shared_ptr<BlockRenderer>> BlockRenderer::funcs;

void StandardFullBlockRenderer::flushTex()
{

}

void StandardFullBlockRenderer::render(ChunkClient* chunk, const Vec3i& pos)
{
    chunk->stdFullBlock(tex, pos);
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
}

Texture BlockTextureBuilder::buildAndFlush()
{

}
