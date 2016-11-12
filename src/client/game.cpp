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

Game::Game(const std::string& name, std::shared_ptr<GameConnection> connection,
           PluginManager& pm, const BlockManager& bm):
    mBlocks(bm), mPlugins(pm), mWorld(name, pm, bm), mPlayer(&mWorld), mConnection(connection)
{
    mWorld.setRenderDistance(4);
    mPlayer.setPosition(Vec3d(-16.0, 48.0, 32.0));
    mPlayer.setRotation(Vec3d(-45.0, -22.5, 0.0));

    // Initialize rendering
    mTexture = Texture::loadTextureRGBA("./res/test.png");
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Initialize Widgets
    mGUIWidgets.addWidget(std::make_shared<WidgetCallback>("Debug", ImVec2(100, 200), [this]
    {
        mRateCounterTimer.refresh();
        if (mRateCounterTimer.shouldRun())
        {
            // Update FPS & UPS
            mFpsLatest = mFpsCounter;
            mUpsLatest = mUpsCounter;
            mFpsCounter = 0;
            mUpsCounter = 0;
            mRateCounterTimer.increaseTimer();
        }
        ImGui::Text("NEWorld %s (v%u)", NEWorldVersionName, NEWorldVersion);
        ImGui::Text("FPS %d, UPS %d", mFpsLatest, mUpsLatest);
        ImGui::Text("Position: x %.1f y %.1f z %.1f", mPlayer.getPosition().x, mPlayer.getPosition().y, mPlayer.getPosition().z);
        ImGui::Text("GUI Widgets: %zu", mGUIWidgets.getSize());
        ImGui::Text("Chunks Loaded: %zu/%zu", mWorld.getChunkCount(), mWorld.getReservedChunkCount());
    }));

    mConnection->setChunkCallback([&](Chunk* chunk)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        Chunk* target = mWorld.getChunkPtr(chunk->getPosition());
        if (target == nullptr) return;
        // Update chunk
        memcpy(target->getBlocks(), chunk->getBlocks(), sizeof(BlockData) * ChunkSize * ChunkSize * ChunkSize);
        target->setUpdated(true);
        // Update neighboring chunks
        const Vec3i delta[6] =
        {
            Vec3i( 1, 0, 0), Vec3i(-1, 0, 0),
            Vec3i( 0, 1, 0), Vec3i( 0,-1, 0),
            Vec3i( 0, 0, 1), Vec3i( 0, 0,-1)
        };
        for (int i = 0; i < 6; i++)
        {
            target = mWorld.getChunkPtr(chunk->getPosition() + delta[i]);
            if (target != nullptr)
                target->setUpdated(true);
        }
        delete chunk;
    });
    mConnection->setWorld(&mWorld);
    mConnection->connect();
    mConnection->waitForConnected();
    mConnection->login("test", "123456");
}

Game::~Game()
{
    mPlugins.unloadPlugins();
}

void Game::update()
{
    std::lock_guard<std::mutex> lock(mMutex);

    mUpsCounter++;

    // TODO: Read keys from the configuration file
    if (Window::isKeyDown(SDL_SCANCODE_UP) && mPlayer.getRotation().x < 90)
        mPlayer.rotate(Vec3d(1.5, 0.0, 0.0));
    if (Window::isKeyDown(SDL_SCANCODE_DOWN) && mPlayer.getRotation().x > -90)
        mPlayer.rotate(Vec3d(-1.5, 0.0, 0.0));
    if (Window::isKeyDown(SDL_SCANCODE_RIGHT))
        mPlayer.rotate(Vec3d(0.0, -2.5, 0.0));
    if (Window::isKeyDown(SDL_SCANCODE_LEFT))
        mPlayer.rotate(Vec3d(0.0, 2.5, 0.0));
    if (Window::isKeyDown(SDL_SCANCODE_W))
        mPlayer.accelerate(Vec3d(0.0, 0.0, -0.05));
    if (Window::isKeyDown(SDL_SCANCODE_S))
        mPlayer.accelerate(Vec3d(0.0, 0.0, 0.05));
    if (Window::isKeyDown(SDL_SCANCODE_A))
        mPlayer.accelerate(Vec3d(-0.05, 0.0, 0.0));
    if (Window::isKeyDown(SDL_SCANCODE_D))
        mPlayer.accelerate(Vec3d(0.05, 0.0, 0.0));
    if (Window::isKeyDown(SDL_SCANCODE_SPACE))
        mPlayer.accelerate(Vec3d(0.0, 0.1, 0.0));
    if (Window::isKeyDown(SDL_SCANCODE_LCTRL) || Window::isKeyDown(SDL_SCANCODE_RCTRL))
        mPlayer.accelerate(Vec3d(0.0, -0.1, 0.0));

    mPlayer.update();
    mWorld.sortChunkLoadUnloadList(Vec3i(mPlayer.getPosition()));
    mWorld.tryLoadChunks(*mConnection);
    mWorld.update();
    mWorld.renderUpdate(Vec3i(mPlayer.getPosition()));
    mGUIWidgets.update();
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
    std::lock_guard<std::mutex> lock(mMutex);

    mFpsCounter++;

    glClearColor(0.6f, 0.9f, 1.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    mUpdateScheduler.refresh();
    double timeDelta = mUpdateScheduler.getDeltaTimeMs() / 1000.0 * GameUpdateFrequency;
    if (timeDelta > 1.0) timeDelta = 1.0;
    Vec3d playerRenderedPosition = mPlayer.getPosition() - mPlayer.getPositionDelta() * (1.0 - timeDelta);

    mTexture.bind(Texture::Texture2D);
    Renderer::clear();
    Renderer::restoreProj();
    Renderer::applyPerspective(70.0f, float(windowWidth) / windowHeight, 0.1f, 300.0f);
    Renderer::restoreScale();
    Renderer::rotate(-mPlayer.getRotation().x, Vec3d(1.0, 0.0, 0.0));
    Renderer::rotate(-mPlayer.getRotation().y, Vec3d(0.0, 1.0, 0.0));
    Renderer::rotate(-mPlayer.getRotation().z, Vec3d(0.0, 0.0, 1.0));
    Renderer::translate(-playerRenderedPosition);

    // Render

    mWorld.render(Vec3i(mPlayer.getPosition()));

    // mPlayer.render();

    glDisable(GL_DEPTH_TEST);

    mGUIWidgets.render();
}

Event::EventBus& Game::getEventBus()
{
    return mEventBus;
}
