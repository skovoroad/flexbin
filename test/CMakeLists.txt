cmake_minimum_required (VERSION 3.1)

project (flexbin_test)

set (CMAKE_CXX_STANDARD 17)
set (PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../include)

add_subdirectory(googletest)
add_executable(flexbin_test 
  main.cpp 
  test_fundamentals.cpp
  test_substructure.cpp
)
add_dependencies(flexbin_test gtest)

include_directories(${CMAKE_SOURCE_DIR}/test/googletest/googletest/include)
target_link_libraries(flexbin_test ${CMAKE_BINARY_DIR}/libgtest.a)
target_link_libraries(flexbin_test -pthread)

enable_testing()
add_test(NAME    flexbin_test 
         COMMAND flexbin_test)
