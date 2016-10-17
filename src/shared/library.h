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

#ifndef LIBRARY_H_
#define LIBRARY_H_
#include <string>
#include <functional>
#include "common.h"
namespace{

#ifdef NEWORLD_TARGET_WINDOWS
using HandleType = HINSTANCE;

HandleType loadLibrary(std::string filename) {
    return LoadLibrary(filename.c_str());
}

template<class T>
std::function<T> getFunc(HandleType handle, std::string name) {
    return (std::function<T>)GetProcAddress(handle, name.c_str());
}

void freeLibrary(HandleType handle) {
    FreeLibrary(handle);
}
#endif

}

class Library {
public:
    Library(std::string filename) :m_dllHandle(loadLibrary(filename)) {}
    ~Library() {freeLibrary(m_dllHandle); }
    template<class T> std::function<T> get(std::string name) {return getFunc<T>(m_dllHandle, name);}
private:
    HandleType m_dllHandle;
};

#endif
