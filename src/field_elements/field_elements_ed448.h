
#ifndef __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_ED448_H
#define __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_ED448_H

#ifdef __cplusplus
extern "C" {
#endif

#include <global_defs.h>
#include <field_elements/field_elements.h>

#if FE3C_32BIT && FE3C_64BIT
    #error "Both FE3C_32BIT and FE3C_64BIT set!"
#elif FE3C_32BIT
    #define FE448_STR "0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x"
    #define FE448_TO_STR(x) \
        x[ 0], x[ 1], x[ 2], x[ 3], \
        x[ 4], x[ 5], x[ 6], x[ 7], \
        x[ 8], x[ 9], x[10], x[11], \
        x[12], x[13], x[14], x[15]
#elif FE3C_64BIT
    #define FE448_STR "0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx"
    #define FE448_TO_STR(x) \
        x[0], x[1], x[2], x[3], \
        x[4], x[5], x[6], x[7]
#else
    #error "Architecture not defined. Set exactly one of FE3C_32BIT or FE3C_64BIT"
#endif /* FE3C_32BIT && FE3C_64BIT */

/* Elliptic curve constant d = -39081 */
extern const fe448 ed448_d;
/* Twisted elliptic curve constant d' = d-1 */
extern const fe448 ed448twist_d;
/* Additive identity in the field */
extern const fe448 fe448_zero;
/* Multiplicative identity in the field */
extern const fe448 fe448_one;

/**
 * @brief Uncondionally copy a field element
 * @param r Destination field element
 * @param a Source field element
 */
void fe448_copy(fe448 r, const fe448 a);

/**
 * @brief Do a constant-time equality check
 * @param[in] a The first element to check
 * @param[in] b The second element to check
 * @return 1 if a = b, 0 otherwise
 * @note The elements should be reduced by the caller first
 */
int fe448_equal(const fe448 a, const fe448 b);

/**
 * @brief Do a constant-time conditional move
 * @param[out] r The result field element set to a conditionally or left alone
 * @param[in] a The element to conditionally move into r
 * @param[in] move Flag deciding on the branch, if set to 0, r ::= r, and if set to 1, r ::= a
 * @note If move is set to anything other than 0 or 1, the results are undefined
 */
void fe448_conditional_move(fe448 r, const fe448 a, int move);

/**
 * @brief Do a weak modular reduction modulo the underlying field's characteristic p
 * @param[out] r Result of the reduction
 * @param[in] a Field element to be reduced
 * @note Note that the result need to be in canonical form, i.e. between 0 and p-1, it need only be less than 2p
 */
void fe448_weak_reduce(fe448 r, const fe448 a);

/**
 * @brief Do a strong modular reduction modulo the underlying field's characteristic p
 * @param[out] r Result of the reduction
 * @param[in] a Field element to be reduced
 * @note The result is guaranteed to be in canonical form, i.e. between 0 and p-1
 */
void fe448_strong_reduce(fe448 r, const fe448 a);

/**
 * @brief Negate an element in the field
 * @param[out] r The result of negation
 * @param[in] a Element to be negated
 */
void fe448_neg(fe448 r, const fe448 a);

/**
 * @brief Add two field elements
 * @param[out] r Result of the addition, i.e. the sum r = a + b
 * @param[in] a Operand
 * @param[in] b Operand
 */
void fe448_add(fe448 r, const fe448 a, const fe448 b);

/**
 * @brief Subtract one field element from another
 * @param[out] r Result of the subtraction, i.e. the difference r = a - b
 * @param[in] a Minuend
 * @param[in] b Subtrahend
 */
void fe448_sub(fe448 r, const fe448 a, const fe448 b);

/**
 * @brief Multiply two field elements
 * @param[out] r Result of the multiplication, i.e. the product r = a b
 * @param[in] a Operand
 * @param[in] b Operand
 */
void fe448_mul(fe448 r, const fe448 a, const fe448 b);

/**
 * @brief Multiply an element by itself
 * @param[out] r Result of the squaring, i.e. the product r = a a
 * @param[in] a Field element to square
 */
void fe448_square(fe448 r, const fe448 a);

/**
 * @brief Encode a field element according to RFC 8032
 * @param[out] buffer Output buffer for the encoded field element
 * @param[in] a Field element to encode
 */
void fe448_encode(u8 * buffer, fe448 a);

/**
 * @brief Decode a field element according to RFC 8032
 * @param[out] r Decoded field element
 * @param[out] buffer Encoding of the field element
 * @return 1 if decoding succeeded, 0 otherwise
 */
__attribute__((warn_unused_result))
int fe448_decode(fe448 r, const u8 * buffer);

/**
 * @brief Find a multiplicative inverse of a field element
 * @param[out] r Result of inversion, i.e. such element r that r a = 1
 * @param[in] a Field element to invert
 */
static inline void fe448_invert(fe448 r, const fe448 a) {

    /* Make a local copy to support aliasing */
    fe448 x;
    fe448_copy(x, a);

    /* Use the Fermat's little theorem identity: a^(p-1) = 1 (mod p) and obtain
     * the inverse as a^(p-2), i.e. a to the power 2^448 - 2^224 - 1 - 2. Note that
     *
     *    p-2 = 0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffe
     *            fffffffffffffffffffffffffffffffffffffffffffffffffffffffd
     *
     * and so p-2 is 223 ones, a zero, another 222 ones, a zero and a one. Use a
     * square and multiply algorithm to obtain the result.
     *
     * Note that with this method "inverting zero" is possible and the result
     * is also zero.
     */

    /* TODO: Investigate other approaches to do this */

    /* Initialize r with the initial squaring and multiplication (this covers
     * the two leftmost ones in the exponent - since the first one corresponds
     * to a no-op) */
    fe448_square(r, x);
    fe448_mul(r, r, x);

    /* Do a square-and-multiply step for the remaining 221 ones in the binary
     * representation of the exponent (p-2) */
    for (int i = 0; i < 221; i++) {

        fe448_square(r, r);
        fe448_mul(r, r, x);
    }

    /* Zero in the exponent - just a squaring, with no multiplication */
    fe448_square(r, r);

    /* Another 222 ones in the exponent */
    for (int i = 0; i < 222; i++) {

        fe448_square(r, r);
        fe448_mul(r, r, x);
    }

    /* Zero in the exponent - only square */
    fe448_square(r, r);

    /* And finally a one - do both a squaring and a multiplication */
    fe448_square(r, r);
    fe448_mul(r, r, x);
}

/**
 * @brief Raise an element to the power (p-3)/4 where p = 2^448 - 2^224 - 1 is the field modulus
 * @param[out] r Result of the exponentation
 * @param[in] a The element to exponentiate
 */
static inline void fe448_exp_p_minus_3_over_4(fe448 r, const fe448 a) {

    /* Make a local copy to support aliasing */
    fe448 x;
    fe448_copy(x, a);

    /* Note that (p-3)/4 is equal to (2^448-2^224-4)/4 = 2^446-2^222-1, which has
     * the following hexadecimal representation:
     *
     *    (p-3)/4 = 0x3fffffffffffffffffffffffffffffffffffffffffffffffffffffff
     *                bfffffffffffffffffffffffffffffffffffffffffffffffffffffff
     *
     * i.e. it is a sequence of 223 ones, a zero, and another sequence of 222
     * ones. Use a square-and-multiply algorithm to obtain the result.
     */

    /* TODO: Investigate other approaches to do this */

    /* Initialize r with the initial squaring and multiplication (this covers
     * the two leftmost ones in the exponent - since the first one corresponds
     * to a no-op) */
    fe448_square(r, x);
    fe448_mul(r, r, x);

    /* Do a square-and-multiply step for the remaining 221 ones in the binary
     * representation of the exponent */
    for (int i = 0; i < 221; i++) {

        fe448_square(r, r);
        fe448_mul(r, r, x);
    }

    /* Zero in the exponent - just a squaring, with no multiplication */
    fe448_square(r, r);

    /* Do a square-and-multiply step for the final 222 ones in the binary
     * representation of the exponent */
    for (int i = 0; i < 222; i++) {

        fe448_square(r, r);
        fe448_mul(r, r, x);
    }
}

/**
 * @brief Return the least-significant bit of the field element
 * @param a Field element
 * @return Least-significant bit of a
 */
static inline int fe448_lsb(fe448 a) {

    fe448_strong_reduce(a, a);
    return a[0] & 1;
}

#ifdef __cplusplus
}
#endif

#endif /* __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_ED448_H */
