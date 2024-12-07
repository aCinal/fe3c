
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
    #if !defined(FE448_STR)
        #define FE448_STR \
            "0x%" PRIx32 ", 0x%" PRIx32 ", 0x%" PRIx32 ", 0x%" PRIx32 ", " \
            "0x%" PRIx32 ", 0x%" PRIx32 ", 0x%" PRIx32 ", 0x%" PRIx32 ", " \
            "0x%" PRIx32 ", 0x%" PRIx32 ", 0x%" PRIx32 ", 0x%" PRIx32 ", " \
            "0x%" PRIx32 ", 0x%" PRIx32 ", 0x%" PRIx32 ", 0x%" PRIx32
    #endif /* !defined(FE448_STR) */
    #if !defined(FE448_TO_STR)
        #define FE448_TO_STR(x) \
            x[ 0], x[ 1], x[ 2], x[ 3], \
            x[ 4], x[ 5], x[ 6], x[ 7], \
            x[ 8], x[ 9], x[10], x[11], \
            x[12], x[13], x[14], x[15]
    #endif /* !defined(FE448_TO_STR) */

    #if !defined(ED448_BASEPOINT_X)
        #define ED448_BASEPOINT_X  { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x8000000, 0xffffffe, 0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff, 0x7ffffff }
    #endif /* !defined(ED448_BASEPOINT_X) */
    #if !defined(ED448_BASEPOINT_Y)
        #define ED448_BASEPOINT_Y  { 0xfdd4a64, 0x6079b4d, 0x470a1c8, 0xc1e3ab, 0x8e5199b, 0x44d73f4, 0x4141818, 0x5045271, 0x3d5242c, 0x4c74c39, 0x6437050, 0x2408052, 0x13078ca, 0xd48d06c, 0x4f04286, 0x8508de1 }
    #endif /* !defined(ED448_BASEPOINT_Y) */
    #if !defined(ED448_BASEPOINT_Z)
        #define ED448_BASEPOINT_Z  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    #endif /* !defined(ED448_BASEPOINT_Z) */
    #if !defined(ED448_BASEPOINT_T)
        #define ED448_BASEPOINT_T  { 0xc198105, 0xe3c816d, 0x3f4e093, 0x6207183, 0x3421403, 0x4dde98e, 0x519c985, 0xa319b57, 0x6382384, 0x794be95, 0x86da60f, 0xe1ddc2b, 0x682a9ff, 0x50e23d5, 0x173c6a4, 0x6d3669e }
    #endif /* !defined(ED448_BASEPOINT_T) */

    #if !defined(ED448_BASEPOINT_TIMES_2_225_X)
        #define ED448_BASEPOINT_TIMES_2_225_X  { 0xfb672fb, 0xcbff257, 0x4b76d0f, 0x7a7fa27, 0xdc438c3, 0xa8e4afe, 0x96b29fa, 0x2a17d02, 0x2288bca, 0x6eb28cf, 0x88c4255, 0x61a9f62, 0x2a94bcf, 0x2a5459d, 0x89abe2d, 0xa7779d }
    #endif /* !defined(ED448_BASEPOINT_TIMES_2_225_X) */
    #if !defined(ED448_BASEPOINT_TIMES_2_225_Y)
        #define ED448_BASEPOINT_TIMES_2_225_Y  { 0x9c557f9, 0x9c8d696, 0x495f462, 0xc84ccc, 0x8c22423, 0x316eb91, 0x5ebe2d9, 0x3e7cd2e, 0x4aafa9f, 0xca4e43f, 0xf825196, 0x7fd244c, 0xd813995, 0x8c6f5c9, 0x783b008, 0xc4dd09e }
    #endif /* !defined(ED448_BASEPOINT_TIMES_2_225_Y) */
    #if !defined(ED448_BASEPOINT_TIMES_2_225_Z)
        #define ED448_BASEPOINT_TIMES_2_225_Z  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    #endif /* !defined(ED448_BASEPOINT_TIMES_2_225_Z) */
    #if !defined(ED448_BASEPOINT_TIMES_2_225_T)
        #define ED448_BASEPOINT_TIMES_2_225_T  { 0xb18a80e, 0x8d74c90, 0x14fc6a9, 0x23fa368, 0x151ae30, 0x3792843, 0x2cb6a9, 0x6a96147, 0x2122f3a, 0x5d04db, 0xb1bcc5d, 0x30749a1, 0x78e6b56, 0x2b37d5c, 0xed1f11f, 0xfd6b2bd }
    #endif /* !defined(ED448_BASEPOINT_TIMES_2_225_T) */

    #if !defined(ED448_BASEPOINT_TIMES_2_225_PLUS_1_X)
        #define ED448_BASEPOINT_TIMES_2_225_PLUS_1_X  { 0x8c2abe3, 0x7a801f5, 0x4455c86, 0x91c6a6f, 0x50b5e31, 0xd3ff2c2, 0x377b768, 0x709f1c, 0x664c158, 0x6c3602a, 0xa675ef8, 0xb7c8592, 0x8779893, 0x85aa129, 0x5936077, 0xdbaca71 }
    #endif /* !defined(ED448_BASEPOINT_TIMES_2_225_PLUS_1_X) */
    #if !defined(ED448_BASEPOINT_TIMES_2_225_PLUS_1_Y)
        #define ED448_BASEPOINT_TIMES_2_225_PLUS_1_Y  { 0xcd14278, 0xfb8a33e, 0x7bd597e, 0x4321353, 0xf90a5f9, 0x9b4b28d, 0x65dd355, 0xb878171, 0xc3c75f, 0x2cdd60b, 0xfc84b1a, 0xf548750, 0xfc82e0d, 0x5b0b5a6, 0x4887031, 0xe9f8be3 }
    #endif /* !defined(ED448_BASEPOINT_TIMES_2_225_PLUS_1_Y) */
    #if !defined(ED448_BASEPOINT_TIMES_2_225_PLUS_1_Z)
        #define ED448_BASEPOINT_TIMES_2_225_PLUS_1_Z  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    #endif /* !defined(ED448_BASEPOINT_TIMES_2_225_PLUS_1_Z) */
    #if !defined(ED448_BASEPOINT_TIMES_2_225_PLUS_1_T)
        #define ED448_BASEPOINT_TIMES_2_225_PLUS_1_T  { 0x9f738df, 0x3afeb47, 0x5673590, 0xec8c77c, 0x4dc60e3, 0x6e114bc, 0x9a7b037, 0x64d084f, 0x939537f, 0x6e145db, 0xdafc480, 0x3be4c02, 0x13bacc2, 0x8e46e01, 0x1c22bc, 0x377d0c1 }
    #endif /* !defined(ED448_BASEPOINT_TIMES_2_225_PLUS_1_T) */

#elif FE3C_64BIT
    #if !defined(FE448_STR)
        #define FE448_STR \
            "0x%" PRIx64 ", 0x%" PRIx64 ", 0x%" PRIx64 ", 0x%" PRIx64 ", " \
            "0x%" PRIx64 ", 0x%" PRIx64 ", 0x%" PRIx64 ", 0x%" PRIx64
    #endif /* !defined(FE448_STR) */
    #if !defined(FE448_TO_STR)
        #define FE448_TO_STR(x) \
            x[0], x[1], x[2], x[3], \
            x[4], x[5], x[6], x[7]
    #endif /* !defined(FE448_TO_STR) */

    #if !defined(ED448_BASEPOINT_X)
        #define ED448_BASEPOINT_X  { 0x0, 0x0, 0x0, 0x80000000000000, 0xfffffffffffffe, 0xffffffffffffff, 0xffffffffffffff, 0x7fffffffffffff }
    #endif /* !defined(ED448_BASEPOINT_X) */
    #if !defined(ED448_BASEPOINT_Y)
        #define ED448_BASEPOINT_Y  { 0x6079b4dfdd4a64, 0xc1e3ab470a1c8, 0x44d73f48e5199b, 0x50452714141818, 0x4c74c393d5242c, 0x24080526437050, 0xd48d06c13078ca, 0x8508de14f04286 }
    #endif /* !defined(ED448_BASEPOINT_Y) */
    #if !defined(ED448_BASEPOINT_Z)
        #define ED448_BASEPOINT_Z  { 1, 0, 0, 0, 0, 0, 0, 0 }
    #endif /* !defined(ED448_BASEPOINT_Z) */
    #if !defined(ED448_BASEPOINT_T)
        #define ED448_BASEPOINT_T  { 0xe3c816dc198105, 0x62071833f4e093, 0x4dde98e3421403, 0xa319b57519c985, 0x794be956382384, 0xe1ddc2b86da60f, 0x50e23d5682a9ff, 0x6d3669e173c6a4 }
    #endif /* !defined(ED448_BASEPOINT_T) */

    #if !defined(ED448_BASEPOINT_TIMES_2_225_X)
        #define ED448_BASEPOINT_TIMES_2_225_X  { 0xcbff257fb672fb, 0x7a7fa274b76d0f, 0xa8e4afedc438c3, 0x2a17d0296b29fa, 0x6eb28cf2288bca, 0x61a9f6288c4255, 0x2a5459d2a94bcf, 0xa7779d89abe2d }
    #endif /* !defined(ED448_BASEPOINT_TIMES_2_225_X) */
    #if !defined(ED448_BASEPOINT_TIMES_2_225_Y)
        #define ED448_BASEPOINT_TIMES_2_225_Y  { 0x9c8d6969c557f9, 0xc84ccc495f462, 0x316eb918c22423, 0x3e7cd2e5ebe2d9, 0xca4e43f4aafa9f, 0x7fd244cf825196, 0x8c6f5c9d813995, 0xc4dd09e783b008 }
    #endif /* !defined(ED448_BASEPOINT_TIMES_2_225_Y) */
    #if !defined(ED448_BASEPOINT_TIMES_2_225_Z)
        #define ED448_BASEPOINT_TIMES_2_225_Z  { 1, 0, 0, 0, 0, 0, 0, 0 }
    #endif /* !defined(ED448_BASEPOINT_TIMES_2_225_Z) */
    #if !defined(ED448_BASEPOINT_TIMES_2_225_T)
        #define ED448_BASEPOINT_TIMES_2_225_T  { 0x8d74c90b18a80e, 0x23fa36814fc6a9, 0x3792843151ae30, 0x6a9614702cb6a9, 0x5d04db2122f3a, 0x30749a1b1bcc5d, 0x2b37d5c78e6b56, 0xfd6b2bded1f11f }
    #endif /* !defined(ED448_BASEPOINT_TIMES_2_225_T) */

    #if !defined(ED448_BASEPOINT_TIMES_2_225_PLUS_1_X)
        #define ED448_BASEPOINT_TIMES_2_225_PLUS_1_X  { 0x7a801f58c2abe3, 0x91c6a6f4455c86, 0xd3ff2c250b5e31, 0x709f1c377b768, 0x6c3602a664c158, 0xb7c8592a675ef8, 0x85aa1298779893, 0xdbaca715936077 }
    #endif /* !defined(ED448_BASEPOINT_TIMES_2_225_PLUS_1_X) */
    #if !defined(ED448_BASEPOINT_TIMES_2_225_PLUS_1_Y)
        #define ED448_BASEPOINT_TIMES_2_225_PLUS_1_Y  { 0xfb8a33ecd14278, 0x43213537bd597e, 0x9b4b28df90a5f9, 0xb87817165dd355, 0x2cdd60b0c3c75f, 0xf548750fc84b1a, 0x5b0b5a6fc82e0d, 0xe9f8be34887031 }
    #endif /* !defined(ED448_BASEPOINT_TIMES_2_225_PLUS_1_Y) */
    #if !defined(ED448_BASEPOINT_TIMES_2_225_PLUS_1_Z)
        #define ED448_BASEPOINT_TIMES_2_225_PLUS_1_Z  { 1, 0, 0, 0, 0, 0, 0, 0 }
    #endif /* !defined(ED448_BASEPOINT_TIMES_2_225_PLUS_1_Z) */
    #if !defined(ED448_BASEPOINT_TIMES_2_225_PLUS_1_T)
        #define ED448_BASEPOINT_TIMES_2_225_PLUS_1_T  { 0x3afeb479f738df, 0xec8c77c5673590, 0x6e114bc4dc60e3, 0x64d084f9a7b037, 0x6e145db939537f, 0x3be4c02dafc480, 0x8e46e0113bacc2, 0x377d0c101c22bc }
    #endif /* !defined(ED448_BASEPOINT_TIMES_2_225_PLUS_1_T) */
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
void fe448_encode(u8 *buffer, fe448 a);

/**
 * @brief Decode a field element according to RFC 8032
 * @param[out] r Decoded field element
 * @param[out] buffer Encoding of the field element
 * @return 1 if decoding succeeded, 0 otherwise
 */
__attribute__((warn_unused_result))
int fe448_decode(fe448 r, const u8 *buffer);

/**
 * @brief Find a multiplicative inverse of a field element
 * @param[out] r Result of inversion, i.e. such element r that r a = 1
 * @param[in] a Field element to invert
 */
static inline void fe448_invert(fe448 r, const fe448 a)
{
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
static inline void fe448_exp_p_minus_3_over_4(fe448 r, const fe448 a)
{
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
static inline int fe448_lsb(fe448 a)
{
    fe448_strong_reduce(a, a);
    return a[0] & 1;
}

#ifdef __cplusplus
}
#endif

#endif /* __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_ED448_H */
