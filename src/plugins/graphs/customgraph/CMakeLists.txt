#=============================================================================
#  Evoplex
#  Copyright (C) 2016-2017 by Marcos Cardinot <mcardinot@gmail.com>
#=============================================================================

get_filename_component(GRAPH_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(GRAPH_NAME EvoplexGraph_${GRAPH_NAME})

add_library(${GRAPH_NAME} SHARED graph.cpp)
target_link_libraries(${GRAPH_NAME} EvoplexCore Qt5::Core)
set_target_properties(${GRAPH_NAME} PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/evoplex/plugins)

install(TARGETS ${GRAPH_NAME} LIBRARY DESTINATION lib/evoplex/plugins)
