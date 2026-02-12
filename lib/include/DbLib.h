/*++

Copyright (c) SubSpace Library.

Module:

    DbLib.h

Description:

    Database library for SubSpace applications.

--*/

#ifndef _DBLIB_H_
#define _DBLIB_H_

#include "base.h"
#include <string>

class DbConnection
{
public:
    DbConnection();
    virtual ~DbConnection();
    
    bool Execute(const std::string & query);
    // Additional database methods would go here
    
private:
    bool mConnected;
};

class DbConnectionManager
{
public:
    static DbConnectionManager & Instance();
    
    void Initialize(const std::string & connectionString);
    DbConnection * GetConnection();
    void ReleaseConnection(DbConnection * connection);
    
private:
    DbConnectionManager();
    std::string mConnectionString;
    bool mInitialized;
};

#endif // _DBLIB_H_
