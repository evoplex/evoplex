##########################################################################
#  This file is part of Evoplex.
#
#  Evoplex is a multi-agent system for networks.
#  Copyright (C) 2017 - Marcos Cardinot <marcos@cardinot.net>
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

function(add_plugins ROOT_DIR PLUGINS)
  foreach(PLUGIN ${PLUGINS})
    set(PLUGIN_NAME plugin_${PLUGIN})
    set(ARCHIVE_DEST "${EVOPLEX_OUTPUT_ARCHIVE}plugins")
    set(LIBRARY_DEST "${EVOPLEX_OUTPUT_LIBRARY}plugins")
    add_library(${PLUGIN_NAME} SHARED ${ROOT_DIR}/${PLUGIN}/plugin.cpp)
    target_link_libraries(${PLUGIN_NAME} PRIVATE EvoplexCore Qt5::Core)
    set_target_properties(${PLUGIN_NAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${ARCHIVE_DEST}
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${ARCHIVE_DEST}
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${ARCHIVE_DEST}
        ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL ${ARCHIVE_DEST}
        ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${ARCHIVE_DEST}
        LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_DEST}
        LIBRARY_OUTPUT_DIRECTORY_DEBUG ${LIBRARY_DEST}
        LIBRARY_OUTPUT_DIRECTORY_RELEASE ${LIBRARY_DEST}
        LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL ${LIBRARY_DEST}
        LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${LIBRARY_DEST}
        RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_DEST}
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${LIBRARY_DEST}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${LIBRARY_DEST}
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${LIBRARY_DEST}
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${LIBRARY_DEST})

    # Qt Object macro is hidden in another macro; let's add them here
    set_property(TARGET ${PLUGIN_NAME} APPEND PROPERTY
        AUTOMOC_MACRO_NAMES "REGISTER_PLUGIN")

    install(TARGETS ${PLUGIN_NAME}
        LIBRARY DESTINATION "${EVOPLEX_INSTALL_LIBRARY}plugins"
        ARCHIVE DESTINATION "${EVOPLEX_INSTALL_ARCHIVE}plugins")
  endforeach()
endfunction(add_plugins)

set(GRAPHS
  cycle
  edgesFromCSV
  path
  squaregrid
  star
  zeroEdges
)
add_plugins(graphs "${GRAPHS}")

set(MODELS
  cellularAutomata1D
  gameOfLife
  nowak92
  populationGrowth
)
add_plugins(models "${MODELS}")
