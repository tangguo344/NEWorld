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

#ifndef UICORE_H_
#define UICORE_H_

#include "uifont.h"
#include <map>
#include <stack>
#include <memory>
#include <vector>
#include <thread>
#include <functional>
#include <boost/signals2.hpp>

namespace UI
{
    namespace Core
    {
        enum class Exceptions : size_t
        {
            WindowCreationFailure,
            VideoInitFailure,
            AudioInitFailure
        };

        enum StatChange
        {
            Minimize, Maximize, Restore
        };

        enum FocusOp
        {
            Lose, Gain
        };

        enum ButtonAction
        {
            Release = 0, Press, Repeat
        };

        enum MouseButton
        {
            Left, Middle, Right, Preserved1, Preserved2
        };

        enum CursorOp
        {
            Leave, Enter
        };

        enum class VerticalAlignment : size_t
        {
            Top, Bottom, Center, Stretch
        };

        enum class HorizontalAlignment : size_t
        {
            Left, Right, Center, Stretch
        };

        //relativeps: percentage relativepc:position
        class UILIB_API Margin
        {
        public:
            Margin() = default;
            Margin(Base::Rect _relative_ps, Base::Rect _relative_pc, HorizontalAlignment _ha, VerticalAlignment _va);
            void updateAbs(Base::Rect Parent_Rect);
            Base::Rect absrect;
            //Quick Constructors
            static auto TopLeft(double TopPct, double LeftPct, double TopDis, double LeftDis, double Width, double Height)
            {
                return Margin(Base::Rect(LeftPct, 0.0, TopPct, 0.0), Base::Rect(LeftDis, Width, TopDis, Height), HorizontalAlignment::Left, VerticalAlignment::Top);
            }
            static auto TopRight(double TopPct, double RightPct, double TopDis, double RightDis, double Width, double Height)
            {
                return Margin(Base::Rect(0.0, RightPct, TopPct, 0.0), Base::Rect(Width, RightDis, TopDis, Height), HorizontalAlignment::Right, VerticalAlignment::Top);
            }
            static auto TopCenter(double TopPct, double TopDis, double Height, double CenterLeft, double CenterRight)
            {
                return Margin(Base::Rect(0.0, 0.0, TopPct, 0.0), Base::Rect(CenterLeft, CenterRight, TopDis, Height), HorizontalAlignment::Center, VerticalAlignment::Top);
            }
            static auto TopStretch(double TopPct, double LeftPct, double RightPct, double TopDis, double Height, double LeftDis, double RightDis)
            {
                return Margin(Base::Rect(LeftPct, RightPct, TopPct, 0.0), Base::Rect(LeftDis, RightDis, TopDis, Height), HorizontalAlignment::Stretch, VerticalAlignment::Top);
            }
            static auto BottomLeft(double LeftPct, double BottomPct, double LeftDis, double Width, double BottomDis, double Height)
            {
                return Margin(Base::Rect(LeftPct, 0.0, 0.0, BottomPct), Base::Rect(LeftDis, Width, Height, BottomDis), HorizontalAlignment::Left, VerticalAlignment::Bottom);
            }
            static auto BottomRight(double RightPct, double BottomPct, double RightDis, double Width, double BottomDis, double Height)
            {
                return Margin(Base::Rect(0.0, RightPct, 0.0, BottomPct), Base::Rect(Width, RightDis, Height, BottomDis), HorizontalAlignment::Right, VerticalAlignment::Bottom);
            }
            static auto BottomCenter(double BottomPct, double CenterLeft, double CenterRight, double BottomDis, double Height)
            {
                return Margin(Base::Rect(0.0, 0.0, 0.0, BottomPct), Base::Rect(CenterLeft, CenterRight, Height, BottomDis), HorizontalAlignment::Center, VerticalAlignment::Bottom);
            }
            static auto BottomStretch(double LeftPct, double RightPct, double BottomPct, double  LeftDis, double RightDis, double BottomDis, double Height)
            {
                return Margin(Base::Rect(LeftPct, RightPct, 0.0, BottomPct), Base::Rect(LeftDis, RightDis, Height, BottomDis), HorizontalAlignment::Stretch, VerticalAlignment::Bottom);
            }
            static auto CenterLeft(double LeftPct, double LeftDis, double Width, double CenterTop, double CenterBottom)
            {
                return Margin(Base::Rect(LeftPct, 0.0, 0.0, 0.0), Base::Rect(LeftDis, Width, CenterTop, CenterBottom), HorizontalAlignment::Left, VerticalAlignment::Center);
            }
            static auto CenterRight(double RightPct, double RightDis, double Width, double CenterTop, double  CenterBottom)
            {
                return Margin(Base::Rect(0.0, RightPct, 0.0, 0.0), Base::Rect(Width, RightDis, CenterTop, CenterBottom), HorizontalAlignment::Right, VerticalAlignment::Center);
            }
            static auto CenterCenter(double CenterLeft, double CenterRight, double CenterTop, double CenterBottom)
            {
                return Margin(Base::Rect(0.0, 0.0, 0.0, 0.0), Base::Rect(CenterLeft, CenterRight, CenterTop, CenterBottom), HorizontalAlignment::Center, VerticalAlignment::Center);
            }
            static auto CenterStretch(double LeftPct, double RightPct, double LeftDis, double RightDis, double CenterTop, double CenterBottom)
            {
                return Margin(Base::Rect(LeftPct, RightPct, 0.0, 0.0), Base::Rect(LeftDis, RightDis, CenterTop, CenterBottom), HorizontalAlignment::Stretch, VerticalAlignment::Center);
            }
            static auto StretchLeft(double LeftPct, double TopPct, double BottomPct, double LeftDis, double Width, double TopDis, double BottomDis)
            {
                return Margin(Base::Rect(LeftPct, 0.0, TopPct, BottomPct), Base::Rect(LeftDis, Width, TopDis, BottomDis), HorizontalAlignment::Left, VerticalAlignment::Stretch);
            }
            static auto StretchRight(double RightPct, double TopPct, double BottomPct, double RightDis, double Width, double TopDis, double BottomDis)
            {
                return Margin(Base::Rect(0.0, RightPct, TopPct, BottomPct), Base::Rect(Width, RightDis, TopDis, BottomDis), HorizontalAlignment::Right, VerticalAlignment::Stretch);
            }
            static auto StretchCenter(double TopPct, double BottomPct, double CenterLeft, double CenterRight, double TopDis, double BottomDis)
            {
                return Margin(Base::Rect(0.0, 0.0, TopPct, BottomPct), Base::Rect(CenterLeft, CenterRight, TopDis, BottomDis), HorizontalAlignment::Center, VerticalAlignment::Stretch);
            }
            static auto StretchStretch(double LeftPct, double RightPct, double TopPct, double BottomPct, double LeftDis, double RightDis, double TopDis, double BottomDis)
            {
                return Margin(Base::Rect(LeftPct, RightPct, TopPct, BottomPct), Base::Rect(LeftDis, RightDis, TopDis, BottomDis), HorizontalAlignment::Stretch, VerticalAlignment::Stretch);
            }
        private:
            HorizontalAlignment ha;
            VerticalAlignment va;
            Base::Rect relativeps, relativepc;
        };

        struct CInpArgs
        {
            std::wstring str;
            bool editing;
        };

        class Grid;

        class UILIB_API Control
        {
        public:
            std::string xName;  //ID
            Margin cMargin;     //Position Data
            size_t gridPosX, gridPosY;

            Control() = default; //Creator
            Control(std::string _xName, Margin _Margin);
            virtual ~Control() = default;

            virtual void setSize(size_t x, size_t y);
            virtual void parentResize(Grid* parent);
            //Event Handlers
            virtual void render();
            virtual void onResize(size_t x, size_t y);
            virtual void onParentResize(Grid* parent);
            boost::signals2::signal<void(FocusOp)> focusFunc;
            boost::signals2::signal<void(int x, int y, int dx, int dy)>mouseMove;
            boost::signals2::signal<void(MouseButton, ButtonAction)> mouseButtonFunc;
            boost::signals2::signal<void(CursorOp)> cursorEnterFunc;
            boost::signals2::signal<void(double, double)> scrollFunc;
            boost::signals2::signal<void(int, ButtonAction)> keyFunc;
            boost::signals2::signal<void(CInpArgs e)> charInputFunc;
            boost::signals2::signal<void(const char*)> dropFunc;
            boost::signals2::signal<void(int x, int y, ButtonAction action)> touchFunc;
            boost::signals2::signal<void(int x, int y, int dx, int dy)> touchmove;

            bool enabled, mouseOn, pressed, focused;
        };

        //CallBack Functions
        using NotifyFunc = std::function<void()>;

        struct Definition
        {
            int min, max, accBegin, accEnd;
        };

        class UILIB_API Grid : public Control
        {
        public:
            std::vector<Definition> colDefinitions;
            std::vector<Definition> rowDefinitions;
            std::map<std::string, std::shared_ptr<Control>> childern;
            Grid();
            void addChild(std::shared_ptr<Control> child);
            void render();
            void setSize(size_t x, size_t y);
            void parentResize(Grid* parent);
            void onResize(size_t x, size_t y);
        private:
            void recalGrid();
            void recal(std::vector<Definition>& definitions, bool col);
            std::vector<std::shared_ptr<Control>> mold, told;
            std::shared_ptr<Control> focused;
            std::shared_ptr<Control> pressed;
        };

        struct _Page
        {
            std::shared_ptr<Grid> c;
            bool et, rt;
        };

        struct _Connections
        {
            boost::signals2::connection _c[10];
        };

        class UILIB_API Window
        {
        public:
            Window(const std::string& name, int width, int height, int _xpos, int _ypos);
            virtual ~Window();

            void close();
            void dorender();
            void resize(size_t x, size_t y);
            virtual void onShow();
            virtual void onHide();
            virtual void onClose();
            virtual void onStatChange(StatChange stat);
            virtual void onResize(size_t x, size_t y);
            //Event Handler
            boost::signals2::signal<void(FocusOp Stat)> focusFunc;
            boost::signals2::signal<void(int x, int y, int dx, int dy)> mouseMove;
            boost::signals2::signal<void(MouseButton Button, ButtonAction Action)> mouseButtonFunc;
            boost::signals2::signal<void(CursorOp Stat)> cursorEnterFunc;
            boost::signals2::signal<void(double dx, double dy)> scrollFunc;
            boost::signals2::signal<void(int Key, ButtonAction Action)> keyFunc;
            boost::signals2::signal<void(CInpArgs e)> charInputFunc;
            boost::signals2::signal<void(const char* Paths)> dropFunc;
            boost::signals2::signal<void(int x, int y, ButtonAction action)> touchFunc;
            boost::signals2::signal<void(int x, int y, int dx, int dy)> touchmove;

            void pushPage(std::shared_ptr<Grid> page, bool event_trans, bool render_trans);
            void popPage();
            void clearPages();
            void pageZero();

            std::vector<std::function<void()>> renderdelegate;
            std::shared_ptr<Base::Brush> background;
            //SDL ONLY
            Uint32 windowID();
            SDL_Window* window;
            bool terminated;
        private:
            _Connections doconnect(std::shared_ptr<Grid>);
            std::deque<_Page> pages;
            std::deque<_Connections> connections;
            std::deque<std::shared_ptr<Grid>> render;
            size_t _x, _y;
            bool resized;
        };

        class UILIB_API Application
        {
        public:
            virtual ~Application() = default;

            void run();
            void processMessages();
            void addWindow(std::shared_ptr<Window> win);
            void setMainWindow(std::shared_ptr<Window> win);
            void terminate();
            virtual void onTerminate();
            virtual void beforeLaunch();
            virtual void afterLaunch();
            virtual void onEnteringBackground();
            virtual void onEnteringForeground();
        private:
            Window* mainWin, *focused;
            std::map<Uint32, std::shared_ptr<Window>> windows;
            void init();
            bool winExist(int id);
            bool sigExit;
        };
        UILIB_API extern Application* application;
    }

    namespace Theme
    {
        using namespace Base;
        struct Theme
        {
            std::shared_ptr<Brush> ActiveBorderBrush;
            std::shared_ptr<Brush> InactiveBorderBrush;

            std::shared_ptr<Brush> ControlBrush;
            std::shared_ptr<Brush> ControlDarkBrush;
            std::shared_ptr<Brush> ControlDarkDarkBrush;
            std::shared_ptr<Brush> ControlLightBrush;
            std::shared_ptr<Brush> ControlLightLightBrush;

            std::shared_ptr<Brush> ControlHeightLightBrush;
            std::shared_ptr<Brush> ControlOnPressBrush;

            std::shared_ptr<Brush> MenuBrush;
            std::shared_ptr<Brush> MenuBarBrush;
            std::shared_ptr<Brush> MenuHighlightBrush;

            std::shared_ptr<Brush> ScrollBarBrush;
            std::shared_ptr<Brush> WindowBrush;

            std::shared_ptr<Font::FontRenderer> DefaultFont;
        };
        UILIB_API extern Theme SystemTheme;
    }
}

#endif
