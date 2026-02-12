# Building SubSpace

This guide provides detailed instructions for building the SubSpace libraries and applications.

## Prerequisites

### Required

- **Python** 2.7 or 3.x
- **SCons** build system
- **GCC** 4.8+ or **Clang** 3.4+ (with C++11 support)
- Standard C++ development libraries

### Installation

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install build-essential python3 python3-pip
pip3 install scons
```

#### Fedora/RHEL/CentOS
```bash
sudo dnf install gcc-c++ python3 python3-pip
pip3 install scons
```

#### macOS
```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install Python and SCons
brew install python3
pip3 install scons
```

#### Windows
1. Install [Visual Studio](https://visualstudio.microsoft.com/) or [MinGW-w64](https://www.mingw-w64.org/)
2. Install [Python](https://www.python.org/downloads/)
3. Install SCons: `pip install scons`

### Optional Dependencies

Some applications require additional libraries:

#### For isometry (Zone Server)
```bash
# Ubuntu/Debian
sudo apt-get install libpq-dev libpqxx-dev

# Fedora/RHEL/CentOS
sudo dnf install postgresql-devel libpqxx-devel

# macOS
brew install postgresql libpqxx
```

#### For starlight (GTK Client)
```bash
# Ubuntu/Debian
sudo apt-get install libgtk2.0-dev libglade2-dev

# Fedora/RHEL/CentOS
sudo dnf install gtk2-devel libglade2-devel

# macOS
brew install gtk+ libglade
```

## Building the Libraries

### Step 1: Clone the Repository

```bash
git clone https://github.com/0xbasedev/subspace.git
cd subspace
```

### Step 2: Build All Libraries

```bash
scons
```

This will build all eight core libraries in the `lib/` directory:
- `libnetlib.a` - Low-level networking
- `libssnetlib.a` - SubSpace protocol
- `libcryptolib.a` - Encryption/decryption
- `libloglib.a` - Logging
- `libtimelib.a` - Timing utilities
- `libthreadlib.a` - Threading
- `libdblib.a` - Database access
- `libnucleus.a` - Core framework

### Build Output

```
Building SubSpace libraries...

Libraries built successfully!

To build individual applications:
  cd dirserver && scons
  cd proxy && scons
```

### Verify Build

```bash
ls -lh lib/*.a
```

You should see eight `.a` (archive/static library) files.

## Building Applications

### Proxy (Network Analyzer)

The proxy is a standalone tool that doesn't require external dependencies.

```bash
cd proxy
scons
```

**Output:** `bin/proxy`

**Usage:**
```bash
./bin/proxy [server_ip] [server_port] [listen_port]
```

### Directory Server

```bash
cd dirserver
scons
```

**Note:** May require updates to SConscript for standalone build.

### Other Applications

Other applications (isometry, oracle, starlight, sscattack) may require:
1. Creating or updating their SConstruct files
2. Installing additional dependencies
3. Adjusting include/library paths

## Troubleshooting

### SCons Not Found

```bash
pip3 install --user scons
# Add ~/.local/bin to PATH if needed
export PATH="$HOME/.local/bin:$PATH"
```

### Compiler Not Found

**Linux:**
```bash
sudo apt-get install build-essential  # Debian/Ubuntu
sudo dnf install gcc-c++              # Fedora/RHEL
```

**macOS:**
```bash
xcode-select --install
```

**Windows:**
- Install Visual Studio with C++ support
- Or install MinGW-w64 and ensure it's in PATH

### Python execfile() Error

If you get `NameError: name 'execfile' is not defined`, the build scripts have already been updated for Python 3. Ensure you're using the latest version from the repository.

### Include Path Errors

If you get errors about missing headers like `base.h` or `NetLib.h`, ensure you're building from the correct directory:

```bash
# Build libraries from root
cd /path/to/subspace
scons

# Build applications from their directories
cd proxy
scons
```

### Linking Errors

If you get undefined reference errors, ensure the libraries are built first:

```bash
# Build libraries first
cd /path/to/subspace
scons

# Then build application
cd proxy
scons
```

## Build Customization

### Debug Build

To build with debug symbols:

```bash
# Edit environment.py and add:
env.Append(CXXFLAGS=['-g', '-O0'])

# Then rebuild
scons -c  # Clean
scons     # Build
```

### Release Build

For optimized release build:

```bash
# Edit environment.py and add:
env.Append(CXXFLAGS=['-O3', '-DNDEBUG'])

# Then rebuild
scons -c  # Clean
scons     # Build
```

### Clean Build

To remove all build artifacts:

```bash
scons -c
rm -rf lib/*.a lib/src/*.o
```

## Platform-Specific Notes

### Windows

- Use forward slashes (/) or escaped backslashes (\\\\) in paths
- May need to specify compiler explicitly if multiple installed
- Some applications may require MinGW/MSYS2 environment

### macOS

- Ensure Xcode Command Line Tools are installed
- May need to use Homebrew for additional dependencies
- Some paths may differ from Linux

### Linux

- Most straightforward platform
- Use package manager for dependencies
- May need to run with `sudo` for system-wide installs

## Next Steps

After building:

1. **Read** [LIBRARIES.md](LIBRARIES.md) for library documentation
2. **Review** application-specific documentation in each subdirectory
3. **Test** the proxy application as a starting point
4. **Explore** the source code to understand the architecture

## Getting Help

If you encounter issues:

1. Check this document for common problems
2. Verify all prerequisites are installed
3. Try a clean build (`scons -c && scons`)
4. Check the GitHub issues for similar problems
5. Create a new issue with build logs and system information

## License

[GNU GPL v3](LICENSE)
