set(EVOPLEX_CORE_INCLUDE
  include/plugininterfaces.h
  include/abstractgraph.h
  include/abstractmodel.h

  include/attributes.h
  include/agent.h
  include/edge.h
  include/constants.h
  include/prg.h
  include/utils.h
  include/value.h
  include/valuespace.h
)
set(EVOPLEX_CORE_H
  abstractplugin.h
  graphplugin.h
  modelplugin.h
  agentsgenerator.h
  output.h
  stats.h

  experiment.h
  experimentsmgr.h
  project.h
  logger.h
  mainapp.h
)
set(EVOPLEX_CORE_CXX
  abstractplugin.cpp
  graphplugin.cpp
  modelplugin.cpp
  prg.cpp

  agentsgenerator.cpp
  edge.cpp
  experiment.cpp
  experimentsmgr.cpp
  output.cpp
  project.cpp
  valuespace.cpp
  logger.cpp
  mainapp.cpp
)

add_library(EvoplexCore SHARED ${EVOPLEX_CORE_CXX})
target_link_libraries(EvoplexCore Qt5::Core Qt5::Concurrent)
set_target_properties(EvoplexCore PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/evoplex)
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
  LIBRARY DESTINATION lib/evoplex
  ARCHIVE DESTINATION lib/evoplex
  INCLUDES DESTINATION include/evoplex
)

install(
  FILES
    ${EVOPLEX_CORE_INCLUDE}
    "${CMAKE_CURRENT_BINARY_DIR}/evoplexcore_export.h"
  DESTINATION include/evoplex
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

set(ConfigPackageLocation lib/cmake/Evoplex)
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
