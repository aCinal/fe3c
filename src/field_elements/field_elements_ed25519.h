
#ifndef __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_ED25519_H
#define __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_ED25519_H

#ifdef __cplusplus
extern "C" {
#endif

#include <global_defs.h>
#include <field_elements/field_elements.h>

#if FE3C_32BIT && FE3C_64BIT
    #error "Both FE3C_32BIT and FE3C_64BIT set!"
#elif FE3C_32BIT
    #if !defined(FE25519_STR)
        #define FE25519_STR \
            "0x%" PRIx32 ", 0x%" PRIx32 ", 0x%" PRIx32 ", 0x%" PRIx32 ", 0x%" PRIx32 ", " \
            "0x%" PRIx32 ", 0x%" PRIx32 ", 0x%" PRIx32 ", 0x%" PRIx32 ", 0x%" PRIx32
    #endif /* !defined(FE25519_STR) */
    #if !defined(FE25519_TO_STR)
        #define FE25519_TO_STR(x) \
            x[0], x[1], x[2], x[3], x[4], \
            x[5], x[6], x[7], x[8], x[9]
    #endif /* !defined(FE25519_TO_STR) */

    #if !defined(ED25519_BASEPOINT_X)
        #define ED25519_BASEPOINT_X  { 0x325d51a, 0x18b5823, 0xf6592a, 0x104a92d, 0x1a4b31d, 0x1d6dc5c, 0x27118fe, 0x7fd814, 0x13cd6e5, 0x85a4db }
    #endif /* !defined(ED25519_BASEPOINT_X) */
    #if !defined(ED25519_BASEPOINT_Y)
        #define ED25519_BASEPOINT_Y  { 0x2666658, 0x1999999, 0xcccccc, 0x1333333, 0x1999999, 0x666666, 0x3333333, 0xcccccc, 0x2666666, 0x1999999 }
    #endif /* !defined(ED25519_BASEPOINT_Y) */
    #if !defined(ED25519_BASEPOINT_Z)
        #define ED25519_BASEPOINT_Z  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    #endif /* !defined(ED25519_BASEPOINT_Z) */
    #if !defined(ED25519_BASEPOINT_T)
        #define ED25519_BASEPOINT_T  { 0x1b7dda3, 0x1a2ace9, 0x25eadbb, 0x3ba8a, 0x83c27e, 0xabe37d, 0x1274732, 0xccacdd, 0xfd78b7, 0x19e1d7c }
    #endif /* !defined(ED25519_BASEPOINT_T) */

    #if !defined(ED25519_BASEPOINT_TIMES_2_128_X)
        #define ED25519_BASEPOINT_TIMES_2_128_X  { 0xb7e824, 0x11eb98, 0x7cbf90, 0x4e1739, 0x2639a17, 0x14e29a0, 0x29cc270, 0x6592a5, 0x3f3c45f, 0x1309ebf }
    #endif /* !defined(ED25519_BASEPOINT_TIMES_2_128_X) */
    #if !defined(ED25519_BASEPOINT_TIMES_2_128_Y)
        #define ED25519_BASEPOINT_TIMES_2_128_Y  { 0x3f5a66b, 0xaf4452, 0x93cb77, 0xf28d26, 0x24342f8, 0xc29c3a, 0x8f5b13, 0x10fb2be, 0x26526dc, 0x17cb267 }
    #endif /* !defined(ED25519_BASEPOINT_TIMES_2_128_Y) */
    #if !defined(ED25519_BASEPOINT_TIMES_2_128_Z)
        #define ED25519_BASEPOINT_TIMES_2_128_Z  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    #endif /* !defined(ED25519_BASEPOINT_TIMES_2_128_Z) */
    #if !defined(ED25519_BASEPOINT_TIMES_2_128_T)
        #define ED25519_BASEPOINT_TIMES_2_128_T  { 0x2f1338a, 0x1cc7251, 0x1b53d3c, 0x13331d, 0x51e192, 0x1924b6a, 0xab0003, 0x5fa9c, 0x3d0bf46, 0x168593 }
    #endif /* !defined(ED25519_BASEPOINT_TIMES_2_128_T) */

    #if !defined(ED25519_BASEPOINT_TIMES_2_128_PLUS_1_X)
        #define ED25519_BASEPOINT_TIMES_2_128_PLUS_1_X  { 0xc969a, 0xbf4a5a, 0x39cdf7c, 0x418f65, 0x2c39bb0, 0xc36053, 0x1e0df46, 0x91f156, 0x152b88e, 0x1b99395 }
    #endif /* !defined(ED25519_BASEPOINT_TIMES_2_128_PLUS_1_X) */
    #if !defined(ED25519_BASEPOINT_TIMES_2_128_PLUS_1_Y)
        #define ED25519_BASEPOINT_TIMES_2_128_PLUS_1_Y  { 0x3afd09b, 0x14a991e, 0x24204a6, 0xc4f62a, 0xd8e445, 0x1b85145, 0x2fe499c, 0x117cd53, 0xfa4cb5, 0xe0f144 }
    #endif /* !defined(ED25519_BASEPOINT_TIMES_2_128_PLUS_1_Y) */
    #if !defined(ED25519_BASEPOINT_TIMES_2_128_PLUS_1_Z)
        #define ED25519_BASEPOINT_TIMES_2_128_PLUS_1_Z  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    #endif /* !defined(ED25519_BASEPOINT_TIMES_2_128_PLUS_1_Z) */
    #if !defined(ED25519_BASEPOINT_TIMES_2_128_PLUS_1_T)
        #define ED25519_BASEPOINT_TIMES_2_128_PLUS_1_T  { 0x14683a9, 0x2c2f20, 0x1ac1427, 0x1c3c1dc, 0x10ff949, 0x1b93591, 0x3975c54, 0x11f2af7, 0x385104d, 0x11d7187 }
    #endif /* !defined(ED25519_BASEPOINT_TIMES_2_128_PLUS_1_T) */

#elif FE3C_64BIT
    #if !defined(FE25519_STR)
        #define FE25519_STR \
            "0x%" PRIx64 ", 0x%" PRIx64 ", 0x%" PRIx64 ", 0x%" PRIx64 ", 0x%" PRIx64
    #endif /* !defined(FE25519_STR) */
    #if !defined(FE25519_TO_STR)
        #define FE25519_TO_STR(x) \
            x[0], x[1], x[2], x[3], x[4]
    #endif /* !defined(FE25519_TO_STR) */

    #if !defined(ED25519_BASEPOINT_X)
        #define ED25519_BASEPOINT_X  { 0x62d608f25d51a, 0x412a4b4f6592a, 0x75b7171a4b31d, 0x1ff60527118fe, 0x216936d3cd6e5 }
    #endif /* !defined(ED25519_BASEPOINT_X) */
    #if !defined(ED25519_BASEPOINT_Y)
        #define ED25519_BASEPOINT_Y  { 0x6666666666658, 0x4cccccccccccc, 0x1999999999999, 0x3333333333333, 0x6666666666666 }
    #endif /* !defined(ED25519_BASEPOINT_Y) */
    #if !defined(ED25519_BASEPOINT_Z)
        #define ED25519_BASEPOINT_Z  { 1, 0, 0, 0, 0 }
    #endif /* !defined(ED25519_BASEPOINT_Z) */
    #if !defined(ED25519_BASEPOINT_T)
        #define ED25519_BASEPOINT_T  { 0x68ab3a5b7dda3, 0xeea2a5eadbb, 0x2af8df483c27e, 0x332b375274732, 0x67875f0fd78b7 }
    #endif /* !defined(ED25519_BASEPOINT_T) */

    #if !defined(ED25519_BASEPOINT_TIMES_2_128_X)
        #define ED25519_BASEPOINT_TIMES_2_128_X  { 0x47ae60b7e824, 0x1385ce47cbf90, 0x538a682639a17, 0x1964a969cc270, 0x4c27afff3c45f }
    #endif /* !defined(ED25519_BASEPOINT_TIMES_2_128_X) */
    #if !defined(ED25519_BASEPOINT_TIMES_2_128_Y)
        #define ED25519_BASEPOINT_TIMES_2_128_Y  { 0x2bd114bf5a66b, 0x3ca349893cb77, 0x30a70ea4342f8, 0x43ecaf88f5b13, 0x5f2c99e6526dc }
    #endif /* !defined(ED25519_BASEPOINT_TIMES_2_128_Y) */
    #if !defined(ED25519_BASEPOINT_TIMES_2_128_Z)
        #define ED25519_BASEPOINT_TIMES_2_128_Z  { 1, 0, 0, 0, 0 }
    #endif /* !defined(ED25519_BASEPOINT_TIMES_2_128_Z) */
    #if !defined(ED25519_BASEPOINT_TIMES_2_128_T)
        #define ED25519_BASEPOINT_TIMES_2_128_T  { 0x731c946f1338a, 0x4ccc75b53d3c, 0x6492da851e192, 0x17ea70ab0003, 0x5a164fd0bf46 }
    #endif /* !defined(ED25519_BASEPOINT_TIMES_2_128_T) */

    #if !defined(ED25519_BASEPOINT_TIMES_2_128_PLUS_1_X)
        #define ED25519_BASEPOINT_TIMES_2_128_PLUS_1_X  { 0x2fd29680c969a, 0x1063d979cdf7c, 0x30d814ec39bb0, 0x247c559e0df46, 0x6e64e5552b88e }
    #endif /* !defined(ED25519_BASEPOINT_TIMES_2_128_PLUS_1_X) */
    #if !defined(ED25519_BASEPOINT_TIMES_2_128_PLUS_1_Y)
        #define ED25519_BASEPOINT_TIMES_2_128_PLUS_1_Y  { 0x52a647bafd09b, 0x313d8aa4204a6, 0x6e14514d8e445, 0x45f354efe499c, 0x383c510fa4cb5 }
    #endif /* !defined(ED25519_BASEPOINT_TIMES_2_128_PLUS_1_Y) */
    #if !defined(ED25519_BASEPOINT_TIMES_2_128_PLUS_1_Z)
        #define ED25519_BASEPOINT_TIMES_2_128_PLUS_1_Z  { 1, 0, 0, 0, 0 }
    #endif /* !defined(ED25519_BASEPOINT_TIMES_2_128_PLUS_1_Z) */
    #if !defined(ED25519_BASEPOINT_TIMES_2_128_PLUS_1_T)
        #define ED25519_BASEPOINT_TIMES_2_128_PLUS_1_T  { 0xb0bc814683a9, 0x70f0771ac1427, 0x6e4d6450ff949, 0x47cabdf975c54, 0x475c61f85104d }
    #endif /* !defined(ED25519_BASEPOINT_TIMES_2_128_PLUS_1_T) */
#else
    #error "Architecture not defined. Set exactly one of FE3C_32BIT or FE3C_64BIT"
#endif /* FE3C_32BIT && FE3C_64BIT */

/* Elliptic curve constant d = -121665/121666 */
extern const fe25519 ed25519_d;
/* Additive identity in the field */
extern const fe25519 fe25519_zero;
/* Multiplicative identity in the field */
extern const fe25519 fe25519_one;
/* "Positive" (even) square root of -1 in the field */
extern const fe25519 fe25519_i;

/**
 * @brief Uncondionally copy a field element
 * @param r Destination field element
 * @param a Source field element
 */
void fe25519_copy(fe25519 r, const fe25519 a);

/**
 * @brief Do a constant-time equality check
 * @param[in] a The first element to check
 * @param[in] b The second element to check
 * @return 1 if a = b, 0 otherwise
 * @note The elements should be reduced by the caller first
 */
int fe25519_equal(const fe25519 a, const fe25519 b);

/**
 * @brief Do a constant-time conditional move
 * @param[out] r The result field element set to a conditionally or left alone
 * @param[in] a The element to conditionally move into r
 * @param[in] move Flag deciding on the branch, if set to 0, r ::= r, and if set to 1, r ::= a
 * @note If move is set to anything other than 0 or 1, the results are undefined
 */
void fe25519_conditional_move(fe25519 r, const fe25519 a, int move);

/**
 * @brief Do a weak modular reduction modulo the underlying field's characteristic p
 * @param[out] r Result of the reduction
 * @param[in] a Field element to be reduced
 * @note Note that the result need to be in canonical form, i.e. between 0 and p-1, it need only be less than 2p
 */
void fe25519_weak_reduce(fe25519 r, const fe25519 a);

/**
 * @brief Do a strong modular reduction modulo the underlying field's characteristic p
 * @param[out] r Result of the reduction
 * @param[in] a Field element to be reduced
 * @note The result is guaranteed to be in canonical form, i.e. between 0 and p-1
 */
void fe25519_strong_reduce(fe25519 r, const fe25519 a);

/**
 * @brief Negate an element in the field
 * @param[out] r The result of negation
 * @param[in] a Element to be negated
 */
void fe25519_neg(fe25519 r, const fe25519 a);

/**
 * @brief Add two field elements
 * @param[out] r Result of the addition, i.e. the sum r = a + b
 * @param[in] a Operand
 * @param[in] b Operand
 */
void fe25519_add(fe25519 r, const fe25519 a, const fe25519 b);

/**
 * @brief Subtract one field element from another
 * @param[out] r Result of the subtraction, i.e. the difference r = a - b
 * @param[in] a Minuend
 * @param[in] b Subtrahend
 */
void fe25519_sub(fe25519 r, const fe25519 a, const fe25519 b);

/**
 * @brief Multiply two field elements
 * @param[out] r Result of the multiplication, i.e. the product r = a b
 * @param[in] a Operand
 * @param[in] b Operand
 */
void fe25519_mul(fe25519 r, const fe25519 a, const fe25519 b);

/**
 * @brief Multiply an element by itself
 * @param[out] r Result of the squaring, i.e. the product r = a a
 * @param[in] a Field element to square
 */
void fe25519_square(fe25519 r, const fe25519 a);

/**
 * @brief Encode a field element according to RFC 8032
 * @param[out] buffer Output buffer for the encoded field element
 * @param[in] a Field element to encode
 */
void fe25519_encode(u8 *buffer, fe25519 a);

/**
 * @brief Decode a field element according to RFC 8032
 * @param[out] r Decoded field element
 * @param[out] buffer Encoding of the field element
 * @return 1 if decoding succeeded, 0 otherwise
 */
__attribute__((warn_unused_result))
int fe25519_decode(fe25519 r, const u8 *buffer);

/**
 * @brief Find a multiplicative inverse of a field element
 * @param[out] r Result of inversion, i.e. such element r that r a = 1
 * @param[in] a Field element to invert
 */
static inline void fe25519_invert(fe25519 r, const fe25519 a)
{
    /* Make a local copy to support aliasing */
    fe25519 x;
    fe25519_copy(x, a);

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

    /* Initialize r with the initial squaring and multiplication (this covers
     * the two leftmost ones in the exponent - since the first one corresponds
     * to a no-op) */
    fe25519_square(r, x);
    fe25519_mul(r, r, x);

    /* Do a square-and-multiply step for the remaining 248 ones in the binary
     * representation of the exponent (p-2) */
    for (int i = 0; i < 248; i++) {

        fe25519_square(r, r);
        fe25519_mul(r, r, x);
    }

    /* Zero in the exponent - just a squaring, with no multiplication */
    fe25519_square(r, r);

    /* One in the exponent - do both a squaring and a multiplication */
    fe25519_square(r, r);
    fe25519_mul(r, r, x);

    /* Zero in the exponent - only square */
    fe25519_square(r, r);

    /* Two last ones in the exponent - do both a squaring and a multiplication */
    fe25519_square(r, r);
    fe25519_mul(r, r, x);
    fe25519_square(r, r);
    fe25519_mul(r, r, x);
}

/**
 * @brief Raise an element to the power (p-5)/8 where p = 2^255 - 19 is the field modulus
 * @param[out] r Result of the exponentation
 * @param[in] a The element to exponentiate
 */
static inline void fe25519_exp_p_minus_5_over_8(fe25519 r, const fe25519 a)
{
    /* Make a local copy to support aliasing */
    fe25519 x;
    fe25519_copy(x, a);

    /* Note that (p-5)/8 is equal to (2^255-24)/8 = 2^252-3, which has the following
     * hexadecimal representation:
     *
     * (p-5)/8 = 0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffd
     *
     * i.e. it is a sequence of 248 ones followed by 0xd = 0b1101 (250 consecutive
     * ones in total). Use a square-and-multiply algorithm to obtain the result.
     */

    /* Initialize r with the initial squaring and multiplication (this covers
     * the two leftmost ones in the exponent - since the first one corresponds
     * to a no-op) */
    fe25519_square(r, x);
    fe25519_mul(r, r, x);

    /* Do a square-and-multiply step for the remaining 248 ones in the binary
     * representation of the exponent */
    for (int i = 0; i < 248; i++) {

        fe25519_square(r, r);
        fe25519_mul(r, r, x);
    }

    /* Zero in the exponent - just a squaring, with no multiplication */
    fe25519_square(r, r);

    /* One in the exponent - do both a squaring and a multiplication */
    fe25519_square(r, r);
    fe25519_mul(r, r, x);
}

/**
 * @brief Return the least-significant bit of the field element
 * @param a Field element
 * @return Least-significant bit of a
 */
static inline int fe25519_lsb(fe25519 a)
{
    fe25519_strong_reduce(a, a);
    return a[0] & 1;
}

#ifdef __cplusplus
}
#endif

#endif /* __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_ED25519_H */
