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

#ifndef BLOCKDATA_H_
#define BLOCKDATA_H_

#include <vector>
#include <string>
#include <cstdint>
#include <engine/common.h>

class BlockData
{
public:
    BlockData() : u{0}
    {
    }

    BlockData(uint32_t id_, uint32_t brightness_, uint32_t state_)
        : u{ id_, brightness_, state_ }
    {
    }
    BlockData(uint32_t allData)
    {
        u.allData = allData;
    }

    BlockData(const BlockData& rhs) : u(rhs.u)
    {
    }

    bool operator==(const BlockData& rhs) const
    {
        return u.allData == rhs.u.allData;
    }
    uint32_t getData() const
    {
        return u.allData;
    }
    uint32_t getID() const
    {
        return u.data.id;
    }

    uint32_t getBrightness() const
    {
        return u.data.brightness;
    }

    uint32_t getState() const
    {
        return u.data.state;
    }

    void setID(uint32_t id_)
    {
        u.data.id = id_;
    }

    void setBrightness(uint32_t brightness_)
    {
        u.data.brightness = brightness_;
    }

    void setState(uint32_t state_)
    {
        u.data.state = state_;
    }

private:
    union BlockDataUnion
    {
        struct BlockDataUnionStruct
        {
            uint32_t id : 12; // Block ID
            uint32_t brightness : 4; // Brightness
            uint32_t state : 16; // Block state
        } data;
        uint32_t allData;
    } u;
};

class BlockType
{
private:
    std::string name;
    bool solid;
    bool translucent;
    bool opaque;
    int explodePower;
    int hardness;

public:
    BlockType(const char* name_, bool solid_, bool translucent_, bool opaque_, int explodePower_, int hardness_) :
        name(name_), solid(solid_), translucent(translucent_), opaque(opaque_), explodePower(explodePower_), hardness(hardness_)
    {
    }

    // Internal block name
    const std::string& getName() const
    {
        return name;
    }

    // Is solid block
    bool isSolid() const
    {
        return solid;
    }

    // Transparency determines how it will rendered
    bool isTranslucent() const
    {
        return translucent;
    }

    // Opaque means it blocks light
    bool isOpaque() const
    {
        return opaque;
    }

    // Explode power, if it isn't a explosive set this to 0
    int getExplodePower() const
    {
        return explodePower;
    }

    // Hardness
    int getHardness() const
    {
        return hardness;
    }
};

class NWCOREAPI BlockManager
{
public:
    BlockManager();
    void showInfo(size_t id) const;
    size_t registerBlock(const BlockType& block);

    const BlockType& operator[](size_t id) const
    {
        return mBlocks[id];
    }

private:
    std::vector<BlockType> mBlocks;
};

#endif // !BLOCKDATA_H_
