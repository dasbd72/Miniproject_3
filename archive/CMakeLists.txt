cmake_minimum_required(VERSION 3.12)
project(Othello VERSION 1.0)

file(GLOB SOURCES CONFIGURE_DEPENDS *.h, *.c)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-O2 -Wall -Wextra")

add_executable(player player.cpp)
add_executable(main main.cpp)
