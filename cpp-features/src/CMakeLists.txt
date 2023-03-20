
#add_definitions(-DUSE_RUNTIME_LOG_LEVEL)
set(RANGES_INCLUDE "${CMAKE_SOURCE_DIR}/../range-v3/include")
include_directories(ranges ${RANGES_INCLUDE})

add_executable(cpp-features
    main.cpp
    structured.cpp
    functional.cpp
    scoped.cpp
    async.cpp
        ranges_2.cpp)

add_executable(ranges
    ranges.cpp
)

add_executable(concept
    concept.cpp
)

add_executable(testSpan
    spanning.cpp
)

add_executable(testPolicy
    policy.cpp
)

add_executable(testShared
    shared_ptr.cpp
)

add_executable(random
    random.cpp
)

add_executable(lookup
    lookup.cpp
)

add_executable(exception
    ExceptionSafety.cpp
)

add_executable(misc
    misc.cpp
)

add_executable(factory
        factory.cpp factory.hpp factory_impl.cpp
)

add_executable(bitset
    bitset.cpp
)

add_executable(bitmap
    bitmap.cpp
)

add_executable(ringbuffer
    ringbuffer.cpp
)

add_executable(std-ranges
    std-ranges.cpp
)

add_executable(layout
    layout.cpp
)

add_executable(WrapOut
    WrapOut.cpp
)

add_executable(ranges_2
    ranges_2.cpp
)

add_executable(par_algo
    par_algo.cpp
)

add_executable(par_algo_none
    par_algo.cpp
)
target_compile_definitions(par_algo PUBLIC "-DPARALLEL")


add_executable(deque_dtor
    deque_dtor.cpp
)

#set(USER_HOME $ENV{HOME})
#set(BOOST_INCLUDE "${USER_HOME}/work/3d/boost_1_78_0")

#target_include_directories(ranges PUBLIC ${RANGES_INCLUDE})
#target_include_directories(ranges_2 PUBLIC ${RANGES_INCLUDE})
#target_include_directories(bitmap PUBLIC ${RANGES_INCLUDE} "${CMAKE_SOURCE_DIR}/include/range-v3")
#target_include_directories(ringbuffer PUBLIC ${RANGES_INCLUDE} "${CMAKE_SOURCE_DIR}/include/range-v3")
#target_include_directories(WrapOut PUBLIC ${RANGES_INCLUDE} "${CMAKE_SOURCE_DIR}/include/range-v3")

if (CMAKE_CXX_STANDARD EQUAL "17")
    set_source_files_properties(bitmap.cpp PROPERTIES COMPILE_FLAGS -std=c++20)
    message(STATUS "Set std=c++20 for bitmap.cpp")
endif()