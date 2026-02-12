/*++

Copyright (c) SubSpace Library.

Module:

    TimeLib.h

Description:

    Time utilities for SubSpace applications.

--*/

#ifndef _TIMELIB_H_
#define _TIMELIB_H_

#include "base.h"

class Timer
{
public:
    Timer();
    
    void Start();
    void Stop();
    uint64 GetElapsedMilliseconds() const;
    
private:
    uint64 mStartTime;
    uint64 mStopTime;
    bool mRunning;
};

uint64 GetCurrentTimeMillis();
void Sleep(uint32 milliseconds);

#endif // _TIMELIB_H_
