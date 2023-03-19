add_functional_test(
    NAME                      scalars_ed25519
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/scalars/Test_scalars_ed25519.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/scalars/scalars_ed25519_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED25519
)

add_functional_test(
    NAME                      scalars_ed448
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/scalars/Test_scalars_ed448.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/scalars/scalars_ed448_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED448
)
