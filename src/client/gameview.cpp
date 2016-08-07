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

MainWindow::MainWindow(int width, int height, const string& title) : UI::Core::Window(title, width, height, 200, 200)
{
    background = std::make_shared<UI::Graphics::Brushes::ImageBrush>(std::make_shared<UI::Base::Image>("./Res/ss.png"));
    loader = std::thread([this]()
    {
        btex[0] = std::make_shared<UI::Base::Texture>("./Res/bkg0.png");
        btex[1] = std::make_shared<UI::Base::Texture>("./Res/bkg3.png");
        btex[2] = std::make_shared<UI::Base::Texture>("./Res/bkg2.png");
        btex[3] = std::make_shared<UI::Base::Texture>("./Res/bkg1.png");
        btex[4] = std::make_shared<UI::Base::Texture>("./Res/bkg4.png");
        btex[5] = std::make_shared<UI::Base::Texture>("./Res/bkg5.png");

        // Load Something
        // std::this_thread::sleep_for(2000ms);

        renderdelegate.push_back([this]()
        {
            background = UI::Theme::SystemTheme.WindowBrush;
            // Load the main menu
            pushPage(std::make_shared<BackGround>(this), false, false);
            pushPage(std::make_shared<MainMenu>(this), false, true);
            loader.join();
        });
    });
}

GameView::GameView(UI::Core::Window* win) :UI::Controls::GLContext(),
    m_world("", m_plugins, m_blocks), m_cpa(8), m_worldLoader(m_world, m_cpa), m_chunk(Vec3i(0, 0, 0)), m_chunkRenderer(m_world, m_chunk)
{
    PluginAPI::Blocks = &m_blocks;
    PluginAPI::Plugins = &m_plugins;
    m_plugins.loadPlugins("./");

    keyFunc.connect([this](int scancode, UI::Core::ButtonAction)
    {
        onKey(scancode);
    });
    win->renderdelegate.push_back([this, win]() { init(win); });

    //m_worldLoader.setLoadRange(4);
    //m_worldLoader.sortChunkLoadUnloadList(Vec3i(0, 0, 0));

    // Test chunk build
    ChunkLoader(m_chunk).build(15);
    /*
    BlockData block = m_chunk.getBlock(Vec3i(0, 0, 0));
    infostream << "Block at (0,0,0) = {ID: " << block.getID() << ", brightness: " << block.getBrightness() << ", state: " << block.getState() << "}";
    debugstream << "Full information:";
    m_blocks.showInfo(block.getID());
    */
}

void GameView::init(UI::Core::Window*)
{
    Renderer::init();

    m_texture = Texture::loadTextureRGBA("./Res/test.png");
    UI::GameUtils::setSwapInterval(0);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Test chunk render
    m_chunkRenderer.buildVertexArray();

    onRenderF = [this]()
    {
        doRender();
    };
}

void GameView::doRender()
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
    Renderer::translate(-m_player.getPosition());
    Renderer::rotate(-m_player.getRotation().x, Vec3f(1.0f, 0.0f, 0.0f));
    Renderer::rotate(-m_player.getRotation().y, Vec3f(0.0f, 1.0f, 0.0f));
    Renderer::rotate(-m_player.getRotation().z, Vec3f(0.0f, 0.0f, 1.0f));

    m_chunkRenderer.render();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);

    m_player.move();
}

void GameView::onResize(size_t w, size_t h)
{
    Grid::onResize(w, h);
    windowWidth = w;
    windowHeight = h;
}

void GameView::onKey(int key)
{
    if (key == SDLK_w) m_player.accelerate(Vec3d( 0.0, 0.0,-0.2));
    if (key == SDLK_s) m_player.accelerate(Vec3d( 0.0, 0.0, 0.2));
    if (key == SDLK_a) m_player.accelerate(Vec3d(-0.2, 0.0, 0.0));
    if (key == SDLK_d) m_player.accelerate(Vec3d( 0.2, 0.0, 0.0));
    if (key == SDLK_LSHIFT || key == SDLK_RSHIFT) m_player.accelerate(Vec3d( 0.0,-0.2, 0.0));
    if (key == SDLK_SPACE) m_player.accelerate(Vec3d( 0.0, 0.2, 0.0));
}
