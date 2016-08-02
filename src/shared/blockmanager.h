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
private:
    vector<BlockType> blocks;

public:
    int registerBlock(const BlockType& block)
    {
        blocks.push_back(block);
        debugstream << "Registered block \"" << block.getName() << "\" with ID = " << blocks.size() << ", attributes:\n"
                    << "Solid: " << block.isSolid() << "\n"
                    << "Translucent: " << block.isTranslucent() << "\n"
                    << "Opaque: " << block.isOpaque() << "\n"
                    << "Explode power: " << block.getExplodePower() << "\n"
                    << "Hardness: " << block.getHardness() << "\n";
        // Air Block (ID = 0) is a built-in block and was skipped in this block list
        // So blocks.size() equals to the last block's ID
        return blocks.size();
    }

};

#endif // !BLOCKMANAGER_H_
