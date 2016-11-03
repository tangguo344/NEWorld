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
#include <jsonhelper.h>
#include <pluginapi.h>

extern "C" NWAPIEXPORT int NWAPICALL main(int, char**);
extern "C" NWAPIEXPORT void* NWAPICALL nwNewServer(int, char **);
extern "C" NWAPIEXPORT void NWAPICALL nwRunServer(void*);
extern "C" NWAPIEXPORT void NWAPICALL nwStopServer(void*);
extern "C" NWAPIEXPORT void NWAPICALL nwFreeServer(void*);


int NWAPICALL main(int argc, char** argv)
{
    Logger::init("server");
    infostream << "\n----------------------------------------"
               << CopyrightString
               << "----------------------------------------";
    infostream << "NEWorld Server v" << NEWorldVersion;
    try
    {
        Server server(std::vector<std::string>(argv + 1, argv + argc));    
        server.run();
    }
    catch (std::exception& e)
    {
        fatalstream << "Unhandled exception: " << e.what() << ".Press Enter to exit.";
    }
    infostream << "Server is stopping...";
    return 0;
}

NWAPIEXPORT void *NWAPICALL nwNewServer(int argc, char ** argv)
{
    Logger::init("server");
    infostream << "\n----------------------------------------"
        << CopyrightString
        << "----------------------------------------";
    infostream << "NEWorld Server v" << NEWorldVersion;
    Server* ret = nullptr;
    try
    {
        ret = new Server(std::vector<std::string>(argv + 1, argv + argc));
    }
    catch (std::exception& e)
    {
        fatalstream << "Unhandled exception: " << e.what();
    }
    return ret;
}

NWAPIEXPORT void NWAPICALL nwRunServer(void * s)
{
    try
    {
        reinterpret_cast<Server*>(s)->run();
    }
    catch (std::exception& e)
    {
        fatalstream << "Unhandled exception: " << e.what();
    }
}

NWAPIEXPORT void NWAPICALL nwStopServer(void * s)
{
    try
    {
        reinterpret_cast<Server*>(s)->stop();
        infostream << "Server is stopping...";
    }
    catch (std::exception& e)
    {
        fatalstream << "Unhandled exception: " << e.what();
    }
}

NWAPIEXPORT void NWAPICALL nwFreeServer(void * s)
{
    try
    {
        delete reinterpret_cast<Server*>(s);
    }
    catch (std::exception& e)
    {
        fatalstream << "Unhandled exception: " << e.what();
    }
}


