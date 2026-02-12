# SubSpace Examples

This directory contains example programs demonstrating how to use the SubSpace libraries.

## Building Examples

```bash
# From the repository root, build the libraries first
cd /path/to/subspace
scons

# Then build the examples
cd examples
scons
```

## simple_server - UDP Echo Server

A basic UDP server that echoes back any packets it receives.

**Build:**
```bash
cd examples
scons
```

**Run:**
```bash
./simple_server [port]
# Default port is 6000
```

**Test:**
The server will listen for UDP packets and echo them back. You can test it using netcat:

```bash
# In terminal 1:
./simple_server 6000

# In terminal 2:
echo "Hello World" | nc -u localhost 6000
```

**What it demonstrates:**
- Creating and binding a UDP socket
- Using the Selector for I/O multiplexing
- Reading and writing UDP packets
- Using InetAddress for network addresses
- Using the logging system
- Signal handling for clean shutdown

## Creating Your Own Programs

To create a new program using the SubSpace libraries:

1. **Include the headers:**
```cpp
#include "base.h"
#include "NetLib.h"
#include "SSNetLib.h"
#include "LogLib.h"
#include "TimeLib.h"
#include "CryptoLib.h"
// etc.
```

2. **Compile and link:**
```bash
g++ -std=c++11 -I../lib/include -L../lib myprogram.cpp \
    -lnetlib -lssnetlib -lcryptolib -lloglib -ltimelib \
    -lthreadlib -ldblib -lnucleus -o myprogram
```

3. **Or add to SConstruct:**
```python
env.Program('myprogram',
    source = ['myprogram.cpp'],
    LIBS = ['netlib', 'loglib', 'timelib'])
```

## See Also

- [LIBRARIES.md](../LIBRARIES.md) - Complete library documentation
- [BUILD.md](../BUILD.md) - Build system documentation
- [README.md](../README.md) - Project overview
