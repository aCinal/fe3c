add_functional_test(
    NAME                      points_ed25519
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/points/Test_points_ed25519.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/field_elements/field_elements_ed25519_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED25519
)

add_functional_test(
    NAME                      points_ed448
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/points/Test_points_ed448.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/points/points_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/field_elements/field_elements_ed448_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED448
)

add_functional_test(
    NAME                      points_ed25519_comb_method
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/points/Test_points_ed25519.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/points/comb/comb_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/points/comb/precomputation/precomputation_ed25519_??.c
                              ${FE3C_UUT_SOURCE_DIR}/field_elements/field_elements_ed25519_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED25519 FE3C_COMB_METHOD
)

add_functional_test(
    NAME                      points_ed448_comb_method
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/points/Test_points_ed448.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/points/points_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/points/comb/comb_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/points/comb/precomputation/precomputation_ed448_??.c
                              ${FE3C_UUT_SOURCE_DIR}/field_elements/field_elements_ed448_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED448 FE3C_COMB_METHOD
)

add_functional_test(
    NAME                      points_ed25519_comb_method_small
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/points/Test_points_ed25519.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/points/comb/comb_ed25519_small.c
                              ${FE3C_UUT_SOURCE_DIR}/points/comb/precomputation/precomputation_ed25519_small_??.c
                              ${FE3C_UUT_SOURCE_DIR}/field_elements/field_elements_ed25519_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED25519
                              FE3C_COMB_METHOD
                              FE3C_ED25519_SMALL_PRECOMPUTATION
)

add_functional_test(
    NAME                      points_ed448_comb_method_small
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/points/Test_points_ed448.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/points/points_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/points/comb/comb_ed448_small.c
                              ${FE3C_UUT_SOURCE_DIR}/points/comb/precomputation/precomputation_ed448_small_??.c
                              ${FE3C_UUT_SOURCE_DIR}/field_elements/field_elements_ed448_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED448
                              FE3C_COMB_METHOD
                              FE3C_ED448_SMALL_PRECOMPUTATION
)

add_functional_test(
    NAME                      points_ed25519_lattice_basis_reduction
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/points/Test_points_ed25519.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/points/lattice_basis_reduction/lattices_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/field_elements/field_elements_ed25519_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED25519 FE3C_ED25519_LATTICE_BASIS_REDUCTION
)

add_functional_test(
    NAME                      points_ed448_lattice_basis_reduction
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/points/Test_points_ed448.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/points/points_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/points/lattice_basis_reduction/lattices_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/field_elements/field_elements_ed448_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED448 FE3C_ED448_LATTICE_BASIS_REDUCTION
)
