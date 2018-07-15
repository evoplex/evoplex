##########################################################################
#  This file is part of Evoplex.
#
#  Evoplex is a multi-agent system for networks.
#  Copyright (C) 2018 - Marcos Cardinot <marcos@cardinot.net>
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
##########################################################################

set(EVOPLEX_GUI_H
  maingui.h
  nodesgeneratordlg.h
  colormap.h
  #contextmenutable.h
  experimentdesigner.h
  experimentwidget.h
  fontstyles.h
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
  nodesgeneratordlg.cpp
  experimentdesigner.cpp
  colormap.cpp
  #contextmenutable.cpp
  experimentwidget.cpp
  fontstyles.cpp
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
  nodesgeneratordlg
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

add_library(EvoplexGUI STATIC ${EVOPLEX_GUI_CXX} ${EVOPLEX_GUI_UI})
target_link_libraries(EvoplexGUI EvoplexCore Qt5::Core Qt5::Concurrent Qt5::Widgets Qt5::Charts)
set_target_properties(EvoplexGUI PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY ${EVOPLEX_OUTPUT_LIBRARY})
target_include_directories(EvoplexGUI PRIVATE ${CMAKE_SOURCE_DIR}/src)

install(
  TARGETS EvoplexGUI
  LIBRARY DESTINATION "${EVOPLEX_INSTALL_LIBRARY}"
  ARCHIVE DESTINATION "${EVOPLEX_INSTALL_ARCHIVE}"
  INCLUDES DESTINATION "${EVOPLEX_INSTALL_INCLUDES}"
)
