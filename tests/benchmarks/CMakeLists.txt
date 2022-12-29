include(add_benchmark_test.cmake)

add_benchmark_test(
    NAME                      benchmark_64bit
    PREPROCESSOR_DEFINITIONS  FE3C_64BIT
)

add_benchmark_test(
    NAME                      benchmark_32bit
    PREPROCESSOR_DEFINITIONS  FE3C_32BIT
)

add_benchmark_test(
    NAME                      benchmark_64bit_optimized
    PREPROCESSOR_DEFINITIONS  FE3C_64BIT
                              FE3C_OPTIMIZATION_FAST_SQUARING
                              FE3C_OPTIMIZATION_COMB_METHOD
)

add_benchmark_test(
    NAME                      benchmark_32bit_optimized
    PREPROCESSOR_DEFINITIONS  FE3C_32BIT
                              FE3C_OPTIMIZATION_FAST_SQUARING
                              FE3C_OPTIMIZATION_COMB_METHOD
)

add_benchmark_test(
    NAME                      benchmark_64bit_optimized_no_zeroization
    PREPROCESSOR_DEFINITIONS  FE3C_64BIT
                              FE3C_OPTIMIZATION_FAST_SQUARING
                              FE3C_OPTIMIZATION_COMB_METHOD
                              FE3C_OPTIMIZATION_SKIP_ZEROIZATION
)

add_benchmark_test(
    NAME                      benchmark_32bit_optimized_no_zeroization
    PREPROCESSOR_DEFINITIONS  FE3C_32BIT
                              FE3C_OPTIMIZATION_FAST_SQUARING
                              FE3C_OPTIMIZATION_COMB_METHOD
                              FE3C_OPTIMIZATION_SKIP_ZEROIZATION
)
