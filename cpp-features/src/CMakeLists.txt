
add_executable(cpp-features
    main.cpp
    structured.cpp
    functional.cpp
    scoped.cpp
    async.cpp
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

add_executable(testRandom
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

add_executable(bitset
    bitset.cpp
)

add_executable(bitmap
    bitmap.cpp
)

set(USER_HOME $ENV{HOME})
set(RANGES_INCLUDE "${USER_HOME}/work/3d/range-v3/include")

target_include_directories(ranges PUBLIC ${RANGES_INCLUDE})
target_include_directories(bitmap PUBLIC ${RANGES_INCLUDE} "${CMAKE_SOURCE_DIR}/include/range-v3")
