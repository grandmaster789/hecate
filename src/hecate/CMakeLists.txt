cmake_minimum_required(VERSION 3.15)

# cmake 3.7 provides a FindVulkan module
find_package(Vulkan)

if (UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

# set the appropriate vulkan preprocessor definition
if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR")

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
  "os/window.h" "graphics/surface.h" "graphics/surface_state.h" "graphics/surface_state.cpp" "graphics/device.h")

find_package(fmt CONFIG REQUIRED)

find_path(VULKAN_HPP_INCLUDE_DIRS "vulkan/vulkan.hpp")

target_include_directories(
    HecateLib PRIVATE 
    ${VULKAN_HPP_INCLUDE_DIRS}
)

target_link_libraries(
        HecateLib PRIVATE

        fmt::fmt
)

if (WIN32)
    target_link_libraries(HecateLib PUBLIC Shcore)
endif()