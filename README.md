# SubSpace

This repository contains various SubSpace-related applications and tools.
The code has been made buildable by providing stub implementations of the
required internal libraries.

## Architecture

SubSpace is a multiplayer game server ecosystem with the following components:

### Core Libraries (lib/)

- **base.h** - Common type definitions and platform abstractions
- **netlib** - Low-level networking (UDP, TCP, sockets, selectors)
- **ssnetlib** - SubSpace-specific network protocol implementation
- **cryptolib** - Encryption/decryption for SubSpace and Continuum protocols
- **loglib** - Logging and diagnostics
- **timelib** - Timing and timer utilities
- **threadlib** - Threading and synchronization primitives
- **dblib** - Database connectivity (PostgreSQL)
- **nucleus** - Core framework

### Applications

1. **dirserver** - Directory Server
   - Manages zone/server discovery
   - Handles client and server connections
   - Facilitates peer communication between zones
   
2. **isometry** - Zone Server
   - Main game server managing players, squads, chats, and arenas
   - Database-driven with PostgreSQL backend
   - Handles game logic and player interactions
   
3. **oracle** - PE Loader
   - Windows/Wine-based executable loader
   - Extracts encryption keys from Continuum client executables
   - Supports versions 3.8 and 4.0
   
4. **proxy** - Network Proxy
   - Intercepts and analyzes SubSpace protocol packets
   - Decrypts/encrypts traffic for protocol analysis
   - Development and debugging tool
   
5. **starlight** - GTK Client
   - Chat-focused client application
   - Connects to zones with chat, player lists, arena views
   - Minimal game client for administration
   
6. **sscattack** - Utility
   - Extracts server IDs from SSC biller
   - Port scanning and ID extraction tool

## Building

### Prerequisites

- Python 2.7 or 3.x
- SCons build system (`pip install scons`)
- GCC or Clang C++ compiler
- Standard C++ libraries

For specific applications:
- **isometry**: PostgreSQL development libraries (libpqxx)
- **starlight**: GTK+ 2.0, libglade-2.0, gdk-pixbuf-2.0

### Building the Libraries

```bash
# Build all core libraries
scons

# This will create static libraries in lib/:
#   - libnetlib.a
#   - libssnetlib.a
#   - libcryptolib.a
#   - libloglib.a
#   - libtimelib.a
#   - libthreadlib.a
#   - libdblib.a
#   - libnucleus.a
```

### Building Applications

Each application can be built independently:

```bash
# Build the directory server
cd dirserver
scons

# Build the proxy
cd proxy
scons

# Other applications require additional dependencies
# and may not build without them installed
```

## Current Status

The core libraries are now implemented as functional stubs that provide:
- Basic networking functionality
- Protocol structure support
- Logging and timing utilities
- Minimal encryption/decryption

**Note**: The stub implementations provide the interfaces and basic functionality
needed for compilation. Full production use would require:
- Complete protocol implementation
- Production-grade encryption
- Database connection pooling
- Enhanced error handling
- Performance optimization

Some applications (isometry, starlight) require external dependencies that
are not included and may not build without them.

## License
[GNU GPL v3](LICENSE)
