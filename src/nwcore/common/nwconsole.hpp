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

#ifndef NWCONSOLE_HPP_
#define NWCONSOLE_HPP_

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <future>
#include <iostream>
#include <functional>
#include <unordered_map>
#include <common/nwsafety.hpp>
#include "utils.h"
#include "common.h"

namespace LColorFunc
{
    typedef std::ostream&(*colorfunc)(std::ostream& s);

#ifdef NEWORLD_TARGET_WINDOWS
    // Microsoft Windows
    static HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    inline std::ostream& black(std::ostream& s)
    {
        SetConsoleTextAttribute(hStdout, 0);
        return s;
    }

    inline std::ostream& lblack(std::ostream& s)
    {
        SetConsoleTextAttribute(hStdout,FOREGROUND_INTENSITY);
        return s;
    }

    inline std::ostream& red(std::ostream& s)
    {
        SetConsoleTextAttribute(hStdout,FOREGROUND_RED);
        return s;
    }

    inline std::ostream& lred(std::ostream& s)
    {
        SetConsoleTextAttribute(hStdout,FOREGROUND_RED | FOREGROUND_INTENSITY);
        return s;
    }

    inline std::ostream& green(std::ostream& s)
    {
        SetConsoleTextAttribute(hStdout,FOREGROUND_GREEN);
        return s;
    }

    inline std::ostream& lgreen(std::ostream& s)
    {
        SetConsoleTextAttribute(hStdout,FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        return s;
    }

    inline std::ostream& blue(std::ostream& s)
    {
        SetConsoleTextAttribute(hStdout,FOREGROUND_BLUE);
        return s;
    }

    inline std::ostream& lblue(std::ostream& s)
    {
        SetConsoleTextAttribute(hStdout,FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        return s;
    }

    inline std::ostream& yellow(std::ostream& s)
    {
        SetConsoleTextAttribute(hStdout,FOREGROUND_RED | FOREGROUND_GREEN);
        return s;
    }

    inline std::ostream& lyellow(std::ostream& s)
    {
        SetConsoleTextAttribute(hStdout,FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        return s;
    }

    inline std::ostream& magenta(std::ostream& s)
    {
        SetConsoleTextAttribute(hStdout,FOREGROUND_RED | FOREGROUND_BLUE);
        return s;
    }

    inline std::ostream& lmagenta(std::ostream& s)
    {
        SetConsoleTextAttribute(hStdout,FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        return s;
    }

    inline std::ostream& cyan(std::ostream& s)
    {
        SetConsoleTextAttribute(hStdout,FOREGROUND_GREEN | FOREGROUND_BLUE);
        return s;
    }

    inline std::ostream& lcyan(std::ostream& s)
    {
        SetConsoleTextAttribute(hStdout,FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        return s;
    }

    inline std::ostream& white(std::ostream& s)
    {
        SetConsoleTextAttribute(hStdout,FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return s;
    }

    inline std::ostream& lwhite(std::ostream& s)
    {
        SetConsoleTextAttribute(hStdout,FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        return s;
    }
#else
    // *nix
    inline std::ostream& black(std::ostream &s)
    { return s << "\033[21;30m"; }

    inline std::ostream& lblack(std::ostream &s)
    { return s << "\033[1;30m"; }

    inline std::ostream& red(std::ostream &s)
    { return s << "\033[21;31m"; }

    inline std::ostream& lred(std::ostream &s)
    { return s << "\033[1;31m"; }

    inline std::ostream& green(std::ostream &s)
    { return s << "\033[21;32m"; }

    inline std::ostream& lgreen(std::ostream &s)
    { return s << "\033[1;32m"; }

    inline std::ostream& blue(std::ostream &s)
    { return s << "\033[21;34m"; }

    inline std::ostream& lblue(std::ostream &s)
    { return s << "\033[1;34m"; }

    inline std::ostream& yellow(std::ostream &s)
    { return s << "\033[21;33m"; }

    inline std::ostream& lyellow(std::ostream &s)
    { return s << "\033[1;33m"; }

    inline std::ostream& magenta(std::ostream &s)
    { return s << "\033[21;35m"; }

    inline std::ostream& lmagenta(std::ostream &s)
    { return s << "\033[1;35m"; }

    inline std::ostream& cyan(std::ostream &s)
    { return s << "\033[21;36m"; }

    inline std::ostream& lcyan(std::ostream &s)
    { return s << "\033[1;36m"; }

    inline std::ostream& white(std::ostream &s)
    { return s << "\033[21;37m"; }

    inline std::ostream& lwhite(std::ostream &s)
    { return s << "\033[1;37m"; }
#endif
}

namespace LColor
{
#define cccp constexpr const char*
    cccp black = "&0";
    cccp red = "&1";
    cccp yellow = "&2";
    cccp green = "&3";
    cccp cyan = "&4";
    cccp blue = "&5";
    cccp magenta = "&6";
    cccp white = "&7";
    cccp lblack = "&8";
    cccp lred = "&9";
    cccp lyellow = "&a";
    cccp lgreen = "&b";
    cccp lcyan = "&c";
    cccp lblue = "&d";
    cccp lmagenta = "&e";
    cccp lwhite = "&f";
#undef cccp
}



class CommandExecuteStat
{
public:
	CommandExecuteStat(bool s, std::string i) : success(s), info(i)
	{
	}

	bool success;
	std::string info;
};

class Command
{
public:
	explicit Command(std::string rawString)
	{
		args = split(rawString, ' ');
		name = args.size() != 0 ? args[0] : "";
		if (args.size() != 0) args.erase(args.begin());
	}

	std::string name;
	std::vector<std::string> args;
};

class CommandInfo
{
public:
	CommandInfo(std::string a, std::string h) : author(a), help(h)
	{
	}

	std::string author;
	std::string help;
};

using CommandHandleFunction = std::function<CommandExecuteStat(Command)>;
using CommandMap = std::unordered_map<std::string, std::pair<CommandInfo, CommandHandleFunction>>;


class CommandManager
{
public:
	CommandManager() :
		mMainloop([this] { inputLoop(); })
	{

	}

	~CommandManager()
	{
		mThreadRunning.store(false, std::memory_order_release);
		if (!mWaitingForInput.load(std::memory_order_acquire))
		{
			mMainloop.join();
			debugstream << "Input thread exited.";
		}
		else
		{
			mMainloop.detach();
			debugstream << "Input thread detached.";
		}
	}

	CommandManager(const CommandManager&) = delete;
	CommandManager& operator=(const CommandManager&) = delete;

	CommandMap& getCommandMap() { return mCommandMap; }

	void inputLoop()
	{
		while (mThreadRunning.load(std::memory_order_acquire))
		{
			std::string input;
			//std::cout << LColorFunc::white << "$> " << LColorFunc::lwhite;
			mWaitingForInput.store(true, std::memory_order_release);
			getline(std::cin, input);
			mWaitingForInput.store(false, std::memory_order_release);
			auto result = handleCommand(Command(input));
			if (result.info != "")
				infostream << result.info;
		}
	}

	void setRunningStatus(bool s)
	{
		mThreadRunning = s;
	}

	void registerCommand(std::string name, CommandInfo info, CommandHandleFunction func)
	{
		mCommandMap.insert({ name,{ info, func } });
	}

private:
	CommandExecuteStat handleCommand(Command cmd)
	{
		strtolower(cmd.name);
		auto result = mCommandMap.find(cmd.name);
		if (result != mCommandMap.end())
			return (*result).second.second(cmd);
		return{ false,"Command not exists, type help for available commands." };
	}

	std::thread mMainloop;
	std::atomic_bool mThreadRunning{ true };
	std::atomic_bool mWaitingForInput{ false };
	CommandMap mCommandMap;
};

#endif
