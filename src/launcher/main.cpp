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
#include <jsonhelper.h>

typedef int NWAPICALL MainFunction(int, char*[]);

void printUsage()
{
    
}

int main(int argc, char *argv[])
{
    Json settings = readJsonFromFile(SettingsFilename);
    
    if(argc >= 2)
    {
        std::string libname;
        std::string in(argv[1]);
        if(in == "client")
        {
            libname = getJsonValue<std::string>(settings["client"]["file"], "nwclient");
        }
        else if(in == "server")
        {
            libname = getJsonValue<std::string>(settings["server"]["file"], "nwserver");
        }
        else
        {
            printUsage();
            return 1;
        }
        argv++;
        argv[0] = const_cast<char*>(libname.c_str());
        return boost::dll::shared_library(libname, boost::dll::load_mode::append_decorations).get<MainFunction>("main")(argc-1, argv);
    }
    else
    {
        printUsage();
    }
    return 1;
}
