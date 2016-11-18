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

#include "logger.h"
#include "consolecolor.h"
#include <fstream>
#include <ctime>
#include <map>
#include <array>
#include <filesys.h>

static std::array<std::string, 6> LevelTags;

Logger::Level Logger::coutLevel = Level::trace;
Logger::Level Logger::cerrLevel = Level::fatal;
Logger::Level Logger::fileLevel = Level::trace;
Logger::Level Logger::lineLevel = Level::error;
std::mutex Logger::mutex;
bool Logger::fileOnly = false;

std::vector<std::ofstream> Logger::fsink;

template <size_t length>
static std::string convert(int arg)
{
    char arr[13];
    int siz = 0u;
    while (arg)
    {
        arr[siz++] = (arg % 10) + '0';
        arg /= 10;
    }
    std::string ret(length - siz, '0');
    ret.reserve(length);
    for (int i = siz - 1; i >= 0; i--)
        ret += arr[i];
    return ret;
}

static std::string getTimeString(char dateSplit, char midSplit, char timeSplit)
{
    time_t timer = time(nullptr);
    tm* currtime = localtime(&timer); // DO NOT `delete` THIS POINTER!
    return convert<4u>(currtime->tm_year + 1900) + dateSplit + convert<2u>(currtime->tm_mon) + dateSplit + convert<2u>(currtime->tm_mday)
           + midSplit + convert<2u>(currtime->tm_hour) + timeSplit + convert<2u>(currtime->tm_min) + timeSplit + convert<2u>(currtime->tm_sec);
}

void Logger::addFileSink(const std::string& path, const std::string& prefix)
{
    fsink.emplace_back(path + prefix + "_" + getTimeString('-', '_', '-') + ".log");
}

void Logger::init(const std::string& prefix)
{
    using namespace FileSystem;
    const char* path = "./logs/";
    if (!exists(path))
        createDirectory(path);

    addFileSink(path, prefix);

    std::stringstream ss;

    int c = 0;
    std::string str_prefix = "[" + prefix + "]";

    ss.str("");
    ss << str_prefix << LColor::white << '[' << "trace" << ']';
    LevelTags[c++] = ss.str();

    ss.str("");
    ss << str_prefix << LColor::white << '[' << "debug" << ']';
    LevelTags[c++] = ss.str();

    ss.str("");
    ss << str_prefix << LColor::lwhite << '[' << "info" << ']';
    LevelTags[c++] = ss.str();

    ss.str("");
    ss << str_prefix << LColor::lyellow << '[' << "warning" << ']';
    LevelTags[c++] = ss.str();

    ss.str("");
    ss << str_prefix << LColor::lred << '[' << "error" << ']';
    LevelTags[c++] = ss.str();

    ss.str("");
    ss << str_prefix << LColor::red << '[' << "fatal" << ']';
    LevelTags[c] = ss.str();
}

Logger::Logger(const char* fileName, const char *funcName, int lineNumber, Level level)
    : mLevel(level), mLock(mutex)
{
    if (mLevel >= lineLevel)
    {
        mFileName = fileName;
        mFuncName = funcName;
        mLineNumber = lineNumber;
    }
    mContent << LColor::white << getTimeString('-', ' ', ':') << LevelTags[static_cast<size_t>(level)];
}

void Logger::writeOstream(std::ostream& ostream, bool noColor) const
{
    using namespace LColorFunc;
    constexpr static char stylechar = '&';
    static std::map<char, colorfunc> cmap =
    {
        {'0',black},{'1',red},{'2',yellow},{'3',green},
        {'4',cyan},{'5',blue},{'6',magenta},{'7',white},
        {'8',lblack},{'9',lred},{'a',lyellow},{'b',lgreen},
        {'c',lcyan},{'d',lblue},{'e',lmagenta},{'f',lwhite},
    };
    std::string str = mContent.str();
    std::string::size_type pos1 = 0, pos2 = str.find(stylechar);
    for (;;)
    {
        if (std::string::npos == pos2)
        {
            ostream << str.substr(pos1, str.size());
            return;
        }
        ostream << str.substr(pos1, pos2 - pos1);
        if (pos2 < str.size())
        {
            char ch = str[pos2 + 1];
            if (!noColor)
            {
                colorfunc cf = cmap[(ch >= 'A' && ch <= 'F') ? ch - 'A' + 'a' : ch];
                if (cf)
                    ostream << cf;
                else
                {
                    if (ch == stylechar)
                    {
                        ostream << stylechar;    // Escaped to `stylechar`
                    }
                    else
                    {
                        ostream << stylechar << ch;    // Wrong color code
                    }
                }
            }
        }
        pos1 = pos2 + 2;
        pos2 = str.find(stylechar, pos1);
    }
}

Logger::~Logger()
{
    if (mLevel >= lineLevel)
    {
        mContent << std::endl
                 << "\tSource :\t" << mFileName << std::endl
                 << "\tAt Line :\t" << mLineNumber << std::endl
                 << "\tFunction :\t" << mFuncName << std::endl
                 ;
    }
    mContent << std::endl;
    if (!fileOnly)
    {
        if (mLevel >= cerrLevel)
            writeOstream(std::cerr);
        else if (mLevel >= coutLevel)
            writeOstream(std::cout);
    }
    if (mLevel >= fileLevel)
    {
        for (auto& it : fsink)
        {
            writeOstream(it, true);
            if (mLevel >= cerrLevel)
                it.flush();
        }
    }
}
