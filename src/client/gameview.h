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

#ifndef GAMEVIEW_H_
#define GAMEVIEW_H_

#include <thread>
#include <uilib.h>
#include "vec3.h"
#include "renderer.h"
#include "pages.h"

#include <world.h>
#include <logger.h>
#include <pluginapi.h>
#include <blockmanager.h>
#include <worldmanager.h>
#include <pluginmanager.h>
#include "../server/worldloader.h"
#include "chunkrenderer.h"

class GameView : public UI::Controls::GLContext
{
public:
    GameView(UI::Core::Window*);
    ~GameView()
    {
        m_plugins.unloadPlugins();
    }
    void doRender();
    void onResize(size_t w, size_t h) override;
    void init(UI::Core::Window* win);
    void onKey(int key);

private:
    int windowWidth = 852, windowHeight = 480;
    Vec3f trans = {0.0f, 0.0f, -100.0f};
    Vec3f transSpeed = {0.0f, 0.0f, 0.0f};

    // Blocks
    BlockManager m_blocks;
    // Loaded plugins
    PluginManager m_plugins;
    /*
    // Loaded worlds
    WorldManager m_worlds;
    */
    // Current world
    World m_world;
    // CPA
    ChunkPointerArray m_cpa;
    // Loading test
    WorldLoader m_worldLoader;
    // Chunk test
    Chunk m_chunk;
    // Rendering test
    ChunkRenderer m_chunkRenderer;
};

class MainWindow : public UI::Core::Window
{
public:
    std::shared_ptr<UI::Base::Texture> btex[6];
    MainWindow(int width, int height, const string& title);

private:
    std::thread loader;
};

#endif // !GAMEVIEW_H_
