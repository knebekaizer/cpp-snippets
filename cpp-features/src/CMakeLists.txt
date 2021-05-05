
add_executable(
        cpp-features
    main.cpp
    structured.cpp
    functional.cpp
    scoped.cpp
    async.cpp
        ../include/trace.hpp concept.cpp)

add_executable(ranges
    ranges.cpp
)


set(USER_HOME $ENV{HOME})
set(RANGES_INCLUDE "${USER_HOME}/work/lib/range-v3/include")
target_include_directories(ranges PUBLIC ${RANGES_INCLUDE})
target_include_directories(ranges PUBLIC "${RANGES_INCLUDE}/range/v3")

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
