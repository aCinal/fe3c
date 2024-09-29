# Start with common defines
set(FE3C_UT_DEFINES
    "FE3C_FUNCTIONAL_TESTS_ESP32"
    "FE3C_32BIT"
    "FE3C_LILENDIAN_TARGET"
)

set(FE3C_UUT_COMMON_SOURCE_DIR   "${CMAKE_SOURCE_DIR}/../../../src")
set(FE3C_UUT_COMMON_INCLUDE_DIR  "${CMAKE_SOURCE_DIR}/../../../include")
set(FE3C_UUT_PORT_INCLUDE_DIR    "${CMAKE_SOURCE_DIR}/../../../esp32/components/cementite/include")
set(FE3C_UT_COMMON_SOURCE_DIR    "${CMAKE_SOURCE_DIR}/../src")
if (CONFIG_IDF_TARGET_ARCH_XTENSA)
    set(FE3C_UT_PORT_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../../../esp32/components/cementite/test/port/xtensa")
    list(APPEND FE3C_UT_DEFINES "FE3C_PORT_OVERRIDES")
elseif (CONFIG_IDF_TARGET_ARCH_RISCV)
    # No target-specific port available for RISC-V at this point
    set(FE3C_UT_PORT_SOURCE_DIR ${FE3C_UT_COMMON_SOURCE_DIR})
endif()

if (CONFIG_FE3C_SUPPORT_CURVE_ED25519)
    list(APPEND FE3C_UT_DEFINES "FE3C_SUPPORT_CURVE_ED25519")
    list(APPEND UT_SOURCES
        "${FE3C_UT_COMMON_SOURCE_DIR}/eddsa/Test_eddsa_cached_public_key_ed25519.cpp"
        "${FE3C_UT_COMMON_SOURCE_DIR}/eddsa/Test_eddsa_fail_cases_ed25519.cpp"
        "${FE3C_UT_COMMON_SOURCE_DIR}/eddsa/Test_eddsa_rfc_vectors_ed25519.cpp"
        "${FE3C_UT_COMMON_SOURCE_DIR}/eddsa/Test_eddsa_taming_ed25519.cpp"
        "${FE3C_UT_COMMON_SOURCE_DIR}/eddsa/Test_eddsa_wycheproof_ed25519.cpp"
        "${FE3C_UT_COMMON_SOURCE_DIR}/hash_functions/Test_hash_function_sha512.cpp"
        "${FE3C_UT_COMMON_SOURCE_DIR}/points/Test_points_ed25519.cpp"
        "${FE3C_UT_COMMON_SOURCE_DIR}/scalars/Test_scalars_ed25519.cpp"
        "${FE3C_UT_PORT_SOURCE_DIR}/field_elements/Test_field_elements_ed25519_32.cpp"

        # These require a larger app partition to fit in (cf. PARTITION_TABLE_SINGLE_APP_LARGE),
        # and so are disabled by default
        # "${FE3C_UT_COMMON_SOURCE_DIR}/eddsa/Test_eddsa_cr_yp_to_ed25519.cpp"
    )
endif()

if (CONFIG_FE3C_SUPPORT_CURVE_ED448)
    list(APPEND FE3C_UT_DEFINES "FE3C_SUPPORT_CURVE_ED448")
    list(APPEND UT_SOURCES
        "${FE3C_UT_COMMON_SOURCE_DIR}/eddsa/Test_eddsa_cached_public_key_ed448.cpp"
        "${FE3C_UT_COMMON_SOURCE_DIR}/eddsa/Test_eddsa_fail_cases_ed448.cpp"
        "${FE3C_UT_COMMON_SOURCE_DIR}/eddsa/Test_eddsa_rfc_vectors_ed448.cpp"
        "${FE3C_UT_COMMON_SOURCE_DIR}/eddsa/Test_eddsa_taming_ed448.cpp"
        "${FE3C_UT_COMMON_SOURCE_DIR}/eddsa/Test_eddsa_wycheproof_ed448.cpp"
        "${FE3C_UT_COMMON_SOURCE_DIR}/hash_functions/Test_hash_function_shake256.cpp"
        "${FE3C_UT_COMMON_SOURCE_DIR}/points/Test_points_ed448.cpp"
        "${FE3C_UT_COMMON_SOURCE_DIR}/scalars/Test_scalars_ed448.cpp"
        "${FE3C_UT_PORT_SOURCE_DIR}/field_elements/Test_field_elements_ed448_32.cpp"
    )
endif()

idf_component_register(SRCS ${UT_SOURCES}
                       PRIV_INCLUDE_DIRS "${FE3C_UUT_COMMON_INCLUDE_DIR}"
                                         "${FE3C_UUT_PORT_INCLUDE_DIR}"
                                         "${FE3C_UUT_COMMON_SOURCE_DIR}"
                       PRIV_REQUIRES unity cementite)

if (CONFIG_FE3C_SUPPORT_CURVE_ED25519 OR CONFIG_FE3C_SUPPORT_CURVE_ED448)
    # Only set compile definitions if there is anything to compile
    target_compile_definitions(${COMPONENT_LIB} PRIVATE ${FE3C_UT_DEFINES})
endif()
