#include "nwsafety.hpp"
#include "./../shared/consolecolor.h"
#include <cstdio>

Safety globalLopPool;
std::mutex globalLoggerMutex;

static std::array<std::string, 6> LevelTags;

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

Safety::Line::Line(const char* fileName, const char* funcName, int lineNumber, Level level)
	: mLevel(level), mLock(globalLoggerMutex)
{
	if (mLevel >= globalLopPool.mDetailLevel)
	{
		mFileName = fileName;
		mFuncName = funcName;
		mLineNumber = lineNumber;
	}
	mContent << LColor::white << getTimeString('-', ' ', ':') << LevelTags[static_cast<size_t>(level)];
}

void Safety::Line::write(std::ostream & ostream, bool noColor) const
{
}

Safety::Line::~Line()
{
}


Safety & Safety::getInstance()
{
	return globalLopPool;
}

