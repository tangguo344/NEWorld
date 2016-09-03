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
#include <boost/dll/shared_library.hpp>
#include <common.h> // For NWAPICALL
#include "../shared/jsonhelper.h"

typedef void NWAPICALL MainFunction(int, char**);

int main(int argc, char** argv)
{
    std::string file;

    Json settings = readJsonFromFile(SettingsFilename);

    std::string in;
    if (argc == 1)
    {
        std::cout << "NEWorld Minimal Launcher" << std::endl;
        std::cout << "Enter 'client' to run client" << std::endl;
        std::cout << "Enter 'server' to run server" << std::endl;
        std::cin >> in;
    }
    else
    {
        in = argv[1];
    }

    std::string clientFilename = getJsonValue<std::string>(settings["client"]["file"], "NEWorld.dll");
    std::string serverFilename = getJsonValue<std::string>(settings["server"]["file"], "NEWorldServer.dll");

    file = in == "server" ? serverFilename : clientFilename;

    boost::dll::shared_library(file, boost::dll::load_mode::append_decorations).get<MainFunction>("main")(argc, argv);
}
