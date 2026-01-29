# Automatically fetch and use vcpkg
# This script downloads vcpkg to the build directory if not already present

if(DEFINED ENV{VCPKG_ROOT} AND EXISTS "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    # Use existing vcpkg installation from environment variable
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
    message(STATUS "Using vcpkg from VCPKG_ROOT: $ENV{VCPKG_ROOT}")
elseif(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    # Download vcpkg locally to build directory
    include(FetchContent)

    set(VCPKG_ROOT "${CMAKE_BINARY_DIR}/vcpkg")

    if(NOT EXISTS "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
        message(STATUS "Downloading vcpkg to ${VCPKG_ROOT}...")

        FetchContent_Declare(
            vcpkg
            GIT_REPOSITORY https://github.com/microsoft/vcpkg.git
            GIT_TAG 2024.12.16
            SOURCE_DIR ${VCPKG_ROOT}
        )

        FetchContent_MakeAvailable(vcpkg)

        # Bootstrap vcpkg
        if(WIN32)
            set(VCPKG_BOOTSTRAP "${VCPKG_ROOT}/bootstrap-vcpkg.bat")
        else()
            set(VCPKG_BOOTSTRAP "${VCPKG_ROOT}/bootstrap-vcpkg.sh")
        endif()

        if(EXISTS "${VCPKG_BOOTSTRAP}")
            message(STATUS "Bootstrapping vcpkg...")
            execute_process(
                COMMAND ${VCPKG_BOOTSTRAP} -disableMetrics
                WORKING_DIRECTORY ${VCPKG_ROOT}
                RESULT_VARIABLE BOOTSTRAP_RESULT
            )

            if(NOT BOOTSTRAP_RESULT EQUAL 0)
                message(FATAL_ERROR "Failed to bootstrap vcpkg")
            endif()
        else()
            message(FATAL_ERROR "vcpkg bootstrap script not found at ${VCPKG_BOOTSTRAP}")
        endif()
    endif()

    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
    message(STATUS "Using local vcpkg at: ${VCPKG_ROOT}")
endif()
