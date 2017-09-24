set(EVOPLEX_CORE_H
  constants.h
  prg.h
  utils.h

  abstractagent.h
  abstractgraph.h
  abstractmodel.h
  attributes.h
  edge.h

  filemgr.h
  experiment.h
  experimentsmgr.h
  project.h
  mainapp.h
)
set(EVOPLEX_CORE_CXX
  filemgr.cpp
  experiment.cpp
  experimentsmgr.cpp
  project.cpp
  mainapp.cpp
)

add_library(EvoplexCore SHARED ${EVOPLEX_CORE_CXX} ${EVOPLEX_CORE_H})
target_link_libraries(EvoplexCore Qt5::Core Qt5::Concurrent Qt5::Widgets)

install(FILES ${EVOPLEX_CORE_H} DESTINATION include/evoplex/core)

install(TARGETS EvoplexCore EXPORT EvoplexCoreTargets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib/evoplex/core
  ARCHIVE DESTINATION lib/evoplex/core
  INCLUDES DESTINATION include/evoplex
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

set(ConfigPackageLocation lib/cmake/EvoplexCore)
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
