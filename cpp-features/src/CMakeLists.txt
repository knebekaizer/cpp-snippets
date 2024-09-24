
#add_definitions(-DUSE_RUNTIME_LOG_LEVEL)
#set(RANGES_INCLUDE "${CMAKE_SOURCE_DIR}/../range-v3/include")
#include_directories(ranges ${RANGES_INCLUDE})

add_executable(cpp-features
    main.cpp
    structured.cpp
    functional.cpp
    scoped.cpp
    ranges_2.cpp
)

add_executable(async
    async.cpp
)

add_executable(booStringlWrapper
    booStringlWrapper.cpp
)

add_executable(deleter
    deleter.cpp
)

add_executable(semaphore
    semaphore.cpp
)

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

add_executable(sfinae sfinae.cpp)

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

add_executable(extern_tmpl
        extern_tmpl.cpp
        extern_tmpl_2.cpp
)

add_executable(stream_failure
        stream_failure.cpp
)

add_executable(bitset_subscript
        bitset_subscript.cpp
)

add_executable(arith_conversion
        arith_conversion.cpp
)

add_executable(float_precision
        float_precision.cpp
)

add_executable(float_random
        float_random.cpp
)

add_executable(set_algo set_algorithm.cpp)

# meta...
add_executable(variadic variadic_tmpl.cpp )

add_executable(enum_array enum_array.cpp)
add_executable(literals literals.cpp)

add_executable(tuple_apply tuple_apply.cpp)
add_executable(tuple_variadic tuple_variadic.cpp)
add_executable(tuple_remap tuple_remap.cpp)
add_executable(tuple_utils tuple_utils.cpp)

add_executable(float_inf float_inf.cpp)
add_executable(printable printable.cpp)
add_executable(filesystem filesystem.cpp)

add_executable(inline inline.cpp inline2.cpp)
add_executable(overload overload.cpp overload2.cpp)

add_executable(graph graph.cpp)

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
