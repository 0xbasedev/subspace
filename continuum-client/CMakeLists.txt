cmake_minimum_required(VERSION 3.10)
project(continuum_client C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler flags
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/checksum.c
    src/md5.c
    src/vie_crypt.c
    src/continuum_crypt.c
)

# Create static library
add_library(continuum_crypt STATIC ${SOURCES})

# Create shared library
add_library(continuum_crypt_shared SHARED ${SOURCES})
set_target_properties(continuum_crypt_shared PROPERTIES OUTPUT_NAME continuum_crypt)

# Main demo executable
add_executable(continuum_demo src/main.c)
target_link_libraries(continuum_demo continuum_crypt)

# Install targets
install(TARGETS continuum_crypt continuum_crypt_shared continuum_demo
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include/continuum)

# Summary
message(STATUS "")
message(STATUS "Continuum 0.40 Encryption Client")
message(STATUS "=================================")
message(STATUS "")
message(STATUS "This library implements:")
message(STATUS "  - VIE (SubSpace) encryption")
message(STATUS "  - Continuum block cipher (64-bit blocks, 640-bit key)")
message(STATUS "  - Key expansion step 3 (modified MD5)")
message(STATUS "  - CRC8, CRC32, VIE checksums")
message(STATUS "  - Protocol packet structures")
message(STATUS "")
message(STATUS "NOT included (proprietary):")
message(STATUS "  - Key expansion steps 1-2")
message(STATUS "  - These must be obtained from an oracle service")
message(STATUS "")
