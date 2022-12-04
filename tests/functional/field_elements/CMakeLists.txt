add_functional_test(
    NAME                      field_elements_ed25519_64
    UT_SOURCE                 Test_field_elements_ed25519_64.cpp
    PREPROCESSOR_DEFINITIONS  FE3C_64BIT FE3C_SUPPORT_CURVE_ED25519
)

add_functional_test(
    NAME                      field_elements_ed25519_32
    UT_SOURCE                 Test_field_elements_ed25519_32.cpp
    PREPROCESSOR_DEFINITIONS  FE3C_32BIT FE3C_SUPPORT_CURVE_ED25519
)

add_functional_test(
    NAME                      field_elements_ed448_64
    UT_SOURCE                 Test_field_elements_ed448_64.cpp
    PREPROCESSOR_DEFINITIONS  FE3C_64BIT FE3C_SUPPORT_CURVE_ED448
)

add_functional_test(
    NAME                      field_elements_ed448_32
    UT_SOURCE                 Test_field_elements_ed448_32.cpp
    PREPROCESSOR_DEFINITIONS  FE3C_32BIT FE3C_SUPPORT_CURVE_ED448
)

add_functional_test(
    NAME                      field_elements_ed25519_64_fast_squaring
    UT_SOURCE                 Test_field_elements_ed25519_64.cpp
    PREPROCESSOR_DEFINITIONS  FE3C_64BIT FE3C_SUPPORT_CURVE_ED25519 FE3C_OPTIMIZATION_FAST_SQUARING
)

add_functional_test(
    NAME                      field_elements_ed25519_32_fast_squaring
    UT_SOURCE                 Test_field_elements_ed25519_32.cpp
    PREPROCESSOR_DEFINITIONS  FE3C_32BIT FE3C_SUPPORT_CURVE_ED25519 FE3C_OPTIMIZATION_FAST_SQUARING
)

add_functional_test(
    NAME                      field_elements_ed448_64_fast_squaring
    UT_SOURCE                 Test_field_elements_ed448_64.cpp
    PREPROCESSOR_DEFINITIONS  FE3C_64BIT FE3C_SUPPORT_CURVE_ED448 FE3C_OPTIMIZATION_FAST_SQUARING
)

add_functional_test(
    NAME                      field_elements_ed448_32_fast_squaring
    UT_SOURCE                 Test_field_elements_ed448_32.cpp
    PREPROCESSOR_DEFINITIONS  FE3C_32BIT FE3C_SUPPORT_CURVE_ED448 FE3C_OPTIMIZATION_FAST_SQUARING
)
