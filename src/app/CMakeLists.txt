cmake_minimum_required(VERSION 3.15)

add_executable(
        Entry
        main.cpp
)

include_directories(../hecate)

target_link_libraries(
        Entry PRIVATE
        HecateLib
)

# [TODO] STILL MISSING: Install Target