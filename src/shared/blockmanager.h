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
using std::vector;

#include "blocktype.h"
#include "logger.h"

class BlockManager
{
public:
    int registerBlock(const BlockType& block)
    {
        m_blocks.push_back(block);
        debugstream << "Registered block:";
        showInfo(m_blocks.size());
        // Air Block (ID = 0) is a built-in block and was skipped in this block list
        // So blocks.size() equals to the last block's ID
        return m_blocks.size();
    }

    void showInfo(int id);

private:
    vector<BlockType> m_blocks;

};

#endif // !BLOCKMANAGER_H_
