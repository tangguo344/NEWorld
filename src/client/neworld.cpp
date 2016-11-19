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

#include "neworld.h"
#include "texture.h"
#include <pluginapi.h>
#include <common/jsonhelper.h>
#include "window.h"
#include "game.h"
NWDECLEARLOGGER("client")
NEWorld::NEWorld() : mPlugins(true)
{
    // Initialize
    getSettings();
    verbosestream << "Initializing...";
    Window& window = Window::getInstance("NEWorld", 852, 480);
    Renderer::init();
    Texture::init();
    PluginAPI::Blocks = &mBlocks;
    PluginAPI::Plugins = &mPlugins;
    mPlugins.loadPlugins();

    // Run
    Game game("TestWorld",
              std::make_shared<LocalConnectionByNetWork>(
                  getJsonValue<std::string>(getSettings()["server"]["ip"], "127.0.0.1"),
                  getJsonValue<unsigned short>(getSettings()["server"]["port"], 31111)),
              window, mPlugins, mBlocks);
    while(!window.shouldQuit())
    {
        // Update
        window.pollEvents();
        game.multiUpdate();
        // Render
        window.newIMGUIFrame();
        game.render();
        Renderer::checkError();
        window.swapBuffers();
    }

    // Terminate
    verbosestream << "Terminating...";
    mPlugins.unloadPlugins();
    Texture::free();
}
