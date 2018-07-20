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

set(TESTS_WITH_QRC
  tst_nodes
)

set(TESTS_WITHOUT_QRC
  tst_attributes
  tst_attributerange
  tst_attrsgenerator
  tst_edge
  tst_node
  tst_prg
  tst_value
)

function(add_utest TEST ADD_QRC)
  if(${ADD_QRC})
    add_executable(${TEST} ${TEST}.cpp data.qrc)
  else()
    add_executable(${TEST} ${TEST}.cpp)
  endif()
  target_link_libraries(${TEST} EvoplexCore Qt5::Test)
  target_include_directories(${TEST} PRIVATE ${CMAKE_SOURCE_DIR}/src)
  add_test(${TEST} ${TEST})
endfunction()

foreach(TEST ${TESTS_WITHOUT_QRC})
  add_utest("${TEST}" FALSE)
endforeach()

foreach(TEST "${TESTS_WITH_QRC}")
  add_utest("${TEST}" TRUE)
endforeach()
