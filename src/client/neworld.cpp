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
#include "gamescene.h"
#include "texture.h"
#include <pluginapi.h>

Application::Application(int width, int height, const string& title, const string& path) :
    m_width(width), m_height(height), m_title(title), m_worlds(m_plugins,m_blocks)
{
}

void Application::beforeLaunch()
{
    // Initialize here
    Logger::init("client");
    infostream << "Initializing...";

    //Set up hooks from UI::Logger(UILib) to Logger(NEWorld)
    UI::Logger::setHookFunc([](size_t level,std::string msg, const char* fileName, const char *funcName, int lineNumber)
    {
        Logger(fileName, funcName, lineNumber, static_cast<Logger::Level>(level)) << msg;
    });

    UI::Logger::setHook(UI::Logger::Level::trace, static_cast<size_t>(Logger::Level::trace));
    UI::Logger::setHook(UI::Logger::Level::info, static_cast<size_t>(Logger::Level::info));
    UI::Logger::setHook(UI::Logger::Level::debug, static_cast<size_t>(Logger::Level::debug));
    UI::Logger::setHook(UI::Logger::Level::warning, static_cast<size_t>(Logger::Level::warning));
    UI::Logger::setHook(UI::Logger::Level::error, static_cast<size_t>(Logger::Level::error));
    UI::Logger::setHook(UI::Logger::Level::fatal, static_cast<size_t>(Logger::Level::fatal));
    UI::Logger::init("./logs");

    Texture::init();
    UI::Font::service.addSearchPaths({ "./res/fonts" });

    auto lservice = UI::Locale::Service::getInstance();
    lservice.setBasePath("./res/langs/");
    lservice.attachLangFiles({ "zh_CN", "en_US" });
    lservice.setLang("zh_CN");

    UI::Theme::SystemTheme.ControlDarkBrush = UIMakeSolidColorBrush(UI::Base::Color(0.2, 0.2, 0.2, 0.6));
    UI::Theme::SystemTheme.ControlHeightLightBrush = UIMakeSolidColorBrush(UI::Base::Color(1.2 * 0.2, 1.2 * 0.2, 1.2 * 0.2, 0.8 * 0.6));
    UI::Theme::SystemTheme.ControlOnPressBrush = UIMakeSolidColorBrush(UI::Base::Color(0.2 * 0.8, 0.2 * 0.8, 0.2 * 0.8, 0.9));
    UI::Theme::SystemTheme.DefaultFont = UI::Font::service.getRenderer("SourceHanSansCN-Normal", 17, UI::Base::Color(1.0, 1.0, 1.0, 1.0));
}

void Application::afterLaunch()
{
    addWindow(std::make_shared<MainWindow>(m_width, m_height, m_title));
    infostream << "Game started!";
}

void Application::onTerminate()
{
    infostream << "Terminating...";
    Texture::free();
}
