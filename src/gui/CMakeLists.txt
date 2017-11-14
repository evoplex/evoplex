INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src)

set(EVOPLEX_GUI_H
  maingui.h
  attributeswidget.h
  #contextmenutable.h
  experimentwidget.h
  linechartwidget.h
  graphwidget.h
  projectwidget.h
  projectswindow.h
  queuewidget.h
  savedialog.h
  settingswidget.h
  tablewidget.h
  titlebar.h
  welcomewidget.h
)

set(EVOPLEX_GUI_CXX
  maingui.cpp
  attributeswidget.cpp
  #contextmenutable.cpp
  experimentwidget.cpp
  linechartwidget.cpp
  graphwidget.cpp
  projectwidget.cpp
  projectswindow.cpp
  queuewidget.cpp
  savedialog.cpp
  settingswidget.cpp
  tablewidget.cpp
  titlebar.cpp
  welcomewidget.cpp
)

qt5_wrap_ui(EVOPLEX_GUI_UI
  forms/attributeswidget.ui
  forms/graphwidget.ui
  forms/linechartsettings.ui
  forms/projectwidget.ui
  forms/queuewidget.ui
  forms/savedialog.ui
  forms/settingswidget.ui
  forms/titlebar.ui
  forms/welcomewidget.ui
)

add_library(EvoplexGUILib SHARED ${EVOPLEX_GUI_CXX} ${EVOPLEX_GUI_UI})

target_link_libraries(EvoplexGUILib EvoplexCore Qt5::Core Qt5::Widgets Qt5::Charts)
