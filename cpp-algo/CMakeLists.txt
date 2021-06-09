cmake_minimum_required(VERSION 3.15)
project(cpp_algo)

set(CMAKE_CXX_STANDARD 20)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -MMD" )
#SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # this does not work? cmdline only

include_directories(${CMAKE_SOURCE_DIR}/../include)
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(cpp_alg main.cpp test_std.cpp)
add_executable(moveZeroes MoveZeroes.cpp)
add_executable(domino Domino.cpp)
add_executable(prefixNotation PrefixNotation.cpp)