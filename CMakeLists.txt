cmake_minimum_required(VERSION 3.10)
project(http_server)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

include_directories(include)
add_executable(http_server src/main.c src/server.c src/router.c)
