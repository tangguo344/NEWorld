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

#include <thread>
#include <uilib.h>
#include "vec3.h"
#include "renderer.h"
#include "pages.h"

class GameView :public UI::Controls::GLContext
{
public:
    GameView(UI::Core::Window*);
    void doRender();
    void onResize(size_t w, size_t h) override;
    void init(UI::Core::Window* win);
    void onKey(int key);
private:
    int windowWidth = 852, windowHeight = 480;
    Vec3f trans{ 0.0f, 0.0f, -100.0f };
    Vec3f transSpeed{0.0f, 0.0f, 0.0f};
    VertexBuffer cube;
};

class MainWindow : public UI::Core::Window
{
    std::thread loader;
public:
    std::shared_ptr<UI::Base::Texture> btex[6];
    MainWindow(int width, int height, const string& title) : UI::Core::Window(title, width, height, 200, 200)
    {
        background = std::make_shared<UI::Graphics::Brushes::ImageBrush>(std::make_shared<UI::Base::Image>("./Res/ss.png"));
        loader = std::thread([this]()
        {
            btex[0] = std::make_shared<UI::Base::Texture>("./Res/bkg0.png");
            btex[1] = std::make_shared<UI::Base::Texture>("./Res/bkg3.png");
            btex[2] = std::make_shared<UI::Base::Texture>("./Res/bkg2.png");
            btex[3] = std::make_shared<UI::Base::Texture>("./Res/bkg1.png");
            btex[4] = std::make_shared<UI::Base::Texture>("./Res/bkg4.png");
            btex[5] = std::make_shared<UI::Base::Texture>("./Res/bkg5.png");
            //Load Something
            //std::this_thread::sleep_for(2000ms);
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
};

#endif
