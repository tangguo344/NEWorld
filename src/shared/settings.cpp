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

#include "settingsmanager.h"
#include "logger.h"
#include "common.h"

void loadSharedSettings(Settings& settings)
{
    using Level = Logger::Level;
    /*
#ifdef NEWORLD_DEBUG
    Logger::coutLevel = settings.get<Level>("shared.logger.coutLevel", Level::trace);
    Logger::cerrLevel = settings.get<Level>("shared.logger.cerrLevel", Level::fatal);
    Logger::fileLevel = settings.get<Level>("shared.logger.fileLevel", Level::trace);
    Logger::lineLevel = settings.get<Level>("shared.logger.lineLevel", Level::warning);
#else
    Logger::coutLevel = settings.get<Level>("shared.logger.coutLevel", Level::info);
    Logger::cerrLevel = settings.get<Level>("shared.logger.cerrLevel", Level::fatal);
    Logger::fileLevel = settings.get<Level>("shared.logger.fileLevel", Level::info);
    Logger::lineLevel = settings.get<Level>("shared.logger.lineLevel", Level::warning);
#endif
    */
    settings.setMinimal(settings.get<bool>("shared.settings.minimal", false));
}
