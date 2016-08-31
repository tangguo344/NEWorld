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

#include "gamescene.h"
#include "renderer.h"
#include "utils.h"
#include <logger.h>
#include "network.h"
#include <boost/dll/shared_library.hpp>
#include <jsonhelper.h>

GameScene::GameScene(UI::Core::Window* win, BlockManager& bm, PluginManager& pm)
    :UI::Controls::GLContext(), m_blocks(bm), m_plugins(pm)
{
    // TODO: start the server only when it's a single player mode.
    m_localServerThread = std::thread([]
    {
        try
        {
            boost::dll::shared_library(getJsonValueWithDefaultValue<std::string>(getSettings()["server"]["file"], "NEWorldServer.dll"), boost::dll::load_mode::append_decorations).get<void NWAPICALL(int, char**)>("main")(0, nullptr); //FIXME: 偷懒没传参数
        }
        catch (std::exception& e)
        {
            fatalstream << "Error in server: " << e.what();
        }
    });
    // FIXME: if the server spends too much time starting, the network thread won't connect to the server.
    m_networkThread = std::thread(networkThread);

    keyFunc.connect([this](int scancode, UI::Core::ButtonAction)
    {
        onKey(scancode);
    });
    win->renderdelegate.push_back([this, win]() { init(win); });

    m_renderer=std::unique_ptr<WorldRenderer>(new WorldRenderer(*m_world));
}

void GameScene::init(UI::Core::Window*)
{
    Renderer::init();

    m_texture = Texture::loadTextureRGBA("./res/test.png");
    UI::GameUtils::setSwapInterval(0);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    onRenderF = [this]()
    {
        doRender();
    };
}

void GameScene::doRender()
{
    // Overwrite UILib parameters
    glClearColor(0.6f, 0.9f, 1.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    m_texture.bind(Texture::Texture2D);
    Renderer::clear();
    Renderer::restoreProj();
    Renderer::applyPerspective(70.0f, cMargin.absrect.xmax / cMargin.absrect.ymax, 1.0f, 300.0f);
    Renderer::restoreScale();
    Renderer::rotate(-m_player.getRotation().x, Vec3f(1.0f, 0.0f, 0.0f));
    Renderer::rotate(-m_player.getRotation().y, Vec3f(0.0f, 1.0f, 0.0f));
    Renderer::rotate(-m_player.getRotation().z, Vec3f(0.0f, 0.0f, 1.0f));
    Renderer::translate(-m_player.getPosition());

    // Render
    m_renderer->render();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);

    // Update
    m_player.update();
    m_renderer->update();
}

void GameScene::onResize(size_t w, size_t h)
{
    Grid::onResize(w, h);
    windowWidth = w;
    windowHeight = h;
}

void GameScene::onKey(int key)
{
    if (key == SDLK_w) m_player.accelerate(Vec3d( 0.0, 0.0,-0.2));
    if (key == SDLK_s) m_player.accelerate(Vec3d( 0.0, 0.0, 0.2));
    if (key == SDLK_a) m_player.accelerate(Vec3d(-0.2, 0.0, 0.0));
    if (key == SDLK_d) m_player.accelerate(Vec3d( 0.2, 0.0, 0.0));
    if (key == SDLK_LSHIFT || key == SDLK_RSHIFT) m_player.accelerate(Vec3d( 0.0,-0.2, 0.0));
    if (key == SDLK_SPACE) m_player.accelerate(Vec3d( 0.0, 0.2, 0.0));
    if (key == SDLK_UP) m_player.rotate(Vec3d( 1.0, 0.0, 0.0));
    if (key == SDLK_DOWN) m_player.rotate(Vec3d(-1.0, 0.0, 0.0));
    if (key == SDLK_LEFT) m_player.rotate(Vec3d( 0.0, 1.0, 0.0));
    if (key == SDLK_RIGHT) m_player.rotate(Vec3d( 0.0,-1.0, 0.0));
}
