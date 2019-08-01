cmake_minimum_required (VERSION 3.1)

project (flexbin_test)

set (CMAKE_CXX_STANDARD 17)
set (PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../include ${PROJECT_SOURCE_DIR})

add_subdirectory(googletest)
add_executable(flexbin_test
  main.cpp
  test_fundamentals.cpp
  test_substructure.cpp
  test_containers.cpp
  test_ptr.cpp
  test_strings.cpp
)
add_dependencies(flexbin_test gtest)

include_directories(${CMAKE_SOURCE_DIR}/test/googletest/googletest/include)

if (MSVC)
  set(CMAKE_CXX_FLAGS "/D_HAS_TR1_NAMESPACE /D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING" CACHE INTERNAL "")
endif()

if (WIN32)
  target_link_libraries(flexbin_test ${CMAKE_BINARY_DIR}/Debug/gtest.lib)
#  target_link_libraries(flexbin_test ${GTEST_LIB})
else()
  target_link_libraries(flexbin_test ${CMAKE_BINARY_DIR}/libgtest.a)
  target_link_libraries(flexbin_test -pthread)
endif()

enable_testing()
add_test(NAME    flexbin_test
         COMMAND flexbin_test)

set_property(TARGET flexbin_test PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")