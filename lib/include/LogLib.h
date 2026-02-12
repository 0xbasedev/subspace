/*++

Copyright (c) SubSpace Library.

Module:

    LogLib.h

Description:

    Logging library for SubSpace applications.

--*/

#ifndef _LOGLIB_H_
#define _LOGLIB_H_

#include "base.h"
#include <string>
#include <cstdarg>

enum LogLevel
{
    KLogDebug,
    KLogInfo,
    KLogWarning,
    KLogError
};

class LogSink
{
public:
    virtual ~LogSink() {}
    virtual void Write(LogLevel level, const std::string & message) = 0;
};

class StdoutLogSink : public LogSink
{
public:
    virtual void Write(LogLevel level, const std::string & message);
};

class Logger
{
public:
    static Logger & Instance();
    
    void AttachSink(LogSink & sink);
    void Log(LogLevel level, const char * format, ...);
    
private:
    Logger();
    LogSink * mSink;
};

#endif // _LOGLIB_H_
