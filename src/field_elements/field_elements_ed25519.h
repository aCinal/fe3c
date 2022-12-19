
#ifndef __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_ED25519_H
#define __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_ED25519_H

#ifdef __cplusplus
extern "C" {
#endif

#include <global_defs.h>

#if FE3C_32BIT && FE3C_64BIT
    #error "Both FE3C_32BIT and FE3C_64BIT set!"
#elif FE3C_32BIT
    #include <field_elements/field_elements_ed25519_32.h>
#elif FE3C_64BIT
    #include <field_elements/field_elements_ed25519_64.h>
#else
    #error "Architecture not defined. Set exactly one of FE3C_32BIT or FE3C_64BIT"
#endif /* FE3C_32BIT && FE3C_64BIT */

/**
 * @brief Find a multiplicative inverse of a field element
 * @param[out] r Result of inversion, i.e. such element r that r a = 1
 * @param[in] a Field element to invert
 */
static inline void fe_invert(fe25519 r, const fe25519 a) {

    /* Make a local copy to support aliasing */
    fe25519 x;
    fe_copy(x, a);

    /* Use the Fermat's little theorem identity: a^(p-1) = 1 (mod p) and obtain
     * the inverse as a^(p-2), i.e. a to the power 2^255 - 19 - 2. Note that
     *
     * p-2 = 0x7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffeb
     *
     * and so p-2 is 250 ones followed by a zero and then 0xb = 0b1011. Use a
     * square and multiply algorithm to obtain the result.
     *
     * Note that with this method "inverting zero" is possible and the result
     * is also zero.
     */

    /* TODO: Investigate other approaches to do this */

    /* Initialize r with the initial squaring and multiplication (this covers
     * the two leftmost ones in the exponent - since the first one corresponds
     * to a no-op) */
    fe_square(r, x);
    fe_mul(r, r, x);

    /* Do a square-and-multiply step for the remaining 248 ones in the binary
     * representation of the exponent (p-2) */
    for (int i = 0; i < 248; i++) {

        fe_square(r, r);
        fe_mul(r, r, x);
    }

    /* Zero in the exponent - just a squaring, with no multiplication */
    fe_square(r, r);

    /* One in the exponent - do both a squaring and a multiplication */
    fe_square(r, r);
    fe_mul(r, r, x);

    /* Zero in the exponent - only square */
    fe_square(r, r);

    /* Two last ones in the exponent - do both a squaring and a multiplication */
    fe_square(r, r);
    fe_mul(r, r, x);
    fe_square(r, r);
    fe_mul(r, r, x);
}

/**
 * @brief Raise an element to the power (p-5)/8 where p = 2^255 - 19 is the field modulus
 * @param[out] r Result of the exponentation
 * @param[in] a The element to exponentiate
 */
static inline void fe_exp_p_minus_5_over_8(fe25519 r, const fe25519 a) {

    /* Make a local copy to support aliasing */
    fe25519 x;
    fe_copy(x, a);

    /* Note that (p-5)/8 is equal to (2^255-24)/8 = 2^252-3, which has the following
     * hexadecimal representation:
     *
     * (p-5)/8 = 0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffd
     *
     * i.e. it is a sequence of 248 ones followed by 0xd = 0b1101 (250 consecutive
     * ones in total). Use a square-and-multiply algorithm to obtain the result.
     */

    /* TODO: Investigate other approaches to do this */

    /* Initialize r with the initial squaring and multiplication (this covers
     * the two leftmost ones in the exponent - since the first one corresponds
     * to a no-op) */
    fe_square(r, x);
    fe_mul(r, r, x);

    /* Do a square-and-multiply step for the remaining 248 ones in the binary
     * representation of the exponent */
    for (int i = 0; i < 248; i++) {

        fe_square(r, r);
        fe_mul(r, r, x);
    }

    /* Zero in the exponent - just a squaring, with no multiplication */
    fe_square(r, r);

    /* One in the exponent - do both a squaring and a multiplication */
    fe_square(r, r);
    fe_mul(r, r, x);
}

/**
 * @brief Return the least-significant bit of the field element
 * @param a Field element
 * @return Least-significant bit of a
 */
static inline int fe_lsb(fe25519 a) {

    fe_strong_reduce(a, a);
    return a[0] & 1;
}

#ifdef __cplusplus
}
#endif

#endif /* __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_ED25519_H */
