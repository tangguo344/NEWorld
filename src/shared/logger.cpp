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
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

static std::array<std::string,6> LevelTags;

Logger::Level Logger::coutLevel = Level::trace;
Logger::Level Logger::cerrLevel = Level::fatal;
Logger::Level Logger::fileLevel = Level::trace;
Logger::Level Logger::lineLevel = Level::error;
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
    using namespace boost::filesystem;
    const char* path = "./logs/";
    if (!exists(path))
    {
        create_directories(path);
    }

    addFileSink(path, prefix);

    std::stringstream ss;

    int c = 0;

    ss.str("");
    ss << LColor::white << '[' << "trace" << ']';
    LevelTags[c++] = ss.str();

    ss.str("");
    ss << LColor::white << '[' << "debug" << ']';
    LevelTags[c++] = ss.str();

    ss.str("");
    ss << LColor::lwhite << '[' << "info" << ']';
    LevelTags[c++] = ss.str();

    ss.str("");
    ss << LColor::lyellow << '[' << "warning" << ']';
    LevelTags[c++] = ss.str();

    ss.str("");
    ss << LColor::lred << '[' << "error" << ']';
    LevelTags[c++] = ss.str();

    ss.str("");
    ss << LColor::red << '[' << "fatal" << ']';
    LevelTags[c] = ss.str();

}

Logger::Logger(const char* fileName, const char *funcName, int lineNumber, Level level)
    : m_level(level)
{
    if (m_level >= lineLevel)
    {
        m_fileName = fileName;
        m_funcName = funcName;
        m_lineNumber = lineNumber;
    }
    m_content << LColor::white << '[' << getTimeString('-', ' ', ':') << ']' << LevelTags[static_cast<size_t>(level)];
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
    std::string str = m_content.str();
    std::string::size_type pos1 = 0, pos2 = str.find(stylechar);
    while (true)
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
                if (cf) ostream << cf;
                else
                {
                    if (ch == stylechar) ostream << stylechar; // Escaped to `stylechar`
                    else ostream << stylechar << ch; // Wrong color code
                }
            }
        }
        pos1 = pos2 + 2;
        pos2 = str.find(stylechar, pos1);
    }
}

Logger::~Logger()
{
    if (m_level >= lineLevel)
    {
        m_content << std::endl
                  << "\tSource :\t" << m_fileName << std::endl
                  << "\tAt Line :\t" << m_lineNumber << std::endl
                  << "\tFunction :\t" << m_funcName << std::endl
                  ;
    }
    m_content << std::endl;
    if (!fileOnly)
    {
        if (m_level >= cerrLevel)
            writeOstream(std::cerr);
        else if (m_level >= coutLevel)
            writeOstream(std::cout);
    }
    if (m_level >= fileLevel)
    {
        for (auto& it : fsink)
        {
            writeOstream(it, true);
            if (m_level >= cerrLevel) it.flush();
        }
    }
}
