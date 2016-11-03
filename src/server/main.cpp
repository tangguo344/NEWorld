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

extern "C" NWAPIEXPORT int NWAPICALL main(int, char**);
extern "C" NWAPIEXPORT bool NWAPICALL nwInitServer(int, char **);
extern "C" NWAPIEXPORT void NWAPICALL nwRunServer();
extern "C" NWAPIEXPORT void NWAPICALL nwStopServer();
Server* server = nullptr;
NWAPIEXPORT int NWAPICALL main(int argc, char** argv)
{
    nwInitServer(argc, argv);
    nwRunServer();
    nwStopServer();
}
NWAPIEXPORT bool NWAPICALL nwInitServer(int argc, char ** argv)
{
    Logger::init("server");
    infostream << "\n----------------------------------------"
               << CopyrightString
               << "----------------------------------------";
    infostream << "NEWorld Server v" << NEWorldVersion;
    try
    {
        server = new Server(std::vector<std::string>(argv + 1, argv + argc));
    }
    catch (std::exception& e)
    {
        fatalstream << "Unhandled exception: " << e.what();
    }
    return true;
}

NWAPIEXPORT void NWAPICALL nwRunServer()
{
    try
    {
        server->run();
    }
    catch (std::exception& e)
    {
        fatalstream << "Unhandled exception: " << e.what();
    }
}

NWAPIEXPORT void NWAPICALL nwStopServer()
{
    try
    {
        server->stop();
        delete server;
        infostream << "Server is stopping...";
    }
    catch (std::exception& e)
    {
        fatalstream << "Unhandled exception: " << e.what();
    }
}

