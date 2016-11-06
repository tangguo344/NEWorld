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

#ifndef VERTEXARRAY_H_
#define VERTEXARRAY_H_

#include <cstring>
#include <initializer_list>
#include <common.h>
#include "opengl.h"
#include <logger.h>

class VertexFormat
{
public:
    // Vertex attribute count
    size_t textureCount, colorCount, normalCount, coordinateCount;
    // Vertex attributes count (sum of all)
    int vertexAttributeCount;

    VertexFormat() : textureCount(0), colorCount(0), normalCount(0), coordinateCount(0), vertexAttributeCount(0)
    {
    }

    VertexFormat(int textureElementCount, int colorElementCount, int normalElementCount, int coordinateElementCount)
        : textureCount(textureElementCount), colorCount(colorElementCount), normalCount(normalElementCount), coordinateCount(coordinateElementCount),
          vertexAttributeCount(textureElementCount + colorElementCount + normalElementCount + coordinateElementCount)
    {
        assert(textureCount <= 3);
        assert(colorCount <= 4);
        assert(normalCount == 0 || normalCount == 3);
        assert(coordinateCount <= 4 && coordinateCount >= 1);
    }
};

class VertexArray
{
public:
    VertexArray(int maxVertexes, const VertexFormat& format)
try :
        mMaxVertexes(maxVertexes), mVertexes(0), mFormat(format),
                      mData(new float[mMaxVertexes * format.vertexAttributeCount]),
                      mVertexAttributes(new float[format.vertexAttributeCount])
    {
    }
    catch (std::bad_alloc)
    {
        warningstream << "Failed to create Vertex Array: Out of memory";
    }

    ~VertexArray()
    {
        delete[] mData;
        delete[] mVertexAttributes;
    }

    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;

    void clear()
    {
        memset(mData, 0, mMaxVertexes * mFormat.vertexAttributeCount * sizeof(float));
        memset(mVertexAttributes, 0, mFormat.vertexAttributeCount * sizeof(float));
        mVertexes = 0;
    }

    // Set texture coordinates
    void setTexture(size_t size, const float* texture)
    {
        assert(size <= mFormat.textureCount);
        memcpy(mVertexAttributes, texture, size * sizeof(float));
    }

    void setTexture(std::initializer_list<float> texture)
    {
        setTexture(texture.size(), texture.begin());
    }

    // Set color value
    void setColor(size_t size, const float* color)
    {
        assert(size <= mFormat.colorCount);
        memcpy(mVertexAttributes + mFormat.textureCount, color, size * sizeof(float));
    }

    void setColor(std::initializer_list<float> color)
    {
        setColor(color.size(), color.begin());
    }

    // Set normal vector
    void setNormal(size_t size, const float* normal)
    {
        assert(size <= mFormat.normalCount);
        memcpy(mVertexAttributes + mFormat.textureCount + mFormat.colorCount, normal, size * sizeof(float));
    }

    void setNormal(std::initializer_list<float> normal)
    {
        setNormal(normal.size(), normal.begin());
    }

    // Add vertex
    void addVertex(const float* coords)
    {
        auto cnt = mFormat.textureCount + mFormat.colorCount + mFormat.normalCount;
        memcpy(mData + mVertexes * mFormat.vertexAttributeCount, mVertexAttributes, cnt * sizeof(float));
        memcpy(mData + mVertexes * mFormat.vertexAttributeCount + cnt, coords, mFormat.coordinateCount * sizeof(float));
        mVertexes++;
    }

    void addVertex(std::initializer_list<float> coords)
    {
        addVertex(coords.begin());
    }

    void addPatch(size_t size, std::initializer_list<float> d)
    {
        memcpy(mData + mVertexes * mFormat.vertexAttributeCount, d.begin(), size * mFormat.vertexAttributeCount * sizeof(float));
        mVertexes += size;
    }

    // Get current vertex format
    const VertexFormat& getFormat() const
    {
        return mFormat;
    }

    // Get current vertex data
    const float* getData() const
    {
        return mData;
    }

    // Get current vertex count
    int getVertexCount() const
    {
        return mVertexes;
    }

private:
    // Max vertex count
    const int mMaxVertexes;
    // Vertex count
    int mVertexes;
    // Vertex array format
    VertexFormat mFormat;
    // Vertex array
    float* mData;
    // Current vertex attributes
    float* mVertexAttributes;
};

class VertexBuffer
{
public:
    VertexBuffer(): id(0), vertexes(0)
    {
    }

    VertexBuffer(VertexBufferID id_, int vertexes_, const VertexFormat& format_):
        id(id_), vertexes(vertexes_), format(format_)
    {
    }

    explicit VertexBuffer(const VertexArray& va);

    // Render vertex buffer
    void render() const;

    // Destroy vertex buffer
    void destroy()
    {
        glDeleteBuffersARB(1, &id);
        vertexes = id = 0;
        format = VertexFormat();
    }

private:
    // Buffer ID
    VertexBufferID id;
    // Vertex count
    int vertexes;
    // Buffer format
    VertexFormat format;

};
#endif // !VERTEXARRAY_H_
