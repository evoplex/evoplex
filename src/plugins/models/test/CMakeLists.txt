#=============================================================================
#  Evoplex
#  Copyright (C) 2016-2017 by Marcos Cardinot <mcardinot@gmail.com>
#=============================================================================

get_filename_component(MODEL_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(MODEL_NAME EvoplexModel_${MODEL_NAME})
qt5_generate_moc(model.h moc_model.cpp)
file(COPY modelMetaData.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_library(${MODEL_NAME} SHARED model.cpp model.h moc_model.cpp)
target_link_libraries(${MODEL_NAME} EvoplexCore Qt5::Core)
set_target_properties(${MODEL_NAME} PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/evoplex/plugins)
