##########################################################################
#  This file is part of Evoplex.
#
#  Evoplex is a multi-agent system for networks.
#  Copyright (C) 2017 - Marcos Cardinot <marcos@cardinot.net>
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

set(EVOPLEX_CORE_INCLUDE
  include/plugininterfaces.h
  include/abstractgraph.h
  include/abstractmodel.h
  include/basegraph.h
  include/baseplugin.h

  include/attributes.h
  include/attributerange.h
  include/node.h
  include/nodes.h
  include/edge.h
  include/edges.h
  include/constants.h
  include/prg.h
  include/utils.h
  include/value.h
  include/stats.h
)
set(EVOPLEX_CORE_H
  abstractplugin.h
  graphplugin.h
  modelplugin.h
  attrsgenerator.h
  output.h

  experiment.h
  expinputs.h
  experimentsmgr.h
  project.h
  logger.h
  mainapp.h
)
set(EVOPLEX_CORE_CXX
  abstractplugin.cpp
  basegraph.cpp
  graphplugin.cpp
  modelplugin.cpp
  nodes.cpp
  prg.cpp

  attributerange.cpp
  attrsgenerator.cpp
  experiment.cpp
  expinputs.cpp
  experimentsmgr.cpp
  output.cpp
  project.cpp
  value.cpp
  logger.cpp
  mainapp.cpp
)

add_library(EvoplexCore STATIC ${EVOPLEX_CORE_CXX})
target_link_libraries(EvoplexCore Qt5::Core Qt5::Concurrent)

set_target_properties(EvoplexCore PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${EVOPLEX_OUTPUT_ARCHIVE}
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${EVOPLEX_OUTPUT_ARCHIVE}
    ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL ${EVOPLEX_OUTPUT_ARCHIVE}
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${EVOPLEX_OUTPUT_ARCHIVE}
    ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${EVOPLEX_OUTPUT_ARCHIVE}
    LIBRARY_OUTPUT_DIRECTORY ${EVOPLEX_OUTPUT_LIBRARY}
    LIBRARY_OUTPUT_DIRECTORY_DEBUG ${EVOPLEX_OUTPUT_LIBRARY}
    LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL ${EVOPLEX_OUTPUT_LIBRARY}
    LIBRARY_OUTPUT_DIRECTORY_RELEASE ${EVOPLEX_OUTPUT_LIBRARY}
    LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${EVOPLEX_OUTPUT_LIBRARY})

target_include_directories(EvoplexCore
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/core/include>
    $<INSTALL_INTERFACE:include/evoplex>
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/core>
)

install(
  TARGETS EvoplexCore
  EXPORT EvoplexCoreTargets
  LIBRARY DESTINATION "${EVOPLEX_INSTALL_LIBRARY}"
  ARCHIVE DESTINATION "${EVOPLEX_INSTALL_ARCHIVE}"
  INCLUDES DESTINATION "${EVOPLEX_INSTALL_INCLUDES}"
)

install(
  FILES
    ${EVOPLEX_CORE_INCLUDE}
    "${CMAKE_CURRENT_BINARY_DIR}/evoplexcore_export.h"
  DESTINATION "${EVOPLEX_INSTALL_INCLUDES}"
  COMPONENT Devel
)

include(GenerateExportHeader)
generate_export_header(EvoplexCore)
set_property(TARGET EvoplexCore PROPERTY VERSION ${EVOPLEX_VERSION})

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/EvoplexCore/EvoplexCoreConfigVersion.cmake"
  VERSION ${EVOPLEX_VERSION}
  COMPATIBILITY AnyNewerVersion
)

export(EXPORT EvoplexCoreTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/EvoplexCore/EvoplexCoreTargets.cmake"
  NAMESPACE Evoplex::
)
configure_file(cmake/EvoplexCoreConfig.cmake
  "${CMAKE_CURRENT_BINARY_DIR}/EvoplexCore/EvoplexCoreConfig.cmake"
  COPYONLY
)

set(ConfigPackageLocation "${EVOPLEX_INSTALL_PREFIX}lib/cmake/Evoplex")
install(EXPORT EvoplexCoreTargets
  FILE EvoplexCoreTargets.cmake
  NAMESPACE Evoplex::
  DESTINATION ${ConfigPackageLocation}
)
install(
  FILES
    cmake/EvoplexCoreConfig.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/EvoplexCore/EvoplexCoreConfigVersion.cmake"
  DESTINATION ${ConfigPackageLocation}
  COMPONENT Devel
)

# add sources to coveralls
if(TESTS AND CMAKE_COMPILER_IS_GNUCXX)
  foreach(src ${EVOPLEX_CORE_CXX} ${EVOPLEX_CORE_H})
    set(EVOPLEX_CORE_SRC ${EVOPLEX_CORE_SRC} ${PROJECT_SOURCE_DIR}/src/core/${src})
  endforeach(src)
  coveralls_setup("${EVOPLEX_CORE_SRC}" ON "${PROJECT_SOURCE_DIR}/src/cmake")
endif()
