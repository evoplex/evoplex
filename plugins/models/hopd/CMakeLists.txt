ADD_LIBRARY(evoplex_model_hopd SHARED
    model.cpp
    model.h
)
TARGET_LINK_LIBRARIES(evoplex_model_hopd evoplex_lib Qt5::Core)

