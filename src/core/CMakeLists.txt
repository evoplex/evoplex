##########################################################################
# Evoplex <https://evoplex.org>
# Copyright (C) 2016-present - Marcos Cardinot <marcos@cardinot.net>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
##########################################################################

set(EVOPLEX_CORE_INCLUDE
  include/plugininterface.h
  include/abstractplugin.h
  include/abstractgraph.h
  include/abstractmodel.h

  include/attributes.h
  include/attributerange.h
  include/attrsgenerator.h
  include/node.h
  include/nodes.h
  include/edge.h
  include/edges.h
  include/constants.h
  include/prg.h
  include/utils.h
  include/value.h
  include/stats.h
  include/enum.h
)
set(EVOPLEX_CORE_H
  graphplugin.h
  modelplugin.h
  output.h
  plugin.h

  trial.h
  edge_p.h
  experiment.h
  expinputs.h
  experimentsmgr.h
  node_p.h
  nodes_p.h
  project.h
  logger.h
  mainapp.h
)
set(EVOPLEX_CORE_CXX
  plugin.cpp
  abstractplugin.cpp
  abstractgraph.cpp
  abstractmodel.cpp
  graphplugin.cpp
  modelplugin.cpp
  node.cpp
  nodes_p.cpp
  prg.cpp

  attributerange.cpp
  attrsgenerator.cpp
  trial.cpp
  edge_p.cpp
  experiment.cpp
  expinputs.cpp
  experimentsmgr.cpp
  node_p.cpp
  output.cpp
  project.cpp
  value.cpp
  logger.cpp
  mainapp.cpp
)

add_library(EvoplexCore STATIC ${EVOPLEX_CORE_CXX})
target_link_libraries(EvoplexCore PUBLIC Qt5::Core PRIVATE Qt5::Concurrent Qt5::Network)

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
if(CODE_COVERAGE)
  foreach(src ${EVOPLEX_CORE_CXX} ${EVOPLEX_CORE_H})
    set(EVOPLEX_CORE_SRC ${EVOPLEX_CORE_SRC} ${PROJECT_SOURCE_DIR}/src/core/${src})
  endforeach(src)
  coveralls_setup("${EVOPLEX_CORE_SRC}" ON "${PROJECT_SOURCE_DIR}/src/cmake")
endif()
