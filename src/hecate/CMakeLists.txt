cmake_minimum_required(VERSION 3.15)

# cmake 3.7 provides a FindVulkan module
find_package(Vulkan REQUIRED)

add_library(
    HecateLib

    "hecate.cpp"
    "dependencies.h"
    "dependencies.cpp" 
    "app/application.cpp"
    "core/engine.cpp"
    "core/logger/log_category.cpp"
    "core/logger/log_message.cpp"
    "core/logger/log_sink.cpp"
    "core/logger.cpp"
    "core/system.cpp"
    "graphics/graphics.h"
    "graphics/graphics.cpp"
    "input/input.cpp"
    "input/keyboard.cpp"
    "input/mouse.cpp"
    $<${WIN32}:${CMAKE_CURRENT_SOURCE_DIR}/platform/platform_win32.cpp>
    "platform/window.h" 
    $<${WIN32}:${CMAKE_CURRENT_SOURCE_DIR}/platform/window_win32.cpp>
    "platform/platform_strings.h" 
    $<${WIN32}:${CMAKE_CURRENT_SOURCE_DIR}/platform/platform_strings_win32.cpp>
)

find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(unofficial-concurrentqueue CONFIG REQUIRED)
find_package(unofficial-vulkan-memory-allocator CONFIG REQUIRED)

find_library(SHADERC_LIB shaderc_combined $ENV{VULKAN_SDK}/lib)

find_path(RANG_INCLUDE_DIRS "rang.hpp")

target_include_directories(
    HecateLib 

PUBLIC
    ${Vulkan_INCLUDE_DIRS}

PRIVATE 
    ${RANG_INCLUDE_DIRS}
)

target_link_libraries(HecateLib
PUBLIC
    unofficial::vulkan-memory-allocator::vulkan-memory-allocator

PRIVATE
    Vulkan::Vulkan
    nlohmann_json::nlohmann_json
    fmt::fmt
    unofficial::concurrentqueue::concurrentqueue    
)

if (WIN32)
    target_link_libraries(HecateLib PUBLIC Shcore dwmapi)
endif()