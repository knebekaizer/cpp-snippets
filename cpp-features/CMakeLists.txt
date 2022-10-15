cmake_minimum_required(VERSION 3.15)
project(cpp-features)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/../include)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Werror -Wall")
#SET(CMAKE_EXE_LINKER_FLAGS "-L/opt/llvm-15/lib/c++ -Wl,-rpath,/opt/llvm-15/lib/c++")

add_subdirectory("src")
