/*++

Copyright (c) SubSpace Library.

Module:

    NetLib.h

Description:

    Network library for SubSpace applications.

--*/

#ifndef _NETLIB_H_
#define _NETLIB_H_

#include "base.h"
#include <string>
#include <vector>
#include <set>

// Forward declarations
class Socket;
class bstring;

class InetAddress
{
public:
    InetAddress();
    InetAddress(const std::string & host, uint16 port);
    
    std::string GetHost() const { return mHost; }
    uint16 GetPort() const { return mPort; }
    
    bool operator==(const InetAddress & other) const;
    bool operator<(const InetAddress & other) const;
    
private:
    std::string mHost;
    uint16 mPort;
};

class bstring
{
public:
    bstring();
    bstring(const char * data, size_t length);
    bstring(const std::string & str);
    
    size_t length() const { return mData.size(); }
    const char * data() const { return mData.data(); }
    char operator[](size_t index) const { return mData[index]; }
    char & operator[](size_t index) { return mData[index]; }
    
    void Append(const bstring & other);
    bstring SubString(size_t start, size_t length = std::string::npos) const;
    std::string AsString() const;
    
private:
    std::vector<char> mData;
};

class Selector
{
public:
    enum Mode { KRead = 1, KWrite = 2 };
    
    Selector();
    ~Selector();
    
    void Select(int32 timeout = -1);
    void Alert();
    const std::set<Socket*> & ReadSet() const { return mReadSet; }
    const std::set<Socket*> & WriteSet() const { return mWriteSet; }
    
    friend class UdpClient;
    
private:
    std::set<Socket*> mReadSet;
    std::set<Socket*> mWriteSet;
    std::set<Socket*> mSockets;
};

class Socket
{
public:
    virtual ~Socket() {}
    virtual void Register(Selector & selector, int32 mode) = 0;
    virtual void Unregister(Selector & selector) = 0;
};

class UdpClient : public Socket
{
public:
    UdpClient();
    virtual ~UdpClient();
    
    void Bind(uint16 port);
    void Register(Selector & selector, int32 mode);
    void Unregister(Selector & selector);
    
    bstring Read(InetAddress & source, size_t maxLength);
    void Write(const InetAddress & destination, const bstring & data);
    
private:
    int32 mSocket;
    Selector * mSelector;
};

class Process
{
public:
    static std::string GetName();
};

template <typename T>
class DataReader
{
public:
    DataReader(const bstring & data) : mData(data), mPosition(0) {}
    
    uint8 ReadUint8() { return (uint8)mData[mPosition++]; }
    uint16 ReadUint16();
    uint32 ReadUint32();
    int32 ReadInt32();
    bstring Readbstring(size_t length);
    
private:
    bstring mData;
    size_t mPosition;
};

class LittleEndian {};
class BigEndian {};

#endif // _NETLIB_H_
