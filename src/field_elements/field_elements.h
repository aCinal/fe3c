
#ifndef __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_H
#define __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <fe3c/global_defs.h>

/* Represent big integers as arrays of "limbs" with each limb corresponding to a machine word. To reduce the number
 * of carries and allow greater instruction-level parallelism use an "unsaturated" representation, i.e. leave unused
 * high bits in each limb. */

#if FE3C_32BIT && FE3C_64BIT
    #error "Both FE3C_32BIT FE3C_64BIT set!"
#elif FE3C_32BIT
    /* Use 10 25.5-bit (mixed-radix) limbs (i.e. alternating pattern of 26 and 25 bits in each limb) */
    #define ED25519_FE_LIMB_COUNT       10
    #define ED448_FE_LIMB_COUNT         10
    typedef u32 fe_limb_type;
#elif FE3C_64BIT
    /* Use 5 51-bit (uniform-radix) limbs allowing for 13 bits of overflow (2^13 additions before any reduction is
     * needed) */
    #define ED25519_FE_LIMB_COUNT       5
    #define ED448_FE_LIMB_COUNT         5
    typedef u64 fe_limb_type;
#else
    #error "Architecture not defined. Set exactly one of FE3C_32BIT or FE3C_64BIT"
#endif

typedef union fe {

#if FE3C_SUPPORT_CURVE_ED25519
    fe_limb_type ed25519[ED25519_FE_LIMB_COUNT];
#endif /* FE3C_SUPPORT_CURVE_ED25519 */

#if FE3C_SUPPORT_CURVE_ED448
    fe_limb_type ed448[ED448_FE_LIMB_COUNT];
#endif /* FE3C_SUPPORT_CURVE_ED448 */

} fe;

#ifdef __cplusplus
}
#endif

#endif /* __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_H */
