cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

# use cmake to install vcpkg dependency management
# https://github.com/bitmeal/vcpkg-cmake-integration
set(VCPKG_VERSION latest) # reasonable options: 'latest', 'edge', '<commit-hash>/<tag>'
include(cmake/vcpkg.cmake)

project(
    hecate
    VERSION 0.1.0
    DESCRIPTION "Vulkan compute testbed"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(
    hecate_lib OBJECT
    source/lib.cpp
)

target_include_directories(
    hecate_lib ${warning_guard}
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source>"
)

target_compile_features(hecate_lib PUBLIC cxx_std_20)

find_package(fmt REQUIRED)
target_link_libraries(hecate_lib PRIVATE fmt::fmt)

# ---- Declare executable ----

add_executable(hecate_exe source/main.cpp)
add_executable(hecate::exe ALIAS hecate_exe)

set_target_properties(
    hecate_exe PROPERTIES
    OUTPUT_NAME hecate
    EXPORT_NAME exe
)

target_compile_features(hecate_exe PRIVATE cxx_std_20)

target_link_libraries(hecate_exe PRIVATE hecate_lib)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT hecate_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of hecate"
  )
endif()

include(cmake/dev-mode.cmake)
