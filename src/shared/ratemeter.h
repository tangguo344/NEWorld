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

#ifndef RATEMETER_H_
#define RATEMETER_H_

#include <chrono>
#include <thread>
#include "common.h"

class RateMeter
{
public:
    explicit RateMeter(int limit = -1) : mLastRefreshTime(getTimeNow()), mLimit(limit)
    {
#ifdef NEWORLD_USE_WINAPI
        LARGE_INTEGER num;
        QueryPerformanceFrequency(&num);
        mFrequency = num.QuadPart;
#endif
    }

    void refresh()
    {
        if (!mValid)
        {
            mValid = true;
            return;
        }
        auto now = getTimeNow();
#ifndef NEWORLD_USE_WINAPI
        mDeltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - mLastRefreshTime).count();
#else
        mDeltaTime = now - mLastRefreshTime;
#endif
        mLastRefreshTime = now;
    }
    double getRate() const
    {
#ifndef NEWORLD_USE_WINAPI
        return 1000 / mDeltaTime;
#else
        return 1.0 / mDeltaTime * mFrequency;
#endif
    }
    long long getDeltaTimeMs() const
    {
#ifndef NEWORLD_USE_WINAPI
        return mDeltaTime;
#else
        return mDeltaTime / mFrequency;
#endif
    }
    // notice that this function will not call refresh()!
    void wait() const
    {
        if (mLimit == 0) return;
        auto now = getTimeNow();
#ifndef NEWORLD_USE_WINAPI
        auto stdDelta = std::chrono::milliseconds(1000 / mLimit);
        auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - mLastRefreshTime);
        if (stdDelta > deltaTime) std::this_thread::sleep_for(stdDelta - deltaTime);
#else
        auto stdDelta = 1000 / mLimit; //ms
        auto deltaTime = static_cast<double>(now - mLastRefreshTime) / mFrequency * 1000;
        if (stdDelta > deltaTime) Sleep(static_cast<unsigned long>(stdDelta - deltaTime));
#endif
    }

    // notice that this function will not call refresh()!
    bool shouldRun() const
    {
        if (mLimit == 0) return true;
        auto now = getTimeNow();
#ifndef NEWORLD_USE_WINAPI
        auto stdDelta = std::chrono::milliseconds(1000 / mLimit);
        auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - mLastRefreshTime);
#else
        auto stdDelta = 1000.0 / mLimit; //ms
        auto deltaTime = static_cast<double>(now - mLastRefreshTime) / mFrequency * 1000; //ms
#endif
        return stdDelta <= deltaTime;
    }

private:
#ifndef NEWORLD_USE_WINAPI
    std::chrono::steady_clock::time_point mLastRefreshTime;
    long long mDeltaTime = 0ll;
    static std::chrono::steady_clock::time_point getTimeNow()
    {
        return std::chrono::steady_clock::now();
    }
#else
    static __int64 getTimeNow()
    {
        LARGE_INTEGER num;
        QueryPerformanceCounter(&num);
        return num.QuadPart;
    }
    __int64 mLastRefreshTime;
    __int64 mFrequency;
    __int64 mDeltaTime = 0ll;
#endif
    int mLimit = 0ll;
    bool mValid = false;
};

#endif // !RATEMETER_H_
