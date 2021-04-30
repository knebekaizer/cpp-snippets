
add_executable(
        cpp-features
    main.cpp
    structured.cpp
    functional.cpp
    scoped.cpp
    async.cpp
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
