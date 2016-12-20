ADD_LIBRARY(evoplex_graph_mooregrid SHARED
    mooregrid.cpp
    mooregrid.h
)
TARGET_LINK_LIBRARIES(evoplex_graph_mooregrid evoplex_lib Qt5::Core)

