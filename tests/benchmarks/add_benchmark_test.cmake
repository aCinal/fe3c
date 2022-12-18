function(add_benchmark_test)
    set(options)
    set(args NAME)
    set(list_args PREPROCESSOR_DEFINITIONS)
    cmake_parse_arguments(
        PARSE_ARGV 0  # Parse ARGV variable, i.e. all args passed into this function starting from 0th
        btest         # Prefix the result variables with btest (benchmark test)
        "${options}"
        "${args}"
        "${list_args}"
    )

    # Abort if any excess arguments provided
    if (btest_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "add_benchmark_test(): Unparsed arguments: ${btest_UNPARSED_ARGUMENTS}")
    endif()

    # Assert the test name was provided
    if (NOT btest_NAME)
        message(FATAL_ERROR
            "add_benchmark_test(): Missing required arguments! NAME='${btest_NAME}'")
    endif()

    set(COMMON_SOURCES
        ${FE3C_SOURCE_DIR}/curves/curve_ed448.c
        ${FE3C_SOURCE_DIR}/curves/curve_ed25519.c
        ${FE3C_SOURCE_DIR}/curves/curves.c
        ${FE3C_SOURCE_DIR}/eddsa/eddsa.c
        ${FE3C_SOURCE_DIR}/hash_functions/hash_function_sha512.c
        ${FE3C_SOURCE_DIR}/hash_functions/hash_function_shake256.c
        ${FE3C_SOURCE_DIR}/points/points_ed448.c
        ${FE3C_SOURCE_DIR}/points/points_ed25519.c
    )

    list(FIND btest_PREPROCESSOR_DEFINITIONS FE3C_32BIT FE3C_32BIT)
    list(FIND btest_PREPROCESSOR_DEFINITIONS FE3C_64BIT FE3C_64BIT)
    if (NOT FE3C_32BIT EQUAL -1)
        set(CUSTOM_SOURCES
            ${FE3C_SOURCE_DIR}/scalars/scalars_ed448_32.c
            ${FE3C_SOURCE_DIR}/scalars/scalars_ed25519_32.c
        )
    elseif (NOT FE3C_64BIT EQUAL -1)
        set(CUSTOM_SOURCES
            ${FE3C_SOURCE_DIR}/scalars/scalars_ed448_64.c
            ${FE3C_SOURCE_DIR}/scalars/scalars_ed25519_64.c
        )
    endif()

    set(BENCHMARK_TESTS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/benchmarks)
    # Build the test group
    add_executable(
        ${btest_NAME}
        # Include the benchmarking code
        ${BENCHMARK_TESTS_SOURCE_DIR}/framework.c
        ${BENCHMARK_TESTS_SOURCE_DIR}/main.c
        ${COMMON_SOURCES}
        ${CUSTOM_SOURCES}
    )

    # Add custom preprocessor definitions
    target_compile_definitions(
        ${btest_NAME}
        PRIVATE
        # Always support both curves
        FE3C_SUPPORT_CURVE_ED25519
        FE3C_SUPPORT_CURVE_ED448
        ${btest_PREPROCESSOR_DEFINITIONS}
    )

endfunction()
