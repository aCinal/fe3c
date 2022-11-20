add_functional_test(
    NAME                      scalars_ed25519_64
    UT_SOURCE                 Test_scalars_ed25519.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/scalars/scalars_ed25519_64.c
    PREPROCESSOR_DEFINITIONS  FE3C_64BIT FE3C_SUPPORT_CURVE_ED25519
)

add_functional_test(
    NAME                      scalars_ed25519_32
    UT_SOURCE                 Test_scalars_ed25519.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/scalars/scalars_ed25519_32.c
    PREPROCESSOR_DEFINITIONS  FE3C_32BIT FE3C_SUPPORT_CURVE_ED25519
)

add_functional_test(
    NAME                      scalars_ed448_64
    UT_SOURCE                 Test_scalars_ed448.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/scalars/scalars_ed448_64.c
    PREPROCESSOR_DEFINITIONS  FE3C_64BIT FE3C_SUPPORT_CURVE_ED448
)

add_functional_test(
    NAME                      scalars_ed448_32
    UT_SOURCE                 Test_scalars_ed448.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/scalars/scalars_ed448_32.c
    PREPROCESSOR_DEFINITIONS  FE3C_32BIT FE3C_SUPPORT_CURVE_ED448
)
