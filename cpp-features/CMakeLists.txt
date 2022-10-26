cmake_minimum_required(VERSION 3.15)
project(cpp-features)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/../include)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Werror -Wall")
#SET(CMAKE_EXE_LINKER_FLAGS "-L/opt/llvm-15/lib/c++ -Wl,-rpath,/opt/llvm-15/lib/c++")

message(STATUS ${CMAKE_CXX_COMPILER_ID})

# Does not work as expected:
#if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2b")
#elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -stdlib=libc++ -MMD")
#elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
#    set(LLVM_CONFIG true)
#    set(CLANG_INCLUDEDIR "C:/Program Files (x86)/LLVM/include")
#    set(CLANG_LIBDIR "C:/Program Files (x86)/LLVM/lib")
#endif()


add_subdirectory("src")
