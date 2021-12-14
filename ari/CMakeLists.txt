cmake_minimum_required(VERSION 3.15)
project(cpp-ari)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/../include)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Werror -Wall -O3")

add_executable(test
        main.cpp
        )
