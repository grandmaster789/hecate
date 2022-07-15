cmake_minimum_required(VERSION 3.15)

find_package(Catch2 REQUIRED)
include(Catch)

add_executable(
  unittest

  "unittest.h"
  "unittest.cpp"
  "util/test_algorithm.cpp" 
)

include_directories(../src/hecate)

target_link_libraries(
  unittest PRIVATE
  HecateLib
  Catch2::Catch2WithMain
)

catch_discover_tests(unittest)