cmake_minimum_required(VERSION 3.12)
project(Othello VERSION 1.0)

file(GLOB SOURCES CONFIGURE_DEPENDS *.h, *.c)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")

add_executable(Player player.cpp)
add_executable(Main src/main.cpp)
