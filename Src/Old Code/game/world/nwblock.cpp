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

#include "nwblock.h"

BlockManager::BlockManager()
    :mBlocks{ BlockType("Air", false, false, false, 0, 0) }
{
}

size_t BlockManager::registerBlock(const BlockType& block)
{
    mBlocks.push_back(block);
    debugstream << "Registered block:";
    showInfo(mBlocks.size() - 1);
    return mBlocks.size() - 1;
}

void BlockManager::showInfo(size_t id) const
{
    BlockType block = mBlocks[id];
    debugstream << "Block \"" << block.getName() << "\"(ID = " << id << ") = {"
                << "Solid: " << block.isSolid()
                << ", Translucent: " << block.isTranslucent()
                << ", Opaque: " << block.isOpaque()
                << ", Explode power: " << block.getExplodePower()
                << ", Hardness: " << block.getHardness()
                << "}";
}
