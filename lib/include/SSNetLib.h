/*++

Copyright (c) SubSpace Library.

Module:

    SSNetLib.h

Description:

    SubSpace network protocol library.

--*/

#ifndef _SSNETLIB_H_
#define _SSNETLIB_H_

#include "base.h"
#include "NetLib.h"
#include <vector>

class SSConnection;

class SSConnectionListener
{
public:
    virtual ~SSConnectionListener() {}
    virtual void OnConnect(SSConnection * connection) = 0;
    virtual void OnDisconnect(SSConnection * connection) = 0;
    virtual void OnData(SSConnection * connection, const bstring & data) = 0;
};

class SSConnection
{
public:
    SSConnection();
    virtual ~SSConnection();
    
    InetAddress GetRemoteAddress() const { return mRemoteAddress; }
    void Send(const bstring & data);
    void Disconnect();
    
private:
    friend class SSConnectionPool;
    InetAddress mRemoteAddress;
    bool mConnected;
};

class SSConnectionPool
{
public:
    SSConnectionPool(Selector & selector);
    ~SSConnectionPool();
    
    void Bind(uint16 port);
    void AddListener(SSConnectionListener & listener);
    void RemoveListener(SSConnectionListener & listener);
    
private:
    Selector & mSelector;
    std::vector<SSConnectionListener*> mListeners;
    std::vector<SSConnection*> mConnections;
    uint16 mPort;
};

#endif // _SSNETLIB_H_
