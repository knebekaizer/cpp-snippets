cmake_minimum_required(VERSION 3.15)
project(Scoped)

set(CMAKE_CXX_STANDARD 14)

include_directories(${CMAKE_SOURCE_DIR}/../include)

add_executable(Scoped main.cpp)