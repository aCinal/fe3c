add_functional_test(
    NAME                      eddsa_rfc_vectors_ed25519
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/eddsa/Test_eddsa_rfc_vectors_ed25519.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curves.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curve_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/scalars/scalars_ed25519_??.c
                              ${FE3C_UUT_SOURCE_DIR}/hash_functions/hash_function_sha512.c
                              ${FE3C_UUT_SOURCE_DIR}/field_elements/field_elements_ed25519_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED25519
)

add_functional_test(
    NAME                      eddsa_fail_cases_ed25519
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/eddsa/Test_eddsa_fail_cases_ed25519.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curves.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curve_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/scalars/scalars_ed25519_??.c
                              ${FE3C_UUT_SOURCE_DIR}/hash_functions/hash_function_sha512.c
                              ${FE3C_UUT_SOURCE_DIR}/field_elements/field_elements_ed25519_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED25519
)

add_functional_test(
    NAME                      eddsa_rfc_vectors_ed448
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/eddsa/Test_eddsa_rfc_vectors_ed448.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curves.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curve_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/points/points_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/scalars/scalars_ed448_??.c
                              ${FE3C_UUT_SOURCE_DIR}/hash_functions/hash_function_shake256.c
                              ${FE3C_UUT_SOURCE_DIR}/field_elements/field_elements_ed448_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED448
)

add_functional_test(
    NAME                      eddsa_fail_cases_ed448
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/eddsa/Test_eddsa_fail_cases_ed448.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curves.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curve_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/points/points_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/scalars/scalars_ed448_??.c
                              ${FE3C_UUT_SOURCE_DIR}/hash_functions/hash_function_shake256.c
                              ${FE3C_UUT_SOURCE_DIR}/field_elements/field_elements_ed448_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED448
)

add_functional_test(
    NAME                      eddsa_rfc_vectors_ed25519_optimized
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/eddsa/Test_eddsa_rfc_vectors_ed25519.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curves.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curve_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/points/comb/comb_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/points/comb/precomputation/precomputation_ed25519_??.c
                              ${FE3C_UUT_SOURCE_DIR}/points/lattice_basis_reduction/lattices_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/scalars/scalars_ed25519_??.c
                              ${FE3C_UUT_SOURCE_DIR}/hash_functions/hash_function_sha512.c
                              ${FE3C_UUT_SOURCE_DIR}/field_elements/field_elements_ed25519_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED25519
                              FE3C_FAST_SQUARING
                              FE3C_COMB_METHOD
                              FE3C_ED25519_LATTICE_BASIS_REDUCTION
)

add_functional_test(
    NAME                      eddsa_rfc_vectors_ed448_optimized
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/eddsa/Test_eddsa_rfc_vectors_ed448.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curves.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curve_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/points/points_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/points/comb/comb_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/points/comb/precomputation/precomputation_ed448_??.c
                              ${FE3C_UUT_SOURCE_DIR}/points/lattice_basis_reduction/lattices_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/scalars/scalars_ed448_??.c
                              ${FE3C_UUT_SOURCE_DIR}/hash_functions/hash_function_shake256.c
                              ${FE3C_UUT_SOURCE_DIR}/field_elements/field_elements_ed448_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED448
                              FE3C_FAST_SQUARING
                              FE3C_COMB_METHOD
                              FE3C_ED448_LATTICE_BASIS_REDUCTION
)

add_functional_test(
    NAME                      eddsa_rfc_vectors_ed25519_optimized_small
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/eddsa/Test_eddsa_rfc_vectors_ed25519.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curves.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curve_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/points/comb/comb_ed25519_small.c
                              ${FE3C_UUT_SOURCE_DIR}/points/comb/precomputation/precomputation_ed25519_small_??.c
                              ${FE3C_UUT_SOURCE_DIR}/points/lattice_basis_reduction/lattices_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/scalars/scalars_ed25519_??.c
                              ${FE3C_UUT_SOURCE_DIR}/hash_functions/hash_function_sha512.c
                              ${FE3C_UUT_SOURCE_DIR}/field_elements/field_elements_ed25519_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED25519
                              FE3C_FAST_SQUARING
                              FE3C_COMB_METHOD
                              FE3C_ED25519_SMALL_PRECOMPUTATION
                              FE3C_ED25519_LATTICE_BASIS_REDUCTION
)

add_functional_test(
    NAME                      eddsa_rfc_vectors_ed448_optimized_small
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/eddsa/Test_eddsa_rfc_vectors_ed448.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curves.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curve_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/points/points_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/points/comb/comb_ed448_small.c
                              ${FE3C_UUT_SOURCE_DIR}/points/comb/precomputation/precomputation_ed448_small_??.c
                              ${FE3C_UUT_SOURCE_DIR}/points/lattice_basis_reduction/lattices_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/scalars/scalars_ed448_??.c
                              ${FE3C_UUT_SOURCE_DIR}/hash_functions/hash_function_shake256.c
                              ${FE3C_UUT_SOURCE_DIR}/field_elements/field_elements_ed448_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED448
                              FE3C_FAST_SQUARING
                              FE3C_COMB_METHOD
                              FE3C_ED448_SMALL_PRECOMPUTATION
                              FE3C_ED448_LATTICE_BASIS_REDUCTION
)

add_functional_test(
    NAME                      eddsa_taming_ed25519
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/eddsa/Test_eddsa_taming_ed25519.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curves.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curve_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/scalars/scalars_ed25519_??.c
                              ${FE3C_UUT_SOURCE_DIR}/hash_functions/hash_function_sha512.c
                              ${FE3C_UUT_SOURCE_DIR}/field_elements/field_elements_ed25519_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED25519
)

add_functional_test(
    NAME                      eddsa_taming_ed448
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/eddsa/Test_eddsa_taming_ed448.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curves.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curve_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/points/points_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/scalars/scalars_ed448_??.c
                              ${FE3C_UUT_SOURCE_DIR}/hash_functions/hash_function_shake256.c
                              ${FE3C_UUT_SOURCE_DIR}/field_elements/field_elements_ed448_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED448
)

add_functional_test(
    NAME                      eddsa_cached_public_key_ed25519
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/eddsa/Test_eddsa_cached_public_key_ed25519.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curves.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curve_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/scalars/scalars_ed25519_??.c
                              ${FE3C_UUT_SOURCE_DIR}/hash_functions/hash_function_sha512.c
                              ${FE3C_UUT_SOURCE_DIR}/field_elements/field_elements_ed25519_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED25519
)

add_functional_test(
    NAME                      eddsa_cached_public_key_ed448
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/eddsa/Test_eddsa_cached_public_key_ed448.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curves.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curve_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/points/points_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/scalars/scalars_ed448_??.c
                              ${FE3C_UUT_SOURCE_DIR}/hash_functions/hash_function_shake256.c
                              ${FE3C_UUT_SOURCE_DIR}/field_elements/field_elements_ed448_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED448
)

add_functional_test(
    NAME                      eddsa_cr_yp_to_ed25519
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/eddsa/Test_eddsa_cr_yp_to_ed25519.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curves.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curve_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/scalars/scalars_ed25519_??.c
                              ${FE3C_UUT_SOURCE_DIR}/hash_functions/hash_function_sha512.c
                              ${FE3C_UUT_SOURCE_DIR}/field_elements/field_elements_ed25519_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED25519
)

add_functional_test(
    NAME                      eddsa_cr_yp_to_ed25519_optimized
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/eddsa/Test_eddsa_cr_yp_to_ed25519.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curves.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curve_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/points/comb/comb_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/points/comb/precomputation/precomputation_ed25519_??.c
                              ${FE3C_UUT_SOURCE_DIR}/points/lattice_basis_reduction/lattices_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/scalars/scalars_ed25519_??.c
                              ${FE3C_UUT_SOURCE_DIR}/hash_functions/hash_function_sha512.c
                              ${FE3C_UUT_SOURCE_DIR}/field_elements/field_elements_ed25519_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED25519
                              FE3C_FAST_SQUARING
                              FE3C_COMB_METHOD
                              FE3C_ED25519_LATTICE_BASIS_REDUCTION
)

add_functional_test(
    NAME                      eddsa_cr_yp_to_ed25519_optimized_small
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/eddsa/Test_eddsa_cr_yp_to_ed25519.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curves.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curve_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/points/comb/comb_ed25519_small.c
                              ${FE3C_UUT_SOURCE_DIR}/points/comb/precomputation/precomputation_ed25519_small_??.c
                              ${FE3C_UUT_SOURCE_DIR}/points/lattice_basis_reduction/lattices_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/scalars/scalars_ed25519_??.c
                              ${FE3C_UUT_SOURCE_DIR}/hash_functions/hash_function_sha512.c
                              ${FE3C_UUT_SOURCE_DIR}/field_elements/field_elements_ed25519_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED25519
                              FE3C_FAST_SQUARING
                              FE3C_COMB_METHOD
                              FE3C_ED25519_SMALL_PRECOMPUTATION
                              FE3C_ED25519_LATTICE_BASIS_REDUCTION
)

add_functional_test(
    NAME                      eddsa_wycheproof_ed25519
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/eddsa/Test_eddsa_wycheproof_ed25519.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curves.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curve_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/scalars/scalars_ed25519_??.c
                              ${FE3C_UUT_SOURCE_DIR}/hash_functions/hash_function_sha512.c
                              ${FE3C_UUT_SOURCE_DIR}/field_elements/field_elements_ed25519_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED25519
)

add_functional_test(
    NAME                      eddsa_wycheproof_ed25519_optimized
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/eddsa/Test_eddsa_wycheproof_ed25519.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curves.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curve_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/points/comb/comb_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/points/comb/precomputation/precomputation_ed25519_??.c
                              ${FE3C_UUT_SOURCE_DIR}/points/lattice_basis_reduction/lattices_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/scalars/scalars_ed25519_??.c
                              ${FE3C_UUT_SOURCE_DIR}/hash_functions/hash_function_sha512.c
                              ${FE3C_UUT_SOURCE_DIR}/field_elements/field_elements_ed25519_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED25519
                              FE3C_FAST_SQUARING
                              FE3C_COMB_METHOD
                              FE3C_ED25519_LATTICE_BASIS_REDUCTION
)

add_functional_test(
    NAME                      eddsa_wycheproof_ed25519_optimized_small
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/eddsa/Test_eddsa_wycheproof_ed25519.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curves.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curve_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/points/points_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/points/comb/comb_ed25519_small.c
                              ${FE3C_UUT_SOURCE_DIR}/points/comb/precomputation/precomputation_ed25519_small_??.c
                              ${FE3C_UUT_SOURCE_DIR}/points/lattice_basis_reduction/lattices_ed25519.c
                              ${FE3C_UUT_SOURCE_DIR}/scalars/scalars_ed25519_??.c
                              ${FE3C_UUT_SOURCE_DIR}/hash_functions/hash_function_sha512.c
                              ${FE3C_UUT_SOURCE_DIR}/field_elements/field_elements_ed25519_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED25519
                              FE3C_FAST_SQUARING
                              FE3C_COMB_METHOD
                              FE3C_ED25519_SMALL_PRECOMPUTATION
                              FE3C_ED25519_LATTICE_BASIS_REDUCTION
)

add_functional_test(
    NAME                      eddsa_wycheproof_ed448
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/eddsa/Test_eddsa_wycheproof_ed448.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curves.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curve_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/points/points_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/scalars/scalars_ed448_??.c
                              ${FE3C_UUT_SOURCE_DIR}/hash_functions/hash_function_shake256.c
                              ${FE3C_UUT_SOURCE_DIR}/field_elements/field_elements_ed448_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED448
)

add_functional_test(
    NAME                      eddsa_wycheproof_ed448_optimized
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/eddsa/Test_eddsa_wycheproof_ed448.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curves.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curve_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/points/points_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/points/comb/comb_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/points/comb/precomputation/precomputation_ed448_??.c
                              ${FE3C_UUT_SOURCE_DIR}/points/lattice_basis_reduction/lattices_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/scalars/scalars_ed448_??.c
                              ${FE3C_UUT_SOURCE_DIR}/hash_functions/hash_function_shake256.c
                              ${FE3C_UUT_SOURCE_DIR}/field_elements/field_elements_ed448_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED448
                              FE3C_FAST_SQUARING
                              FE3C_COMB_METHOD
                              FE3C_ED448_LATTICE_BASIS_REDUCTION
)

add_functional_test(
    NAME                      eddsa_wycheproof_ed448_optimized_small
    UT_SOURCE                 ${FE3C_UT_SOURCE_DIR}/eddsa/Test_eddsa_wycheproof_ed448.cpp
    UUT_SOURCES               ${FE3C_UUT_SOURCE_DIR}/eddsa/eddsa.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curves.c
                              ${FE3C_UUT_SOURCE_DIR}/curves/curve_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/points/points_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/points/comb/comb_ed448_small.c
                              ${FE3C_UUT_SOURCE_DIR}/points/comb/precomputation/precomputation_ed448_small_??.c
                              ${FE3C_UUT_SOURCE_DIR}/points/lattice_basis_reduction/lattices_ed448.c
                              ${FE3C_UUT_SOURCE_DIR}/scalars/scalars_ed448_??.c
                              ${FE3C_UUT_SOURCE_DIR}/hash_functions/hash_function_shake256.c
                              ${FE3C_UUT_SOURCE_DIR}/field_elements/field_elements_ed448_??.c
    PREPROCESSOR_DEFINITIONS  FE3C_SUPPORT_CURVE_ED448
                              FE3C_FAST_SQUARING
                              FE3C_COMB_METHOD
                              FE3C_ED448_SMALL_PRECOMPUTATION
                              FE3C_ED448_LATTICE_BASIS_REDUCTION
)
