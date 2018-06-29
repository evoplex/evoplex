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
endif()

#### EXECUTABLE ####
set(EXE_SRC main.cpp evoplex.rc gui/res/guiRes.qrc)
set(RUNTIME_DEST "${CMAKE_BINARY_DIR}/bin")
if (APPLE)
  set(APP_ICON "${CMAKE_CURRENT_SOURCE_DIR}/evoplex.icns")
  set_source_files_properties(${APP_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

  add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${EXE_SRC} ${APP_ICON})
  set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}
    INSTALL_RPATH "@executable_path/../Frameworks"
    RUNTIME_OUTPUT_DIRECTORY ${RUNTIME_DEST}
    MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
    MACOSX_BUNDLE_COPYRIGHT "Copyright (c) 2018 Marcos Cardinot et al."
    MACOSX_BUNDLE_GUI_IDENTIFIER "org.evoplex.evoplex"
    MACOSX_BUNDLE_INFO_STRING "A multi-agent system for networks (graphs)."
    MACOSX_BUNDLE_ICON_FILE "evoplex.icns"
    MACOSX_BUNDLE_LONG_VERSION_STRING "${EVOPLEX_VERSION}-${EVOPLEX_RELEASE}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
    MACOSX_BUNDLE_BUNDLE_VERSION "${EVOPLEX_VERSION}-${EVOPLEX_RELEASE}"
  )
else()
  add_executable(${PROJECT_NAME} WIN32 ${EXE_SRC})
  set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${RUNTIME_DEST}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${RUNTIME_DEST}
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${RUNTIME_DEST}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${RUNTIME_DEST}
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${RUNTIME_DEST}
  )
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC EvoplexCore EvoplexGUI)

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION "${EVOPLEX_INSTALL_PREFIX}bin"
  BUNDLE DESTINATION .
)
