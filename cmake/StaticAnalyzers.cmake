find_program(CLANGTIDY clang-tidy)
if(CLANGTIDY)
    set(CMAKE_CXX_CLANG_TIDY ${CLANGTIDY} -extra-arg=-Wno-unknown-warning-option)
    message("Clang-Tidy finished setting up.")
else()
    message(STATUS "Clang-Tidy requested but executable not found.")
endif()

find_program(CPPCHECK cppcheck)
if(CPPCHECK)
    set(CMAKE_CXX_CPPCHECK ${CPPCHECK}
        --suppress=missingInclude
        --enable=all
        --inline-suppr
        --inconclusive
        -i ${CMAKE_SOURCE_DIR}/lib
    )
    message("Cppcheck finished setting up.")
else()
    message(STATUS "Cppcheck requested but executable not found.")
endif()