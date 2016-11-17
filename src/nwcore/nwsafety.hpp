#pragma once
#include "nwexport.h"
#include <mutex>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

class NWCOREAPI Safety final
{
public:
	enum class Level : size_t
	{
		trace, debug, info, warning, error, fatal
	};

	class Line
	{
	public:
		Line(const char* fileName, const char* funcName, int lineNumber, Level level);
		~Line();

		template <typename T>
		Logger& operator<<(const T& rhs)
		{
			mContent << rhs;
			return *this;
		}
	private:
		Safety& mManager;
		std::stringstream mContent;
		Level mLevel;
		const char *mFileName;
		const char *mFuncName;
		int mLineNumber;
		std::lock_guard<std::mutex> mLock;
		void write(std::ostream& ostream, bool noColor = false) const;
	};
private:
	Level mFilelevel, mDetailLevel, mCoutLevel, mCerrLevel, mThrowLevel, mKillLevel;
};

#define loggerstream(level) Safety::Line(__FILE__, __FUNCTION__, __LINE__, Logger::Level::level)
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
