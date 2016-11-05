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

#ifndef JSONHELPER_H_
#define JSONHELPER_H_

#include <string>
#include <fstream>
#include "json.hpp"
#include "logger.h"

using Json = nlohmann::json;

const std::string SettingsFilename = "./settings";

inline Json readJsonFromFile(std::string filename)
{
    std::ifstream file(filename);
    if (file.good())
    {
        std::string content = std::string((std::istreambuf_iterator<char>(file)),
                                          std::istreambuf_iterator<char>());
        if (!content.empty())
            return Json::parse(content);
    }
    else
    {
        fatalstream << "Can't open file\"" << filename << "\"for reading\n";
    }
    return Json();
}

inline void writeJsonToFile(std::string filename, Json& json)
{
    if(!json.is_null()) std::ofstream(filename) << json.dump();
}

// get a json value. If it does not exist, return the default value and write it to the json
template<class T>
T getJsonValue(Json& json, T defaultValue=T())
{
    if (json.is_null())
    {
        json = defaultValue;
        return defaultValue;
    }
    return json;
}
class JsonSaveHelper
{
public:
    JsonSaveHelper(Json& json,std::string filename) :mJson(json), mFilename(filename) {}
    ~JsonSaveHelper()
    {
        writeJsonToFile(mFilename, mJson);
    }
private:
    Json& mJson;
    std::string mFilename;
};
Json& getSettings(const std::string& suffix = "");
#endif
