cmake_minimum_required (VERSION 3.1)

project (flexbin)
set (CMAKE_CXX_STANDARD 17)

#if (POLICY CMP0074)
#  cmake_policy(SET CMP0074 NEW)
#endif()
#set(GTEST_LANG_CXX11, 1)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/.build)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

include_directories("${PROJECT_INCLUDE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}")

add_subdirectory(example)

if(DEFINED FLEXBIN_ENABLE_TEST)
  add_subdirectory(test)
endif()
