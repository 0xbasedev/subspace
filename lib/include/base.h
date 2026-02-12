/*++

Copyright (c) SubSpace Library.

Module:

    base.h

Description:

    Base types and definitions for SubSpace libraries.

--*/

#ifndef _BASE_H_
#define _BASE_H_

#include <cstdint>
#include <string>

// Platform detection
#if defined(_WIN32) || defined(_WIN64)
#   define PLATFORM_WIN32
#endif

// Type definitions
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int nativeint;
typedef const char * const_cstring;

// Helper macro for main function
#define main$ main

#endif // _BASE_H_
