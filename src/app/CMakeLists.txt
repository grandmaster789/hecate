cmake_minimum_required(VERSION 3.16)

add_executable(
    hecate
    main.cpp
)

include_directories(../hecate)

target_link_libraries(
    hecate PRIVATE
    HecateLib
)

if (WIN32)
    set_property(TARGET hecate PROPERTY VS_DPI_AWARE "PerMonitor")
endif()

# [TODO] STILL MISSING: Install Target