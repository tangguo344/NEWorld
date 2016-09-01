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

#include "server.h"
#include <common.h>
#include <logger.h>
#include <thread>
#include "settings.h"
#include <pluginapi.h>

extern "C" NWAPIEXPORT int NWAPICALL main(int, char**);

int NWAPICALL main(int argc, char** argv)
{
    loadSettings();
    Logger::init("server");
    infostream << "\n----------------------------------------"
               << CopyrightString
               << "----------------------------------------";
    infostream << "NEWorld Server v" << NEWorldVersion << ", Release Version:" << ReleaseVersion << ", compiled at " << __TIME__ << " " << __DATE__;
    infostream << "Server starting...";
    try
    {
        Server s(ioService, globalPort);
        ioService.run();
    }
    catch (std::exception& e)
    {
        fatalstream << "Exception: " << e.what() << ".Press Enter to exit.";
    }
    infostream << "Server is stopping...";
    saveSettings();
    return 0;
}
