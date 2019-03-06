add_executable(allTests
    main.cpp
    TransformTests.cpp
    VecDequeTests.cpp
)

set_target_properties(allTests PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)

target_link_libraries(allTests PRIVATE stock)
