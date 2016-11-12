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

#ifndef BLOCKRENDERER_H
#define BLOCKRENDERER_H

#include "texture.h"
#include <vector>
#include <memory>
#include "vec3.h"

struct BlockTexCrood
{
    size_t pos = 0;
    float d[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
};

class BlockRenderer
{
public:
    virtual void flushTex() = 0;
    virtual void render(class ChunkClient* chunk, const Vec3i& pos) = 0;
    virtual ~BlockRenderer() = default;
    //RenderList
    static void invoke(size_t id, class ChunkClient* chunk, const Vec3i& pos);
    static std::vector<std::shared_ptr<BlockRenderer>> funcs;
};

class StandardFullBlockRenderer : public BlockRenderer
{
public:
    StandardFullBlockRenderer(size_t data[]);
    void flushTex() override;
    void render(class ChunkClient* chunk, const Vec3i& pos) override;
    BlockTexCrood tex[6];
};

class BlockTextureBuilder
{
public:
    static size_t capacity();
    static size_t capacityRaw();
    static void setWidthPerTex(size_t wid);
    static size_t getWidthPerTex();
    static size_t push(const char* s);
    static size_t push(const Texture::RawTexture& raw);
    //Finalize Func
    static Texture buildAndFlush();
    //In
    static size_t getTexPerLine();
    static void getTexPos(float pos[], size_t id);
private:
    static size_t mPPT, mTPL;
    static std::vector<Texture::RawTexture> mRawTexs;
};

#endif //BLOCKRENDERER_H
