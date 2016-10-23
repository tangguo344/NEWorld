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

#ifndef WINDOW_H_
#define WINDOW_H_

#include <string>
#include <SDL2/SDL.h>

class Window
{
public:
    Window(const std::string& title, int width, int height);
    ~Window();

    void makeCurrentDraw() const
    {
        SDL_GL_MakeCurrent(mWindow, mContext);
    }

    void swapBuffers() const
    {
        SDL_GL_SwapWindow(mWindow);
    }

    static bool isKeyDown(SDL_Scancode scancode)
    {
        return SDL_GetKeyboardState(nullptr)[scancode];
    }

    static void pollEvents()
    {
        /*
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            // TODO: Process events here
        }
        */
        SDL_PumpEvents();
    }

private:
    SDL_Window* mWindow = nullptr;
    std::string mTitle;
    int mWidth, mHeight;

    static size_t mRefCount;
    static SDL_GLContext mContext;
};

#endif
