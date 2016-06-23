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

#include <utility>
#include <string>
#include "server.h"
#include <common.h>
#include <logger.h>
#include <thread>
#include "settings.h"
void inputThreadFunc();

int main(int, char*[])
{
    loadSettings();
    loggerInit();
    std::thread inputThread(inputThreadFunc);
    infostream << CopyrightString;
    infostream << "NEWorld Server v" << NEWorldVersion << ", which was compiled at " << __TIME__ << " " << __DATE__;
    infostream << "Server starting...";
    try
    {
        Server s(ioService, Port);
        ioService.run();
    }
    catch (std::exception& e)
    {
        fatalstream << "Exception: " << e.what();
    }
    infostream << "Server is stopping...";
    inputThread.join();
    saveSettings();
    return 0;
}
