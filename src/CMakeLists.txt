add_subdirectory(core)
add_subdirectory(gui)
add_subdirectory(plugins) # built-in plugins (models and graph generators)
add_subdirectory(test)

#### EXECUTABLE ####
ADD_EXECUTABLE(evoplex main.cpp gui/icons/icons.qrc)
TARGET_LINK_LIBRARIES(evoplex EvoplexCore EvoplexGUILib)
