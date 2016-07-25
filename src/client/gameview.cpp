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

#include "gameview.h"
#include "renderer.h"
#include <logger.h>
#include "texture.h"

GameView::GameView(UI::Core::Window* win) :UI::Controls::GLContext()
{
    keyFunc.connect([this](int scancode, UI::Core::ButtonAction)
    {
        onKey(scancode);
    });
    win->renderdelegate.push_back([this, win]() { init(win); });
}

Texture texture;

void GameView::init(UI::Core::Window*)
{
    Renderer::init();

    // Example for Texture
    texture = Texture::loadTextureRGBA("./Res/test.png");
    UI::GameUtils::setSwapInterval(0);
    VertexArray cubeArray(3000000, VertexFormat(2, 3, 0, 3));

    glClearColor(0.6f, 0.9f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    for (int x = -25; x < 25; x++)
    {
        for (int y = -25; y < 25; y++)
        {
            for (int z = -25; z < 25; z++)
            {
                // Front
                cubeArray.setColor({ 0.7f, 0.7f, 0.7f });
                cubeArray.setTexture({ 0.0f, 0.0f });
                cubeArray.addVertex({ x*2.0f - 0.5f, y*2.0f + 0.5f, z*2.0f + 0.5f });
                cubeArray.setTexture({ 0.0f, 1.0f });
                cubeArray.addVertex({ x*2.0f - 0.5f, y*2.0f - 0.5f, z*2.0f + 0.5f });
                cubeArray.setTexture({ 1.0f, 1.0f });
                cubeArray.addVertex({ x*2.0f + 0.5f, y*2.0f - 0.5f, z*2.0f + 0.5f });
                cubeArray.setTexture({ 1.0f, 0.0f });
                cubeArray.addVertex({ x*2.0f + 0.5f, y*2.0f + 0.5f, z*2.0f + 0.5f });
                // Back
                cubeArray.setColor({ 0.7f, 0.7f, 0.7f });
                cubeArray.setTexture({ 0.0f, 0.0f });
                cubeArray.addVertex({ x*2.0f + 0.5f, y*2.0f + 0.5f, z*2.0f - 0.5f });
                cubeArray.setTexture({ 0.0f, 1.0f });
                cubeArray.addVertex({ x*2.0f + 0.5f, y*2.0f - 0.5f, z*2.0f - 0.5f });
                cubeArray.setTexture({ 1.0f, 1.0f });
                cubeArray.addVertex({ x*2.0f - 0.5f, y*2.0f - 0.5f, z*2.0f - 0.5f });
                cubeArray.setTexture({ 1.0f, 0.0f });
                cubeArray.addVertex({ x*2.0f - 0.5f, y*2.0f + 0.5f, z*2.0f - 0.5f });
                // Top
                cubeArray.setColor({ 1.0f, 1.0f, 1.0f });
                cubeArray.setTexture({ 0.0f, 0.0f });
                cubeArray.addVertex({ x*2.0f - 0.5f, y*2.0f + 0.5f, z*2.0f - 0.5f });
                cubeArray.setTexture({ 0.0f, 1.0f });
                cubeArray.addVertex({ x*2.0f - 0.5f, y*2.0f + 0.5f, z*2.0f + 0.5f });
                cubeArray.setTexture({ 1.0f, 1.0f });
                cubeArray.addVertex({ x*2.0f + 0.5f, y*2.0f + 0.5f, z*2.0f + 0.5f });
                cubeArray.setTexture({ 1.0f, 0.0f });
                cubeArray.addVertex({ x*2.0f + 0.5f, y*2.0f + 0.5f, z*2.0f - 0.5f });
                // Bottom
                cubeArray.setColor({ 1.0f, 1.0f, 1.0f });
                cubeArray.setTexture({ 0.0f, 0.0f });
                cubeArray.addVertex({ x*2.0f - 0.5f, y*2.0f - 0.5f, z*2.0f + 0.5f });
                cubeArray.setTexture({ 0.0f, 1.0f });
                cubeArray.addVertex({ x*2.0f - 0.5f, y*2.0f - 0.5f, z*2.0f - 0.5f });
                cubeArray.setTexture({ 1.0f, 1.0f });
                cubeArray.addVertex({ x*2.0f + 0.5f, y*2.0f - 0.5f, z*2.0f - 0.5f });
                cubeArray.setTexture({ 1.0f, 0.0f });
                cubeArray.addVertex({ x*2.0f + 0.5f, y*2.0f - 0.5f, z*2.0f + 0.5f });
                // Right
                cubeArray.setColor({ 0.5f, 0.5f, 0.5f });
                cubeArray.setTexture({ 0.0f, 0.0f });
                cubeArray.addVertex({ x*2.0f + 0.5f, y*2.0f + 0.5f, z*2.0f + 0.5f });
                cubeArray.setTexture({ 0.0f, 1.0f });
                cubeArray.addVertex({ x*2.0f + 0.5f, y*2.0f - 0.5f, z*2.0f + 0.5f });
                cubeArray.setTexture({ 1.0f, 1.0f });
                cubeArray.addVertex({ x*2.0f + 0.5f, y*2.0f - 0.5f, z*2.0f - 0.5f });
                cubeArray.setTexture({ 1.0f, 0.0f });
                cubeArray.addVertex({ x*2.0f + 0.5f, y*2.0f + 0.5f, z*2.0f - 0.5f });
                // Left
                cubeArray.setColor({ 0.5f, 0.5f, 0.5f });
                cubeArray.setTexture({ 0.0f, 0.0f });
                cubeArray.addVertex({ x*2.0f - 0.5f, y*2.0f + 0.5f, z*2.0f - 0.5f });
                cubeArray.setTexture({ 0.0f, 1.0f });
                cubeArray.addVertex({ x*2.0f - 0.5f, y*2.0f - 0.5f, z*2.0f - 0.5f });
                cubeArray.setTexture({ 1.0f, 1.0f });
                cubeArray.addVertex({ x*2.0f - 0.5f, y*2.0f - 0.5f, z*2.0f + 0.5f });
                cubeArray.setTexture({ 1.0f, 0.0f });
                cubeArray.addVertex({ x*2.0f - 0.5f, y*2.0f + 0.5f, z*2.0f + 0.5f });
            }
        }
        if ((x + 26)*2 % 20 == 0) infostream << "Building vertax array: " << (x + 26)*2 << "%";
    }

    infostream << "Generating VBO...";
    cube = VertexBuffer(cubeArray);
    infostream << "Complete!";

    onRenderF = [this]()
    {
        doRender();
    };
}

void GameView::doRender()
{
    glClearColor(0.6f, 0.9f, 1.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    texture.bind(Texture::Texture2D);
    Renderer::clear();
    Renderer::restoreProj();
    Renderer::applyPerspective(60.0f, cMargin.absrect.xmax / cMargin.absrect.ymax, 1.0f, 1000.0f);
    Renderer::restoreScale();
    Renderer::translate(Vec3f(0.0f, 0.0f, trans.z));
    Renderer::rotate(trans.x, Vec3f(1.0f, 0.0f, 0.0f));
    Renderer::rotate(trans.y, Vec3f(0.0f, 1.0f, 0.0f));

    cube.render();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    trans += transSpeed;
    transSpeed *= 0.9f;
}

void GameView::onResize(size_t w, size_t h)
{
    Grid::onResize(w, h);
    windowWidth = w;
    windowHeight = h;
}

void GameView::onKey(int key)
{
    if (key == SDLK_LEFT) transSpeed.y -= 0.1f;
    else if (key == SDLK_RIGHT) transSpeed.y += 0.1f;
    else if (key == SDLK_UP) transSpeed.x -= 0.1f;
    else if (key == SDLK_DOWN) transSpeed.x += 0.1f;
    else if (key == SDLK_w) transSpeed.z += 0.1f;
    else if (key == SDLK_s) transSpeed.z -= 0.1f;
}
