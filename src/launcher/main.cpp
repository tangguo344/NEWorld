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
#include <common.h> // For NWAPICALL
#include <library.h>
#include <logger.h>
#include <jsonhelper.h>

typedef void NWAPICALL MainFunction(int, char**);

int main(int argc, char** argv)
{
    Logger::init("launcher");
    std::string file;

    Json settings = readJsonFromFile(SettingsFilename);

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

    std::string clientFilename = getJsonValue<std::string>(settings["client"]["file"], "nwclient.dll");
    std::string serverFilename = getJsonValue<std::string>(settings["server"]["file"], "nwserver.dll");

    if (argc == 2)
    {
        file = in == "server" ? serverFilename : clientFilename;
    }
    else
    {
        infostream << "Opening file:" << argv[2] << " directly";
        file = argv[2];
    }

    Library(file).get<MainFunction>("main")(argc, argv);
}
