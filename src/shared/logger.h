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

    static void addFileSink(const std::string& path, const std::string& prefix);
    static void init(const std::string& prefix);

    template <typename T>
    Logger& operator<<(const T& rhs)
    {
        mContent << rhs;
        return *this;
    }

    static Level coutLevel;
    static Level cerrLevel;
    static Level fileLevel;
    static Level lineLevel;
    static bool fileOnly;

private:
    std::stringstream mContent;

    Level mLevel;
    const char *mFileName;
    const char *mFuncName;
    int mLineNumber;

    static std::vector<std::ofstream> fsink;

    void writeOstream(std::ostream& ostream, bool noColor = false) const;
};

#define loggerstream(level) Logger(__FILE__, __FUNCTION__, __LINE__, Logger::Level::level)
// Information for tracing
#define tracestream loggerstream(trace)
// Information for developers
#define debugstream loggerstream(debug)
// Information for common users
#define infostream loggerstream(info)
// Problems that may affect facility, performance or stability but may not lead the game to crash immediately
#define warningstream loggerstream(warning)
// The game crashes, but may be resumed by ways such as reloading the world which don't restart the program
#define errorstream loggerstream(error)
// Unrecoverable error and program termination is required
#define fatalstream loggerstream(fatal)

#endif // !LOGGER_H_
