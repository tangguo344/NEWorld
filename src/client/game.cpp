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

#include <string.h>
#include <atomic>
#include <chrono>
#include "game.h"
#include "renderer.h"
#include "utils.h"
#include <logger.h>
#include "network.h"
#include <jsonhelper.h>
#include <exception.h>
#include "window.h"

Game::Game(PluginManager& pm, const BlockManager& bm)
    : mBlocks(bm), mPlugins(pm),
      mWorld("TestWorld", pm, bm),// TODO: read from settings
      mPlayer(&mWorld),
      mConnection(std::make_shared<MultiplayerConnection>( // TODO: single-player mode.
                      getJsonValue<std::string>(getSettings()["server"]["ip"], "127.0.0.1"),
                      getJsonValue<unsigned short>(getSettings()["server"]["port"], 9887), &mWorld)),
      mSinglePlayManager([this](bool success) {if (success) mConnection->connect();})
{
    mSinglePlayManager.run(); // TODO: start the server only when it's a single player mode.
    // TEMP CODE
    // Load some chunks at client side to test rendering
    mWorld.setRenderDistance(4);
    mPlayer.setPosition(Vec3d(-16.0, 48.0, 32.0));
    mPlayer.setRotation(Vec3d(-45.0, -22.5, 0.0));
    // END TEMP CODE

    // Initialize rendering
    mTexture = Texture::loadTextureRGBA("./res/test.png");
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Initialize Widgets
    mWidgetManager.addWidget(std::make_shared<WidgetCallback>("Debug", ImVec2(100, 200), [this]
    {
        ImGui::Text("NEWorld %s(%u)", NEWorldStringVersion, NEWorldVersion);
        ImGui::Text("FPS %.1f, UPS %.1f", ImGui::GetIO().Framerate, mUpdateScheduler.getRate());
        ImGui::Text("Pos: x %.1f y %.1f z %.1f", mPlayer.getPosition().x, mPlayer.getPosition().y, mPlayer.getPosition().z);
        ImGui::Text("Widgets Loaded: %zu", mWidgetManager.getSize());
        ImGui::Text("Chunks Loaded: %zu/%zu", mWorld.getChunkCount(), mWorld.getReservedChunkCount());
    }));

    mConnection->waitForConnected();
    mConnection->login("test", "123456");
    mConnection->setChunkCallback([&](Chunk* chunk)
    {
        Chunk* target = mWorld.getChunkPtr(chunk->getPosition());
        if (target == nullptr) return;
        memcpy(target->getBlocks(), chunk->getBlocks(), sizeof(BlockData)*ChunkSize*ChunkSize*ChunkSize);
        target->setUpdated(true);
    });
}

Game::~Game()
{
    mPlugins.unloadPlugins();
}

void Game::update()
{
    auto& win = Window::getInstance();
    // TODO: Read keys from the configuration file
    if (win.isKeyDown(SDL_SCANCODE_UP))
        mPlayer.rotate(Vec3d(1.5, 0.0, 0.0));
    if (win.isKeyDown(SDL_SCANCODE_DOWN))
        mPlayer.rotate(Vec3d(-1.5, 0.0, 0.0));
    if (win.isKeyDown(SDL_SCANCODE_RIGHT))
        mPlayer.rotate(Vec3d(0.0, -1.5, 0.0));
    if (win.isKeyDown(SDL_SCANCODE_LEFT))
        mPlayer.rotate(Vec3d(0.0, 1.5, 0.0));
    if (win.isKeyDown(SDL_SCANCODE_W))
        mPlayer.accelerate(Vec3d(0.0, 0.0, -0.03));
    if (win.isKeyDown(SDL_SCANCODE_S))
        mPlayer.accelerate(Vec3d(0.0, 0.0, 0.03));
    if (win.isKeyDown(SDL_SCANCODE_A))
        mPlayer.accelerate(Vec3d(-0.03, 0.0, 0.0));
    if (win.isKeyDown(SDL_SCANCODE_D))
        mPlayer.accelerate(Vec3d(0.03, 0.0, 0.0));
    if (win.isKeyDown(SDL_SCANCODE_SPACE))
        mPlayer.accelerate(Vec3d(0.0, 0.05, 0.0));
    if (win.isKeyDown(SDL_SCANCODE_LCTRL) || win.isKeyDown(SDL_SCANCODE_RCTRL))
        mPlayer.accelerate(Vec3d(0.0, -0.05, 0.0));

    mPlayer.update();
    mWorld.sortChunkLoadUnloadList(Vec3i(mPlayer.getPosition()));
    mWorld.tryLoadChunks(*mConnection);
    mWorld.update();
    mWorld.renderUpdate(Vec3i(mPlayer.getPosition()));
    mWidgetManager.update();
}

void Game::multiUpdate()
{
    const long long updateTimeout = 4000;
    mUpdateScheduler.refresh();
    if (mUpdateScheduler.getDeltaTimeMs() >= updateTimeout)
    {
        warningstream << "Can't keep up! " << mUpdateScheduler.getDeltaTimeMs() << "ms skipped.";
        mUpdateScheduler.sync();
    }
    while (mUpdateScheduler.shouldRun())
    {
        update();
        mUpdateScheduler.increaseTimer();
    }
}

// TEMP FUNCTION: to show the world coordinates
void drawAxes()
{
    glDisable(GL_CULL_FACE);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    // X
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(-256.0f, 0.0f, 0.0f);
    glVertex3f(256.0f, 0.0f, 0.0f);
    glEnd();
    // Y
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, -256.0f, 0.0f);
    glVertex3f(0.0f, 256.0f, 0.0f);
    glEnd();
    // Z
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, -256.0f);
    glVertex3f(0.0f, 0.0f, 256.0f);
    glEnd();
    glEnable(GL_CULL_FACE);
}

void Game::render()
{
    glClearColor(0.6f, 0.9f, 1.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    mTexture.bind(Texture::Texture2D);
    Renderer::clear();
    Renderer::restoreProj();
    Renderer::applyPerspective(70.0f, float(windowWidth) / windowHeight, 0.1f, 300.0f);
    Renderer::restoreScale();
    Renderer::rotate(-mPlayer.getRotation().x, Vec3d(1.0, 0.0, 0.0));
    Renderer::rotate(-mPlayer.getRotation().y, Vec3d(0.0, 1.0, 0.0));
    Renderer::rotate(-mPlayer.getRotation().z, Vec3d(0.0, 0.0, 1.0));
    Renderer::translate(-mPlayer.getPosition());

    // Render
    mWorld.render(Vec3i(mPlayer.getPosition()));

    glDisable(GL_TEXTURE_2D);

    // TEMP CODE
    drawAxes();
    // END TEMP CODE

    glDisable(GL_DEPTH_TEST);

    mWidgetManager.render();
}

Event::EventBus& Game::getEventBus()
{
    return mEventBus;
}
