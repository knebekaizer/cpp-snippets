cmake_minimum_required(VERSION 3.19)
project(TwistList)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -MMD" )
#SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # this does not work? cmdline only

set(USER_HOME $ENV{HOME})
set(RANGES_INCLUDE "${USER_HOME}/work/lib/range-v3/include")
include_directories(${RANGES_INCLUDE})
include_directories("${RANGES_INCLUDE}/range/v3")

include_directories(${CMAKE_SOURCE_DIR}/../include)
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(TwistList
        main.cpp
)

add_executable(fourSum
        fourSum.cpp
)
