cmake_minimum_required(VERSION 3.17)
project(untitled C)

set(CMAKE_C_STANDARD 99)

add_executable(untitled main.c board.c move.c board.h magic.h magic.c rays.c rays.h bitutil.h)