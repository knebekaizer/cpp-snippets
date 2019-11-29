cmake_minimum_required(VERSION 3.15)
project(filesystem)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost)
find_package(Boost REQUIRED COMPONENTS filesystem)
message(STATUS "Boost found: ${Boost_FOUND} ${Boost_VERSION}")
message(STATUS "Boost include path: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost library path: ${Boost_LIBRARY_DIRS}")
message(STATUS "Boost libraries: ${Boost_LIBRARIES}")

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/../include)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(filesystem main.cpp)

target_link_libraries(filesystem ${Boost_LIBRARIES})
target_link_libraries(filesystem Boost::filesystem)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # this does not work? cmdline only
