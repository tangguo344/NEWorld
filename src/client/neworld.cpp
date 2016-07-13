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
#include "renderer.h"

void NEWorld::run()
{
    // Initialize here
    loggerInit("NEWorld");
    infostream << "Initializing...";

    UI::Logger::init("./Logs");
    UI::Font::service.addSearchPaths({"./Fonts"});
    UI::Globalization::Service::getInstance().setBasePath("./Langs/");
    UI::Globalization::Service::getInstance().attachLangFiles({"chinese", "english"});
    UI::Globalization::Service::getInstance().setLang("chinese");

    //std::thread serverThread(networkThread);
    App app(852, 480, "NEWorld");

    // Start to run
    infostream << "Game start!";
    app.run();

    // Destroy here
    infostream << "Terminating...";
    UI::Logger::service.dump();
    //serverThread.join();
    disconnect();
}