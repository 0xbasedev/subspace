# SubSpace Libraries Documentation

This document describes the stub libraries provided to make the SubSpace code buildable.

## Overview

The SubSpace ecosystem relies on several custom libraries that were not included in the original code release. This implementation provides stub versions of these libraries with the necessary interfaces to allow compilation and basic functionality.

## Library Reference

### base.h - Common Types and Definitions

Provides platform-independent type definitions and macros:

- **Type definitions**: `int8`, `int16`, `int32`, `int64`, `uint8`, `uint16`, `uint32`, `uint64`
- **Platform types**: `nativeint`, `const_cstring`
- **Platform detection**: `PLATFORM_WIN32` macro defined on Windows
- **Main function helper**: `main$` macro for cross-platform main functions

**Usage:**
```cpp
#include "base.h"

int32 main$(int32 argc, const_cstring * argv)
{
    uint16 port = 5000;
    // ...
}
```

### LogLib - Logging and Diagnostics

Provides a singleton logging system with multiple log levels and pluggable sinks.

**Key Classes:**
- `Logger` - Singleton logger with printf-style formatting
- `LogSink` - Abstract base for log output destinations
- `StdoutLogSink` - Writes logs to standard output

**Log Levels:**
- `KLogDebug` - Debug information
- `KLogInfo` - Informational messages
- `KLogWarning` - Warning messages
- `KLogError` - Error messages

**Usage:**
```cpp
#include "LogLib.h"

StdoutLogSink sink;
Logger::Instance().AttachSink(sink);
Logger::Instance().Log(KLogInfo, "Server started on port %d", port);
```

### TimeLib - Timing Utilities

Provides timing and delay functionality.

**Key Classes:**
- `Timer` - Stopwatch-style timer for measuring elapsed time
- `GetCurrentTimeMillis()` - Get current time in milliseconds
- `Sleep()` - Sleep for specified milliseconds

**Usage:**
```cpp
#include "TimeLib.h"

Timer timer;
timer.Start();
// ... do work ...
timer.Stop();
uint64 elapsed = timer.GetElapsedMilliseconds();
```

### NetLib - Low-Level Networking

Provides basic networking primitives including UDP sockets, selectors, and byte strings.

**Key Classes:**
- `InetAddress` - IP address and port wrapper
- `bstring` - Binary string for packet data
- `Selector` - I/O multiplexing (select/epoll abstraction)
- `UdpClient` - UDP socket wrapper
- `DataReader<T>` - Binary data parser (LittleEndian/BigEndian)
- `Process` - Process information utilities

**Usage:**
```cpp
#include "NetLib.h"

Selector selector;
UdpClient client;
client.Bind(6001);
client.Register(selector, Selector::KRead);

while(true)
{
    selector.Select();
    if(!selector.ReadSet().empty())
    {
        InetAddress source;
        bstring packet = client.Read(source, 65536);
        // Process packet...
    }
}
```

### SSNetLib - SubSpace Protocol

Provides SubSpace-specific network protocol implementation.

**Key Classes:**
- `SSConnection` - Represents a connection to a client
- `SSConnectionListener` - Interface for handling connection events
- `SSConnectionPool` - Manages multiple connections

**Usage:**
```cpp
#include "SSNetLib.h"

class MyListener : public SSConnectionListener
{
    virtual void OnConnect(SSConnection * conn) {
        // Handle new connection
    }
    
    virtual void OnDisconnect(SSConnection * conn) {
        // Handle disconnect
    }
    
    virtual void OnData(SSConnection * conn, const bstring & data) {
        // Handle incoming data
    }
};

Selector selector;
SSConnectionPool pool(selector);
MyListener listener;

pool.Bind(6006);
pool.AddListener(listener);

while(true)
    selector.Select(100);
```

### CryptoLib - Encryption

Provides encryption/decryption for SubSpace and Continuum protocols.

**Key Classes:**
- `ICryptosystem` - Abstract cryptography interface
- `SubSpaceCrypt` - Original SubSpace encryption (key-based)
- `ContinuumCrypt` - Continuum client encryption (key exchange)

**Usage:**
```cpp
#include "CryptoLib.h"

// SubSpace encryption
SubSpaceCrypt crypt(serverKey);
crypt.Decrypt(packet);

// Continuum encryption
ContinuumCrypt crypt(keyData);
crypt.Decrypt(packet);
```

### DbLib - Database Access

Provides database connectivity (designed for PostgreSQL).

**Key Classes:**
- `DbConnection` - Database connection wrapper
- `DbConnectionManager` - Singleton connection pool manager

**Usage:**
```cpp
#include "DbLib.h"

DbConnectionManager::Instance().Initialize(
    "dbname=subspace user=subspace password=secret");

DbConnection * conn = DbConnectionManager::Instance().GetConnection();
conn->Execute("SELECT * FROM players");
DbConnectionManager::Instance().ReleaseConnection(conn);
```

### ThreadLib - Threading

Provides basic threading and synchronization primitives.

**Key Classes:**
- `Mutex` - Mutual exclusion lock
- `Thread` - Abstract thread base class

**Usage:**
```cpp
#include "ThreadLib.h"

class WorkerThread : public Thread
{
    virtual void Run() {
        // Thread work here
    }
};

WorkerThread worker;
worker.Start();
// ... do other work ...
worker.Join();
```

### nucleus - Core Framework

Minimal core framework library.

**Functions:**
- `nucleus::Initialize()` - Initialize framework
- `nucleus::Shutdown()` - Shutdown framework

**Usage:**
```cpp
#include "nucleus.h"

int main()
{
    nucleus::Initialize();
    // ... application code ...
    nucleus::Shutdown();
}
```

## Implementation Notes

### Current Status

These are **stub implementations** designed to:
1. Provide the interfaces needed for compilation
2. Offer basic functionality for simple use cases
3. Demonstrate the library architecture

### Limitations

The stub implementations have the following limitations:

1. **NetLib**: 
   - Selector uses simple sleep() instead of true I/O multiplexing
   - Limited error handling
   - No connection pooling optimizations

2. **SSNetLib**:
   - Minimal protocol implementation
   - No connection state management
   - No packet acknowledgment/retransmission

3. **CryptoLib**:
   - Uses simple XOR instead of proper encryption algorithms
   - Not cryptographically secure
   - For compilation/testing only

4. **DbLib**:
   - No actual database connectivity
   - Stubs return false/null
   - Requires PostgreSQL library for real use

5. **ThreadLib**:
   - Basic implementation using C++11 std::thread
   - No advanced synchronization primitives

### Production Use

For production use, these libraries should be enhanced with:
- Proper protocol state machines
- Real cryptographic algorithms (e.g., using OpenSSL)
- Full database connectivity (libpqxx for PostgreSQL)
- Advanced I/O multiplexing (epoll/kqueue)
- Connection pooling and resource management
- Comprehensive error handling
- Security hardening
- Performance optimizations

## Building

See the main [README.md](README.md) for build instructions.

## License

[GNU GPL v3](LICENSE)
