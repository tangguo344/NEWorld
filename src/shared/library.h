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
#include "logger.h"
namespace
{

#ifdef NEWORLD_TARGET_WINDOWS
    using HandleType = HMODULE;

    HandleType loadLibrary(std::string filename, bool& success)
    {
        HandleType handle = LoadLibraryA(filename.c_str());
        success = handle != nullptr;
        if (!success) warningstream << "Failed to load " << filename << ". Error code:" << GetLastError();
        return handle;
    }

    template<class T> T* getFunc(HandleType handle, std::string name)
    {
        assert(handle != nullptr);
        return reinterpret_cast<T*>(GetProcAddress(handle, name.c_str()));
    }

    void freeLibrary(HandleType handle)
    {
        FreeLibrary(handle);
    }
#else
#include <dlfcn.h>
    using HandleType = void*;

    HandleType loadLibrary(std::string filename, bool& success)
    {
        HandleType handle = dlopen(filename.c_str(), RTLD_LAZY);
        success = handle != nullptr;
        return handle;
    }

    template<class T> T* getFunc(HandleType handle, std::string name)
    {
        assert(handle != nullptr);
        return reinterpret_cast<T*>(dlsym(handle, name.c_str()));
    }

    void freeLibrary(HandleType handle)
    {
        dlclose(handle);
    }
#endif

}

class Library
{
public:
    Library() = default;
    Library(std::string filename) :Library{} { load(filename); }
    Library(Library&& library) :Library{}
    {
        std::swap(library.m_dllHandle, m_dllHandle);
        std::swap(library.m_loaded, m_loaded);
    }
    ~Library() {if(isLoaded()) freeLibrary(m_dllHandle); }

    Library(const Library&) = delete;
    Library& operator=(const Library&) = delete;

    template<class T> T* get(std::string name) {return (T*)getFunc<T*>(m_dllHandle, name);}

    operator bool() const { return isLoaded(); }
    bool isLoaded() const { return m_loaded; }

    void load(std::string filename)
    {
        if (isLoaded()) unload();
        m_dllHandle = loadLibrary(filename, m_loaded);
    }

    void unload() { freeLibrary(m_dllHandle); m_loaded = false; }
private:
    HandleType m_dllHandle;
    bool m_loaded = false;
};

#endif
