cmake_minimum_required(VERSION 3.15)
project(cimg)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -MMD" )
#SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -v ")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # this does not work? cmdline only

## --- ranges/v3 ---------------------------------------------
#set(USER_HOME $ENV{HOME})
#set(RANGES_INCLUDE "${USER_HOME}/work/lib/range-v3/include")
#include_directories(ranges PUBLIC ${RANGES_INCLUDE})

# --- root/include dir ??? ---------------------------------------------
include_directories(${CMAKE_SOURCE_DIR}/../include)
include_directories(${CMAKE_SOURCE_DIR}/include)


#target_include_directories(magick PUBLIC ${IMAGEMAGICK_H})
include_directories(PUBLIC ${IMAGEMAGICK_H})

add_executable(cimg main.cpp)

add_executable(magick
        magick_test.cpp)

include(FindPkgConfig)

# --- Magick++ ---------------------------------------------
pkg_search_module(MagickXX REQUIRED IMPORTED_TARGET "Magick++")

# pkg_check_modules(YOUR_PKG REQUIRED IMPORTED_TARGET ya-package)
target_link_libraries(magick PUBLIC
        PkgConfig::MagickXX
    )
