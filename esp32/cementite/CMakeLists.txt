set(FE3C_ROOT_DIR "../../")
set(FE3C_SRC_DIR "${FE3C_ROOT_DIR}/src")

set(FE3C_SOURCES
    "${FE3C_SRC_DIR}/eddsa/eddsa.c"
    "${FE3C_SRC_DIR}/curves/curves.c"
)

set(FE3C_DEFINES
    "FE3C_32BIT"
    "FE3C_LILENDIAN"
)

if (CONFIG_FE3C_SUPPORT_CURVE_ED25519)
    list(APPEND FE3C_SOURCES "${FE3C_SRC_DIR}/curves/curve_ed25519.c")
    # TODO: Use target-specific hardware-accelerated hash function
    list(APPEND FE3C_SOURCES "${FE3C_SRC_DIR}/hash_functions/hash_function_sha512.c")
    list(APPEND FE3C_SOURCES "${FE3C_SRC_DIR}/points/points_ed25519.c")
    list(APPEND FE3C_SOURCES "${FE3C_SRC_DIR}/scalars/scalars_ed25519_32.c")
    list(APPEND FE3C_SOURCES "${FE3C_SRC_DIR}/field_elements/field_elements_ed25519_32.c")

    list(APPEND FE3C_DEFINES "FE3C_SUPPORT_CURVE_ED25519")
endif()

if (CONFIG_FE3C_SUPPORT_CURVE_ED448)
    list(APPEND FE3C_SOURCES "${FE3C_SRC_DIR}/curves/curve_ed448.c")
    list(APPEND FE3C_SOURCES "${FE3C_SRC_DIR}/hash_functions/hash_function_shake256.c")
    list(APPEND FE3C_SOURCES "${FE3C_SRC_DIR}/points/points_ed448.c")
    list(APPEND FE3C_SOURCES "${FE3C_SRC_DIR}/scalars/scalars_ed448_32.c")
    list(APPEND FE3C_SOURCES "${FE3C_SRC_DIR}/field_elements/field_elements_ed448_32.c")

    list(APPEND FE3C_DEFINES "FE3C_SUPPORT_CURVE_ED448")
endif()

if (CONFIG_FE3C_OPTIMIZATION_SKIP_ZEROIZATION)
    list(APPEND FE3C_DEFINES "FE3C_OPTIMIZATION_SKIP_ZEROIZATION")
endif()

if (CONFIG_FE3C_OPTIMIZATION_COMB_METHOD)
    list(APPEND FE3C_DEFINES "FE3C_OPTIMIZATION_COMB_METHOD")
endif()

if (CONFIG_FE3C_OPTIMIZATION_FAST_SQUARING)
    list(APPEND FE3C_DEFINES "FE3C_OPTIMIZATION_FAST_SQUARING")
endif()

# Disable FE3C_OPTIMIZATION_BASE_FOUR_SHAMIR_TRICK permanently since it does not have any benefits
# on ESP32. Stack usage is more than twice as high with this optimization and despite the number
# of iterations being cut by half, each iteration takes much longer resulting in twofold performance
# degradation

# TODO: Consider removing the optimization for host build as well (performance improvement is only marginal)

idf_component_register(SRCS ${FE3C_SOURCES}
                       INCLUDE_DIRS "${FE3C_ROOT_DIR}/include"
                       PRIV_INCLUDE_DIRS "${FE3C_SRC_DIR}")

target_compile_definitions(${COMPONENT_LIB} PRIVATE ${FE3C_DEFINES})
