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

Application::Application(int width, int height, const string& title, const string& path) :
    m_width(width), m_height(height), m_title(title), m_world("", m_plugins), m_cpa(8), m_worldLoader(m_world, m_cpa)
{
    PluginAPI::Blocks = &m_blocks;
    m_plugins.loadPlugins(path);
}

void Application::beforeLaunch()
{
    // Initialize here
    loggerInit("NEWorld");
    infostream << "Initializing...";

    Texture::init();
    //m_worldLoader.sortChunkLoadUnloadList(Vec3i(0, 0, 0));

    UI::Logger::init("./Logs");
    m_worldLoader.setLoadRange(4);
    UI::Font::service.addSearchPaths({ "./Res/Fonts" });
    UI::Globalization::Service::getInstance().setBasePath("./Res/Langs/");
    UI::Globalization::Service::getInstance().attachLangFiles({ "zh_CN", "en_US" });
    UI::Globalization::Service::getInstance().setLang("zh_CN");

    UI::Theme::SystemTheme.ControlDarkBrush = UIMakeSolidColorBrush(UI::Base::Color(0.2, 0.2, 0.2, 0.6));
    UI::Theme::SystemTheme.ControlHeightLightBrush = UIMakeSolidColorBrush(UI::Base::Color(1.2 * 0.2, 1.2 * 0.2, 1.2 * 0.2, 0.8 * 0.6));
    UI::Theme::SystemTheme.ControlOnPressBrush = UIMakeSolidColorBrush(UI::Base::Color(0.2 * 0.8, 0.2 * 0.8, 0.2 * 0.8, 0.9));
    UI::Theme::SystemTheme.DefaultFont = UI::Font::service.getRenderer("SourceHanSansCN-Normal", 17, UI::Base::Color(1.0, 1.0, 1.0, 1.0));
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
