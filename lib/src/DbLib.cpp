/*++

Copyright (c) SubSpace Library.

Module:

    DbLib.cpp

--*/

#include "DbLib.h"

// DbConnection implementation
DbConnection::DbConnection() : mConnected(false)
{
}

DbConnection::~DbConnection()
{
}

bool DbConnection::Execute(const std::string & query)
{
    // Stub implementation
    return false;
}

// DbConnectionManager implementation
DbConnectionManager::DbConnectionManager() : mInitialized(false)
{
}

DbConnectionManager & DbConnectionManager::Instance()
{
    static DbConnectionManager instance;
    return instance;
}

void DbConnectionManager::Initialize(const std::string & connectionString)
{
    mConnectionString = connectionString;
    mInitialized = true;
}

DbConnection * DbConnectionManager::GetConnection()
{
    if(!mInitialized)
        return nullptr;
    
    return new DbConnection();
}

void DbConnectionManager::ReleaseConnection(DbConnection * connection)
{
    delete connection;
}
