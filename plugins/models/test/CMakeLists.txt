SET(MODEL_NAME test)

qt5_generate_moc(model.h moc_model.cpp)
FILE(COPY modelMetaData.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
ADD_LIBRARY(evoplex_model_${MODEL_NAME} SHARED model.cpp model.h moc_model.cpp)
TARGET_LINK_LIBRARIES(evoplex_model_${MODEL_NAME} evoplex_core_lib Qt5::Core)
