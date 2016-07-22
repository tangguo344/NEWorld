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

#include <uicore.h>
#include <uilogger.h>
#include <uibrushes.h>
#include <uishapes.h>
#include <chrono>
#include <thread>
#include <GL/glew.h>
#include <boost/bind.hpp>
#include <boost/locale/encoding_utf.hpp>

using namespace std::chrono_literals;

namespace UI
{
    namespace Base
    {
        void init();
    }

    namespace Core
    {
        Application* application;
        SDL_GLContext context = nullptr;
        std::thread rdt;

        Window::Window(const std::string & name, int width, int height, int _xpos, int _ypos)
            :terminated(false), resized(false), renderdelegate(), background(Theme::SystemTheme.WindowBrush)
        {
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

            try
            {
                window = SDL_CreateWindow(name.c_str(), _xpos, _ypos, width, height,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

                if(window == nullptr) throw Exceptions::WindowCreationFailure;
            }
            catch(Exceptions)
            {
                logerror("SDL_Window Creation Failure of name = " + name);
            }

            if (!context)
            {
                context = SDL_GL_CreateContext(window);
                SDL_GL_SetSwapInterval(2);
                SDL_GL_MakeCurrent(NULL, NULL);
            }
            resize(width, height);
        }

        Window::~Window()
        {
            if (!terminated) close();
        }

        void Window::dorender()
        {
            for (auto f : renderdelegate) f();
            renderdelegate.clear();

            glViewport(0, 0, (GLint)_x, (GLint)_y);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, _x, _y, 0, -100, 10000);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glMatrixMode(GL_MODELVIEW);
            glClearColor(0.0, 0.0, 0.0, 1.0);
            glLoadIdentity();
            Graphics::Shapes::Rectangle::draw(Base::Rect(0.0, _x, 0.0, _y), background, nullptr);
            if (!render.empty()) for (auto& c : render) c->render();
            UI::Font::flush();
            SDL_GL_SwapWindow(window);
        }

        void Window::close()
        {
            terminated = true;
            this->onClose();
            SDL_DestroyWindow(window);
        }

        void Window::onResize(size_t x, size_t y)
        {
        }

        _Connections Window::doconnect(std::shared_ptr<Grid> page)
        {
            return
            {
                focusFunc.connect([page](FocusOp Stat)
                {
                    page->focusFunc(Stat);
                }),
                mouseMove.connect([page](int x, int y, int dx, int dy)
                {
                    page->mouseMove(x, y, dx, dy);
                }),
                mouseButtonFunc.connect([page](MouseButton Button, ButtonAction Action)
                {
                    page->mouseButtonFunc(Button, Action);
                }),
                cursorEnterFunc.connect([page](CursorOp Stat)
                {
                    page->cursorEnterFunc(Stat);
                }),
                scrollFunc.connect([page](double dx, double dy)
                {
                    page->scrollFunc(dx, dy);
                }),
                keyFunc.connect([page](int Key, ButtonAction Action)
                {
                    page->keyFunc(Key, Action);
                }),
                charInputFunc.connect([page](CInpArgs e)
                {
                    page->charInputFunc(e);
                }),
                dropFunc.connect([page](const char* Paths)
                {
                    page->dropFunc(Paths);
                }),
                touchFunc.connect([page](int x, int y, ButtonAction action)
                {
                    page->touchFunc(x, y, action);
                }),
                touchmove.connect([page](int x, int y, int dx, int dy)
                {
                    page->touchmove(x, y, dx, dy);
                })
            };
        }

        void Window::pushPage(std::shared_ptr<Grid> page, bool event_trans, bool render_trans)
        {
            pages.push_back({page, event_trans, render_trans});
            if (!event_trans)
            {
                for (auto c : connections)
                    for (size_t cc = 0; cc < 10; ++cc) c._c[cc].disconnect();
                connections.clear();
            }
            connections.push_back(doconnect(page));
            if (!render_trans) render.clear();
            render.push_back(page);
            page->cMargin.updateAbs(Base::Rect(0, _x, 0, _y));
            page->setSize(_x, _y);
        }

        void Window::popPage()
        {
            int pos = pages.size() - 1;
            if (pos > 0)
            {
                do
                {
                    connections.push_front(doconnect(pages[pos].c));
                    --pos;
                }
                while (pages[pos].et && pos > 0);
                do
                {
                    render.push_back(pages[pos].c);
                    pages[pos].c->cMargin.updateAbs(Base::Rect(0, _x, 0, _y));
                    pages[pos].c->setSize(_x, _y);
                    --pos;
                }
                while (pages[pos].rt && pos > 0);
            }
        }

        void Window::clearPages()
        {
            for (auto c : connections)
                for (size_t cc = 0; cc < 10; ++cc) c._c[cc].disconnect();
            connections.clear();
            pages.clear();
            render.clear();
        }

        void Window::pageZero()
        {
            _Page p = *pages.begin();
            for (auto c : connections)
                for (size_t cc = 0; cc < 10; ++cc) c._c[cc].disconnect();
            connections.clear();
            pages.clear();
            render.clear();
            connections.push_back(doconnect(p.c));
            pages.push_back(p);
            render.push_back(p.c);
            p.c->cMargin.updateAbs(Base::Rect(0, _x, 0, _y));
            p.c->setSize(_x, _y);
        }

        void Window::resize(size_t x, size_t y)
        {
            _x = x;
            _y = y;
            resized = true;
            for (auto& c : render)
            {
                c->cMargin.updateAbs(Base::Rect(0, _x, 0, _y));
                c->setSize(_x, _y);
            }

            this->onResize(x, y);
        }

        void Window::onShow()
        {
        }

        void Window::onHide()
        {
        }

        void Window::onClose()
        {
        }

        void Window::onStatChange(StatChange stat)
        {
        }

        Uint32 Window::windowID()
        {
            return SDL_GetWindowID(window);
        }

        void Application::run()
        {
            init();

            rdt = std::thread([this]()
            {
                SDL_GL_MakeCurrent(mainWin->window, context);
                glShadeModel(GL_SMOOTH);
                glEnable(GL_BLEND);
                glDisable(GL_DEPTH_TEST);
                glEnable(GL_TEXTURE_2D);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
                glEnableClientState(GL_VERTEX_ARRAY);
                glEnableClientState(GL_COLOR_ARRAY);
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                while (!sigExit)
                {
                    if (windows.size() > 1)
                    {
                        for (auto w:windows)
                        {
                            if (!w.second->terminated)
                            {
                                SDL_GL_MakeCurrent(w.second->window, context);
                                w.second->dorender();
                            }
                        }
                        SDL_GL_MakeCurrent(mainWin->window, context);
                    }
                    else if (!mainWin->terminated) mainWin->dorender();
                }
                glDisableClientState(GL_VERTEX_ARRAY);
                glDisableClientState(GL_COLOR_ARRAY);
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            });
            while (!sigExit)
            {
                processMessages();
                std::this_thread::sleep_for(1ms);
            }
            rdt.join();
            for (auto w:windows) w.second->close();
            windows.clear();
            SDL_Quit();
            this->onTerminate();
        }

        MouseButton buttonTrans(Uint8 _button)
        {
            MouseButton button;

            switch(_button)
            {
                case SDL_BUTTON_LEFT:
                    button = MouseButton::Left;
                    break;

                case SDL_BUTTON_MIDDLE:
                    button = MouseButton::Middle;
                    break;

                case SDL_BUTTON_RIGHT:
                    button = MouseButton::Right;
                    break;

                case SDL_BUTTON_X1:
                    button = MouseButton::Preserved1;
                    break;

                case SDL_BUTTON_X2:
                    button = MouseButton::Preserved2;
                    break;
                default:
                    logwarning("Unsupported Mouse Button");
                    button = MouseButton::Preserved2;
                    break;
            }

            return button;
        }

        void Application::processMessages()
        {
            SDL_Event event;//事件
            std::shared_ptr<Window> curWin;
            while(SDL_PollEvent(&event)) //从队列里取出事件
            {
                switch(event.type)  //根据事件类型分门别类去处理
                {
                    case SDL_APP_TERMINATING:
                        break;

                    case SDL_APP_LOWMEMORY:
                        break;

                    case SDL_APP_WILLENTERBACKGROUND:
                        this->onEnteringBackground();
                        break;

                    case SDL_APP_WILLENTERFOREGROUND:
                        this->onEnteringForeground();
                        break;

                    case SDL_WINDOWEVENT:
                        switch(event.window.event)
                        {
                            case SDL_WINDOWEVENT_RESIZED:
                            case SDL_WINDOWEVENT_SIZE_CHANGED:
                                if (!winExist(event.window.windowID)) break;
                                windows[event.window.windowID]->resize(event.window.data1, event.window.data2);
                                break;

                            case SDL_WINDOWEVENT_CLOSE:
                            {
                                if (!winExist(event.window.windowID)) break;
                                windows[event.window.windowID]->close();

                                if(windows[event.window.windowID].get() == mainWin)
                                {
                                    event.type = SDL_QUIT;
                                    SDL_PushEvent(&event);
                                }

                                windows.erase(event.window.windowID);
                                break;
                            }

                            case SDL_WINDOWEVENT_MINIMIZED:
                                if (!winExist(event.window.windowID)) break;
                                windows[event.window.windowID]->onStatChange(StatChange::Minimize);
                                break;

                            case SDL_WINDOWEVENT_MAXIMIZED:
                                if (!winExist(event.window.windowID)) break;
                                windows[event.window.windowID]->onStatChange(StatChange::Maximize);
                                break;

                            case SDL_WINDOWEVENT_RESTORED:
                                if (!winExist(event.window.windowID)) break;
                                windows[event.window.windowID]->onStatChange(StatChange::Restore);
                                break;

                            case SDL_WINDOWEVENT_SHOWN:
                                if (!winExist(event.window.windowID)) break;
                                windows[event.window.windowID]->onShow();
                                break;

                            case SDL_WINDOWEVENT_HIDDEN:
                                if (!winExist(event.window.windowID)) break;
                                windows[event.window.windowID]->onHide();
                                break;

                            case SDL_WINDOWEVENT_MOVED:
                                break;

                            case SDL_WINDOWEVENT_FOCUS_GAINED:
                                if (!winExist(event.window.windowID)) break;
                                focused = windows[event.window.windowID].get();
                                break;

                            case SDL_WINDOWEVENT_FOCUS_LOST:
                                focused = nullptr;
                                break;
                        }

                        break;

                    case SDL_KEYDOWN:
                        if(focused != nullptr) focused->keyFunc(event.key.keysym.sym, ButtonAction::Press);

                        break;

                    case SDL_KEYUP:
                        if(focused != nullptr) focused->keyFunc(event.key.keysym.sym, ButtonAction::Release);
                        break;

                    case SDL_TEXTEDITING:
                        if (!winExist(event.edit.windowID)) break;
                        windows[event.text.windowID]->charInputFunc({ boost::locale::conv::utf_to_utf<wchar_t>(event.edit.text), true});
                        break;

                    case SDL_TEXTINPUT:
                        if (!winExist(event.text.windowID)) break;
                        windows[event.text.windowID]->charInputFunc({boost::locale::conv::utf_to_utf<wchar_t>(event.text.text), false});
                        break;

                    case SDL_MOUSEMOTION:
                        if (!winExist(event.motion.windowID)) break;
                        windows[event.motion.windowID]->mouseMove(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
                        break;

                    case SDL_MOUSEBUTTONDOWN:
                        if (!winExist(event.button.windowID)) break;
                        windows[event.button.windowID]->mouseButtonFunc(buttonTrans(event.button.button), ButtonAction::Press);
                        break;

                    case SDL_MOUSEBUTTONUP:
                        if (!winExist(event.button.windowID)) break;
                        windows[event.button.windowID]->mouseButtonFunc(buttonTrans(event.button.button), ButtonAction::Release);
                        break;

                    case SDL_MOUSEWHEEL:
                        if (!winExist(event.wheel.windowID)) break;
                        windows[event.button.windowID]->scrollFunc(event.wheel.x, event.wheel.y);
                        break;

                    case SDL_FINGERDOWN:
                        break;

                    case SDL_FINGERUP:
                        break;

                    case SDL_FINGERMOTION:
                        break;

                    case SDL_DOLLARGESTURE:
                        break;

                    case SDL_DOLLARRECORD:
                        break;

                    case SDL_MULTIGESTURE:
                        break;

                    case SDL_CLIPBOARDUPDATE:
                        break;

                    case SDL_DROPFILE:
                        if (!winExist(event.button.windowID)) break;
                        windows[event.button.windowID]->dropFunc(event.drop.file);
                        break;

                    case SDL_QUIT:
                        sigExit = true;
                        break;

                    default:
                        break;
                }
            }
        }

        void Application::addWindow(std::shared_ptr<Window> win)
        {
            if(windows.size() == 0)
                setMainWindow(win);  //This is the first window

            windows.insert({ (win->windowID()), win });
        }

        void Application::setMainWindow(std::shared_ptr<Window> win)
        {
            mainWin = win.get();
        }

        void Application::terminate()
        {
            sigExit = true;
        }

        void Application::onTerminate()
        {
        }

        void Application::beforeLaunch()
        {
        }

        void Application::afterLaunch()
        {
        }

        void Application::onEnteringBackground()
        {
        }

        void Application::onEnteringForeground()
        {
        }

        void Application::init()
        {
            glewInit();
            application = this;
            //Set Up Place Holders
            Theme::Theme _t = {nullptr};
            Theme::SystemTheme = _t;
            this->beforeLaunch();

            //If params are not set, set them to default values
            if (!Theme::SystemTheme.ActiveBorderBrush)
                Theme::SystemTheme.ActiveBorderBrush = UIMakeSolidColorBrush(Base::Color(0.7, 0.7, 1.0, 1.0));
            if (!Theme::SystemTheme.InactiveBorderBrush)
                Theme::SystemTheme.InactiveBorderBrush = UIMakeSolidColorBrush(Base::Color(0.7, 0.7, 0.7, 1.0));
            if (!Theme::SystemTheme.ControlBrush)
                Theme::SystemTheme.ControlBrush = UIMakeSolidColorBrush(Base::Color(0.0, 0.0, 0.0, 0.0));
            if (!Theme::SystemTheme.ControlDarkBrush)
                Theme::SystemTheme.ControlDarkBrush = UIMakeSolidColorBrush(Base::Color(0.7 , 0.7, 0.7, 1.0));
            if (!Theme::SystemTheme.ControlDarkDarkBrush)
                Theme::SystemTheme.ControlDarkDarkBrush = UIMakeSolidColorBrush(Base::Color(0.25, 0.25, 0.25, 1.0));
            if (!Theme::SystemTheme.ControlLightBrush)
                Theme::SystemTheme.ControlLightBrush = UIMakeSolidColorBrush(Base::Color(0.85, 0.85, 0.85, 1.0));
            if (!Theme::SystemTheme.ControlLightLightBrush)
                Theme::SystemTheme.ControlLightLightBrush = UIMakeSolidColorBrush(Base::Color(1.0, 1.0, 1.0, 1.0));
            if (!Theme::SystemTheme.ControlHeightLightBrush)
                Theme::SystemTheme.ControlHeightLightBrush = UIMakeSolidColorBrush(Base::Color(0.6, 0.6, 1.0, 1.0));
            if (!Theme::SystemTheme.ControlOnPressBrush)
                Theme::SystemTheme.ControlOnPressBrush = UIMakeSolidColorBrush(Base::Color(0.8, 0.8, 1.0, 1.0));
            if (!Theme::SystemTheme.MenuBrush)
                Theme::SystemTheme.MenuBrush = UIMakeSolidColorBrush(Base::Color(0.95, 0.95, 0.95, 0.0));
            if (!Theme::SystemTheme.MenuBarBrush)
                Theme::SystemTheme.MenuBarBrush = UIMakeSolidColorBrush(Base::Color(0.5, 0.5, 1.0, 1.0));
            if (!Theme::SystemTheme.MenuHighlightBrush)
                Theme::SystemTheme.MenuHighlightBrush = UIMakeSolidColorBrush(Base::Color(0.6, 0.6, 1.0, 1.0));
            if (!Theme::SystemTheme.ScrollBarBrush)
                Theme::SystemTheme.ScrollBarBrush = UIMakeSolidColorBrush(Base::Color(0.6, 0.6, 0.6, 1.0));
            if (!Theme::SystemTheme.WindowBrush)
                Theme::SystemTheme.WindowBrush = UIMakeSolidColorBrush(Base::Color(1.0, 1.0, 1.0, 1.0));
            if (!Theme::SystemTheme.DefaultFont)
                Theme::SystemTheme.DefaultFont = Font::service.getRenderer("SourceHanSansCN-Normal", 30, Base::Color(0.0, 0.0, 0.0, 1));

            UI::Base::init();

            try
            {
                if (SDL_Init(SDL_INIT_VIDEO) < 0) throw Exceptions::VideoInitFailure;
            }
            catch (Exceptions)
            {
                logfatal("SDL_Video_Init Failure.");
            }

            this->afterLaunch();
            sigExit = false;
        }

        bool Application::winExist(int id)
        {
            auto iter = windows.find(id);
            if (iter != windows.end())
                return true;
            else
                return false;
        }

        Control::Control(std::string _xName, Margin _Margin) :
            xName(_xName), cMargin(_Margin), pressed(false), focused(false), mouseOn(false), enabled(true)
        {
        }

        void Control::setSize(size_t x, size_t y)
        {
        }

        void Control::parentResize(Grid * parent)
        {
            int accrow = (parent->rowDefinitions.size() < gridPosY) ? parent->rowDefinitions.size() - 1 : gridPosY;
            int acccol = (parent->colDefinitions.size() < gridPosX) ? parent->colDefinitions.size() - 1 : gridPosX;
            cMargin.updateAbs(Base::Rect(parent->colDefinitions[acccol].accBegin, parent->colDefinitions[acccol].accEnd,
                                         parent->rowDefinitions[accrow].accBegin, parent->rowDefinitions[accrow].accEnd));
            this->onResize(cMargin.absrect.xmax - cMargin.absrect.xmin, cMargin.absrect.ymax - cMargin.absrect.ymin);
        }

        void Control::render()
        {
        }

        void Control::onResize(size_t x, size_t y)
        {
        }

        void Control::onParentResize(Grid * parent)
        {
        }

        Grid::Grid() :
            Control("Anonymous", Core::Margin(Base::Rect(0, 1, 0, 1), Base::Rect(0, 0, 0, 0),
                                              Core::HorizontalAlignment::Stretch, Core::VerticalAlignment::Stretch)),
            colDefinitions( { {0, 1 << 16, 0, 0} }), rowDefinitions({ { 0, 1 << 16, 0, 0 } })
        {
            focusFunc.connect([this](FocusOp Stat) {});
            mouseMove.connect([this](int x, int y, int dx, int dy)
            {
                for(auto c : mold)
                {
                    Base::Rect r = c->cMargin.absrect;

                    if(!(r.xmin < x && r.xmax > x && r.ymin < y && r.ymax > y))
                    {
                        c->cursorEnterFunc(CursorOp::Leave);
                        c->mouseOn = false;
                    }
                }

                mold.clear();

                for(auto c : childern)
                {
                    Base::Rect r = c.second->cMargin.absrect;

                    if(r.xmin < x && r.xmax > x && r.ymin < y && r.ymax > y)
                    {
                        c.second->cursorEnterFunc(CursorOp::Enter);
                        c.second->mouseOn = true;
                        mold.push_back(c.second);
                    }
                }
            });

            mouseButtonFunc.connect([this](MouseButton Button, ButtonAction Action)
            {
                switch(Action)
                {
                    case ButtonAction::Press:
                        if(mold.size() > 0)
                        {
                            mold.back()->mouseButtonFunc(Button, Action);

                            if(focused != mold.back() && focused)
                            {
                                focused->focused = false;
                                focused->focusFunc(FocusOp::Lose);
                                focused = nullptr;
                            }

                            focused = mold.back();
                            focused->pressed = true;
                            focused->focused = true;
                            focused->focusFunc(FocusOp::Gain);
                        }

                        break;

                    case ButtonAction::Release:
                        if(focused && focused->pressed)
                        {
                            focused->mouseButtonFunc(Button, Action);
                            focused->pressed = false;
                        }

                        if(mold.size() == 0 && focused)
                        {
                            focused->focused = false;
                            focused->focusFunc(FocusOp::Lose);
                            focused = nullptr;
                        }

                        break;

                    default:
                        break;
                }
            });

            cursorEnterFunc.connect([this](CursorOp Stat)
            {
                if(Stat == CursorOp::Leave)
                    for(auto c : mold)
                    {
                        c->cursorEnterFunc(CursorOp::Leave);
                        c->mouseOn = false;
                    }
            });

            scrollFunc.connect([this](double dx, double dy)
            {
                if(mold.size() > 0) mold.back()->scrollFunc(dx, dy);
            });

            keyFunc.connect([this](int Key, ButtonAction Action)
            {
                if(focused) focused->keyFunc(Key, Action);
            });

            charInputFunc.connect([this](CInpArgs e)
            {
                if(focused) focused->charInputFunc(e);
            });

            dropFunc.connect([this](const char* Paths)
            {
                if(mold.size() > 0) mold.back()->dropFunc(Paths);
            });

            touchFunc.connect([this](int x, int y, ButtonAction action)
            {
                told.clear();

                for(auto c : childern)
                {
                    Base::Rect r = c.second->cMargin.absrect;

                    if(r.xmin < x && r.xmax > x && r.ymin < y && r.ymax > y)
                    {
                        told.push_back(c.second);
                    }
                }

                switch(action)
                {
                    case ButtonAction::Press:
                        if(told.size() > 0)
                        {
                            told.back()->touchFunc(x, y, action);

                            if(focused)
                            {
                                focused->focused = false;
                                focused->focusFunc(FocusOp::Lose);
                            }

                            focused = told.back();
                            focused->focused = true;
                            focused->pressed = true;
                            focused->focusFunc(FocusOp::Gain);
                        }

                        break;

                    case ButtonAction::Release:
                        if(focused)
                        {
                            focused->touchFunc(x, y, action);
                            focused->pressed = false;
                        }

                        if (mold.size() == 0 && focused)
                        {
                            focused->focused = false;
                            focused->focusFunc(FocusOp::Lose);
                        }

                        break;

                    default:
                        break;
                }
            });

            touchmove.connect([this](int x, int y, int dx, int dy)
            {
                if(told.size() > 0) told.back()->touchmove(x, y, dx, dy);
            });
        }

        void Grid::onResize(size_t x, size_t y)
        {
        }

        void Grid::recalGrid()
        {
            recal(colDefinitions, true);
            recal(rowDefinitions, false);
        }

        void Grid::addChild(std::shared_ptr<Control> child)
        {
            childern.insert({ child->xName, child });
            child->parentResize(this);
        }

        void Grid::render()
        {
            for(auto c : childern) c.second->render();
        }

        void Grid::setSize(size_t x, size_t y)
        {
            Control::setSize(x, y);
            recalGrid();

            for(auto c : childern) c.second->parentResize(this);
        }

        void Grid::parentResize(Grid * parent)
        {
            Control::parentResize(parent);
            recalGrid();

            for(auto c : childern) c.second->parentResize(this);
        }

        void Grid::recal(std::vector<Definition>& definitions, bool col)
        {
            //check average space;
            std::unique_ptr<int[]> nums(new int[definitions.size()]);
            std::unique_ptr<bool[]> change(new bool[definitions.size()]);
            int freed = col ? (cMargin.absrect.xmax - cMargin.absrect.xmin) : (cMargin.absrect.ymax - cMargin.absrect.ymin);
            int avg = freed / definitions.size();

            for(size_t c = 0; c < definitions.size(); ++c)
            {
                freed -= nums[c] = avg;
                change[c] = false;
            }

            size_t unchanged = definitions.size();

            while(true)
            {
                bool changed = false;

                for(size_t c = 0; c < definitions.size(); ++c)
                {
                    int max = definitions[c].max;

                    if(max < nums[c])  //avg size exceeds size limit
                    {
                        freed += nums[c] - max;
                        nums[c] = max;
                        changed = change[c] = true;
                        unchanged--;
                    }

                    int min = definitions[c].min;

                    if(min > nums[c])  //avg size below min-size reqirement
                    {
                        freed -= min - nums[c];
                        nums[c] = min;
                        changed = change[c] = true;
                        unchanged--;
                    }
                }

                if((unchanged == 0) || (!changed)) break;  //no changes occoured , perfectly fit

                //try re-average
                avg = freed / unchanged;

                for(size_t c = 0; c < definitions.size(); ++c)
                    if(!change[c])
                    {
                        nums[c] += avg;
                        freed -= avg;
                    }
            }

            //apply changes
            int intend = (freed > 0) ? freed / 2 : 0;

            for(size_t c = 0; c < definitions.size(); ++c)
            {
                definitions[c].accBegin = intend;
                intend += nums[c];
                definitions[c].accEnd = intend;
            }
        }

        Margin::Margin(Base::Rect _relative_ps, Base::Rect _relative_pc, HorizontalAlignment _ha, VerticalAlignment _va):
            relativeps(_relative_ps), relativepc(_relative_pc), ha(_ha), va(_va)
        {
        }

        void Margin::updateAbs(Base::Rect Parent_Rect)
        {
            Math::Vec2 p1, p2;  //p1:left-up corner p2:right-bottom corner
            int wid = Parent_Rect.xmax - Parent_Rect.xmin;
            int hei = Parent_Rect.ymax - Parent_Rect.ymin;

            switch(ha)
            {
                case HorizontalAlignment::Left:
                    p1.x = Parent_Rect.xmin + relativepc.xmin + wid * relativeps.xmin;
                    p2.x = p1.x + relativepc.xmax;
                    break;

                case HorizontalAlignment::Right:
                    p2.x = Parent_Rect.xmax - relativepc.xmax - wid * relativeps.xmax;
                    p1.x = p2.x - relativepc.xmin;
                    break;

                case HorizontalAlignment::Center:
                    p1.x = Parent_Rect.xmin + wid / 2 - relativepc.xmin;
                    p2.x = Parent_Rect.xmin + wid / 2 + relativepc.xmax;
                    break;

                case HorizontalAlignment::Stretch:
                    p1.x = Parent_Rect.xmin + relativepc.xmin + wid * relativeps.xmin;
                    p2.x = Parent_Rect.xmin - relativepc.xmax + wid * relativeps.xmax;
                    break;

                default:
                    logwarning("Unknown Horizontal Alignment Setting. Aborting...");
                    return;
                    break;
            }

            switch(va)
            {
                case VerticalAlignment::Top:
                    p1.y = Parent_Rect.ymin + relativepc.ymin + hei * relativeps.ymin;
                    p2.y = p1.y + relativepc.ymax;
                    break;

                case VerticalAlignment::Bottom:
                    p2.y = Parent_Rect.ymax - relativepc.ymax - hei * relativeps.ymax;
                    p1.y = p2.y - relativepc.ymin;
                    break;

                case VerticalAlignment::Center:
                    p1.y = Parent_Rect.ymin + hei / 2 - relativepc.ymin;
                    p2.y = Parent_Rect.ymin + hei / 2 + relativepc.ymax;
                    break;

                case VerticalAlignment::Stretch:
                    p1.y = Parent_Rect.ymin + relativepc.ymin + hei * relativeps.ymin;
                    p2.y = Parent_Rect.ymin - relativepc.ymax + hei * relativeps.ymax;
                    break;

                default:
                    logwarning("Unknown Vertical Alignment Setting. Aborting...");
                    return;
                    break;
            }

            absrect = Base::Rect(p1, p2);

            //size restriction. wid and height cant be zero
            if(absrect.xmax < absrect.xmin) absrect.xmax = absrect.xmin;

            if(absrect.ymax < absrect.ymin) absrect.ymax = absrect.ymin;
        }
    }
    namespace Theme
    {
        Theme SystemTheme;
    }
}
