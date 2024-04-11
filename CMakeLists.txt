cmake_minimum_required(VERSION 3.27)
project(UNIXTelnetClient C)

set(CMAKE_C_STANDARD 11)

add_executable(UNIXTelnetClient main.c
        ThreadFunction.c
        ThreadFunction.h)
