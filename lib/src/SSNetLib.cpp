/*++

Copyright (c) SubSpace Library.

Module:

    SSNetLib.cpp

--*/

#include "SSNetLib.h"
#include <algorithm>

// SSConnection implementation
SSConnection::SSConnection() : mConnected(false)
{
}

SSConnection::~SSConnection()
{
}

void SSConnection::Send(const bstring & data)
{
    // Stub implementation
}

void SSConnection::Disconnect()
{
    mConnected = false;
}

// SSConnectionPool implementation
SSConnectionPool::SSConnectionPool(Selector & selector)
    : mSelector(selector), mPort(0)
{
}

SSConnectionPool::~SSConnectionPool()
{
    for(auto conn : mConnections)
        delete conn;
}

void SSConnectionPool::Bind(uint16 port)
{
    mPort = port;
}

void SSConnectionPool::AddListener(SSConnectionListener & listener)
{
    mListeners.push_back(&listener);
}

void SSConnectionPool::RemoveListener(SSConnectionListener & listener)
{
    auto it = std::find(mListeners.begin(), mListeners.end(), &listener);
    if(it != mListeners.end())
        mListeners.erase(it);
}
