/*++

Copyright (c) SubSpace Library.

Module:

    TimeLib.cpp

--*/

#include "TimeLib.h"
#include <chrono>
#include <thread>

Timer::Timer() : mStartTime(0), mStopTime(0), mRunning(false)
{
}

void Timer::Start()
{
    mStartTime = GetCurrentTimeMillis();
    mRunning = true;
}

void Timer::Stop()
{
    if(mRunning)
    {
        mStopTime = GetCurrentTimeMillis();
        mRunning = false;
    }
}

uint64 Timer::GetElapsedMilliseconds() const
{
    if(mRunning)
        return GetCurrentTimeMillis() - mStartTime;
    return mStopTime - mStartTime;
}

uint64 GetCurrentTimeMillis()
{
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

void Sleep(uint32 milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}
