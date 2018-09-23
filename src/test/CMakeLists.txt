##########################################################################
#  Evoplex <https://evoplex.org>
#  Copyright (C) 2018 - Marcos Cardinot <marcos@cardinot.net>
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
