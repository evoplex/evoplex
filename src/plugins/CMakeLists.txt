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
    set(DESTINATION "${CMAKE_BINARY_DIR}/lib/evoplex/plugins")
    add_library(${PLUGIN_NAME} SHARED ${ROOT_DIR}/${PLUGIN}/plugin.cpp)
    target_link_libraries(${PLUGIN_NAME} EvoplexCore Qt5::Core)
    set_target_properties(${PLUGIN_NAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${DESTINATION}
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${DESTINATION}
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${DESTINATION}
        ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL ${DESTINATION}
        ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${DESTINATION}
        LIBRARY_OUTPUT_DIRECTORY ${DESTINATION}
        LIBRARY_OUTPUT_DIRECTORY_DEBUG ${DESTINATION}
        LIBRARY_OUTPUT_DIRECTORY_RELEASE ${DESTINATION}
        LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL ${DESTINATION}
        LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${DESTINATION}
        RUNTIME_OUTPUT_DIRECTORY ${DESTINATION}
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${DESTINATION}
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${DESTINATION}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${DESTINATION}
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${DESTINATION})
    install(TARGETS ${PLUGIN_NAME}
        ARCHIVE DESTINATION lib/evoplex/plugins
        LIBRARY DESTINATION lib/evoplex/plugins
        RUNTIME DESTINATION lib/evoplex/plugins)
  endforeach()
endfunction(add_plugins)

set(GRAPHS
  customgraph
  squaregrid
)
add_plugins(graphs "${GRAPHS}")

set(MODELS
  nowak92
  test
)
add_plugins(models "${MODELS}")
