
#ifndef __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_H
#define __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <global_defs.h>

/* Represent big integers as arrays of "limbs" with each limb corresponding to a machine word. Unless overridden by
 * port implementation, use an "unsaturated" representation, i.e. leave unused high bits ("nails") in each limb, to
 * reduce the number of carries and allow greater instruction-level parallelism. */

#if FE3C_32BIT && FE3C_64BIT
    #error "Both FE3C_32BIT and FE3C_64BIT set!"
#elif FE3C_32BIT

    #if !defined(ED25519_FE_LIMB_COUNT)
        /* By default use 10 25.5-bit (mixed-radix) limbs (i.e. alternating pattern of 26 and 25 bits in each limb) */
        #define ED25519_FE_LIMB_COUNT       10
    #endif /* !defined(ED25519_FE_LIMB_COUNT) */

    #if !defined(ED448_FE_LIMB_COUNT)
        /* Use 16 28-bit (uniform-radix) limbs */
        #define ED448_FE_LIMB_COUNT         16
    #endif /* !defined(ED448_FE_LIMB_COUNT) */

    typedef u32 fe_limb_type;

#elif FE3C_64BIT

    #if !defined(ED25519_FE_LIMB_COUNT)
        /* Use 5 51-bit (uniform-radix) limbs allowing for 13 bits of overflow (2^13 additions before any reduction is
         * needed) */
        #define ED25519_FE_LIMB_COUNT       5
    #endif /* !defined(ED25519_FE_LIMB_COUNT) */

    #if !defined(ED448_FE_LIMB_COUNT)
        /* Use 8 56-bit (uniform-radix) limbs */
        #define ED448_FE_LIMB_COUNT         8
    #endif /* !defined(ED448_FE_LIMB_COUNT) */

    typedef u64 fe_limb_type;

#else
    #error "Architecture not defined. Set exactly one of FE3C_32BIT or FE3C_64BIT"
#endif /* FE3C_32BIT && FE3C_64BIT */

typedef fe_limb_type fe25519[ED25519_FE_LIMB_COUNT];
typedef fe_limb_type fe448[ED448_FE_LIMB_COUNT];

typedef union fe {

#if FE3C_SUPPORT_CURVE_ED25519
    fe25519 ed25519;
#endif /* FE3C_SUPPORT_CURVE_ED25519 */

#if FE3C_SUPPORT_CURVE_ED448
    fe448 ed448;
#endif /* FE3C_SUPPORT_CURVE_ED448 */

} fe;

#ifdef __cplusplus
}
#endif

#endif /* __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_H */
