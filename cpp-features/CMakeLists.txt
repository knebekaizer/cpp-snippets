cmake_minimum_required(VERSION 3.15)
project(cpp-features)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"
        AND "${CMAKE_CXX_COMPILER_VERSION}" VERSION_GREATER_EQUAL "10")
    set(CMAKE_CXX_STANDARD 20)
#    if ("${CMAKE_CXX_COMPILER_VERSION}" VERSION_GREATER "11")
#        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2b")
#    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"
        AND "${CMAKE_CXX_COMPILER_VERSION}" VERSION_GREATER_EQUAL "13")
    set(CMAKE_CXX_STANDARD 20)
    #	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -stdlib=libc++ -MMD")
    #elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    #	set(LLVM_CONFIG true)
    #	set(CLANG_INCLUDEDIR "C:/Program Files (x86)/LLVM/include")
    #	set(CLANG_LIBDIR "C:/Program Files (x86)/LLVM/lib")
else()
    set(CMAKE_CXX_STANDARD 17)
endif()
message(STATUS "CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
include_directories(${CMAKE_SOURCE_DIR}/include)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Werror -Wall")
#SET(CMAKE_EXE_LINKER_FLAGS "-L/opt/llvm-15/lib/c++ -Wl,-rpath,/opt/llvm-15/lib/c++")

#message(STATUS ${CMAKE_CXX_COMPILER_ID})

add_subdirectory("src")
