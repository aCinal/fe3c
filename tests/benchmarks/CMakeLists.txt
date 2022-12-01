include(add_benchmark_test.cmake)

add_benchmark_test(
    NAME                      benchmark_64bit
    PREPROCESSOR_DEFINITIONS  FE3C_64BIT
)

add_benchmark_test(
    NAME                      benchmark_32bit
    PREPROCESSOR_DEFINITIONS  FE3C_32BIT
)
