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
#include "settings.h"
#include <logger.h>
#include "server.h"

void loadSharedSettings(Settings& settings);

Settings settings("Configs/server_settings.conf");

void loadSettings()
{
    loadSharedSettings(settings);
    globalPort = static_cast<unsigned short>(settings.get<int>("server.server.port", 8090));
    saveSettings();
}

void saveSettings()
{
    settings.save();
}
