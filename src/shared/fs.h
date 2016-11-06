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

#include <common.h>
#include <string>
#include <functional>

#ifdef NEWORLD_TARGET_WINDOWS
    #include <io.h>
    #include <direct.h>
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/stat.h>
#endif

namespace filesystem
{
    inline bool exists(const std::string& path)
    {
        return _access(path.c_str(), 0) == 0;
    }
    inline void create_directories(const std::string& path)
    {
#ifdef NEWORLD_TARGET_WINDOWS
        _mkdir(path.c_str());
#else
        mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
    }

    inline void files_in_dir(std::string path, std::function<void(std::string)> callback)
    {
#ifdef NEWORLD_TARGET_WINDOWS
        WIN32_FIND_DATA ffd;
        HANDLE hFind = FindFirstFileA((path+"\\*").c_str(), &ffd);
        do
        {
            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
            callback(path + "\\" + ffd.cFileName);
        }
        while (FindNextFileA(hFind, &ffd) != 0);
        FindClose(hFind);
#else
        DIR* pDir = opendir(path.c_str());
        struct dirent *ent;
        while ((ent = readdir(pDir)) != nullptr)
        {
            if (ent->d_type & DT_DIR) continue;
            callback(ent->d_name);
        }
#endif
    }
}
