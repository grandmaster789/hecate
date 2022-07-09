cmake_minimum_required(VERSION 3.15)

# try to find vulkan
#include(cmake/vulkan.cmake)

# use cmake to install vcpkg dependency management
set(VCPKG_VERSION latest)
include(cmake/vcpkg.cmake)

# make a single reference to the project name
# if you don't do this, it becomes more work 
# to copy this cmakelists.txt to another project
set(This hecate)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(${This} CXX)

if (WIN32)
	add_definitions(-DUNICODE -D_UNICODE)
	add_definitions(-DWIN32_LEAN_AND_MEAN)
	#add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-D_USE_MATH_DEFINES)
	
	add_compile_options("/MP")          # build with multiple processes
	add_compile_options("/W4")          # high warning level warnings
	add_compile_options("/permissive-") # strict adherence to the c++ standard
	add_compile_options("/utf-8")       # set source and executable character sets to UTF-8
endif()

# -----------------------------------------------------------------
option(ENABLE_TESTING "Enable Test Builds" ON)

# -----------------------------------------------------------------
# the actual application

add_subdirectory("src/app")
add_subdirectory("src/${This}")

if (ENABLE_TESTING)
	enable_testing()
	add_subdirectory("test")
endif()