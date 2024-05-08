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

    # Automatically build both 32- and 64-bit variant
    foreach(ARCHITECTURE "32" "64")
        set(TEST_NAME "${ftest_NAME}_${ARCHITECTURE}")

        string(REPLACE "??" "${ARCHITECTURE}" UT_SOURCE ${ftest_UT_SOURCE})

        set(UUT_SOURCES "")
        foreach(UUT_SOURCE ${ftest_UUT_SOURCES})
            string(REPLACE "??" "${ARCHITECTURE}" __UUT_SOURCE ${UUT_SOURCE})
            list(APPEND UUT_SOURCES ${__UUT_SOURCE})
        endforeach()

        set(MOCK_SOURCES "")
        foreach(MOCK_SOURCE ${ftest_MOCK_SOURCES})
            string(REPLACE "??" "${ARCHITECTURE}" __MOCK_SOURCE ${MOCK_SOURCE})
            list(APPEND MOCK_SOURCES ${__MOCK_SOURCE})
        endforeach()

        # Build the test group
        add_executable(
            ${TEST_NAME}
            # Always include the common main
            ${FUNCTIONAL_TESTS_SOURCE_DIR}/Main.cpp
            # Include the unit-test code
            ${UT_SOURCE}
            # Include the sources under test
            ${UUT_SOURCES}
            # Include mocks
            ${MOCK_SOURCES}
        )

        # Add custom preprocessor definitions
        target_compile_definitions(
            ${TEST_NAME}
            PRIVATE
            # Always enable sanity checks
            FE3C_ENABLE_SANITY_CHECKS
            FE3C_${ARCHITECTURE}BIT
            ${ftest_PREPROCESSOR_DEFINITIONS}
        )

        # Link against CppUTest
        target_link_libraries(
            ${TEST_NAME}
            PRIVATE
            CppUTest
            CppUTestExt
        )

    endforeach()

endfunction()
