function(add_plugins ROOT_DIR PLUGINS)
  foreach(PLUGIN ${PLUGINS})
    set(PLUGIN_NAME plugin_${PLUGIN})
    add_library(${PLUGIN_NAME} SHARED ${ROOT_DIR}/${PLUGIN}/plugin.cpp)
    target_link_libraries(${PLUGIN_NAME} EvoplexCore Qt5::Core)
    set_target_properties(${PLUGIN_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/evoplex/plugins)
    install(TARGETS ${PLUGIN_NAME} LIBRARY DESTINATION lib/evoplex/plugins)
  endforeach()
endfunction(add_plugins)

set(GRAPHS
  customgraph
  squaregrid
)
add_plugins(graphs "${GRAPHS}")

set(MODELS
  nowak92
  test
)
add_plugins(models "${MODELS}")
