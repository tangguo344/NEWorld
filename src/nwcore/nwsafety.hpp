#pragma once
#include "nwexport.h"
#include "nwstring.hpp"
#include <map>
#include <string>
#include <vector>
#include <iostream>

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
		Line(Level l, std::string msg, const char* mfi, const char* mfc, int line);
		std::ostream& operator << (std::ostream& s);
	private:
		Level mLevel;
		int mLineNumber;
		const char *mFileName;
		const char *mFuncName;
		std::string mMessaage;
	};

	template <class ...Ts>
	void log(Level l, const char* mfc, const char* mfi, int line, const std::string& fmt, Ts&&... args)
	{
		logRaw(l, mfi, mfc, lineStringUtils::FormatString(fmt, std::forward<Ts>(args)...));
	}

	template <class ...Ts>
	void log(Level l, const char* mfc,  const char* mfi,int line, const char* fmt, Ts&&... args)
	{
		logRaw(l, mfi, mfc, lineStringUtils::FormatString(fmt, std::forward<Ts>(args)...));
	}

	static Safety& getInstance();
private:
	std::vector<Line> mBuffer;
	void logRaw(Level l, const char* mfi, const char* mfc, int line, std::string msg);
	Level mFilelevel, mDetailLevel, mCoutLevel, mCerrLevel, mThrowLevel, mKillLevel;
};

#define nw_trace(fmt, ...)\
 Safety::getInstance().log(Safety::Level::trace, __FUNCTION__, __FILE__, __LINE__, fmt, __VA_ARGS__)
#define nw_debug(fmt, ...)\
 Safety::getInstance().log(Safety::Level::debug, __FUNCTION__, __FILE__, __LINE__, fmt, __VA_ARGS__)
#define nw_info(fmt, ...)\
 Safety::getInstance().log(Safety::Level::info, __FUNCTION__, __FILE__, __LINE__, fmt, __VA_ARGS__)
#define nw_warn(fmt, ...)\
 Safety::getInstance().log(Safety::Level::warning, __FUNCTION__, __FILE__, __LINE__, fmt, __VA_ARGS__)
#define nw_error(fmt, ...)\
 Safety::getInstance().log(Safety::Level::error, __FUNCTION__, __FILE__, __LINE__, fmt, __VA_ARGS__)
#define nw_fatal(fmt, ...)\
 Safety::getInstance().log(Safety::Level::fatal, __FUNCTION__, __FILE__, __LINE__, fmt, __VA_ARGS__)
