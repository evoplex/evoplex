set(EVOPLEX_CORE_H
  constants.h
  enums.h
  output.h
  plugininterfaces.h
  prg.h
  stats.h
  utils.h

  abstractgraph.h
  abstractmodel.h
  abstractplugin.h
  graphplugin.h
  modelplugin.h
  attributes.h
  agent.h
  edge.h
  value.h
  valuespace.h

  filemgr.h
  experiment.h
  experimentsmgr.h
  project.h
  mainapp.h
)
set(EVOPLEX_CORE_CXX
  abstractplugin.cpp
  graphplugin.cpp
  modelplugin.cpp

  edge.cpp
  filemgr.cpp
  experiment.cpp
  experimentsmgr.cpp
  output.cpp
  project.cpp
  valuespace.cpp
  mainapp.cpp
)

add_library(EvoplexCore SHARED ${EVOPLEX_CORE_CXX})
target_link_libraries(EvoplexCore Qt5::Core Qt5::Concurrent)
set_target_properties(EvoplexCore PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/evoplex)

install(
  TARGETS EvoplexCore
  EXPORT EvoplexCoreTargets
  LIBRARY DESTINATION lib/evoplex
  ARCHIVE DESTINATION lib/evoplex
  INCLUDES DESTINATION include/evoplex/core
)

install(
  FILES
    ${EVOPLEX_CORE_H}
    "${CMAKE_CURRENT_BINARY_DIR}/evoplexcore_export.h"
  DESTINATION include/evoplex/core
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

set(ConfigPackageLocation share/evoplex/cmake)
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
