cmake_minimum_required(VERSION 3.0.0)
project(chess VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
enable_testing()
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -Wextra -flto")
set(CMAKE_CXX_FLAGS_DEBUG, "-g")

file(GLOB move CONFIGURE_DEPENDS
        "move/*")
file(GLOB magic CONFIGURE_DEPENDS
        "magic/*")
file(GLOB board CONFIGURE_DEPENDS
        "board/*")
file(GLOB util CONFIGURE_DEPENDS
        "util/*")

include_directories("move" "board" "magic" "util")

add_executable(perft perft.cpp ${move} ${magic} ${util} ${board})

# add_executable(t testing/fen.c ${src_files})
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
