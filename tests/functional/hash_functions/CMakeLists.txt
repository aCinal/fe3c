add_functional_test(
    NAME                      hash_function_sha512
    UT_SOURCE                 Test_hash_function_sha512.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/hash_functions/hash_function_sha512.c
                              ${FE3C_SOURCE_DIR}/utils/utils.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED25519
)
