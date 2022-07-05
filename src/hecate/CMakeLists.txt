cmake_minimum_required(VERSION 3.15)

# cmake 3.7 provides a FindVulkan module
find_package(Vulkan)

if (UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

# set the appropriate vulkan preprocessor definition
if (WIN32)
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR") # this is provided directly in code

elseif(LINUX)
    # TODO maybe add a headless linux config?
    find_package(Threads REQUIRED)
    
    find_program(PKG_CONFIG pkg-config)
    
    find_package(Wayland REQUIRED)

    pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols)

    find_program(WAYLAND_SCANNER wayland-scanner)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WAYLAND_KHR")

    include_directories(${WAYLAND_INCLUDE_DIR})
    include_directories(${CMAKE_BINARY_DIR})

    pkg_get_variable(protocol_dir wayland-protocols pkgdatadir)
    execute_process(
        COMMAND ${WAYLAND_SCANNER} client-header ${protocol_dir}/stable/xdg-shell/xdg-shell.xml ${CMAKE_BINARY_DIR}/xdg-shell-client-protocol.h
        COMMAND ${WAYLAND_SCANNER} private-code ${protocol_dir}/stable/xdg-shell/xdg-shell.xml ${CMAKE_BINARY_DIR}/xdg-shell-protocol.c)
    
    link_libraries(${XCB_LIBRARIES} ${Vulkan_LIBRARY} ${Vulkan_LIBRARY} ${DIRECTFB_LIBRARIES} ${WAYLAND_CLIENT_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

elseif(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_MACOS_VK -DVK_EXAMPLE_XCODE_GENERATED")

    link_libraries(${Vulkan_LIBRARY} "-framework AppKit" "-framework QuartzCore")
endif()

add_library(
    HecateLib

    "hecate.cpp"
    "dependencies.cpp" 
    "app/application.cpp"
    "core/engine.cpp"
    "core/logger/log_category.cpp"
    "core/logger/log_message.cpp"
    "core/logger/log_sink.cpp"
    "core/logger.cpp"
    "core/system.cpp"

    "graphics/surface_state.cpp" 

    "util/codec/utf.cpp"
    "util/debugger.cpp"
    "util/filesystem.cpp"
    "util/string_util.cpp"
    "util/spin_mutex.cpp"
    "util/threads.cpp"
  "graphics/graphics.h" "graphics/graphics.cpp")

find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(unofficial-concurrentqueue CONFIG REQUIRED)
find_package(unofficial-vulkan-memory-allocator CONFIG REQUIRED)

find_library(SHADERC_LIB shaderc_combined $ENV{VULKAN_SDK}/lib)

find_path(RANG_INCLUDE_DIRS "rang.hpp")

target_include_directories(
    HecateLib PRIVATE 
    ${Vulkan_INCLUDE_DIRS}
    ${RANG_INCLUDE_DIRS}
)

target_link_libraries(HecateLib 
PUBLIC
        Vulkan::Vulkan
        nlohmann_json::nlohmann_json

PRIVATE
        fmt::fmt
        unofficial::concurrentqueue::concurrentqueue
        unofficial::vulkan-memory-allocator::vulkan-memory-allocator
)

if (WIN32)
    target_link_libraries(HecateLib PUBLIC Shcore)
endif()