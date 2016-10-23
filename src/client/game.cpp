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

#include "game.h"
#include "renderer.h"
#include "utils.h"
#include <logger.h>
#include "network.h"
#include <boost/dll/shared_library.hpp>
#include <jsonhelper.h>

Game::Game(const PluginManager& pm, const BlockManager& bm)
    : m_blocks(bm), m_plugins(pm), m_world("TestWorld", pm, bm), // TODO: read from settings
      m_player(&m_world), m_connection("127.0.0.1", 8090)
{
    // TODO: start the server only when it's a single player mode.
    m_localServerThread = std::thread([]
    {
        char* argv[] = { "","--single-player-mode" };
        boost::dll::shared_library
        (
            getJsonValue<std::string>(getSettings()["server"]["file"], "nwserver.dll"),
            boost::dll::load_mode::append_decorations
        ).get<void NWAPICALL(int, char**)>("main")(sizeof(argv)/sizeof(argv[0]), argv);
    });

    // TEMP CODE
    // Load some chunks at client side to test rendering
    m_world.setRenderDistance(4);
    m_player.setPosition(Vec3d(-16.0, 48.0, 32.0));
    m_player.setRotation(Vec3d(-45.0, -22.5, 0.0));
    Vec3i::for_range(-6, 6, [&](const Vec3i& pos)
    {
        m_world.addChunk(pos);
    });
    // END TEMP CODE

    // FIXME: if the server spends too much time starting, the network thread won't be able to connect to the server.
    m_connection.connect();

    // Initialize rendering
    m_texture = Texture::loadTextureRGBA("./res/test.png");
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}

void Game::render()
{
    glClearColor(0.6f, 0.9f, 1.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    m_texture.bind(Texture::Texture2D);
    Renderer::clear();
    Renderer::restoreProj();
    Renderer::applyPerspective(70.0f, windowWidth / windowHeight, 0.1f, 300.0f);
    Renderer::restoreScale();
    Renderer::rotate(-m_player.getRotation().x, Vec3f(1.0f, 0.0f, 0.0f));
    Renderer::rotate(-m_player.getRotation().y, Vec3f(0.0f, 1.0f, 0.0f));
    Renderer::rotate(-m_player.getRotation().z, Vec3f(0.0f, 0.0f, 1.0f));
    Renderer::translate(-m_player.getPosition());

    // Render
    m_world.render(Vec3i(m_player.getPosition()));

    glDisable(GL_TEXTURE_2D);

    // TEMP CODE
    // To show the world coordinates
    glDisable(GL_CULL_FACE);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    /*
    glBegin(GL_QUADS);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 32.0f);
    glVertex3f(32.0f, 0.0f, 32.0f);
    glVertex3f(32.0f, 0.0f, 0.0f);
    glEnd();
    */
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
    // END TEMP CODE

    glDisable(GL_DEPTH_TEST);

    // Update
    m_player.update();
    m_world.renderUpdate(Vec3i(m_player.getPosition()));
    m_world.update();
}
