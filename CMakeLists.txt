CMAKE_MINIMUM_REQUIRED(VERSION 3.1.0 FATAL_ERROR)
PROJECT(evoplex)

# Find includes in corresponding build directories
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
SET(CMAKE_AUTOMOC ON)

# c++11
SET (CMAKE_CXX_STANDARD 11)

# Find qt5 packages
FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Widgets REQUIRED)

# Handle data stuff
#ADD_SUBDIRECTORY(data)

# Compile source
ADD_SUBDIRECTORY(src)
