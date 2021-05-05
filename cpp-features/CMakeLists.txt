cmake_minimum_required(VERSION 3.15)
project(cpp-features)

set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/../include)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Werror -Wall")

add_subdirectory("src")
