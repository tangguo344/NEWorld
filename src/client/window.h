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

#include <uilib.h>

class MainWindow : public UI::Core::Window
{
public:
    std::shared_ptr<UI::Base::Texture> btex[6]; //What is it? --Miigon; May be the background images of the main menu, but it shouldn't be here --Null
    MainWindow(int width, int height, const std::string& title);

private:
    std::thread loader;
};


#endif
