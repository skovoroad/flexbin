cmake_minimum_required(VERSION 2.6.2)

project( googletest-distribution )

enable_testing()

option(BUILD_GTEST "Builds the googletest subproject" OFF)

#Note that googlemock target already builds googletest
option(BUILD_GMOCK "Builds the googlemock subproject" ON)

if (MSVC)
  set(CMAKE_CXX_FLAGS "/D_HAS_TR1_NAMESPACE /D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING" CACHE INTERNAL "")
endif()

if(BUILD_GMOCK)
  add_subdirectory( googlemock )
elseif(BUILD_GTEST)
  add_subdirectory( googletest )
endif()
