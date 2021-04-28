cmake_minimum_required(VERSION 3.15)
project(cpp-features)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/../include)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Werror -Wall")

add_executable(cpp-features
        spanning.cpp
        structured.cpp
        functional.cpp
        scoped.cpp
        main.cpp
    )

add_executable(testPolicy
        policy.cpp
    )
