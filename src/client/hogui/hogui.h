#pragma once
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

#include <list>
#include <memory>
#include <string>
#include <functional>
#include <GL/glew.h>
#include <./../backends/sdlgl/sdlgl.hpp>
#include <common/common.h>

class Visual
{
public:
    enum class Alignment { Left, Right, Centre, Stretch };
    using container_type = std::list<std::shared_ptr<Visual>>;
    using iterator = container_type::iterator;
    virtual iterator begin() { return mRange.first; }
    virtual iterator end() {return mRange.second; }
    void render() {}
    void update() { doUpdate(); }
    // Get / Set
    void setName(const std::string& newName) { mName = newName; }
    std::string getName() const { return mName; }
    void setVerticalAlignment(Alignment newAlignment) { mVertical = newAlignment; }
    void setHorizontalAlignment(Alignment newAlignment) { mHorizontal = newAlignment; }
protected:
    virtual void updateThis() { glClearColor(1.0, 1.0, 1.0, 1.0); };
private:
    void doUpdate()
    {
		bkBeginRenderToTexture(reinterpret_cast<void*>(mTexBuffer), mSize.x, mSize.y);
        this->updateThis();
        for (auto&& iter : *this)
            iter->render();
		bkStopRenderToTexture();
    }
    std::pair<iterator, iterator> mRange;
    std::string mName;
    Vec2i mSize;
    Alignment mVertical, mHorizontal;
    Mat4f mTramsform;
    GLuint mTexBuffer;
};

class VisualRoot : public Visual
{
public:

};
