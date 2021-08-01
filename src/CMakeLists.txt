cmake_minimum_required(VERSION 3.17)
project(untitled C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS -O3)
add_executable(main board.c move.c board.h magic.h magic.c rays.c rays.h bitutil.h engine.c perft.c engine.h move_gen.c move_gen.h move.h)