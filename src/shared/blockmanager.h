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

#ifndef BLOCKMANAGER_H_
#define BLOCKMANAGER_H_

#include <vector>

#include "blocktype.h"
#include "logger.h"

class BlockManager
{
public:
    BlockManager()
    {
        m_blocks.push_back(BlockType("Air", false, false, false, 0, 0));
    }

    int registerBlock(const BlockType& block)
    {
        m_blocks.push_back(block);
        debugstream << "Registered block:";
        showInfo(m_blocks.size() - 1);
        return m_blocks.size() - 1;
    }

    const BlockType& getType(int id) const
    {
        return m_blocks[id];
    }

    void showInfo(int id) const;

private:
    std::vector<BlockType> m_blocks;

};

#endif // !BLOCKMANAGER_H_
