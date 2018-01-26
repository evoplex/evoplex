set(EVOPLEX_GUI_H
  maingui.h
  agentsgeneratordlg.h
  attributes.h
  colormap.h
  #contextmenutable.h
  experimentwidget.h
  linechart.h
  outputwidget.h
  graphwidget.h
  gridwidget.h
  projectwidget.h
  savedialog.h
  tablewidget.h
  titlebar.h

  pluginspage.h
  projectspage.h
  queuepage.h
  settingspage.h
  welcomepage.h
)

set(EVOPLEX_GUI_CXX
  maingui.cpp
  agentsgeneratordlg.cpp
  attributeswidget.cpp
  colormap.cpp
  #contextmenutable.cpp
  experimentwidget.cpp
  linechart.cpp
  outputwidget.cpp
  graphwidget.cpp
  gridwidget.cpp
  projectwidget.cpp
  savedialog.cpp
  tablewidget.cpp
  titlebar.cpp

  pluginspage.cpp
  projectspage.cpp
  queuepage.cpp
  settingspage.cpp
  welcomepage.cpp
)

qt5_wrap_ui(EVOPLEX_GUI_UI
  forms/agentsgeneratordlg.ui
  forms/attributeswidget.ui
  forms/graphwidget.ui
  forms/graphsettings.ui
  forms/linechartsettings.ui
  forms/outputwidget.ui
  forms/pluginspage.ui
  forms/projectwidget.ui
  forms/queuepage.ui
  forms/savedialog.ui
  forms/settingspage.ui
  forms/titlebar.ui
  forms/welcomepage.ui
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
