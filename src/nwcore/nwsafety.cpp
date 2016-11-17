#include "nwsafety.hpp"
#include <cstdio>

Safety globalLopPool;

Safety::Line::Line(Level l, std::string msg, const char * mfi, const char * mfc, int line)
{
}

std::ostream & Safety::Line::operator<<(std::ostream& s)
{
	return s;
}

void Safety::logRaw(Level l, const char * mfi, const char * mfc, int line, std::string msg)
{
	Line(l, msg, mfi, mfc, line);
}

Safety & Safety::getInstance()
{
	return globalLopPool;
}
