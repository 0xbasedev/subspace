/*++

Copyright (c) SubSpace Library.

Module:

    LogLib.cpp

--*/

#include "LogLib.h"
#include <iostream>
#include <cstdio>

void StdoutLogSink::Write(LogLevel level, const std::string & message)
{
    const char * levelStr;
    switch(level)
    {
        case KLogDebug:   levelStr = "DEBUG"; break;
        case KLogInfo:    levelStr = "INFO"; break;
        case KLogWarning: levelStr = "WARNING"; break;
        case KLogError:   levelStr = "ERROR"; break;
        default:          levelStr = "UNKNOWN"; break;
    }
    
    std::cout << "[" << levelStr << "] " << message << std::endl;
}

Logger::Logger() : mSink(nullptr)
{
}

Logger & Logger::Instance()
{
    static Logger instance;
    return instance;
}

void Logger::AttachSink(LogSink & sink)
{
    mSink = &sink;
}

void Logger::Log(LogLevel level, const char * format, ...)
{
    char buffer[4096];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    if(mSink)
        mSink->Write(level, std::string(buffer));
}
