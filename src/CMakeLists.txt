add_subdirectory(core)
add_subdirectory(gui)
add_subdirectory(plugins) # built-in plugins (models and graph generators)
add_subdirectory(test)

#### EXECUTABLE ####
add_executable(evoplex main.cpp gui/data/data.qrc)
target_link_libraries(evoplex EvoplexCore EvoplexGUI)
set_target_properties(evoplex PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

install(TARGETS evoplex
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib/evoplex
  ARCHIVE DESTINATION lib/evoplex
  INCLUDES DESTINATION include/evoplex
)
