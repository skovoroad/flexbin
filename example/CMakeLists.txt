cmake_minimum_required (VERSION 3.1)

project (flexbin_example)
set (CMAKE_CXX_STANDARD 17)

set (PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../include)

add_executable(flexbin_example example.cpp)

