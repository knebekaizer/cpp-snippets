
#add_definitions(-DUSE_RUNTIME_LOG_LEVEL)
#set(RANGES_INCLUDE "${CMAKE_SOURCE_DIR}/../range-v3/include")
#include_directories(ranges ${RANGES_INCLUDE})

add_executable(demo_code
    demo_code.cpp)

add_executable(vector_negative
    vector_negative.cpp)

add_executable(fancy_ptr
        fancy_ptr.cpp
        fancy_ptr.h
        )

add_executable(fault_injection_ptr
        fault_injection_ptr.cpp
        )

#set(USER_HOME $ENV{HOME})
#set(BOOST_INCLUDE "${USER_HOME}/work/3d/boost_1_78_0")

#target_include_directories(ranges PUBLIC ${RANGES_INCLUDE})
#target_include_directories(ranges_2 PUBLIC ${RANGES_INCLUDE})
#target_include_directories(bitmap PUBLIC ${RANGES_INCLUDE} "${CMAKE_SOURCE_DIR}/include/range-v3")
#target_include_directories(ringbuffer PUBLIC ${RANGES_INCLUDE} "${CMAKE_SOURCE_DIR}/include/range-v3")
#target_include_directories(WrapOut PUBLIC ${RANGES_INCLUDE} "${CMAKE_SOURCE_DIR}/include/range-v3")