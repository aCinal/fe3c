add_functional_test(
    NAME                      hash_function_sha512
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/hash_functions/Test_hash_function_sha512.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/hash_functions/hash_function_sha512.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED25519
)

add_functional_test(
    NAME                      hash_function_shake256
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/hash_functions/Test_hash_function_shake256.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/hash_functions/hash_function_shake256.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED448
)
