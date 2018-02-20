set(EVOPLEX_GUI_H
  maingui.h
  agentsgeneratordlg.h
  colormap.h
  #contextmenutable.h
  experimentdesigner.h
  experimentwidget.h
  linechart.h
  outputwidget.h
  graphview.h
  gridview.h
  graphsettings.h
  graphwidget.h
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
  experimentdesigner.cpp
  colormap.cpp
  #contextmenutable.cpp
  experimentwidget.cpp
  linechart.cpp
  outputwidget.cpp
  graphview.cpp
  gridview.cpp
  graphsettings.cpp
  graphwidget.cpp
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

set(EVOPLEX_GUI_UIFORM
  agentsgeneratordlg
  experimentdesigner
  graphwidget
  graphsettings
  linechartsettings
  outputwidget
  pluginspage
  projectwidget
  queuepage
  savedialog
  settingspage
  titlebar
  welcomepage
)
foreach(uiform ${EVOPLEX_GUI_UIFORM})
  set(EVOPLEX_GUI_UI ${EVOPLEX_GUI_UI} forms/${uiform}.ui)
endforeach(uiform)
qt5_wrap_ui(EVOPLEX_GUI_UI ${EVOPLEX_GUI_UI})

add_library(EvoplexGUI SHARED ${EVOPLEX_GUI_CXX} ${EVOPLEX_GUI_UI})
target_link_libraries(EvoplexGUI EvoplexCore Qt5::Core Qt5::Concurrent Qt5::Widgets Qt5::Charts)
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
