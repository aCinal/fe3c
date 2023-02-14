add_functional_test(
    NAME                      points_ed25519_64
    UT_SOURCE                 Test_points_ed25519.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_SOURCE_DIR}/field_elements/field_elements_ed25519_64.c
    PREPROCESSOR_DEFINITIONS  FE3C_64BIT FE3C_SUPPORT_CURVE_ED25519
)

add_functional_test(
    NAME                      points_ed25519_32
    UT_SOURCE                 Test_points_ed25519.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_SOURCE_DIR}/field_elements/field_elements_ed25519_32.c
    PREPROCESSOR_DEFINITIONS  FE3C_32BIT FE3C_SUPPORT_CURVE_ED25519
)

add_functional_test(
    NAME                      points_ed448_64
    UT_SOURCE                 Test_points_ed448.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/points/points_ed448.c
                              ${FE3C_SOURCE_DIR}/field_elements/field_elements_ed448_64.c
    PREPROCESSOR_DEFINITIONS  FE3C_64BIT FE3C_SUPPORT_CURVE_ED448
)

add_functional_test(
    NAME                      points_ed448_32
    UT_SOURCE                 Test_points_ed448.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/points/points_ed448.c
                              ${FE3C_SOURCE_DIR}/field_elements/field_elements_ed448_32.c
    PREPROCESSOR_DEFINITIONS  FE3C_32BIT FE3C_SUPPORT_CURVE_ED448
)

add_functional_test(
    NAME                      points_ed25519_64_comb_method
    UT_SOURCE                 Test_points_ed25519.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_SOURCE_DIR}/field_elements/field_elements_ed25519_64.c
    PREPROCESSOR_DEFINITIONS  FE3C_64BIT FE3C_SUPPORT_CURVE_ED25519 FE3C_OPTIMIZATION_COMB_METHOD
)

add_functional_test(
    NAME                      points_ed25519_32_comb_method
    UT_SOURCE                 Test_points_ed25519.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_SOURCE_DIR}/field_elements/field_elements_ed25519_32.c
    PREPROCESSOR_DEFINITIONS  FE3C_32BIT FE3C_SUPPORT_CURVE_ED25519 FE3C_OPTIMIZATION_COMB_METHOD
)

add_functional_test(
    NAME                      points_ed448_64_comb_method
    UT_SOURCE                 Test_points_ed448.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/points/points_ed448.c
                              ${FE3C_SOURCE_DIR}/field_elements/field_elements_ed448_64.c
    PREPROCESSOR_DEFINITIONS  FE3C_64BIT FE3C_SUPPORT_CURVE_ED448 FE3C_OPTIMIZATION_COMB_METHOD
)

add_functional_test(
    NAME                      points_ed448_32_comb_method
    UT_SOURCE                 Test_points_ed448.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/points/points_ed448.c
                              ${FE3C_SOURCE_DIR}/field_elements/field_elements_ed448_32.c
    PREPROCESSOR_DEFINITIONS  FE3C_32BIT FE3C_SUPPORT_CURVE_ED448 FE3C_OPTIMIZATION_COMB_METHOD
)

add_functional_test(
    NAME                      points_ed25519_64_base_four_shamir
    UT_SOURCE                 Test_points_ed25519.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_SOURCE_DIR}/field_elements/field_elements_ed25519_64.c
    PREPROCESSOR_DEFINITIONS  FE3C_64BIT FE3C_SUPPORT_CURVE_ED25519 FE3C_OPTIMIZATION_BASE_FOUR_SHAMIR_TRICK
)

add_functional_test(
    NAME                      points_ed25519_32_base_four_shamir
    UT_SOURCE                 Test_points_ed25519.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_SOURCE_DIR}/field_elements/field_elements_ed25519_32.c
    PREPROCESSOR_DEFINITIONS  FE3C_32BIT FE3C_SUPPORT_CURVE_ED25519 FE3C_OPTIMIZATION_BASE_FOUR_SHAMIR_TRICK
)

add_functional_test(
    NAME                      points_ed448_64_base_four_shamir
    UT_SOURCE                 Test_points_ed448.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/points/points_ed448.c
                              ${FE3C_SOURCE_DIR}/field_elements/field_elements_ed448_64.c
    PREPROCESSOR_DEFINITIONS  FE3C_64BIT FE3C_SUPPORT_CURVE_ED448 FE3C_OPTIMIZATION_BASE_FOUR_SHAMIR_TRICK
)

add_functional_test(
    NAME                      points_ed448_32_base_four_shamir
    UT_SOURCE                 Test_points_ed448.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/points/points_ed448.c
                              ${FE3C_SOURCE_DIR}/field_elements/field_elements_ed448_32.c
    PREPROCESSOR_DEFINITIONS  FE3C_32BIT FE3C_SUPPORT_CURVE_ED448 FE3C_OPTIMIZATION_BASE_FOUR_SHAMIR_TRICK
)
