cmake_minimum_required(VERSION 3.14)

add_library(
        HecateLib

        hecate.cpp
)

find_package(fmt CONFIG REQUIRED)

target_link_libraries(
        HecateLib PRIVATE

        fmt::fmt
)