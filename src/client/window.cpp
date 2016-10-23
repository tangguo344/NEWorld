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

#include "window.h"
#include <logger.h>

size_t Window::mRefCount;
SDL_GLContext Window::mContext;

Window::Window(const std::string& title, int width, int height)
    : mTitle(title), mWidth(width), mHeight(height)
{
    if (!mRefCount)
    {
        // First window, init SDL
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
        SDL_GL_SetSwapInterval(0);
    }

    mWindow = SDL_CreateWindow(mTitle.c_str(), 100, 100, mWidth, mHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (mWindow == nullptr)
        fatalstream << "Failed to create SDL window!";

    if (!mRefCount)
        mContext = SDL_GL_CreateContext(mWindow);
    else
        makeCurrentDraw();

    mRefCount++;
}

Window::~Window()
{
    SDL_DestroyWindow(mWindow);

    mRefCount--;

    if (!mRefCount)
    {
        // No window exists, terminate SDL
        SDL_GL_DeleteContext(mContext);
        SDL_Quit();
    }
}
