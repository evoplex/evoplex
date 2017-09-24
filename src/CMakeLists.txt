add_subdirectory(core)

# built-in plugins (models and graph generators)
add_subdirectory(plugins)

#### GUI ####
set(EVOPLEX_GUI_SRC
  gui/maingui.cpp
  gui/maingui.h
  gui/attributeswidget.cpp
  gui/attributeswidget.h
  gui/experimentwidget.cpp
  gui/experimentwidget.h
  gui/graphwidget.cpp
  gui/graphwidget.h
  gui/projectwidget.cpp
  gui/projectwidget.h
  gui/projectswindow.cpp
  gui/projectswindow.h
  gui/queuewidget.cpp
  gui/queuewidget.h
  gui/tablewidget.cpp
  gui/tablewidget.h
  gui/settingswidget.cpp
  gui/settingswidget.h
  gui/welcomewidget.cpp
  gui/welcomewidget.h
  #gui/contextmenutable.cpp
  #gui/contextmenutable.h
)
qt5_wrap_ui(EVOPLEX_GUI_UI
  gui/forms/attributeswidget.ui
  gui/forms/queuewidget.ui
  gui/forms/projectwidget.ui
  gui/forms/settingswidget.ui
  gui/forms/welcomewidget.ui
)
ADD_LIBRARY(EvoplexGUILib SHARED ${EVOPLEX_GUI_SRC} ${EVOPLEX_GUI_UI})
TARGET_LINK_LIBRARIES(EvoplexGUILib EvoplexCore Qt5::Core Qt5::Widgets)

#### EXECUTABLE ####
ADD_EXECUTABLE(evoplex main.cpp gui/icons/icons.qrc)
TARGET_LINK_LIBRARIES(evoplex EvoplexCore EvoplexGUILib)
