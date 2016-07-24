/*
UILib - A Open-Source UI-Library

Copyright(C) 2016 Infinideastudio-UITeam

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and / or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/

#include <uicontrols.h>
#include <uishapes.h>
#include <GL/glew.h>

namespace UI
{
    namespace Controls
    {
        using namespace Graphics::Shapes;
        void Label::render()
        {
            Rectangle::draw(cMargin.absrect, backgroundBrush, borderBrush);
            Font->renderStr((cMargin.absrect.xmin + cMargin.absrect.xmax - Font->getStrWidth((std::string)caption)) / 2,
                            (cMargin.absrect.ymin + cMargin.absrect.ymax - Font->fos.height * 1.25 )/ 2, cMargin.absrect, (std::string)caption);
        }

        Label::Label(std::string _xName, Margin _Margin, Str _caption) :
            Control(_xName, _Margin), caption(_caption)
        {
            gridPosX = gridPosY = 0;
            backgroundBrush = Theme::SystemTheme.ControlBrush;
            borderBrush = nullptr;
            Font = Theme::SystemTheme.DefaultFont;
        }

        void Button::render()
        {
            if (pressed)
                Rectangle::draw(cMargin.absrect, backgroundOnPressBrush, borderOnPressBrush);
            else if (mouseOn)
                Rectangle::draw(cMargin.absrect, backgroundHighlightBrush, borderHighlightBrush);
            else
                Rectangle::draw(cMargin.absrect, backgroundBrush, borderBrush);
            Font->renderStr((cMargin.absrect.xmin + cMargin.absrect.xmax - Font->getStrWidth((std::string)caption)) / 2,
                            (cMargin.absrect.ymin + cMargin.absrect.ymax - Font->fos.height * 1.25) / 2, cMargin.absrect, (std::string)caption);
        }

        Button::Button(std::string _xName, Margin _Margin, Str _caption, NotifyFunc _onClick) :
            Control(_xName, _Margin), caption(_caption), onClick(_onClick),
            backgroundOnPressBrush(Theme::SystemTheme.ControlOnPressBrush),
            backgroundHighlightBrush(Theme::SystemTheme.ControlHeightLightBrush),
            backgroundBrush(Theme::SystemTheme.ControlDarkBrush),
            borderOnPressBrush(Theme::SystemTheme.ActiveBorderBrush),
            borderHighlightBrush(Theme::SystemTheme.ActiveBorderBrush),
            borderBrush(Theme::SystemTheme.InactiveBorderBrush),
            Font(Theme::SystemTheme.DefaultFont)
        {
            gridPosX = gridPosY = 0;
            mouseButtonFunc.connect([this](MouseButton Button, ButtonAction Action)
            {
                if ((Action == ButtonAction::Release) && mouseOn) onClick();
            });
        }

        void TextBox::render()
        {
            const int wid = Font->getStrWidth(text);
            int cursorpxpos = Font->getStrWidth(text.substr(0, cursorpos) + (imeinp ? appendingstr : L""));
            int leftp = (wid >= cMargin.absrect.xmax - 20) ? cMargin.absrect.xmax - wid - 10 : cMargin.absrect.xmin + 10;
            if (-leftp - 10 > cursorpxpos) leftp = -cursorpxpos + 10;
            if (focused)
            {
                cposrdcount++;
                Rectangle::draw(cMargin.absrect, backgroundHighlightBrush, borderHighlightBrush);
                if (cposrdcount > 15)
                {
                    Rect r = cMargin.absrect;
                    r.xmin = leftp + cursorpxpos + 2;
                    r.xmax = r.xmin + 2;
                    r.ymin += 5;
                    r.ymax -= 5;
                    Rectangle::draw(r, cursorBrush, nullptr);
                    if (cposrdcount > 50) cposrdcount = 0;
                }
            }
            else
            {
                Rectangle::draw(cMargin.absrect, backgroundBrush, borderBrush);
            }

            if (!imeinp)
                Font->renderStr(leftp, cMargin.absrect.ymin + 2, cMargin.absrect, text);
            else
                Font->renderStr(leftp,  cMargin.absrect.ymin + 2, cMargin.absrect, text.substr(0, cursorpos) + appendingstr + text.substr(cursorpos));
        }

        SDL_Rect inputRect = { 0,0,800,50 };


        TextBox::TextBox(std::string _xName, Margin _Margin, std::wstring _text, Str _mask, NotifyFunc _onEditDone) :
            Control(_xName, _Margin), text(_text), mask(_mask), onEditDone(_onEditDone), cursorpos(0), cposrdcount(0), imeinp(false)
        {
            gridPosX = gridPosY = 0;
            backgroundHighlightBrush = Theme::SystemTheme.ControlBrush;
            backgroundBrush = Theme::SystemTheme.ControlBrush;
            borderHighlightBrush = Theme::SystemTheme.ActiveBorderBrush;
            borderBrush = Theme::SystemTheme.InactiveBorderBrush;
            cursorBrush = Theme::SystemTheme.ControlDarkDarkBrush;
            Font = Theme::SystemTheme.DefaultFont;
            charInputFunc.connect([this](CInpArgs e)
            {
                imeinp = e.editing;
                if (!imeinp)
                {
                    appendingstr = L"";
                    text = text.substr(0, cursorpos)+ e.str + text.substr(cursorpos);
                    cursorpos += e.str.size();
                }
                else
                {
                    appendingstr = e.str;
                }
            });

            keyFunc.connect([this](int Key, ButtonAction Action)
            {
                switch (Action)
                {
                case UI::Core::Press:
                    if (appendingstr.empty())
                    {
                        switch (Key)
                        {
                        case SDLK_BACKSPACE:
                            if (!text.empty())
                            {
                                text = text.substr(0, cursorpos - 1) + text.substr(cursorpos);
                                cursorpos--;
                                cposrdcount = 16;
                            }
                            break;
                        case SDLK_LEFT:
                            if (cursorpos > 0)
                            {
                                cursorpos--;
                                cposrdcount = 16;
                            }
                            break;
                        case SDLK_DELETE:
                            if (cursorpos < text.length())
                            {
                                text = text.substr(0, cursorpos) + text.substr(cursorpos + 1);
                                cposrdcount = 16;
                            }
                            break;
                        case SDLK_RIGHT:
                            if (cursorpos < text.length())
                            {
                                cursorpos++;
                                cposrdcount = 16;
                            }
                        default:
                            break;
                        }
                    }
                    break;
                default:
                    break;
                }
            });

            focusFunc.connect([this](FocusOp Stat)
            {
                switch (Stat)
                {
                case UI::Core::Lose:
                    SDL_StopTextInput();
                    imeinp = false;
                    appendingstr.clear();
                    break;
                case UI::Core::Gain:
                    inputRect.x = cMargin.absrect.xmin;
                    inputRect.y = cMargin.absrect.ymax;
                    inputRect.w = 500;
                    inputRect.h = 50;
                    SDL_SetTextInputRect(&inputRect);
                    SDL_StartTextInput();
                    break;
                default:
                    break;
                }
            });

        }

        void CheckBox::render()
        {
        }

        CheckBox::CheckBox(std::string _xName, Margin _Margin, Str _caption, bool _threeStat, CheckStat _stat, NotifyFunc _onStatChange) :
            Control(_xName, _Margin), threeStatus(_threeStat), stat(_stat), onStatChange(_onStatChange), caption(_caption)
        {
        }

        GLContext::GLContext() :
            Grid()
        {
        }

        void GLContext::render()
        {
            if (onRenderF)
            {
                glPushMatrix();
                onRenderF();
                glPopMatrix();
                glClear(GL_DEPTH_BUFFER_BIT);
                glViewport(0, 0, (GLint)cMargin.absrect.xmax, (GLint)cMargin.absrect.ymax);
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glOrtho(0, cMargin.absrect.xmax, cMargin.absrect.ymax, 0, -100, 10000);
            }

            Grid::render();
        }

        ImageBox::ImageBox(std::string _xName, Margin _Margin, std::shared_ptr<Brush> _imgbrush):
            Control(_xName, _Margin), imgbrush(_imgbrush)
        {
        }

        void ImageBox::render()
        {
            Rectangle::draw(cMargin.absrect, imgbrush, nullptr);
        }
    }
}
