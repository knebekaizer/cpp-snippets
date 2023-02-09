cmake_minimum_required(VERSION 3.15)
project(cpp_filesystem)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -MMD -Wno-unused-command-line-argument" )

#SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # this does not work? cmdline only

include_directories(${CMAKE_SOURCE_DIR}/../include)
include_directories(${CMAKE_SOURCE_DIR}/include)
set(RANGES_INCLUDE "${CMAKE_SOURCE_DIR}/../range-v3/include")
include_directories(ranges ${RANGES_INCLUDE})


add_executable(files
        src/files.cpp
)

add_executable(md5_log
        src/md5_log.cpp
        src/atoi128.c
#        src/timing.cpp
)

add_executable(testMaxOf
        src/maxOf.cpp
)
