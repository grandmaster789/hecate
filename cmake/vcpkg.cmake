# download vcpkg if needed
# NOTE this does not support version pinning yet

if (NOT EXISTS "${CMAKE_BINARY_DIR}/vcpkg.cmake")
    message(STATUS "Downloading vcpkg.cmake")
    file(DOWNLOAD "https://github.com/microsoft/vcpkg/raw/master/scripts/buildsystems/vcpkg.cmake" "${CMAKE_BINARY_DIR}/vcpkg.cmake")
    message(STATUS "Download complete")
endif()

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_TOOLCHAIN_FILE}" "${CMAKE_BINARY_DIR}/vcpkg.cmake")