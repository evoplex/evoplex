ADD_LIBRARY(evoplex_graph_squaregrid SHARED
    squaregrid.cpp
    squaregrid.h
)
TARGET_LINK_LIBRARIES(evoplex_graph_squaregrid evoplex_lib Qt5::Core)

