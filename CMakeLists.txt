#=============================================================================
#  Evoplex
#  Copyright (C) 2016-2017 by Marcos Cardinot <mcardinot@gmail.com>
#=============================================================================

cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

set(EVOPLEX_RELEASE "alpha")
set(EVOPLEX_VERSION 0.0.0)
project(evoplex VERSION ${EVOPLEX_VERSION})

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
if(POLICY CMP0071)
  cmake_policy(SET CMP0071 NEW) # process generated files
endif()
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Generate the commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# c++11
set (CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Get the current working branch
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE EVOPLEX_GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
# Get the latest abbreviated commit hash of the working branch
execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE EVOPLEX_GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
add_definitions("-DGIT_COMMIT_HASH=${EVOPLEX_GIT_COMMIT_HASH}")
add_definitions("-DGIT_BRANCH=${EVOPLEX_GIT_BRANCH}")

# https://cmake.org/Wiki/CMake_RPATH_handling
# use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH  FALSE)
# when building, don't use the install RPATH already (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib/evoplex")
# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
# the RPATH to be used when installing, but only if it's not a system directory
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib/evoplex" isSystemDir)
if("${isSystemDir}" STREQUAL "-1")
   set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib/evoplex")
endif("${isSystemDir}" STREQUAL "-1")

# pass some of the CMake settings to the source code
configure_file("${CMAKE_SOURCE_DIR}/src/config.h.in" "${CMAKE_BINARY_DIR}/src/config.h")

# Find qt5 packages
find_package(Qt5Core 5.8.0 REQUIRED)
find_package(Qt5Concurrent 5.8.0 REQUIRED)
find_package(Qt5Charts 5.8.0 REQUIRED)
find_package(Qt5Widgets 5.8.0 REQUIRED)
find_package(Qt5Test 5.8.0 REQUIRED)

# Compile source
add_subdirectory(src)

# CPack: general configuration
set(CPACK_GENERATOR "RPM;DEB")
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "Marcos Cardinot <marcos@cardinot.net>")
set(CPACK_PACKAGE_DESCRIPTION "Evoplex is a multi-agent system for networks.")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${CPACK_PACKAGE_DESCRIPTION})
set(CPACK_PACKAGE_HOMEPAGE_URL "https://evoplex.github.io")
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/src/icon.bmp")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${EVOPLEX_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION / /usr/local /usr/local/bin /usr/local/include /usr/local/lib)
# CPack: Debian package generator
set(CPACK_DEBIAN_PACKAGE_DEPENDS "cmake (>= 3.1), qt5-default (>= 5.8), libqt5charts5 (>=5.8)")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION})
set(CPACK_DEBIAN_PACKAGE_MAINTAINER ${CPACK_PACKAGE_VENDOR})
set(CPACK_DEBIAN_PACKAGE_SECTION "contrib/science")
# CPack: RPM package generator
set(CPACK_RPM_PACKAGE_REQUIRES "cmake >= 3.1, qt5-qtbase-devel >= 5.8, qt5-qtbase-devel-gui >= 5.8, qt5-qtcharts >= 5.8")
set(CPACK_RPM_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION})
set(CPACK_RPM_PACKAGE_RELEASE ${EVOPLEX_RELEASE})
set(CPACK_RPM_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}.rpm")
set(CPACK_RPM_CHANGELOG_FILE "${CMAKE_SOURCE_DIR}/CHANGELOG.md")
set(CPACK_RPM_PACKAGE_GROUP "Development/Tools")
set(CPACK_RPM_PACKAGE_LICENSE "GPLv3")
# CPack call
include(CPack)
