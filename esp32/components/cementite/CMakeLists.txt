set(FE3C_ROOT_DIR "../../..")
set(FE3C_SRC_DIR "${FE3C_ROOT_DIR}/src")
set(FE3C_PORT_DIR "port")

set(FE3C_SOURCES
    "${FE3C_SRC_DIR}/eddsa/eddsa.c"
    "${FE3C_SRC_DIR}/curves/curves.c"
)

set(FE3C_DEFINES
    "FE3C_32BIT"
    "FE3C_LILENDIAN_TARGET"
    "FE3C_PORT_OVERRIDES"
)

if (CONFIG_FE3C_ENABLE_SANITY_CHECKS)
    list(APPEND FE3C_DEFINES "FE3C_ENABLE_SANITY_CHECKS")
endif()

if (CONFIG_FE3C_COMB_METHOD)
    list(APPEND FE3C_DEFINES "FE3C_COMB_METHOD")
endif()

if (CONFIG_FE3C_ED25519_SMALL_PRECOMPUTATION)
    list(APPEND FE3C_DEFINES "FE3C_ED25519_SMALL_PRECOMPUTATION")
    set(ed25519_small_precomp_tag "_small")
endif()

if (CONFIG_FE3C_ED448_SMALL_PRECOMPUTATION)
    list(APPEND FE3C_DEFINES "FE3C_ED448_SMALL_PRECOMPUTATION")
    set(ed448_small_precomp_tag "_small")
endif()

if (CONFIG_FE3C_ED448_ISOGENY)
    list(APPEND FE3C_DEFINES "FE3C_ED448_ISOGENY")
    set(ed448_isogeny_tag "_isogeny")
else()
    # No isogeny in use, enforce small precomputation tables
    set(ed448_small_precomp_tag "_small")
endif()

if (CONFIG_FE3C_SKIP_ZEROIZATION)
    list(APPEND FE3C_DEFINES "FE3C_SKIP_ZEROIZATION")
endif()

if (CONFIG_FE3C_PARALLEL_COMB_METHOD)
    list(APPEND FE3C_SOURCES "${FE3C_PORT_DIR}/points/comb/comb_parallel.c")
endif()

if (CONFIG_FE3C_SUPPORT_CURVE_ED25519)
    list(APPEND FE3C_SOURCES "${FE3C_SRC_DIR}/curves/curve_ed25519.c")
    list(APPEND FE3C_SOURCES "${FE3C_PORT_DIR}/scalars/scalars_ed25519_32.c")
    list(APPEND FE3C_SOURCES "${FE3C_PORT_DIR}/field_elements/field_elements_ed25519_32.c")
    list(APPEND FE3C_SOURCES "${FE3C_PORT_DIR}/hash_functions/hash_function_sha512.c")
    list(APPEND FE3C_SOURCES "${FE3C_PORT_DIR}/hash_functions/hash_function_sha512_impl_sw.c")

    if (CONFIG_FE3C_COMB_METHOD)
        list(APPEND FE3C_SOURCES "${FE3C_SRC_DIR}/points/comb/comb_ed25519${ed25519_small_precomp_tag}.c")
        list(APPEND FE3C_SOURCES "${FE3C_PORT_DIR}/points/comb/precomputation/precomputation_ed25519${ed25519_small_precomp_tag}_32.c")
    endif()

    if (CONFIG_FE3C_PARALLEL_COMB_METHOD)
        list(APPEND FE3C_SOURCES "${FE3C_PORT_DIR}/points/points_ed25519.c")
        list(APPEND FE3C_SOURCES "${FE3C_PORT_DIR}/points/comb/comb_parallel_ed25519.c")
    else()
        # No parallelism - use common group arithmetic
        list(APPEND FE3C_SOURCES "${FE3C_SRC_DIR}/points/points_ed25519.c")
    endif()

    if (CONFIG_FE3C_USE_SHA512_HARDWARE)
        list(APPEND FE3C_SOURCES "${FE3C_PORT_DIR}/hash_functions/hash_function_sha512_impl_hw.c")
        list(APPEND FE3C_DEFINES "FE3C_USE_SHA512_HARDWARE")
    endif()

    list(APPEND FE3C_DEFINES "FE3C_SUPPORT_CURVE_ED25519")
endif()

if (CONFIG_FE3C_SUPPORT_CURVE_ED448)
    list(APPEND FE3C_SOURCES "${FE3C_SRC_DIR}/curves/curve_ed448.c")
    list(APPEND FE3C_SOURCES "${FE3C_SRC_DIR}/hash_functions/hash_function_shake256.c")
    list(APPEND FE3C_SOURCES "${FE3C_PORT_DIR}/scalars/scalars_ed448_32.c")
    list(APPEND FE3C_SOURCES "${FE3C_PORT_DIR}/field_elements/field_elements_ed448_32.c")

    if (CONFIG_FE3C_COMB_METHOD)
        list(APPEND FE3C_SOURCES "${FE3C_SRC_DIR}/points/comb/comb_ed448${ed448_small_precomp_tag}.c")
        list(APPEND FE3C_SOURCES "${FE3C_PORT_DIR}/points/comb/precomputation/precomputation_ed448${ed448_isogeny_tag}${ed448_small_precomp_tag}_32.c")
    endif()

    if (CONFIG_FE3C_PARALLEL_COMB_METHOD)
        list(APPEND FE3C_SOURCES "${FE3C_PORT_DIR}/points/points_ed448${ed448_isogeny_tag}.c")
        list(APPEND FE3C_SOURCES "${FE3C_PORT_DIR}/points/comb/comb_parallel_ed448.c")
    else()
        # No parallelism - use common group arithmetic
        list(APPEND FE3C_SOURCES "${FE3C_SRC_DIR}/points/points_ed448${ed448_isogeny_tag}.c")
    endif()

    list(APPEND FE3C_DEFINES "FE3C_SUPPORT_CURVE_ED448")
endif()

idf_component_register(SRCS ${FE3C_SOURCES}
                       INCLUDE_DIRS "${FE3C_ROOT_DIR}/include"
                       PRIV_INCLUDE_DIRS "."
                                         "${FE3C_SRC_DIR}"
# We need a dependency to mbedtls when using the SHA-2 hardware, to use their API
# and this way not risk any race conditions. Due to the two-pass nature of CMake
# processing in esp-idf, requirements cannot depend on configuration options, so
# specify this dependency always
                        PRIV_REQUIRES mbedtls
)

target_compile_definitions(${COMPONENT_LIB} PRIVATE ${FE3C_DEFINES})
