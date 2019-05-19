cmake_minimum_required(VERSION 2.6)
project(flexbin_test)

set (CMAKE_CXX_STANDARD 17)

################################
# GTest
################################

enable_testing()

link_directories(/usr/src/gtest)

################################
# Unit Tests
################################
# Add test cpp file
add_executable( runUnitTests fbstream_test.cpp )
# Link test executable against gtest & gtest_main
target_link_libraries(runUnitTests gtest gtest_main -pthread)
add_test( runUnitTests runUnitTests )
