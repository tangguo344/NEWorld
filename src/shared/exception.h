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

#ifndef EXCEPTION_H_
#define EXCEPTION_H_
#include <string>
#include <chrono>

#pragma warning(disable: 4673)

namespace StringUtils
{
    template <typename... Args>
    std::string FormatString(const char* lpStr, Args&&... args);
}

namespace Exception
{
    namespace detail_
    {
        struct ExceptionStorage
        {
            ExceptionStorage(std::exception_ptr nestedException, std::chrono::system_clock::time_point const& time, std::string file, unsigned line, std::string src, std::string desc) noexcept
                : m_NestedException(nestedException), m_Time(time), m_File(file), m_Line(line), m_Source(src), m_Description(desc)
            {
            }

            std::exception_ptr m_NestedException;
            std::chrono::system_clock::time_point m_Time;
            std::string m_File;
            unsigned m_Line;
            std::string m_Source;
            std::string m_Description;
        };
    }

    class Exception
        : protected detail_::ExceptionStorage, public virtual std::exception
    {
    public:
        template <typename... Args>
        Exception(std::exception_ptr nestedException, const char* Src, const char* File, unsigned Line, const char* Desc, Args&&... args) noexcept
            : detail_::ExceptionStorage{ nestedException, std::chrono::system_clock::now(), File, Line, Src, StringUtils::FormatString(Desc, std::forward<Args>(args)...) }
        {
        }

        template <typename... Args>
        Exception(const char* Src, const char* File, unsigned Line, const char* Desc, Args&&... args) noexcept
            : detail_::ExceptionStorage{ {}, std::chrono::system_clock::now(), File, Line, Src, StringUtils::FormatString(Desc, std::forward<Args>(args)...) }
        {
        }

        ~Exception();

        std::chrono::system_clock::time_point GetTime() const noexcept
        {
            return m_Time;
        }

        const char* GetFile() const noexcept
        {
            return m_File.c_str();
        }

        unsigned GetLine() const noexcept
        {
            return m_Line;
        }

        const char* GetSource() const noexcept
        {
            return m_Source.c_str();
        }

        const char* GetDesc() const noexcept
        {
            return m_Description.c_str();
        }

        std::exception_ptr GetNestedException() const noexcept
        {
            return m_NestedException;
        }

        const char* what() const noexcept override
        {
            return m_Description.c_str();
        }
    };

#define DeclareException(ExceptionClass, ExtendException, DefaultDescription) \
class ExceptionClass : public ExtendException\
{\
public:\
    typedef ExtendException BaseException;\
    ExceptionClass(const char* Src, const char* File, unsigned Line) noexcept\
        : BaseException(Src, File, Line, DefaultDescription)\
    {\
    }\
    ExceptionClass(std::exception_ptr nestedException, const char* Src, const char* File, unsigned Line) noexcept\
        : BaseException(nestedException, Src, File, Line, DefaultDescription)\
    {\
    }\
    template <typename... Args>\
    ExceptionClass(const char* Src, const char* File, unsigned Line, const char* Desc, Args&&... args) noexcept\
        : BaseException(Src, File, Line, Desc, std::forward<Args>(args)...)\
    {\
    }\
    template <typename... Args>\
    ExceptionClass(std::exception_ptr nestedException, const char* Src, const char* File, unsigned Line, const char* Desc, Args&&... args) noexcept\
        : BaseException(nestedException, Src, File, Line, Desc, std::forward<Args>(args)...)\
    {\
    }\
}
}

#define nw_throw(ExceptionClass, ...) do { throw ExceptionClass{ __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__ }; } while (false)
#define nw_throwwithnested(ExceptionClass, ...) do { throw ExceptionClass{ std::current_exception(), __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__ }; } while (false)

#include "stringutils.h"

#endif // !EXCEPTION_H_
