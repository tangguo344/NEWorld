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
#include <common/common.h>
#include <common/nwsafety.hpp>
#include <thread>
#include <common/jsonhelper.h>

NWDECLEARLOGGER("server")

extern "C"
{
    NWAPIEXPORT int NWAPICALL main(int, char**);
    NWAPIEXPORT bool NWAPICALL nwInitServer(void*);
    NWAPIEXPORT void NWAPICALL nwRunServer();
    NWAPIEXPORT void NWAPICALL nwStopServer();
    NWAPIEXPORT World* NWAPICALL nwLocalServerGetWorld(size_t id);
    NWAPIEXPORT void NWAPICALL nwLocalServerLogin(const char* username, const char* password);
    NWAPIEXPORT Chunk* NWAPICALL nwLocalServerGetChunk(int32_t x, int32_t y, int32_t z);
}

Server* server = nullptr;

NWAPIEXPORT int NWAPICALL main(int, char**)
{
    nwInitServer(0);
    nwRunServer();
    nwStopServer();
}

NWAPIEXPORT bool NWAPICALL nwInitServer(void* mode)
{
    getSettings();
    infostream << "\n----------------------------------------"
               << CopyrightString
               << "----------------------------------------";
    infostream << "NEWorld Server v" << NEWorldVersion;
    try
    {
        if (!mode) 
            server = new Server();
        else
            server = new LocalTunnelServer(mode);
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
        infostream << "Server is stopping...";
        server->stop();
        delete server;
    }
    catch (std::exception& e)
    {
        fatalstream << "Unhandled exception: " << e.what();
    }
}

NWAPIEXPORT World* NWAPICALL nwLocalServerGetWorld(size_t id)
{
    return reinterpret_cast<LocalTunnelServer*>(server)->getWorld(id);
}

NWAPIEXPORT void NWAPICALL nwLocalServerLogin(const char* username, const char* password)
{
    reinterpret_cast<LocalTunnelServer*>(server)->login(username, password);
}

NWAPIEXPORT Chunk* NWAPICALL nwLocalServerGetChunk(int32_t x, int32_t y, int32_t z)
{
    return reinterpret_cast<LocalTunnelServer*>(server)->getChunk(x, y, z);
}
