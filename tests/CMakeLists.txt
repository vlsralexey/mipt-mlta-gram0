add_executable(gram0_tests
    parse_rules_test.cpp
    run_test.cpp
    examples_test.cpp
)

target_include_directories(gram0_tests PRIVATE ${CMAKE_SOURCE_DIR}/lib)
target_link_libraries(gram0_tests PRIVATE gram0lib GTest::gtest_main)
target_compile_definitions(gram0_tests PRIVATE EXAMPLES_DIR="${CMAKE_SOURCE_DIR}/examples")

add_test(
        NAME gram0_tests
        COMMAND $<TARGET_FILE:gram0_tests>
)