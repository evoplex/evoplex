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

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(EvoplexCore 0.2.0 REQUIRED)
find_package(Qt5Core 5.8.0 REQUIRED)
find_package(Qt5Concurrent 5.8.0 REQUIRED)
find_package(Qt5Network 5.8.0 REQUIRED)

# set compilation and installation directories
if(APPLE)
  set(PLUGIN_INSTALL_LIBRARY "evoplex.app/Contents/MacOS/lib/evoplex/plugins")
else()
  set(PLUGIN_INSTALL_LIBRARY "lib/evoplex/plugins")
endif()
set(PLUGIN_OUTPUT_LIBRARY "${CMAKE_BINARY_DIR}/plugin")

add_library(${PLUGIN_NAME} SHARED plugin.cpp)
target_link_libraries(${PLUGIN_NAME} PUBLIC Evoplex::EvoplexCore)
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
