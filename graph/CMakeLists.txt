cmake_minimum_required(VERSION 3.15)
project(graph)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -MMD" )
#SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -v ")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # this does not work? cmdline only

# --- ranges/v3 ---------------------------------------------
set(USER_HOME $ENV{HOME})
set(RANGES_INCLUDE "${USER_HOME}/work/lib/range-v3/include")
include_directories(ranges PUBLIC ${RANGES_INCLUDE})

# --- root/include dir ??? ---------------------------------------------
include_directories(${CMAKE_SOURCE_DIR}/../include)
include_directories(${CMAKE_SOURCE_DIR}/include)

# === TARGETS ===============================================
add_executable(graph
        main.cpp
        graph.cpp
        graph.hpp
        cgraph_api.cpp cgraph_api.hpp maze.cpp maze.hpp magick.cpp)

# === PER TARGET: ===============================================

# --- Use pkg-config files --------------------------------------
include(FindPkgConfig)

# --- libcgraph ---------------------------------------------
# pkg_search_module(CGRAPH REQUIRED libcgraph)
# target_include_directories(graph SYSTEM PUBLIC ${CGRAPH_INCLUDE_DIRS})
# # next won't work with non-standard path like that: -L/usr/local/Cellar/graphviz/2.47.1/lib -lcgraph -lcdt
# target_link_libraries(graph ${CGRAPH_LIBRARIES})

# +++ to use custom libs path: ++++++++++++++
# the `pkg_check_modules` function is created with this call
#find_package(PkgConfig REQUIRED)

# these calls create special `PkgConfig::<MODULE>` variables
#pkg_check_modules(CGRAPH REQUIRED IMPORTED_TARGET libcgraph)
pkg_search_module(CGRAPH REQUIRED IMPORTED_TARGET libcgraph)
pkg_search_module(GVC REQUIRED IMPORTED_TARGET libgvc)

# --- Magick++ ---------------------------------------------
pkg_search_module(MagickXX REQUIRED IMPORTED_TARGET "Magick++")

target_link_libraries(graph PUBLIC
        PkgConfig::CGRAPH
        PkgConfig::GVC
        PkgConfig::MagickXX
    )

# Print all variables
#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

# Print all ENV variables
# execute_process(COMMAND "${CMAKE_COMMAND}" "-E" "environment")

add_subdirectory("gvpp-master")
