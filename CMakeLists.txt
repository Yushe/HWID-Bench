cmake_minimum_required(VERSION 3.9)
project(HWID C)

include_directories(
        /usr/x86_64-w64-mingw32/include/
        /usr/x86_64-w64-mingw32/include/ddk
)

set(CMAKE_C_STANDARD 99)

add_executable(HWID main.c disk.h)
