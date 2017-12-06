enable_testing(true)

set (TEST tst_value)
add_executable(${TEST} ${TEST}.cpp)
target_link_libraries(${TEST} EvoplexCore Qt5::Test)
target_include_directories(${TEST} PUBLIC ${CMAKE_SOURCE_DIR}/src)
add_test(${TEST} ${TEST}.cpp)
