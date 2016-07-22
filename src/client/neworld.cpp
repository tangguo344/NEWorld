/*
* NEWorld: A free game with similar rules to Minecraft.
* Copyright (C) 2016 NEWorld Team
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "neworld.h"
#include "gameview.h"
#include "texture.h"
#include <pluginapi.h>

Application::Application(int width, int height, const string& title) :
    m_width(width), m_height(height), m_title(title), m_world("", m_plugins), m_cpa(8), m_worldLoader(m_world, m_cpa)
{
}

void Application::beforeLaunch()
{
    // Initialize here
    loggerInit("NEWorld");
    infostream << "Initializing...";

    Texture::init();
    PluginAPI::Blocks = &m_blocks;
    m_plugins.loadPlugins();
    m_worldLoader.setLoadRange(4);
    //m_worldLoader.sortChunkLoadUnloadList(Vec3i(0, 0, 0));

    UI::Logger::init("./Logs");
    UI::Font::service.addSearchPaths({ "./Fonts" });
    UI::Globalization::Service::getInstance().setBasePath("./Langs/");
    UI::Globalization::Service::getInstance().attachLangFiles({ "chinese", "english" });
    UI::Globalization::Service::getInstance().setLang("chinese");
    //std::thread serverThread(networkThread);
}

void Application::afterLaunch()
{
    addWindow(std::static_pointer_cast<UI::Core::Window>(std::make_shared<MainWindow>(m_width, m_height, m_title)));
    infostream << "Game start!";
}

void Application::onTerminate()
{
    // Destroy here
    infostream << "Terminating...";
    UI::Logger::service.dump();
    Texture::uninit();
    m_plugins.unloadPlugins();
    //serverThread.join();
    disconnect();
}
