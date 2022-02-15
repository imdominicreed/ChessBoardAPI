cmake_minimum_required(VERSION 3.0.0)
project(chess VERSION 0.1.0)
set(CMAKE_C_STANDARD 99)

enable_testing()
set(CMAKE_C_FLAGS_RELEASE "-O3 -Wall -Wextra")

file(GLOB src_files CONFIGURE_DEPENDS
        "src/*")
add_executable(chess perft.c ${src_files})
add_executable(t testing/fen.c ${src_files})


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
