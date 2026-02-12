/*++

Copyright (c) SubSpace Library.

Module:

    NetLib.cpp

--*/

#include "NetLib.h"
#include <cstring>
#include <sstream>
#include <iomanip>

#if defined(PLATFORM_WIN32)
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <fcntl.h>
#endif

// InetAddress implementation
InetAddress::InetAddress() : mHost(""), mPort(0)
{
}

InetAddress::InetAddress(const std::string & host, uint16 port)
    : mHost(host), mPort(port)
{
}

bool InetAddress::operator==(const InetAddress & other) const
{
    return mHost == other.mHost && mPort == other.mPort;
}

bool InetAddress::operator<(const InetAddress & other) const
{
    if(mHost < other.mHost) return true;
    if(mHost > other.mHost) return false;
    return mPort < other.mPort;
}

// bstring implementation
bstring::bstring()
{
}

bstring::bstring(const char * data, size_t length)
    : mData(data, data + length)
{
}

bstring::bstring(const std::string & str)
    : mData(str.begin(), str.end())
{
}

void bstring::Append(const bstring & other)
{
    mData.insert(mData.end(), other.mData.begin(), other.mData.end());
}

bstring bstring::SubString(size_t start, size_t length) const
{
    if(start >= mData.size())
        return bstring();
    
    size_t end = (length == std::string::npos) ? mData.size() : std::min(start + length, mData.size());
    return bstring(mData.data() + start, end - start);
}

std::string bstring::AsString() const
{
    std::stringstream ss;
    for(size_t i = 0; i < mData.size(); ++i)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') 
           << (int)(unsigned char)mData[i] << " ";
        if((i + 1) % 16 == 0)
            ss << "\n";
    }
    return ss.str();
}

// Selector implementation
Selector::Selector()
{
}

Selector::~Selector()
{
}

void Selector::Select(int32 timeout)
{
    // Stub implementation - just sleep
    if(timeout > 0)
    {
        #if defined(PLATFORM_WIN32)
            Sleep(timeout);
        #else
            usleep(timeout * 1000);
        #endif
    }
}

void Selector::Alert()
{
    // Stub implementation
}

// UdpClient implementation
UdpClient::UdpClient() : mSocket(-1), mSelector(nullptr)
{
}

UdpClient::~UdpClient()
{
    if(mSocket != -1)
    {
        #if defined(PLATFORM_WIN32)
            closesocket(mSocket);
        #else
            close(mSocket);
        #endif
    }
}

void UdpClient::Bind(uint16 port)
{
    #if defined(PLATFORM_WIN32)
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
    #endif
    
    mSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if(mSocket < 0)
        return;
    
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    
    bind(mSocket, (struct sockaddr*)&addr, sizeof(addr));
}

void UdpClient::Register(Selector & selector, int32 mode)
{
    mSelector = &selector;
    if(mode & Selector::KRead)
        selector.mReadSet.insert(this);
    if(mode & Selector::KWrite)
        selector.mWriteSet.insert(this);
}

void UdpClient::Unregister(Selector & selector)
{
    selector.mReadSet.erase(this);
    selector.mWriteSet.erase(this);
    mSelector = nullptr;
}

bstring UdpClient::Read(InetAddress & source, size_t maxLength)
{
    char buffer[65536];
    sockaddr_in addr;
    socklen_t addrLen = sizeof(addr);
    
    int received = recvfrom(mSocket, buffer, maxLength, 0, (struct sockaddr*)&addr, &addrLen);
    if(received > 0)
    {
        char hostStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr.sin_addr, hostStr, sizeof(hostStr));
        source = InetAddress(hostStr, ntohs(addr.sin_port));
        return bstring(buffer, received);
    }
    
    return bstring();
}

void UdpClient::Write(const InetAddress & destination, const bstring & data)
{
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(destination.GetPort());
    inet_pton(AF_INET, destination.GetHost().c_str(), &addr.sin_addr);
    
    sendto(mSocket, data.data(), data.length(), 0, (struct sockaddr*)&addr, sizeof(addr));
}

// Process implementation
std::string Process::GetName()
{
    return "subspace";
}

// DataReader template specializations
template <>
uint16 DataReader<LittleEndian>::ReadUint16()
{
    uint8 b1 = ReadUint8();
    uint8 b2 = ReadUint8();
    return b1 | (b2 << 8);
}

template <>
uint32 DataReader<LittleEndian>::ReadUint32()
{
    uint16 w1 = ReadUint16();
    uint16 w2 = ReadUint16();
    return w1 | (w2 << 16);
}

template <>
int32 DataReader<LittleEndian>::ReadInt32()
{
    return (int32)ReadUint32();
}

template <>
bstring DataReader<LittleEndian>::Readbstring(size_t length)
{
    bstring result = mData.SubString(mPosition, length);
    mPosition += length;
    return result;
}
