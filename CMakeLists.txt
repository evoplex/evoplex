##########################################################################
#  This file is part of Evoplex.
#
#  Evoplex is a multi-agent system for networks.
#  Copyright (C) 2018 - Marcos Cardinot <marcos@cardinot.net>
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
##########################################################################

cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

set(EVOPLEX_VERSION 0.2.0)
set(EVOPLEX_RELEASE "development")

if(EVOPLEX_RELEASE STREQUAL "")
  set(EVOPLEX_VERSION_RELEASE "${EVOPLEX_VERSION}")
else()
  set(EVOPLEX_VERSION_RELEASE "${EVOPLEX_VERSION}-${EVOPLEX_RELEASE}")
endif()

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

# c++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# clang-3.8 and 3.9 need to be linked against libc++ to make c++14 work
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND
     #${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER 3.6 AND
     ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 4.0)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/src/cmake)

option(TESTS "Turn on tests" OFF)
option(CODE_COVERAGE "Turn on code coverage" OFF)
if(CODE_COVERAGE)
  set(TESTS ON)
  if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_BUILD_TYPE Debug) # coverage must be in debug mode
    include(CodeCoverage) # codecov.io
    include(Coveralls) # coveralls.io
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
    append_coverage_compiler_flags()
    setup_target_for_coverage(NAME "${PROJECT_NAME}_coverage" EXECUTABLE ctest --parallel 4)
  else()
    message("CODE_COVERAGE cannot be ON for this compiler!")
    set(CODE_COVERAGE OFF)
  endif()
endif()

if(TESTS)
  enable_testing() # Tests must be enabled from root
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

# http://doc.qt.io/qt-5/qmessagelogcontext.html
# By default QMessageLogContext only works in Debug mode.
# To make it permanent for all modes, we need to define QT_MESSAGELOGCONTEXT
add_definitions("-DQT_MESSAGELOGCONTEXT")

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
find_package(Qt5Svg 5.8.0 REQUIRED)

# set compilation and installation directories
if(APPLE)
  set(CMAKE_INSTALL_PREFIX "/Applications")
  set(EVOPLEX_OUTPUT_PREFIX "${CMAKE_BINARY_DIR}/bin/")
  set(EVOPLEX_INSTALL_PREFIX "${PROJECT_NAME}.app/Contents/MacOS/")

  set(EVOPLEX_OUTPUT_RUNTIME "${EVOPLEX_OUTPUT_PREFIX}")
  set(EVOPLEX_OUTPUT_RESOURCES "${EVOPLEX_OUTPUT_PREFIX}${PROJECT_NAME}.app/Contents/Resources/")
  set(EVOPLEX_OUTPUT_LIBRARY "${EVOPLEX_OUTPUT_PREFIX}${PROJECT_NAME}.app/Contents/MacOS/lib/evoplex/")
  set(EVOPLEX_INSTALL_RUNTIME "${EVOPLEX_INSTALL_PREFIX}")
  set(EVOPLEX_INSTALL_RESOURCES "${EVOPLEX_INSTALL_PREFIX}../Resources/")
elseif(WIN32)
  set(EVOPLEX_OUTPUT_PREFIX "${CMAKE_BINARY_DIR}/releases/")

  set(EVOPLEX_OUTPUT_RUNTIME "${EVOPLEX_OUTPUT_PREFIX}")
  set(EVOPLEX_OUTPUT_RESOURCES "${EVOPLEX_OUTPUT_PREFIX}")
  set(EVOPLEX_OUTPUT_LIBRARY "${EVOPLEX_OUTPUT_PREFIX}lib/evoplex/")
  set(EVOPLEX_INSTALL_RUNTIME ".")
  set(EVOPLEX_INSTALL_RESOURCES ".")
else()
  set(EVOPLEX_OUTPUT_PREFIX "${CMAKE_BINARY_DIR}/")

  set(EVOPLEX_OUTPUT_RUNTIME "${EVOPLEX_OUTPUT_PREFIX}bin/")
  set(EVOPLEX_OUTPUT_RESOURCES "${EVOPLEX_OUTPUT_PREFIX}share/evoplex/")
  set(EVOPLEX_OUTPUT_LIBRARY "${EVOPLEX_OUTPUT_PREFIX}lib/evoplex/")
  set(EVOPLEX_INSTALL_RUNTIME "bin/")
  set(EVOPLEX_INSTALL_RESOURCES "share/evoplex/")
endif()
set(EVOPLEX_OUTPUT_ARCHIVE "${EVOPLEX_OUTPUT_LIBRARY}")
set(EVOPLEX_INSTALL_LIBRARY "${EVOPLEX_INSTALL_PREFIX}lib/evoplex/")
set(EVOPLEX_INSTALL_ARCHIVE "${EVOPLEX_INSTALL_PREFIX}lib/evoplex/")
set(EVOPLEX_INSTALL_INCLUDES "${EVOPLEX_INSTALL_PREFIX}include/evoplex/")

# Compile source
add_subdirectory(src)

# copy text files
foreach(TXTFILE LICENSE.txt README.md CHANGELOG.md)
  configure_file(${TXTFILE} "${EVOPLEX_OUTPUT_RESOURCES}${TXTFILE}" COPYONLY)
  install(FILES ${TXTFILE} DESTINATION "${EVOPLEX_INSTALL_RESOURCES}")
endforeach(TXTFILE)

# Packaging
include(EvoplexCPack)
