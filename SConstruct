#!/usr/bin/env python
"""
SubSpace Root Build Script

This is the main build script for the SubSpace project.
It builds all libraries and optionally all applications.
"""

import os
from glob import glob

# Import environment configuration
with open('environment.py') as f:
    exec(f.read())

# Build the SubSpace libraries
print("Building SubSpace libraries...")

lib_sources = glob('lib/src/*.cpp')

# Build static libraries
env.StaticLibrary('lib/netlib', ['lib/src/NetLib.cpp'])
env.StaticLibrary('lib/ssnetlib', ['lib/src/SSNetLib.cpp'])
env.StaticLibrary('lib/cryptolib', ['lib/src/CryptoLib.cpp'])
env.StaticLibrary('lib/loglib', ['lib/src/LogLib.cpp'])
env.StaticLibrary('lib/timelib', ['lib/src/TimeLib.cpp'])
env.StaticLibrary('lib/threadlib', ['lib/src/ThreadLib.cpp'])
env.StaticLibrary('lib/dblib', ['lib/src/DbLib.cpp'])
env.StaticLibrary('lib/nucleus', ['lib/src/nucleus.cpp'])

# Note: Applications can be built individually by running scons in their directories
# Example: cd dirserver && scons
print("")
print("Libraries built successfully!")
print("")
print("To build individual applications:")
print("  cd dirserver && scons")
print("  cd proxy && scons")
print("")
