/*++

Copyright (c) SubSpace Library.

Module:

    ThreadLib.cpp

--*/

#include "ThreadLib.h"
#include <thread>
#include <mutex>

// Mutex implementation
Mutex::Mutex()
{
    mHandle = new std::mutex();
}

Mutex::~Mutex()
{
    delete static_cast<std::mutex*>(mHandle);
}

void Mutex::Lock()
{
    static_cast<std::mutex*>(mHandle)->lock();
}

void Mutex::Unlock()
{
    static_cast<std::mutex*>(mHandle)->unlock();
}

// Thread implementation
Thread::Thread() : mHandle(nullptr), mRunning(false)
{
}

Thread::~Thread()
{
    if(mHandle)
    {
        delete static_cast<std::thread*>(mHandle);
    }
}

void Thread::Start()
{
    if(!mRunning)
    {
        // Join any existing thread before starting a new one
        if(mHandle)
        {
            static_cast<std::thread*>(mHandle)->join();
            delete static_cast<std::thread*>(mHandle);
            mHandle = nullptr;
        }
        
        mRunning = true;
        mHandle = new std::thread([this]() { this->Run(); });
    }
}

void Thread::Join()
{
    if(mHandle && mRunning)
    {
        static_cast<std::thread*>(mHandle)->join();
        mRunning = false;
    }
}
