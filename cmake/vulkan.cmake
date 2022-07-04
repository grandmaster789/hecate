# based on https://github.com/SaschaWillems/Vulkan

OPTION(USE_D2D_WSI      "Build the project using Direct to Display swapchain"  OFF)
OPTION(USE_DIRECTFB_WSI "Build the project using DirectFB swapchain"           OFF)
OPTION(USE_WAYLAND_WSI  "Build the project using Wayland swapchain"            OFF)
OPTION(USE_HEADLESS     "Build the project using headless extension swapchain" OFF)

message(STATUS "Trying to find Vulkan\n")
find_package(Vulkan)

if(UNIX AND NOT APPLE)
	set(LINUX TRUE)
endif()

if(WIN32)
	if (NOT Vulkan_FOUND)
		find_library(Vulkan_LIBRARY NAMES vulkan-1 vulkan PATHS ${CMAKE_SOURCE_DIR}/dependencies/vulkan)

		if (Vulkan_LIBRARY)
			set(Vulkan_FOUND ON)
			message(STATUS "Using bundled Vulkan library version")
		endif()
	endif()

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR")

elseif(LINUX)
	if (NOT Vulkan_FOUND)
		find_library(Vulkan_LIBRARY NAMES vulkan HINTS "$ENV{VULKAN_SDK}/lib" "${CMAKE_SOURCE_DIR}/dependencies/vulkan" REQUIRED)

		if (Vulkan_LIBRARY)
			set(Vulkan_FOUND ON)
			message(STATUS "Using bundled Vulkan library version")
		endif()
	endif()

	find_package(Threads REQUIRED)
	if(USE_D2D_WSI)
		message("Using direct to display extension...")
		add_definitions(-D_DIRECT2DISPLAY)

	elseif(USE_DIRECTFB_WSI)
		find_package(DirectFB REQUIRED)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_DIRECTFB_EXT")
		include_directories(${DIRECTFB_INCLUDE_DIR})

	elseif(USE_WAYLAND_WSI)
		find_program(PKG_CONFIG pkg-config)
		if (NOT PKG_CONFIG)
			message(FATAL_ERROR "pkg-config binary not found")
		endif ()

		find_package(Wayland REQUIRED)
		if (NOT WAYLAND_FOUND)
			message(FATAL_ERROR "Wayland development package not found")
		endif ()

		pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols)
		if (NOT WAYLAND_PROTOCOLS_FOUND)
			message(FATAL_ERROR "Wayland protocols package not found")
		endif ()

		find_program(WAYLAND_SCANNER wayland-scanner)
		if (NOT WAYLAND_SCANNER)
			message(FATAL_ERROR "wayland-scanner binary not found")
		endif ()

		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WAYLAND_KHR")

		include_directories(${WAYLAND_INCLUDE_DIR})
		pkg_get_variable(protocol_dir wayland-protocols pkgdatadir)
		execute_process(
			COMMAND ${WAYLAND_SCANNER} client-header ${protocol_dir}/stable/xdg-shell/xdg-shell.xml ${CMAKE_BINARY_DIR}/xdg-shell-client-protocol.h
			COMMAND ${WAYLAND_SCANNER} private-code ${protocol_dir}/stable/xdg-shell/xdg-shell.xml ${CMAKE_BINARY_DIR}/xdg-shell-protocol.c
		)
		include_directories(${CMAKE_BINARY_DIR})

	elseif(USE_HEADLESS)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_HEADLESS_EXT")

	else(USE_D2D_WSI)
		find_package(XCB REQUIRED)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_XCB_KHR")

	endif(USE_D2D_WSI)

elseif(APPLE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_MACOS_MVK -DVK_EXAMPLE_XCODE_GENERATED")
	# Todo : android?

endif(WIN32)

if (NOT Vulkan_FOUND)
	message(FATAL_ERROR "Could not find Vulkan library!")
else()
	message(STATUS "Vulkan found: ${Vulkan_LIBRARY}")
endif()