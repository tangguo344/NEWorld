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

#ifndef BLOCKTYPE_H_
#define BLOCKTYPE_H_

#include <string>

class BlockType {
public:
    BlockType(const char* name_, bool solid_, bool translucent_, bool opaque_, int hardness_):
        name(name_), solid(solid_), translucent(translucent_), opaque(opaque_), hardness(hardness_) {}

    // Internal block name
    const std::string& getName() const { return name; }
    // Is solid block
    bool isSolid() const { return solid; }
    // Transparency determines how it will rendered
    bool isTranslucent() const { return translucent; }
    // Opaque means it blocks light
    bool isOpaque() const { return opaque; }
    // Hardness
    int getHardness() const { return hardness; }

private:
    std::string name;
    bool solid;
    bool translucent;
    bool opaque;
    int hardness;
};

#endif // !BLOCKTYPE_H_
