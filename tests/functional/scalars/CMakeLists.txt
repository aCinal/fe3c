add_functional_test(
    NAME                      scalars_ed25519_64
    UT_SOURCE                 Test_scalars_ed25519_64.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/scalars/scalars_ed25519_64.c
    PREPROCESSOR_DEFINITIONS  FE3C_64BIT FE3C_SUPPORT_CURVE_ED25519
)
