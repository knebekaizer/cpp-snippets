cmake_minimum_required(VERSION 3.15)
project(cpp-features)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/../include)

add_executable(cpp-features
        policy.cpp
        spanning.cpp
        structural.cpp
        functional.cpp
        scoped.cpp
        main.cpp
    )