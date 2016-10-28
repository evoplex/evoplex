ADD_LIBRARY(evoplex_models SHARED
    hopd/model.cpp
    hopd/model.h
)
TARGET_LINK_LIBRARIES(evoplex_models evoplex_lib Qt5::Core)

