SET(GRAPH_NAME mooregrid)

qt5_generate_moc(graph.h moc_graph.cpp)
FILE(COPY graphMetaData.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
ADD_LIBRARY(evoplex_graph_${GRAPH_NAME} SHARED graph.cpp graph.h moc_graph.cpp)
TARGET_LINK_LIBRARIES(evoplex_graph_${GRAPH_NAME} evoplex_core_lib Qt5::Core)
