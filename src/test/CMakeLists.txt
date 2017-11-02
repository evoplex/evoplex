enable_testing()

set(tests
  evoplex.cpp
)

add_executable(evoplex_test EXCLUDE_FROM_ALL ${tests})
target_link_libraries(evoplex_test Qt5::Test)
add_test(evoplex_test ${tests})
