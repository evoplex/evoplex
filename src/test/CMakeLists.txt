enable_testing()

set (TESTS tst_prg tst_value)

foreach(TEST ${TESTS})
  add_executable(${TEST} ${TEST}.cpp)
  target_link_libraries(${TEST} EvoplexCore Qt5::Test)
  target_include_directories(${TEST} PRIVATE ${CMAKE_SOURCE_DIR}/src)
  add_test(${TEST} ${TEST}.cpp)
endforeach()
