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
    static GLuint gFBO = 0;
    static void initializeFBO(GLuint target, int w, int h)
    {
        if (gFBO == 0)
        {
            glGenFramebuffers(1, &gFBO);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, gFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            errorstream << "FrameBuffer Creation Failure";
            Assert(false);
        }
        glViewport(0, 0, w, h);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    }
    static void finalizeFBO()
    {
        glBindFramebuffer(0);
    }
    void doUpdate()
    {
        initializeFBO(mTexBuffer, mSize.x, mSize.y);
        this->updateThis();
        for (auto&& iter : *this)
            iter->render();
        finalizeFBO();
    }
    std::pair<iterator, iterator> mRange;
    std::string mName;
    Vec2i mSize;
    Alignment mVertical, mHorizontal;
    Mat4f mTramsform;
    GLuint mTexBuffer;
};

class VisualRoot : public VisualNode
{
public:

};
