add_functional_test(
    NAME                      field_elements_ed25519
    UT_SOURCE                 Test_field_elements_ed25519_??.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/field_elements/field_elements_ed25519_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED25519
)

add_functional_test(
    NAME                      field_elements_ed448
    UT_SOURCE                 Test_field_elements_ed448_??.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/field_elements/field_elements_ed448_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED448
)

add_functional_test(
    NAME                      field_elements_ed25519_fast_squaring
    UT_SOURCE                 Test_field_elements_ed25519_??.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/field_elements/field_elements_ed25519_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED25519 FE3C_OPTIMIZATION_FAST_SQUARING
)

add_functional_test(
    NAME                      field_elements_ed448_fast_squaring
    UT_SOURCE                 Test_field_elements_ed448_??.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/field_elements/field_elements_ed448_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED448 FE3C_OPTIMIZATION_FAST_SQUARING
)
