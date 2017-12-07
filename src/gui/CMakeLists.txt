set(EVOPLEX_GUI_H
  maingui.h
  attributeswidget.h
  colormap.h
  #contextmenutable.h
  experimentwidget.h
  linechartwidget.h
  outputwidget.h
  graphwidget.h
  gridwidget.h
  pluginswidget.h
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
  colormap.cpp
  #contextmenutable.cpp
  experimentwidget.cpp
  linechartwidget.cpp
  outputwidget.cpp
  graphwidget.cpp
  gridwidget.cpp
  pluginswidget.cpp
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
  forms/graphsettings.ui
  forms/linechartsettings.ui
  forms/outputwidget.ui
  forms/pluginswidget.ui
  forms/projectwidget.ui
  forms/queuewidget.ui
  forms/savedialog.ui
  forms/settingswidget.ui
  forms/titlebar.ui
  forms/welcomewidget.ui
)

add_library(EvoplexGUI SHARED ${EVOPLEX_GUI_CXX} ${EVOPLEX_GUI_UI})
target_link_libraries(EvoplexGUI EvoplexCore Qt5::Core Qt5::Widgets Qt5::Charts)
set_target_properties(EvoplexGUI PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/evoplex)

install(TARGETS EvoplexGUI
  LIBRARY DESTINATION lib/evoplex
  ARCHIVE DESTINATION lib/evoplex
  INCLUDES DESTINATION include/evoplex/gui
)

target_include_directories(EvoplexGUI PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:include/evoplex>
)
