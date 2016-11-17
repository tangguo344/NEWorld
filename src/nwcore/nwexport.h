#pragma once
#if defined _WIN32 || defined __CYGWIN__
#ifdef _MSC_VER
#define NWAPIENTRY __declspec(dllimport)
#define NWAPIEXPORT __declspec(dllexport)
#else
#define NWAPIENTRY __attribute__((dllimport))
#define NWAPIEXPORT __attribute__((dllexport))
#endif
#else
#define NWAPIENTRY __attribute__((visibility("default")))
#define NWAPIEXPORT __attribute__((visibility("default")))
#endif

#ifndef NWAPICALL
#ifdef _MSC_VER
#define NWAPICALL __cdecl
#else
#define NWAPICALL __attribute__((__cdecl__))
#endif
#endif

#ifdef NWCOREEXPORTS
#define NWCOREAPI NWAPIEXPORT
#else
#define NWCOREAPI NWAPIENTRY
#endif // NWCOREEXPORTS
