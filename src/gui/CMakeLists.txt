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

###############################################################################
# EXTERNAL
###############################################################################
set(_EXTERNAL_QT_MATERIAL_WIDGETS
    qtmaterialappbar.cpp
    qtmaterialautocomplete.cpp
    qtmaterialautocomplete_internal.cpp
    qtmaterialavatar.cpp
    qtmaterialbadge.cpp
    qtmaterialcheckbox.cpp
    qtmaterialcircularprogress.cpp
    qtmaterialcircularprogress_internal.cpp
    qtmaterialdialog.cpp
    qtmaterialdialog_internal.cpp
    qtmaterialdrawer.cpp
    qtmaterialdrawer_internal.cpp
    qtmaterialfab.cpp
    qtmaterialflatbutton.cpp
    qtmaterialflatbutton_internal.cpp
    qtmaterialiconbutton.cpp
#    qtmateriallist.cpp
#    qtmateriallistitem.cpp
#    qtmaterialmenu.cpp
#    qtmaterialmenu_internal.cpp
#    qtmaterialpaper.cpp
    qtmaterialprogress.cpp
    qtmaterialprogress_internal.cpp
    qtmaterialradiobutton.cpp
    qtmaterialraisedbutton.cpp
    qtmaterialscrollbar.cpp
    qtmaterialscrollbar_internal.cpp
    qtmaterialslider.cpp
    qtmaterialslider_internal.cpp
    qtmaterialsnackbar.cpp
    qtmaterialsnackbar_internal.cpp
#    qtmaterialtable.cpp
    qtmaterialtabs.cpp
    qtmaterialtabs_internal.cpp
    qtmaterialtextfield.cpp
    qtmaterialtextfield_internal.cpp
    qtmaterialtoggle.cpp
    qtmaterialtoggle_internal.cpp

    lib/qtmaterialcheckable.cpp
    lib/qtmaterialcheckable_internal.cpp
    lib/qtmaterialoverlaywidget.cpp
    lib/qtmaterialripple.cpp
    lib/qtmaterialrippleoverlay.cpp
    lib/qtmaterialstatetransition.cpp
    lib/qtmaterialstyle.cpp
    lib/qtmaterialtheme.cpp
)
foreach(cpp ${_EXTERNAL_QT_MATERIAL_WIDGETS})
  set(EXTERNAL_QT_MATERIAL_WIDGETS ${EXTERNAL_QT_MATERIAL_WIDGETS}
      external/qt-material-widgets/${cpp})
endforeach(cpp)

###############################################################################
# EVOPLEX GUI
###############################################################################
set(EVOPLEX_GUI_H
  maingui.h
  attrcolorselector.h
  attrwidget.h
  attrsgendlg.h
  colormap.h
  #contextmenutable.h
  experimentdesigner.h
  experimentwidget.h
  fontstyles.h
  #linechart.h
  linebutton.h
  outputwidget.h
  basegraphgl.h
  graphview.h
  gridview.h
  graphwidget.h
  graphsettings.h
  gridsettings.h
  projectwidget.h
  savedialog.h
  tablewidget.h
  graphtitlebar.h

  consolewidget.h
  pluginspage.h
  projectspage.h
  #queuepage.h
  settingspage.h
  titlebar.h
  welcomepage.h
)

set(EVOPLEX_GUI_CXX
  maingui.cpp
  attrcolorselector.cpp
  attrwidget.cpp
  attrsgendlg.cpp
  experimentdesigner.cpp
  colormap.cpp
  #contextmenutable.cpp
  experimentwidget.cpp
  fontstyles.cpp
  #linechart.cpp
  linebutton.cpp
  outputwidget.cpp
  basegraphgl.cpp
  graphview.cpp
  gridview.cpp
  graphwidget.cpp
  graphsettings.cpp
  gridsettings.cpp
  projectwidget.cpp
  savedialog.cpp
  tablewidget.cpp
  graphtitlebar.cpp

  consolewidget.cpp
  pluginspage.cpp
  projectspage.cpp
  #queuepage.cpp
  settingspage.cpp
  titlebar.cpp
  welcomepage.cpp
)

set(EVOPLEX_GUI_UIFORM
  attrcolorselector
  attrsgendlg
  consoletitlebar
  experimentdesigner
  basegraphgl
  graphsettings
  gridsettings
  graphtitlebar
  #linechartsettings
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

add_library(EvoplexGUI STATIC ${EVOPLEX_GUI_CXX} ${EVOPLEX_GUI_UI} ${EXTERNAL_QT_MATERIAL_WIDGETS})
target_link_libraries(EvoplexGUI PRIVATE EvoplexCore Qt5::Core Qt5::Concurrent Qt5::Network Qt5::Widgets Qt5::Charts Qt5::Svg)

set_target_properties(EvoplexGUI PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${EVOPLEX_OUTPUT_ARCHIVE}
  ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${EVOPLEX_OUTPUT_ARCHIVE}
  ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL ${EVOPLEX_OUTPUT_ARCHIVE}
  ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${EVOPLEX_OUTPUT_ARCHIVE}
  ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${EVOPLEX_OUTPUT_ARCHIVE}
  LIBRARY_OUTPUT_DIRECTORY ${EVOPLEX_OUTPUT_LIBRARY}
  LIBRARY_OUTPUT_DIRECTORY_DEBUG ${EVOPLEX_OUTPUT_LIBRARY}
  LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL ${EVOPLEX_OUTPUT_LIBRARY}
  LIBRARY_OUTPUT_DIRECTORY_RELEASE ${EVOPLEX_OUTPUT_LIBRARY}
  LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${EVOPLEX_OUTPUT_LIBRARY})

target_include_directories(EvoplexGUI PRIVATE ${CMAKE_SOURCE_DIR}/src)

install(
  TARGETS EvoplexGUI
  LIBRARY DESTINATION "${EVOPLEX_INSTALL_LIBRARY}"
  ARCHIVE DESTINATION "${EVOPLEX_INSTALL_ARCHIVE}"
  INCLUDES DESTINATION "${EVOPLEX_INSTALL_INCLUDES}"
)
