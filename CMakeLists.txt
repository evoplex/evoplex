#=============================================================================
#  Evoplex
#  Copyright (C) 2016-2017 by Marcos Cardinot <mcardinot@gmail.com>
#=============================================================================

CMAKE_MINIMUM_REQUIRED(VERSION 3.1.0 FATAL_ERROR)

PROJECT(evoplex)
SET (EVOPLEX_VERSION 0.0.0)

# Find includes in corresponding build directories
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)

# Move the libs to /lib and executable to /bin
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Generate the commands
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# c++11
SET (CMAKE_CXX_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# pass some of the CMake settings to the source code
CONFIGURE_FILE("${CMAKE_SOURCE_DIR}/src/config.h.in" "${CMAKE_BINARY_DIR}/src/config.h")

# Find qt5 packages
FIND_PACKAGE(Qt5Core 5.8.0 REQUIRED)
FIND_PACKAGE(Qt5Concurrent 5.8.0 REQUIRED)
FIND_PACKAGE(Qt5Charts 5.8.0 REQUIRED)
FIND_PACKAGE(Qt5Widgets 5.8.0 REQUIRED)
find_package(Qt5Test 5.8.0 REQUIRED)

# Compile source
ADD_SUBDIRECTORY(src)
