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

add_subdirectory(core)
add_subdirectory(gui)
add_subdirectory(plugins) # built-in plugins (models and graph generators)

if(TESTS)
  add_subdirectory(test)
  #  coveralls_setup("${EVOPLEX_CORE_CXX} ${EVOPLEX_GUI_CXX}" ON "${PROJECT_SOURCE_DIR}/src/cmake")
endif()

#### EXECUTABLE ####
add_executable(evoplex WIN32 main.cpp evoplex.rc gui/res/guiRes.qrc)
target_link_libraries(evoplex PUBLIC EvoplexCore EvoplexGUI)
set(RUNTIME_DEST "${CMAKE_BINARY_DIR}/bin")
set_target_properties(evoplex PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${RUNTIME_DEST}
  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${RUNTIME_DEST}
  RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${RUNTIME_DEST}
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${RUNTIME_DEST}
  RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${RUNTIME_DEST})

install(TARGETS evoplex
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib/evoplex
  ARCHIVE DESTINATION lib/evoplex
  INCLUDES DESTINATION include/evoplex
)
