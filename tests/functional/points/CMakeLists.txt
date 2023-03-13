add_functional_test(
    NAME                      points_ed25519
    UT_SOURCE                 Test_points_ed25519.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_SOURCE_DIR}/field_elements/field_elements_ed25519_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED25519
)

add_functional_test(
    NAME                      points_ed448
    UT_SOURCE                 Test_points_ed448.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/points/points_ed448.c
                              ${FE3C_SOURCE_DIR}/field_elements/field_elements_ed448_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED448
)

add_functional_test(
    NAME                      points_ed448_isogeny
    UT_SOURCE                 Test_points_ed448_isogeny.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/points/points_ed448_isogeny.c
                              ${FE3C_SOURCE_DIR}/field_elements/field_elements_ed448_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED448 FE3C_OPTIMIZATION_ED448_ISOGENY
)

add_functional_test(
    NAME                      points_ed25519_comb_method
    UT_SOURCE                 Test_points_ed25519.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_SOURCE_DIR}/points/comb/comb_ed25519.c
                              ${FE3C_SOURCE_DIR}/points/comb/precomputation/precomputation_ed25519_??.c
                              ${FE3C_SOURCE_DIR}/field_elements/field_elements_ed25519_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED25519 FE3C_OPTIMIZATION_COMB_METHOD
)

add_functional_test(
    NAME                      points_ed448_comb_method
    UT_SOURCE                 Test_points_ed448.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/points/points_ed448.c
                              ${FE3C_SOURCE_DIR}/points/comb/comb_ed448_small.c
                              ${FE3C_SOURCE_DIR}/points/comb/precomputation/precomputation_ed448_small_??.c
                              ${FE3C_SOURCE_DIR}/field_elements/field_elements_ed448_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED448 FE3C_OPTIMIZATION_COMB_METHOD
)

add_functional_test(
    NAME                      points_ed448_isogeny_comb_method
    UT_SOURCE                 Test_points_ed448_isogeny.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/points/points_ed448_isogeny.c
                              ${FE3C_SOURCE_DIR}/points/comb/comb_ed448.c
                              ${FE3C_SOURCE_DIR}/points/comb/precomputation/precomputation_ed448_isogeny_??.c
                              ${FE3C_SOURCE_DIR}/field_elements/field_elements_ed448_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED448
                              FE3C_OPTIMIZATION_ED448_ISOGENY
                              FE3C_OPTIMIZATION_COMB_METHOD
)

add_functional_test(
    NAME                      points_ed25519_comb_method_small
    UT_SOURCE                 Test_points_ed25519.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_SOURCE_DIR}/points/comb/comb_ed25519_small.c
                              ${FE3C_SOURCE_DIR}/points/comb/precomputation/precomputation_ed25519_small_??.c
                              ${FE3C_SOURCE_DIR}/field_elements/field_elements_ed25519_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED25519
                              FE3C_OPTIMIZATION_COMB_METHOD
                              FE3C_SMALL_PRECOMPUTATION
)

add_functional_test(
    NAME                      points_ed448_isogeny_comb_method_small
    UT_SOURCE                 Test_points_ed448_isogeny.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/points/points_ed448_isogeny.c
                              ${FE3C_SOURCE_DIR}/points/comb/comb_ed448_small.c
                              ${FE3C_SOURCE_DIR}/points/comb/precomputation/precomputation_ed448_isogeny_small_??.c
                              ${FE3C_SOURCE_DIR}/field_elements/field_elements_ed448_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED448
                              FE3C_OPTIMIZATION_ED448_ISOGENY
                              FE3C_OPTIMIZATION_COMB_METHOD
                              FE3C_SMALL_PRECOMPUTATION
)
