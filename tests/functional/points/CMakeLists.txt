add_functional_test(
    NAME                      points_ed25519
    UT_SOURCE                 Test_points_ed25519.cpp
    UUT_SOURCES               ${FE3C_SOURCE_DIR}/points/points_ed25519.c
    PREPROCESSOR_DEFINITIONS  FE3C_64BIT FE3C_SUPPORT_CURVE_ED25519
)
