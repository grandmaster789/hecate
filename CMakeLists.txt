cmake_minimum_required(VERSION 3.14)

# append "cmake/modules" to the module path
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/modules")

# use cmake itself to install a local vcpkg environment
# https://github.com/bitmeal/vcpkg-cmake-integration
set(VCPKG_VERSION latest) # latest == latest release tag, edge == bleeding edge
include(cmake/vcpkg.cmake)

project(hecate)
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER) # provide a variable with the project name in lowercase

option(UNIT_TESTING "Build unit tests for ${PROJECT_NAME}" ON)

# provide 4 build types (most optimized is MinSizeRel, most debuggable is Debug)
set(CMAKE_BUILD_TYPE "MinSizeRel" CACHE STRING "Select build type")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

# provide centralized flags for all compilers
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# set compiler-specific flags where needed
if (WIN32)
  add_compile_definitions(UNICODE)
  add_compile_definitions(_UNICODE)
  add_compile_definitions(WIN32_LEAN_AND_MEAN)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
  add_compile_definitions(_USE_MATH_DEFINES)
  add_compile_definitions(NOMINMAX)

  add_compile_options("/MP")          # build with multiple processes
  add_compile_options("/W4")          # high warning level
  add_compile_options("/permissive-") # strict
  add_compile_options("/utf-8")       # set source code and character sets to UTF-8
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/hecate)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/app)

if (UNIT_TESTING)
  enable_testing()
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test/${PROJECT_NAME})
endif()