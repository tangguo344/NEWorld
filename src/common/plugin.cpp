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

#include "plugin.h"
#include "logger.h"

typedef NWplugindata* NWAPICALL GetInfoFunction();
typedef void NWAPICALL InitFunction();
typedef void NWAPICALL UnloadFunction();

int Plugin::init() {
    InitFunction* init = nullptr;
    try {
        init = mLib.get<InitFunction>("init");
        init();
    } catch (std::exception& e) {
        if (!mLib.isLoaded()) return mStatus = 1; // Failed: could not load
        if (init == nullptr) return mStatus = 2; // Failed: entry not found
        warningstream << "Failed: unhandled exception: " << e.what();
    }
    return mStatus = 0;
}

int Plugin::loadFrom(const std::string& filename) {
    GetInfoFunction* getinfo = nullptr;
    try {
        mLib.load(filename);
        getinfo = mLib.get<GetInfoFunction>("getInfo");
        mData = getinfo();
    } catch (std::exception& e) {
        if (!mLib.isLoaded()) return mStatus = 1; // Failed: could not load
        if (getinfo == nullptr) return mStatus = 2; // Failed: entry not found
        warningstream << "Failed: unhandled exception: " << e.what();
    }
    return mStatus = 0;
}

void Plugin::unload() {
    if (mStatus != 0) return;
    mStatus = -1;
    UnloadFunction* unload = nullptr;
    try {
        unload = mLib.get<UnloadFunction>("unload");
        unload();
        mLib.unload();
    } catch (std::exception& e) {
        if (unload == nullptr) {
            // Warning: entry not found
            warningstream << "Subroutine unload() not found in plugin " << mData->internalName << ", skipped unloading!";
            return;
        }
        warningstream << "Failed: unhandled exception: " << e.what();
    }
}
