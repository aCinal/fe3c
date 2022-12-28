add_functional_test(
    NAME                      eddsa_rfc_vectors_ed25519_64
    UT_SOURCE                 Test_eddsa_rfc_vectors_ed25519.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_SOURCE_DIR}/curves/curves.c
                              ${FE3C_SOURCE_DIR}/curves/curve_ed25519.c
                              ${FE3C_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_SOURCE_DIR}/scalars/scalars_ed25519_64.c
                              ${FE3C_SOURCE_DIR}/hash_functions/hash_function_sha512.c
    PREPROCESSOR_DEFINITIONS  FE3C_64BIT FE3C_SUPPORT_CURVE_ED25519
)

add_functional_test(
    NAME                      eddsa_fail_cases_ed25519_64
    UT_SOURCE                 Test_eddsa_fail_cases_ed25519.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_SOURCE_DIR}/curves/curves.c
                              ${FE3C_SOURCE_DIR}/curves/curve_ed25519.c
                              ${FE3C_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_SOURCE_DIR}/scalars/scalars_ed25519_64.c
                              ${FE3C_SOURCE_DIR}/hash_functions/hash_function_sha512.c
    PREPROCESSOR_DEFINITIONS  FE3C_64BIT FE3C_SUPPORT_CURVE_ED25519
)

add_functional_test(
    NAME                      eddsa_rfc_vectors_ed25519_32
    UT_SOURCE                 Test_eddsa_rfc_vectors_ed25519.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_SOURCE_DIR}/curves/curves.c
                              ${FE3C_SOURCE_DIR}/curves/curve_ed25519.c
                              ${FE3C_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_SOURCE_DIR}/scalars/scalars_ed25519_32.c
                              ${FE3C_SOURCE_DIR}/hash_functions/hash_function_sha512.c
    PREPROCESSOR_DEFINITIONS  FE3C_32BIT FE3C_SUPPORT_CURVE_ED25519
)

add_functional_test(
    NAME                      eddsa_fail_cases_ed25519_32
    UT_SOURCE                 Test_eddsa_fail_cases_ed25519.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_SOURCE_DIR}/curves/curves.c
                              ${FE3C_SOURCE_DIR}/curves/curve_ed25519.c
                              ${FE3C_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_SOURCE_DIR}/scalars/scalars_ed25519_32.c
                              ${FE3C_SOURCE_DIR}/hash_functions/hash_function_sha512.c
    PREPROCESSOR_DEFINITIONS  FE3C_32BIT FE3C_SUPPORT_CURVE_ED25519
)

add_functional_test(
    NAME                      eddsa_rfc_vectors_ed448_64
    UT_SOURCE                 Test_eddsa_rfc_vectors_ed448.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_SOURCE_DIR}/curves/curves.c
                              ${FE3C_SOURCE_DIR}/curves/curve_ed448.c
                              ${FE3C_SOURCE_DIR}/points/points_ed448.c
                              ${FE3C_SOURCE_DIR}/scalars/scalars_ed448_64.c
                              ${FE3C_SOURCE_DIR}/hash_functions/hash_function_shake256.c
    PREPROCESSOR_DEFINITIONS  FE3C_64BIT FE3C_SUPPORT_CURVE_ED448
)

add_functional_test(
    NAME                      eddsa_fail_cases_ed448_64
    UT_SOURCE                 Test_eddsa_fail_cases_ed448.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_SOURCE_DIR}/curves/curves.c
                              ${FE3C_SOURCE_DIR}/curves/curve_ed448.c
                              ${FE3C_SOURCE_DIR}/points/points_ed448.c
                              ${FE3C_SOURCE_DIR}/scalars/scalars_ed448_64.c
                              ${FE3C_SOURCE_DIR}/hash_functions/hash_function_shake256.c
    PREPROCESSOR_DEFINITIONS  FE3C_64BIT FE3C_SUPPORT_CURVE_ED448
)

add_functional_test(
    NAME                      eddsa_rfc_vectors_ed448_32
    UT_SOURCE                 Test_eddsa_rfc_vectors_ed448.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_SOURCE_DIR}/curves/curves.c
                              ${FE3C_SOURCE_DIR}/curves/curve_ed448.c
                              ${FE3C_SOURCE_DIR}/points/points_ed448.c
                              ${FE3C_SOURCE_DIR}/scalars/scalars_ed448_32.c
                              ${FE3C_SOURCE_DIR}/hash_functions/hash_function_shake256.c
    PREPROCESSOR_DEFINITIONS  FE3C_32BIT FE3C_SUPPORT_CURVE_ED448
)

add_functional_test(
    NAME                      eddsa_fail_cases_ed448_32
    UT_SOURCE                 Test_eddsa_fail_cases_ed448.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_SOURCE_DIR}/curves/curves.c
                              ${FE3C_SOURCE_DIR}/curves/curve_ed448.c
                              ${FE3C_SOURCE_DIR}/points/points_ed448.c
                              ${FE3C_SOURCE_DIR}/scalars/scalars_ed448_32.c
                              ${FE3C_SOURCE_DIR}/hash_functions/hash_function_shake256.c
    PREPROCESSOR_DEFINITIONS  FE3C_32BIT FE3C_SUPPORT_CURVE_ED448
)

add_functional_test(
    NAME                      eddsa_rfc_vectors_ed448_64_optimized
    UT_SOURCE                 Test_eddsa_rfc_vectors_ed448.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_SOURCE_DIR}/curves/curves.c
                              ${FE3C_SOURCE_DIR}/curves/curve_ed448.c
                              ${FE3C_SOURCE_DIR}/points/points_ed448.c
                              ${FE3C_SOURCE_DIR}/scalars/scalars_ed448_64.c
                              ${FE3C_SOURCE_DIR}/hash_functions/hash_function_shake256.c
    PREPROCESSOR_DEFINITIONS  FE3C_64BIT FE3C_SUPPORT_CURVE_ED448
                              FE3C_OPTIMIZATION_FAST_SQUARING
                              FE3C_OPTIMIZATION_COMB_METHOD
)

add_functional_test(
    NAME                      eddsa_rfc_vectors_ed448_32_optimized
    UT_SOURCE                 Test_eddsa_rfc_vectors_ed448.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_SOURCE_DIR}/curves/curves.c
                              ${FE3C_SOURCE_DIR}/curves/curve_ed448.c
                              ${FE3C_SOURCE_DIR}/points/points_ed448.c
                              ${FE3C_SOURCE_DIR}/scalars/scalars_ed448_32.c
                              ${FE3C_SOURCE_DIR}/hash_functions/hash_function_shake256.c
    PREPROCESSOR_DEFINITIONS  FE3C_32BIT FE3C_SUPPORT_CURVE_ED448
                              FE3C_OPTIMIZATION_FAST_SQUARING
                              FE3C_OPTIMIZATION_COMB_METHOD
)

add_functional_test(
    NAME                      eddsa_taming_the_many_eddsas_64
    UT_SOURCE                 Test_eddsa_taming_the_many_eddsas.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_SOURCE_DIR}/curves/curves.c
                              ${FE3C_SOURCE_DIR}/curves/curve_ed25519.c
                              ${FE3C_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_SOURCE_DIR}/scalars/scalars_ed25519_64.c
                              ${FE3C_SOURCE_DIR}/hash_functions/hash_function_sha512.c
    PREPROCESSOR_DEFINITIONS  FE3C_64BIT FE3C_SUPPORT_CURVE_ED25519
)

add_functional_test(
    NAME                      eddsa_taming_the_many_eddsas_32
    UT_SOURCE                 Test_eddsa_taming_the_many_eddsas.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_SOURCE_DIR}/curves/curves.c
                              ${FE3C_SOURCE_DIR}/curves/curve_ed25519.c
                              ${FE3C_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_SOURCE_DIR}/scalars/scalars_ed25519_32.c
                              ${FE3C_SOURCE_DIR}/hash_functions/hash_function_sha512.c
    PREPROCESSOR_DEFINITIONS  FE3C_32BIT FE3C_SUPPORT_CURVE_ED25519
)
