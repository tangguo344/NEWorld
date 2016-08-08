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

#ifndef LOGGER_H_
#define LOGGER_H_

#include <iostream>
#include <vector>
#include <sstream>
using std::string;
#include "common.h"

class Logger
{
public:
    enum class Level
    {
        trace,
        debug,
        info,
        warning,
        error,
        fatal
    };

    Logger(const char* fileName, const char* funcName, int lineNumber, Level level);
    ~Logger();

    static void addFileSink(const string& path, const string& prefix);
    static void init(const string& prefix);

    template <typename T>
    Logger& operator<<(const T& rhs)
    {
        m_content << rhs;
        return *this;
    }

    static Level coutLevel;
    static Level cerrLevel;
    static Level fileLevel;
    static Level lineLevel;
    static bool fileOnly;

private:
    std::stringstream m_content;

    Level m_level;
    const char *m_fileName;
    const char *m_funcName;
    int m_lineNumber;

    static std::vector<std::ofstream> fsink;

    void writeOstream(std::ostream& ostream, bool noColor = false) const;
};

#define loggerstream(level) Logger(__FILE__, __FUNCTION__, __LINE__, Logger::Level::level)
// information for tracing
#define tracestream loggerstream(trace)
// information for developers
#define debugstream loggerstream(debug)
// information for common users
#define infostream loggerstream(info)
// problems that may affect facility, performance or stability but don't lead the game to crash immediately
#define warningstream loggerstream(warning)
// the game crashes, but can be resumed by ways such as reloading the world which don't restart the program
#define errorstream loggerstream(error)
// unrecoverable error and program termination is required
#define fatalstream loggerstream(fatal)

#endif // !LOGGER_H_
