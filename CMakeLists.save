cmake_minimum_required (VERSION 3.1)

project (fbstream)
set (CMAKE_CXX_STANDARD 17)

##################################
# Download and install GoogleTest

include(ExternalProject)
ExternalProject_Add(gtest
#URL https://googletest.googlecode.com/files/gtest-1.7.0.zip
URL https://github.com/google/googletest/archive/release-1.8.0.zip
# Comment above line, and uncomment line below to use subversion.
# SVN_REPOSITORY http://googletest.googlecode.com/svn/trunk/ 
# Uncomment line below to freeze a revision (here the one for 1.7.0)
# SVN_REVISION -r700

PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
INSTALL_COMMAND ""
)

ExternalProject_Get_Property(gtest source_dir binary_dir)

message(${source_dir})
message(${binary_dir})

################
# Define a test
add_executable(fbstream_test fbstream_test.cpp)

######################################
# Configure the test to use GoogleTest
#
# If used often, could be made a macro.

add_dependencies(fbstream_test gtest)
include_directories(${source_dir}/googletest/include)
target_link_libraries(fbstream_test ${binary_dir}/googlemock/gtest/libgtest.a)
target_link_libraries(fbstream_test ${binary_dir}/googlemock/gtest/libgtest_main.a)
target_link_libraries(fbstream_test -pthread)

##################################
# Just make the test runnable with
#   $ make test

enable_testing()
add_test(NAME    fbstream_test 
         COMMAND fbstream_test)