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

#ifndef NWSAFETY_HPP_
#define NWSAFETY_HPP_
#include "nwexport.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <array>
#include <mutex>
#include "common.h"

class LoggerManager;

class NWCOREAPI Logger
{
public:
    enum class Level
    {
        verbose, debug, info, warning, error, fatal
    };

    Logger(const char* fileName, const char* funcName, int lineNumber, Level level, LoggerManager* mgr);
    ~Logger();

    template <typename T>
    Logger& operator<<(const T& rhs)
    {
        mContent << rhs;
        return *this;
    }

    static void addFileSink(const std::string& path, const std::string& prefix);
private:
    Level mLevel;
    int mLineNumber;
    const char *mFileName;
    const char *mFuncName;
    LoggerManager* mManager;
    std::stringstream mContent;
    std::lock_guard<std::mutex> mLock;

    static std::mutex mutex;
    static std::vector<std::ofstream> fsink;

    void writeOstream(std::ostream& ostream, bool noColor = false) const;
};

class NWCOREAPI LoggerManager
{
public:
    LoggerManager() = default;
    LoggerManager(const std::string& prefix);
    Logger::Level coutLevel = Logger::Level::verbose;
    Logger::Level cerrLevel = Logger::Level::fatal;
    Logger::Level fileLevel = Logger::Level::info;
    Logger::Level lineLevel = Logger::Level::error;
    bool fileOnly{ false };
    std::array<std::string, 6> LevelTags;
};

#ifndef NWNOLOGGER

#define NWDECLEARLOGGER(prefix) \
    namespace NWCOREINTERNAL {namespace _LOGGER { LoggerManager gManager(prefix); }}

namespace NWCOREINTERNAL {namespace _LOGGER { extern LoggerManager gManager; }}

#define loggerstream(level) Logger(__FILE__, __FUNCTION__, __LINE__, Logger::Level::level, &NWCOREINTERNAL::_LOGGER::gManager)
// Information for tracing
#define verbosestream loggerstream(verbose)
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

#endif

#endif // !LOGGER_H_
