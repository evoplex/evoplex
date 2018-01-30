#=============================================================================
#  Evoplex <evoplex.github.io>
#  Copyright (C) 2016-2018 by Marcos Cardinot <mcardinot@gmail.com>
#=============================================================================

cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

get_filename_component(MODEL_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(MODEL_NAME EvoplexModel_${MODEL_NAME})
project(${MODEL_NAME})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(EvoplexCore 0.0.0 REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Concurrent REQUIRED)

if(NOT Evoplex_SRC)
  set(Evoplex_SRC ${CMAKE_INSTALL_PREFIX}/include/evoplex)
endif()
MESSAGE("Evoplex_SRC :" ${Evoplex_SRC})

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

add_library(${MODEL_NAME} SHARED model.cpp)
target_link_libraries(${MODEL_NAME} Evoplex::EvoplexCore Qt5::Core)
target_include_directories(${MODEL_NAME} PUBLIC ${Evoplex_SRC})
set_target_properties(${MODEL_NAME} PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/evoplex/plugins)

install(TARGETS ${MODEL_NAME} LIBRARY DESTINATION lib/evoplex/plugins)
