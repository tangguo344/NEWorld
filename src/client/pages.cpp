/*
* NEWorld: A free game with similar rules to Minecraft.
* Copyright (C) 2016 NEWorld Team
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "pages.h"
#include <thread>
#include "gameview.h"

MainMenu::MainMenu(UI::Core::Window * win)
{
    addChild(std::make_shared<UI::Controls::Button>("BTPlay", UI::Core::Margin::CenterCenter(100, 100, 50, 50), UI::Globalization::Str("Play", false), [this, win]() { win->pushPage(std::make_shared<GameView>(win), false, false); }));
}

BackGround::BackGround(UI::Core::Window * win) :elapsed(0)
{
    onRenderF = [this, win]()
    {
        Renderer::clear();
        Renderer::restoreProj();
        Renderer::applyPerspective(60.0f, cMargin.absrect.xmax / cMargin.absrect.ymax, 0.1f, 1000.0f);
        Renderer::restoreScale();
        Renderer::rotate((elapsed++) * 0.1, Vec3f(0.1f, 1.0f, 0.1f));

        glDisable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_2D);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        const float g[6][4][3] = { { { -1.0f, 1.0f, -1.0f },{ 1.0f, 1.0f, -1.0f },{ 1.0f, -1.0f, -1.0f },{ -1.0f, -1.0f, -1.0f } },
        { { 1.0f, 1.0f, -1.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, -1.0f, 1.0f },{ 1.0f, -1.0f, -1.0f } },
        { { 1.0f, 1.0f, 1.0f },{ -1.0f, 1.0f, 1.0f },{ -1.0f, -1.0f, 1.0f },{ 1.0f, -1.0f, 1.0f } },
        { { -1.0f, 1.0f, 1.0f },{ -1.0f, 1.0f, -1.0f },{ -1.0f, -1.0f, -1.0f },{ -1.0f, -1.0f, 1.0f } },
        { { -1.0f, -1.0f, 1.0f },{ -1.0f, -1.0f, -1.0f },{ 1.0f, -1.0f, -1.0f },{ 1.0f, -1.0f, 1.0f } },
        { { 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f, -1.0f },{ -1.0f, 1.0f, -1.0f },{ -1.0f, 1.0f, 1.0f } } };
        //Begin to draw a cube
        for (int i = 0; i < 6; ++i)
        {
            GLuint tex = ((MainWindow*)win)->btex[i]->gettex();
            glBindTexture(GL_TEXTURE_2D, tex);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(g[i][0][0], g[i][0][1], g[i][0][2]);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(g[i][1][0], g[i][1][1], g[i][1][2]);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(g[i][2][0], g[i][2][1], g[i][2][2]);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(g[i][3][0], g[i][3][1], g[i][3][2]);
            glEnd();
        }
        glDisable(GL_TEXTURE_2D);
    };
}
