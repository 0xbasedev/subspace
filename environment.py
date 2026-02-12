#!/usr/bin/env python
"""
SubSpace Build Environment Configuration

This file configures the SCons build environment for SubSpace applications.
"""

import os
from glob import glob

# Create the base environment
env = Environment()

# Get the current directory and configure paths
current_dir = os.getcwd()
lib_include = os.path.join(current_dir, 'lib/include')
lib_path = os.path.join(current_dir, 'lib')

# Configure include paths
env.Append(CPPPATH=[lib_include])

# Configure library paths
env.Append(LIBPATH=[lib_path])

# Set C++ standard
env.Append(CXXFLAGS=['-std=c++11', '-Wall'])

# Platform-specific configuration
if os.name == 'nt':  # Windows
    env['PLATFORM_DEFINES'] = ['PLATFORM_WIN32']
else:  # Unix-like (Linux, macOS)
    env['PLATFORM_DEFINES'] = []

env.Append(CPPDEFINES=env['PLATFORM_DEFINES'])

# Export the environment for use in SConscript files
Export('env')
Export('glob')
