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

#include <iostream>
#include <string>
#include <climits>
#include <common.h>
#include <library.h>
#include <logger.h>
#include <jsonhelper.h>

typedef void NWAPICALL MainFunction(int, char**);

int main(int argc, char** argv)
{
    getSettings("launcher");
    Logger::init("launcher");
    std::string file;

    std::string in;
    if (argc == 1)
    {
        infostream << "NEWorld Minimal Launcher";
        infostream << "Enter 'client' to run client";
        infostream << "Enter 'server' to run server";
        std::cin >> in;

        // Clear cin buffer
        std::cin.ignore(INT_MAX, '\n');
    }
    else
    {
        in = argv[1];
    }
		
    std::string clientFilename = getJsonValue<std::string>(getSettings()["client"]["file"], "nwclient");
    std::string serverFilename = getJsonValue<std::string>(getSettings()["server"]["file"], "nwserver");

    if (argc == 2)
    {
        file = in == "server" ? serverFilename : clientFilename;
    }
    else if (argc == 3)
    {
        file = argv[2];
    }

    debugstream << "Load:" << file;
    Library(file).get<MainFunction>("main")(argc, argv);
}
