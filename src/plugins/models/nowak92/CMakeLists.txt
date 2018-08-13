##########################################################################
#
#  Evoplex <https://evoplex.org>
#  Copyright (C) 2016-present
#
##########################################################################

cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

get_filename_component(PLUGIN_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(PLUGIN_NAME plugin_${PLUGIN_NAME})
project(${PLUGIN_NAME})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(EvoplexCore 0.1.0 REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Concurrent REQUIRED)

# set compilation and installation directories
if(APPLE)
  set(PLUGIN_INSTALL_LIBRARY "evoplex.app/Contents/MacOS/lib/evoplex/plugins")
else()
  set(PLUGIN_INSTALL_LIBRARY "lib/evoplex/plugins")
endif()
set(PLUGIN_OUTPUT_LIBRARY "${CMAKE_BINARY_DIR}/plugin")

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

add_library(${PLUGIN_NAME} SHARED plugin.cpp)
target_link_libraries(${PLUGIN_NAME} PRIVATE Evoplex::EvoplexCore Qt5::Core)
set_target_properties(${PLUGIN_NAME} PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_LIBRARY}
  ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PLUGIN_OUTPUT_LIBRARY}
  ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PLUGIN_OUTPUT_LIBRARY}
  ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL ${PLUGIN_OUTPUT_LIBRARY}
  ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PLUGIN_OUTPUT_LIBRARY}
  LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_LIBRARY}
  LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PLUGIN_OUTPUT_LIBRARY}
  LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PLUGIN_OUTPUT_LIBRARY}
  LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL ${PLUGIN_OUTPUT_LIBRARY}
  LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PLUGIN_OUTPUT_LIBRARY}
  RUNTIME_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_LIBRARY}
  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PLUGIN_OUTPUT_LIBRARY}
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PLUGIN_OUTPUT_LIBRARY}
  RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${PLUGIN_OUTPUT_LIBRARY}
  RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PLUGIN_OUTPUT_LIBRARY})

# Qt Object macro is hidden in another macro; let's add them here
set_property(TARGET ${PLUGIN_NAME} APPEND PROPERTY
  AUTOMOC_MACRO_NAMES "REGISTER_PLUGIN")

install(TARGETS ${PLUGIN_NAME}
  LIBRARY DESTINATION "${PLUGIN_INSTALL_LIBRARY}"
  ARCHIVE DESTINATION "${PLUGIN_INSTALL_LIBRARY}")
