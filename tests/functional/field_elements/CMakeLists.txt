add_functional_test(
    NAME                      field_elements_ed25519_64
    UT_SOURCE                 Test_field_elements_ed25519_64.cpp
    PREPROCESSOR_DEFINITIONS  FE3C_64BIT FE3C_SUPPORT_CURVE_ED25519
)
