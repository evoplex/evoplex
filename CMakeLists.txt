#=============================================================================
#  Evoplex
#  Copyright (C) 2016-2017 by Marcos Cardinot
#=============================================================================

PROJECT(evoplex)

CMAKE_MINIMUM_REQUIRED(VERSION 3.1.0 FATAL_ERROR)

# Find includes in corresponding build directories
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)

# Move the libs to /lib and executable to /bin
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Generate the commands
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# c++11
SET (CMAKE_CXX_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Find qt5 packages
FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Concurrent REQUIRED)
FIND_PACKAGE(Qt5Widgets REQUIRED)

# Handle data stuff
#ADD_SUBDIRECTORY(data)

# Compile source
ADD_SUBDIRECTORY(src)

# default plugins (models and graphs)
ADD_SUBDIRECTORY(plugins)
