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

# CPack: general configuration
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "Marcos Cardinot <marcos@cardinot.net>")
set(CPACK_PACKAGE_DESCRIPTION "Evoplex is a multi-agent system for networks (graphs).")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${CPACK_PACKAGE_DESCRIPTION})
set(CPACK_PACKAGE_HOMEPAGE_URL "https://evoplex.github.io")
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/src/evoplex.bmp")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${EVOPLEX_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_OUTPUT_FILE_PREFIX releases)

if(UNIX)
  set(CPACK_GENERATOR "RPM;DEB")

  # CPack: Debian package generator
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "cmake (>= 3.1), qt5-default (>= 5.8), libqt5charts5 (>=5.8)")
  set(CPACK_DEBIAN_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION})
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER ${CPACK_PACKAGE_VENDOR})
  set(CPACK_DEBIAN_PACKAGE_SECTION "contrib/science")

  # CPack: RPM package generator
  set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION / /usr/local /usr/local/bin /usr/local/include /usr/local/lib)
  set(CPACK_RPM_PACKAGE_REQUIRES "cmake >= 3.1, qt5-qtbase-devel >= 5.8, qt5-qtbase-devel-gui >= 5.8, qt5-qtcharts >= 5.8")
  set(CPACK_RPM_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION})
  set(CPACK_RPM_PACKAGE_RELEASE ${EVOPLEX_RELEASE})
  set(CPACK_RPM_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}.rpm")
  set(CPACK_RPM_CHANGELOG_FILE "${CMAKE_SOURCE_DIR}/CHANGELOG.md")
  set(CPACK_RPM_PACKAGE_GROUP "Development/Tools")
  set(CPACK_RPM_PACKAGE_LICENSE "GPLv3")

elseif(APPLE)
  install(CODE "execute_process(COMMAND macdeployqt ${CMAKE_BINARY_DIR}/Evoplex.app -executable=${CMAKE_BINARY_DIR}/Evoplex.app/Contents/MacOS/Evoplex)")
  install(TARGETS evoplex DESTINATION ./)
  set(CPACK_GENERATOR "DragNDrop")
  set(CPACK_DMG_DS_STORE "${CMAKE_SOURCE_DIR}/installer/osx/ds_store")
  set(CPACK_DMG_VOLUME_NAME "Evoplex")
endif(APPLE)

# CPack call
include(CPack)
