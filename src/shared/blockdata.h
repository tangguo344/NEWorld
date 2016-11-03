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

class BlockData
{
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

public:
    BlockData() : u{0}
    {
    }

    BlockData(uint32_t id_, uint32_t brightness_, uint32_t state_)
        : u{ id_, brightness_, state_ }
    {
    }

    BlockData(const BlockData& rhs) : u(rhs.u)
    {
    }

    bool operator==(const BlockData& rhs) const
    {
        return u.allData == rhs.u.allData;
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
};

#endif // !BLOCKDATA_H_
