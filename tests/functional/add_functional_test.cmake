function(add_functional_test)
    set(options)
    set(args NAME UT_SOURCE)
    set(list_args UUT_SOURCES MOCK_SOURCES PREPROCESSOR_DEFINITIONS)
    cmake_parse_arguments(
        PARSE_ARGV 0  # Parse ARGV variable, i.e. all args passed into this function starting from 0th
        ftest         # Prefix the result variables with ftest (functional test)
        "${options}"
        "${args}"
        "${list_args}"
    )

    # Abort if any excess arguments provided
    if (ftest_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "add_functional_test(): Unparsed arguments: ${ftest_UNPARSED_ARGUMENTS}")
    endif()

    # Assert the test name and the unit-test source were provided
    if (NOT ftest_NAME OR NOT ftest_UT_SOURCE)
        message(FATAL_ERROR
            "add_functional_test(): Missing required arguments! NAME='${ftest_NAME}', "
            "UT_SOURCE='${ftest_UT_SOURCE}'")
    endif()

    set(FUNCTIONAL_TESTS_SOURCE_DIR ${CMAKE_SOURCE_DIR})
    # Build the test group
    add_executable(
        ${ftest_NAME}
        # Always include the common main
        ${FUNCTIONAL_TESTS_SOURCE_DIR}/Main.cpp
        # Include the unit-test code
        ${ftest_UT_SOURCE}
        # Include the sources under test
        ${ftest_UUT_SOURCES}
        # Include mocks
        ${ftest_MOCK_SOURCES}
    )

    # Add custom preprocessor definitions
    target_compile_definitions(
        ${ftest_NAME}
        PRIVATE
        # Always enable sanity checks
        FE3C_ENABLE_SANITY_CHECKS
        ${ftest_PREPROCESSOR_DEFINITIONS}
    )

    # Link against CppUTest
    target_link_libraries(
        ${ftest_NAME}
        PRIVATE
        CppUTest
        CppUTestExt
    )

endfunction()
