/*++

Copyright (c) SubSpace Library.

Module:

    ThreadLib.h

Description:

    Threading library for SubSpace applications.

--*/

#ifndef _THREADLIB_H_
#define _THREADLIB_H_

#include "base.h"

class Mutex
{
public:
    Mutex();
    ~Mutex();
    
    void Lock();
    void Unlock();
    
private:
    void * mHandle;
};

class Thread
{
public:
    Thread();
    virtual ~Thread();
    
    void Start();
    void Join();
    
    virtual void Run() = 0;
    
private:
    void * mHandle;
    bool mRunning;
};

#endif // _THREADLIB_H_
