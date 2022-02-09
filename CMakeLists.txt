cmake_minimum_required(VERSION 3.0.0)
project(chess VERSION 0.1.0)
set(CMAKE_C_STANDARD 99)
include(CTest)
enable_testing()

file(GLOB src_files CONFIGURE_DEPENDS
        "src/*")
add_executable(chess ${src_files})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
