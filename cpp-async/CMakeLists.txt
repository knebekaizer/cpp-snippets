cmake_minimum_required(VERSION 3.15)
project(cpp_async)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/../include)

add_executable(cpp_async main.cpp)