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
#include "mainmenu.h"

MainWindow::MainWindow(int width, int height, const std::string& title) : UI::Core::Window(title, width, height, 200, 200)
{
    background = std::make_shared<UI::Graphics::Brushes::ImageBrush>(std::make_shared<UI::Base::Image>("./res/ss.png"));
    loader = std::thread([this]()
    {
        btex[0] = std::make_shared<UI::Base::Texture>("./res/bkg0.png");
        btex[1] = std::make_shared<UI::Base::Texture>("./res/bkg3.png");
        btex[2] = std::make_shared<UI::Base::Texture>("./res/bkg2.png");
        btex[3] = std::make_shared<UI::Base::Texture>("./res/bkg1.png");
        btex[4] = std::make_shared<UI::Base::Texture>("./res/bkg4.png");
        btex[5] = std::make_shared<UI::Base::Texture>("./res/bkg5.png");

        // Load Something
        //         std::this_thread::sleep_for(2000ms);

        renderdelegate.push_back([this]()
        {
            background = UI::Theme::SystemTheme.WindowBrush;
            // Load the main menu
            pushPage(std::make_shared<BackGround>(this), false, false);
            pushPage(std::make_shared<MainMenu>(this), false, true);
            loader.join();
        });
    });
}
