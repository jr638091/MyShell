cmake_minimum_required(VERSION 3.8)
project(shell)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        shell.c)

add_executable(shell ${SOURCE_FILES})