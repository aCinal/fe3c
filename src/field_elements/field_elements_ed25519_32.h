
#ifndef __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_ED25519_32_H
#define __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_ED25519_32_H

#ifdef __cplusplus
extern "C" {
#endif

#include <field_elements/field_elements.h>
#include <utils/utils.h>

#if !FE3C_32BIT
    #error "Build system inconsistency detected! field_elements_ed25519_32.h in use despite FE3C_32BIT not being set"
#endif /* !FE3C_32BIT */

/* Use a mixed-radix representation, the first (0th) limb is 26-bits long, the next is 25-bits long, the one after
 * that - 26-bits long - and so on. */
#define LOW_25_BITS_MASK  0x1ffffffU
#define LOW_26_BITS_MASK  0x3ffffffU

/* Elliptic curve constant d = -121665/121666 */
static const fe ed25519_d = {
    .ed25519 = {
        0x35978a3, 0xd37284, 0x3156ebd, 0x6a0a0e, 0x1c029, 0x179e898, 0x3a03cbb, 0x1ce7198, 0x2e2b6ff, 0x1480db3
    }
};
/* Additive identity in the field */
static const fe fe_zero = { .ed25519 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
/* Multiplicative identity in the field */
static const fe fe_one = { .ed25519 = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
/* "Positive" (even) square root of -1 in the field */
static const fe fe_i = {
    .ed25519 = {
        0x20ea0b0, 0x186c9d2, 0x8f189d, 0x35697f, 0xbd0c60, 0x1fbd7a7, 0x2804c9e, 0x1e16569, 0x4fc1d, 0xae0c92
    }
};

static inline u32 _load_32(const u8 src[4]) {

    /* Integer encoding is always little endian according to RFC 8032 */
    u32 dst;
#if FE3C_LILENDIAN_TARGET
    /* Target already little endian - copy the bytes with no shifts */
    (void) memcpy(&dst, src, 4);
#else
    /* Big-endian target or endianness unknown (take the safe route) */
    dst  = (u32) src[0];
    dst |= (u32) src[1] << 8;
    dst |= (u32) src[2] << 16;
    dst |= (u32) src[3] << 24;
#endif
    return dst;
}

static inline void _store_32(u8 dst[4], u32 src) {

    /* Integer encoding is always little endian according to RFC 8032 */
#if FE3C_LILENDIAN_TARGET
    (void) memcpy(dst, &src, 4);
#else
    /* After each write do a shift (src is a local variable) */
    dst[0] = (u8) src;  src >>= 8;
    dst[1] = (u8) src;  src >>= 8;
    dst[2] = (u8) src;  src >>= 8;
    dst[3] = (u8) src;
#endif
}

/**
 * @brief Check if an elements is in its canonical form, i.e. less than the modulus
 * @param a Field element to check
 * @return 1 if a is in canonical form, 0 otherwise
 */
static inline int fe_is_canonical(const fe * a) {

    int canonical = 1;
    canonical &= (a->ed25519[0] <  0x3ffffed);
    canonical &= (a->ed25519[1] <= 0x1ffffff);
    canonical &= (a->ed25519[2] <= 0x3ffffff);
    canonical &= (a->ed25519[3] <= 0x1ffffff);
    canonical &= (a->ed25519[4] <= 0x3ffffff);
    canonical &= (a->ed25519[5] <= 0x1ffffff);
    canonical &= (a->ed25519[6] <= 0x3ffffff);
    canonical &= (a->ed25519[7] <= 0x1ffffff);
    canonical &= (a->ed25519[8] <= 0x3ffffff);
    canonical &= (a->ed25519[9] <= 0x1ffffff);
    return canonical;
}

/**
 * @brief Do a constant-time equality check
 * @param[in] a The first element to check
 * @param[in] b The second element to check
 * @return 1 if a = b, 0 otherwise
 * @note The elements should be reduced by the caller first
 */
static inline int fe_equal(const fe * a, const fe * b) {

    fe_limb_type sum = 0;

    /* Do an XOR between the two elements, if they are equal this should amount
     * to zero */
    sum |= a->ed25519[0] ^ b->ed25519[0];
    sum |= a->ed25519[1] ^ b->ed25519[1];
    sum |= a->ed25519[2] ^ b->ed25519[2];
    sum |= a->ed25519[3] ^ b->ed25519[3];
    sum |= a->ed25519[4] ^ b->ed25519[4];
    sum |= a->ed25519[5] ^ b->ed25519[5];
    sum |= a->ed25519[6] ^ b->ed25519[6];
    sum |= a->ed25519[7] ^ b->ed25519[7];
    sum |= a->ed25519[8] ^ b->ed25519[8];
    sum |= a->ed25519[9] ^ b->ed25519[9];

    /* Bring any 1's down to the least significant bit */
    sum |= (sum >> 16);
    sum |= (sum >> 8);
    sum |= (sum >> 4);
    sum |= (sum >> 2);
    sum |= (sum >> 1);

    /* If the least significant bit is 1 at this point, a != b,
     * otherwise a = b */
    return (sum ^ 1) & 1;
}

/**
 * @brief Do a constant-time conditional move
 * @param[out] r The result field element set to a conditionally or left alone
 * @param[in] a The element to conditionally move into r
 * @param[in] move Flag deciding on the branch, if set to 0, r ::= r, and if set to 1, r ::= a
 * @note If move is set to anything other than 0 or 1, the results are undefined
 */
static inline void fe_conditional_move(fe * r, const fe * a, int move) {

    /* Set the mask to 0x00000000 if move is 0 or to 0xFFFFFFFF if it is 1 */
    const fe_limb_type mask = (fe_limb_type)( -(i32) move );

    /* The conditional move implementation relies on the identity (x ^ y) ^ y = x */

    /* Copy the current contents of r into the t array */
    fe_limb_type t0 = r->ed25519[0];
    fe_limb_type t1 = r->ed25519[1];
    fe_limb_type t2 = r->ed25519[2];
    fe_limb_type t3 = r->ed25519[3];
    fe_limb_type t4 = r->ed25519[4];
    fe_limb_type t5 = r->ed25519[5];
    fe_limb_type t6 = r->ed25519[6];
    fe_limb_type t7 = r->ed25519[7];
    fe_limb_type t8 = r->ed25519[8];
    fe_limb_type t9 = r->ed25519[9];

    /* XOR the t's with the limbs of a */
    fe_limb_type x0 = t0 ^ a->ed25519[0];
    fe_limb_type x1 = t1 ^ a->ed25519[1];
    fe_limb_type x2 = t2 ^ a->ed25519[2];
    fe_limb_type x3 = t3 ^ a->ed25519[3];
    fe_limb_type x4 = t4 ^ a->ed25519[4];
    fe_limb_type x5 = t5 ^ a->ed25519[5];
    fe_limb_type x6 = t6 ^ a->ed25519[6];
    fe_limb_type x7 = t7 ^ a->ed25519[7];
    fe_limb_type x8 = t8 ^ a->ed25519[8];
    fe_limb_type x9 = t9 ^ a->ed25519[9];

    /* Set the x array to all zeroes if move is zero or leave it be if move is one */
    x0 &= mask;
    x1 &= mask;
    x2 &= mask;
    x3 &= mask;
    x4 &= mask;
    x5 &= mask;
    x6 &= mask;
    x7 &= mask;
    x8 &= mask;
    x9 &= mask;

    /* If move=0 then x0-x9 are all zero and so we write back the limbs of r into r.
     * If move=1 then x0-x9 contain r^a and so by XORing back with the limbs of r we
     * write the limbs of a into r */
    r->ed25519[0] = t0 ^ x0;
    r->ed25519[1] = t1 ^ x1;
    r->ed25519[2] = t2 ^ x2;
    r->ed25519[3] = t3 ^ x3;
    r->ed25519[4] = t4 ^ x4;
    r->ed25519[5] = t5 ^ x5;
    r->ed25519[6] = t6 ^ x6;
    r->ed25519[7] = t7 ^ x7;
    r->ed25519[8] = t8 ^ x8;
    r->ed25519[9] = t9 ^ x9;
}

/**
 * @brief Do a weak modular reduction modulo the underlying field's characteristic p
 * @param[out] r Result of the reduction
 * @param[in] a Field element to be reduced
 * @note Note that the result need to be in canonical form, i.e. between 0 and p-1, it need only be less than 2p
 */
static inline void fe_weak_reduce(fe * r, const fe * a) {

    /* Do a "relaxed" reduction (to borrow terminology form Michael Scott's "Slothful reduction" paper)
     * - this ensures the result is less than 2p (where p = 2^255 - 19) */

    /* Use 64-bit-wide auxiliary variables */
    u64 t0 = a->ed25519[0];
    u64 t1 = a->ed25519[1];
    u64 t2 = a->ed25519[2];
    u64 t3 = a->ed25519[3];
    u64 t4 = a->ed25519[4];
    u64 t5 = a->ed25519[5];
    u64 t6 = a->ed25519[6];
    u64 t7 = a->ed25519[7];
    u64 t8 = a->ed25519[8];
    u64 t9 = a->ed25519[9];

    /* The modulus can be expressed as p = s - c, where s = 2^255 and c = 19.
     * Express (split) the value a as a = x + sy. Then
     *
     *     a mod p = (x + sy) mod (s - c)
     *             = (x + (s - c)y + cy) mod (s - c)
     *             = (x + cy) mod m
     *
     * To split a into (x, y) we simply let the carries bubble up to the top
     * limb. Whatever has overflown the top limb is equal to y and in the limbs
     * we have the value x.
     */

    /* Add whatever has overflown in t0 (a->ed25519[0]) to t1 (a->ed25519[1])
     * and mask out the overflow in t0 */
    t1 += t0 >> 26;  t0 &= LOW_26_BITS_MASK;
    /* Rinse and repeat for the other limbs */
    t2 += t1 >> 25;  t1 &= LOW_25_BITS_MASK;
    t3 += t2 >> 26;  t2 &= LOW_26_BITS_MASK;
    t4 += t3 >> 25;  t3 &= LOW_25_BITS_MASK;
    t5 += t4 >> 26;  t4 &= LOW_26_BITS_MASK;
    t6 += t5 >> 25;  t5 &= LOW_25_BITS_MASK;
    t7 += t6 >> 26;  t6 &= LOW_26_BITS_MASK;
    t8 += t7 >> 25;  t7 &= LOW_25_BITS_MASK;
    t9 += t8 >> 26;  t8 &= LOW_26_BITS_MASK;
    /* Add c*y to x */
    t0 += 19 * (t9 >> 25);
    t9 &= LOW_25_BITS_MASK;

    /* Do this once more to ensure we correctly normalize
     * all integers possible to be represented in our system,
     * i.e. we don't get surprised by 19 * (t9 >> 25) leading
     * to overflow in t0 */
    t1 += t0 >> 26;  t0 &= LOW_26_BITS_MASK;
    t2 += t1 >> 25;  t1 &= LOW_25_BITS_MASK;
    t3 += t2 >> 26;  t2 &= LOW_26_BITS_MASK;
    t4 += t3 >> 25;  t3 &= LOW_25_BITS_MASK;
    t5 += t4 >> 26;  t4 &= LOW_26_BITS_MASK;
    t6 += t5 >> 25;  t5 &= LOW_25_BITS_MASK;
    t7 += t6 >> 26;  t6 &= LOW_26_BITS_MASK;
    t8 += t7 >> 25;  t7 &= LOW_25_BITS_MASK;
    t9 += t8 >> 26;  t8 &= LOW_26_BITS_MASK;
    t0 += 19 * (t9 >> 25);
    t9 &= LOW_25_BITS_MASK;

    r->ed25519[0] = t0;
    r->ed25519[1] = t1;
    r->ed25519[2] = t2;
    r->ed25519[3] = t3;
    r->ed25519[4] = t4;
    r->ed25519[5] = t5;
    r->ed25519[6] = t6;
    r->ed25519[7] = t7;
    r->ed25519[8] = t8;
    r->ed25519[9] = t9;
}

/**
 * @brief Do a strong modular reduction modulo the underlying field's characteristic p
 * @param[out] r Result of the reduction
 * @param[in] a Field element to be reduced
 * @note The result is guaranteed to be in canonical form, i.e. between 0 and p-1
 */
static inline void fe_strong_reduce(fe * r, const fe * a) {

    fe_weak_reduce(r, a);
    /* After the weak reduction r is congruent to a and less than 2p */

    /* Compute r-p and conditionally use it as a result if r is larger than p */
    fe t;
    /* Store r + 19 in t (we will subtract 2^255 from it resulting in t = r - p) */
    t.ed25519[0] = r->ed25519[0] + 19;
    t.ed25519[1] = r->ed25519[1];
    t.ed25519[2] = r->ed25519[2];
    t.ed25519[3] = r->ed25519[3];
    t.ed25519[4] = r->ed25519[4];
    t.ed25519[5] = r->ed25519[5];
    t.ed25519[6] = r->ed25519[6];
    t.ed25519[7] = r->ed25519[7];
    t.ed25519[8] = r->ed25519[8];
    t.ed25519[9] = r->ed25519[9];

    t.ed25519[1] += t.ed25519[0] >> 26;  t.ed25519[0] &= LOW_26_BITS_MASK;
    t.ed25519[2] += t.ed25519[1] >> 25;  t.ed25519[1] &= LOW_25_BITS_MASK;
    t.ed25519[3] += t.ed25519[2] >> 26;  t.ed25519[2] &= LOW_26_BITS_MASK;
    t.ed25519[4] += t.ed25519[3] >> 25;  t.ed25519[3] &= LOW_25_BITS_MASK;
    t.ed25519[5] += t.ed25519[4] >> 26;  t.ed25519[4] &= LOW_26_BITS_MASK;
    t.ed25519[6] += t.ed25519[5] >> 25;  t.ed25519[5] &= LOW_25_BITS_MASK;
    t.ed25519[7] += t.ed25519[6] >> 26;  t.ed25519[6] &= LOW_26_BITS_MASK;
    t.ed25519[8] += t.ed25519[7] >> 25;  t.ed25519[7] &= LOW_25_BITS_MASK;
    t.ed25519[9] += t.ed25519[8] >> 26;  t.ed25519[8] &= LOW_26_BITS_MASK;

    /* At this point t.ed25519[9] contains the highest limb of r + 19 (extended to 128 bits). Try
     * subtracting 2^255 - if we get an underflow this means that r < 2^255 - 19 and so r is the
     * final result. Otherwise we must return r - p (note that this includes the case where r = p,
     * as no underflow will occur then and t.ed25519[4] will be equal to zero). */
    t.ed25519[9] -= (1ULL << 25);

    /* Check the highest bit of t.ed25519[4] for underflow. If the highest bit is set then
     * underflow occurred and so we return r, otherwise we set r ::= t and return that */
    fe_conditional_move(r, &t, (t.ed25519[9] >> 31) ^ 1);
}

/**
 * @brief Negate an element in the field
 * @param[out] r The result of negation
 * @param[in] a Element to be negated
 */
static inline void fe_neg(fe * r, const fe * a) {

    /* Check against underflow */
    FE3C_SANITY_CHECK(a->ed25519[0] <= 0x7ffffda);
    FE3C_SANITY_CHECK(a->ed25519[1] <= 0x3fffffe);
    FE3C_SANITY_CHECK(a->ed25519[2] <= 0x7fffffe);
    FE3C_SANITY_CHECK(a->ed25519[3] <= 0x3fffffe);
    FE3C_SANITY_CHECK(a->ed25519[4] <= 0x7fffffe);
    FE3C_SANITY_CHECK(a->ed25519[5] <= 0x3fffffe);
    FE3C_SANITY_CHECK(a->ed25519[6] <= 0x7fffffe);
    FE3C_SANITY_CHECK(a->ed25519[7] <= 0x3fffffe);
    FE3C_SANITY_CHECK(a->ed25519[8] <= 0x7fffffe);
    FE3C_SANITY_CHECK(a->ed25519[9] <= 0x3fffffe);

    /* Set r to 2p-a so as to not require strong reduction of a */
    r->ed25519[0] = 0x7ffffda - a->ed25519[0];
    r->ed25519[1] = 0x3fffffe - a->ed25519[1];
    r->ed25519[2] = 0x7fffffe - a->ed25519[2];
    r->ed25519[3] = 0x3fffffe - a->ed25519[3];
    r->ed25519[4] = 0x7fffffe - a->ed25519[4];
    r->ed25519[5] = 0x3fffffe - a->ed25519[5];
    r->ed25519[6] = 0x7fffffe - a->ed25519[6];
    r->ed25519[7] = 0x3fffffe - a->ed25519[7];
    r->ed25519[8] = 0x7fffffe - a->ed25519[8];
    r->ed25519[9] = 0x3fffffe - a->ed25519[9];
}

/**
 * @brief Add two field elements
 * @param[out] r Result of the addition, i.e. the sum r = a + b
 * @param[in] a Operand
 * @param[in] b Operand
 */
static inline void fe_add(fe * r, const fe * a, const fe * b) {

    r->ed25519[0] = a->ed25519[0] + b->ed25519[0];
    r->ed25519[1] = a->ed25519[1] + b->ed25519[1];
    r->ed25519[2] = a->ed25519[2] + b->ed25519[2];
    r->ed25519[3] = a->ed25519[3] + b->ed25519[3];
    r->ed25519[4] = a->ed25519[4] + b->ed25519[4];
    r->ed25519[5] = a->ed25519[5] + b->ed25519[5];
    r->ed25519[6] = a->ed25519[6] + b->ed25519[6];
    r->ed25519[7] = a->ed25519[7] + b->ed25519[7];
    r->ed25519[8] = a->ed25519[8] + b->ed25519[8];
    r->ed25519[9] = a->ed25519[9] + b->ed25519[9];
}

/**
 * @brief Subtract one field element from another
 * @param[out] r Result of the subtraction, i.e. the difference r = a - b
 * @param[in] a Minuend
 * @param[in] b Subtrahend
 */
static inline void fe_sub(fe * r, const fe * a, const fe * b) {

    /* Compute a + 2p - b so as to not risk underflow */
    r->ed25519[0] = a->ed25519[0] + 0x7ffffda - b->ed25519[0];
    r->ed25519[1] = a->ed25519[1] + 0x3fffffe - b->ed25519[1];
    r->ed25519[2] = a->ed25519[2] + 0x7fffffe - b->ed25519[2];
    r->ed25519[3] = a->ed25519[3] + 0x3fffffe - b->ed25519[3];
    r->ed25519[4] = a->ed25519[4] + 0x7fffffe - b->ed25519[4];
    r->ed25519[5] = a->ed25519[5] + 0x3fffffe - b->ed25519[5];
    r->ed25519[6] = a->ed25519[6] + 0x7fffffe - b->ed25519[6];
    r->ed25519[7] = a->ed25519[7] + 0x3fffffe - b->ed25519[7];
    r->ed25519[8] = a->ed25519[8] + 0x7fffffe - b->ed25519[8];
    r->ed25519[9] = a->ed25519[9] + 0x3fffffe - b->ed25519[9];
    /* We could also call fe_neg() followed by fe_add(), but this would require
     * an intermediate fe variable to support aliasing */
}


/**
 * @brief Do a quick doubling of a point by shifting the bits
 * @param[out] r Result of the doubling, i.e. 2a
 * @param[in] a The element to double
 */
static inline void fe_double(fe * r, const fe * a) {

    r->ed25519[0] = a->ed25519[0] << 1;
    r->ed25519[1] = a->ed25519[1] << 1;
    r->ed25519[2] = a->ed25519[2] << 1;
    r->ed25519[3] = a->ed25519[3] << 1;
    r->ed25519[4] = a->ed25519[4] << 1;
    r->ed25519[5] = a->ed25519[5] << 1;
    r->ed25519[6] = a->ed25519[6] << 1;
    r->ed25519[7] = a->ed25519[7] << 1;
    r->ed25519[8] = a->ed25519[8] << 1;
    r->ed25519[9] = a->ed25519[9] << 1;
}

/**
 * @brief Multiply two field elements
 * @param[out] r Result of the multiplication, i.e. the product r = a b
 * @param[in] a Operand
 * @param[in] b Operand
 */
static inline void fe_mul(fe * r, const fe * a, const fe * b) {

    u64 r0, r1, r2, r3, r4, r5, r6, r7, r8, r9;

    u64 a0 = a->ed25519[0];
    u64 a1 = a->ed25519[1];
    u64 a2 = a->ed25519[2];
    u64 a3 = a->ed25519[3];
    u64 a4 = a->ed25519[4];
    u64 a5 = a->ed25519[5];
    u64 a6 = a->ed25519[6];
    u64 a7 = a->ed25519[7];
    u64 a8 = a->ed25519[8];
    u64 a9 = a->ed25519[9];

    u64 b0 = b->ed25519[0];
    u64 b1 = b->ed25519[1];
    u64 b2 = b->ed25519[2];
    u64 b3 = b->ed25519[3];
    u64 b4 = b->ed25519[4];
    u64 b5 = b->ed25519[5];
    u64 b6 = b->ed25519[6];
    u64 b7 = b->ed25519[7];
    u64 b8 = b->ed25519[8];
    u64 b9 = b->ed25519[9];

    /* Fuse the multiplication with (weak) reduction using the identity:
     *
     *                       a + b 2^255 = a + (2^255 - 19) b + 19 b = a + 19 b (mod p)
     *
     * We use the schoolbook multiplication algorithm (for now) which multiplies
     *
     *                              a = (a9, a8, a7, a6, a5, a4, a3, a2, a1, a0)
     *                              b = (b9, b8, b7, b6, b5, b4, b3, b2, b1, b0)
     *
     * (where ai, bi are 26-bit digits for i even and 25-bit digits for i odd) by computing the simple products
     * and adding them together in columns:
     *
     *                                                    (25) (26) (25) (26) (25) (26) (25) (26) (25) (26)
     *                                                     a9   a8   a7   a6   a5   a4   a3   a2   a1   a0
     *                                                     b9   b8   b7   b6   b5   b4   b3   b2   b1   b0
     * ------------------------------------------------------------------------------------------------------
     *                                                  | a0b9 a0b8 a0b7 a0b6 a0b5 a0b4 a0b3 a0b2 a0b1 a0b0
     *                                             a1b9 | a1b8 a1b7 a1b6 a1b5 a1b4 a1b3 a1b2 a1b1 a1b0
     *                                        a2b9 a2b8 | a2b7 a2b6 a2b5 a2b4 a2b3 a2b2 a2b1 a2b0
     *                                   a3b9 a3b8 a3b7 | a3b6 a3b5 a3b4 a3b3 a3b2 a3b1 a3b0
     *                              a4b9 a4b8 a4b7 a4b6 | a4b5 a4b4 a4b3 a4b2 a4b1 a4b0
     *                         a5b9 a5b8 a5b7 a5b6 a5b5 | a5b4 a5b3 a5b2 a5b1 a5b0
     *                    a6b9 a6b8 a6b7 a6b6 a6b5 a6b4 | a6b3 a6b2 a6b1 a6b0
     *               a7b9 a7b8 a7b7 a7b6 a7b5 a7b4 a7b3 | a7b2 a7b1 a7b0
     *          a8b9 a8b8 a8b7 a8b6 a8b5 a8b4 a8b3 a8b2 | a8b1 a8b0
     *     a9b9 a9b8 a9b7 a9b6 a9b5 a9b4 a9b3 a9b2 a9b1 | a9b0
     *                                                  |
     *                              2^255 cutoff________|
     *
     * All the partial products to the left of the 2^255 cutoff correspond to the "b" term in the identity above
     * and so are shifted down by 2^255 after multiplying by 19. Also keep in mind that we use a mixed-radix
     * representation of integers, and so some digits are 25-bits wide and some are 26-bits wide. This results
     * in some digits being multiplied by two to account for the difference in exponents (26-25). Consider
     * multiplying (x, y, z) * (a, b, c). Naive multiplication would result in:
     *
     *     zc*2^0 + (zb + yc)*2^26 + (za + yb + xc)*2^{26+25} + (ya + xb)^{26+25+26} + xa*2{26+25+26+25}
     *
     * but actually the partial product yb has a weight of 2^{26+26}. To account for this we multiply it by two
     * and put it in the limb of weight 2^{26+25}.
     */

    /* Start with the lowest-weight terms */
    r0 = a0*b0;
    /* Bring down the overflow from above the 2^255 cutoff point. Note that terms which are products of two odd-numbered limbs
     * are multiplied by two. This is because odd-numbered limbs have weights of the form two to the power of n*(26 + 25) + 26,
     * i.e. there is one more factor of 2^26 in their weighted value. This means that if we multiply two such limbs of weights,
     * say, exp{i*(26 + 25) + 26} and exp{j*(26 + 25) + 26} (where exp denotes exponentation with base 2), then the product has
     * weight exp{(i+j)(26 + 25) + 2*26}. We would like to put this result in an even-numbered limb, which has weight of the
     * form exp{k*(26+25)}. To allow this we multiply the value by two, thus reducing the weight of the product to the desired
     * exp{(i+j)(26 + 25) + 26 + 25} = exp{(i + j + 1)(26 + 25)}. */
    r0 += 19 * ( 2*a1*b9 + a2*b8 + 2*a3*b7 + a4*b6 + 2*a5*b5 + a6*b4 + 2*a7*b3 + a8*b2 + 2*a9*b1 );
    /* Any overflow in r0 put in r1 and clear it in r0 */
    r1 = r0 >> 26;  r0 &= LOW_26_BITS_MASK;

    r1 += a0*b1 + a1*b0;
    /* Bring down the overflow from above the 2^255 cutoff point */
    r1 += 19 * ( a2*b9 + a3*b8 + a4*b7 + a5*b6 + a6*b5 + a7*b4 + a8*b3 + a9*b2 );
    /* Rinse and repeat */
    r2 = r1 >> 25;  r1 &= LOW_25_BITS_MASK;

    r2 += a0*b2 + 2*a1*b1 + a2*b0;
    r2 += 19 * (2*a3*b9 + a4*b8 + 2*a5*b7 + a6*b6 + 2*a7*b5 + a8*b4 + 2*a9*b3);
    r3 = r2 >> 26;  r2 &= LOW_26_BITS_MASK;

    r3 += a0*b3 + a1*b2 + a2*b1 + a3*b0;
    r3 += 19 * ( a4*b9 + a5*b8 + a6*b7 + a7*b6 + a8*b5 + a9*b4 );
    r4 = r3 >> 25;  r3 &= LOW_25_BITS_MASK;

    r4 += a0*b4 + 2*a1*b3 + a2*b2 + 2*a3*b1 + a4*b0;
    r4 += 19 * ( 2*a5*b9 + a6*b8 + 2*a7*b7 + a8*b6 + 2*a9*b5 );
    r5 = r4 >> 26;  r4 &= LOW_26_BITS_MASK;

    r5 += a0*b5 + a1*b4 + a2*b3 + a3*b2 + a4*b1 + a5*b0;
    r5 += 19 * ( a6*b9 + a7*b8 + a8*b7 + a9*b6 );
    r6 = r5 >> 25;  r5 &= LOW_25_BITS_MASK;

    r6 += a0*b6 + 2*a1*b5 + a2*b4 + 2*a3*b3 + a4*b2 + 2*a5*b1 + a6*b0;
    r6 += 19 * ( 2*a7*b9 + a8*b8 + 2*a9*b7 );
    r7 = r6 >> 26;  r6 &= LOW_26_BITS_MASK;

    r7 += a0*b7 + a1*b6 + a2*b5 + a3*b4 + a4*b3 + a5*b2 + a6*b1 + a7*b0;
    r7 += 19 * ( a8*b9 + a9*b8);
    r8 = r7 >> 25;  r7 &= LOW_25_BITS_MASK;

    r8 += a0*b8 + 2*a1*b7 + a2*b6 + 2*a3*b5 + a4*b4 + 2*a5*b3 + a6*b2 + 2*a7*b1 + a8*b0;
    r8 += 19 * ( 2*a9*b9 );
    r9 = r8 >> 26;  r8 &= LOW_26_BITS_MASK;

    r9 += a0*b9 + a1*b8 + a2*b7 + a3*b6 + a4*b5 + a5*b4 + a6*b3 + a7*b2 + a8*b1 + a9*b0;
    /* The result may have exceeded 2^255, do another round just as in
     * fe_weak_reduce() */
    r0 += 19 * (r9 >> 25);
    r9 &= LOW_25_BITS_MASK;
    r1 += r0 >> 26;  r0 &= LOW_26_BITS_MASK;
    r2 += r1 >> 25;  r1 &= LOW_25_BITS_MASK;
    r3 += r2 >> 26;  r2 &= LOW_26_BITS_MASK;
    r4 += r3 >> 25;  r3 &= LOW_25_BITS_MASK;
    r5 += r4 >> 26;  r4 &= LOW_26_BITS_MASK;
    r6 += r5 >> 25;  r5 &= LOW_25_BITS_MASK;
    r7 += r6 >> 26;  r6 &= LOW_26_BITS_MASK;
    r8 += r7 >> 25;  r7 &= LOW_25_BITS_MASK;
    r9 += r8 >> 26;  r8 &= LOW_26_BITS_MASK;
    r0 += 19 * (r9 >> 25);
    r9 &= LOW_25_BITS_MASK;

    r->ed25519[0] = r0;
    r->ed25519[1] = r1;
    r->ed25519[2] = r2;
    r->ed25519[3] = r3;
    r->ed25519[4] = r4;
    r->ed25519[5] = r5;
    r->ed25519[6] = r6;
    r->ed25519[7] = r7;
    r->ed25519[8] = r8;
    r->ed25519[9] = r9;
}

/**
 * @brief Multiply an element by itself
 * @param[out] r Result of the squaring, i.e. the product r = a a
 * @param[in] a Field element to square
 */
static inline void fe_square(fe * r, const fe * a) {

    /* TODO: Provide an optimized implementation of squaring if possible */
    fe_mul(r, a, a);
}

/**
 * @brief Find a multiplicative inverse of a field element
 * @param[out] r Result of inversion, i.e. such element r that r a = 1
 * @param[in] a Field element to invert
 */
static inline void fe_invert(fe * r, const fe * a) {

    /* Make a local copy to support aliasing */
    fe x = *a;

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
    fe_square(r, &x);
    fe_mul(r, r, &x);

    /* Do a square-and-multiply step for the remaining 248 ones in the binary
     * representation of the exponent (p-2) */
    for (int i = 0; i < 248; i++) {

        fe_square(r, r);
        fe_mul(r, r, &x);
    }

    /* Zero in the exponent - just a squaring, with no multiplication */
    fe_square(r, r);

    /* One in the exponent - do both a squaring and a multiplication */
    fe_square(r, r);
    fe_mul(r, r, &x);

    /* Zero in the exponent - only square */
    fe_square(r, r);

    /* Two last ones in the exponent - do both a squaring and a multiplication */
    fe_square(r, r);
    fe_mul(r, r, &x);
    fe_square(r, r);
    fe_mul(r, r, &x);
}

/**
 * @brief Raise an element to the power (p-5)/8 where p = 2^255 - 19 is the field modulus
 * @param[out] r Result of the exponentation
 * @param[in] a The element to exponentiate
 */
static inline void fe_exp_p_minus_5_over_8(fe * r, const fe * a) {

    /* Make a local copy to support aliasing */
    fe x = *a;

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
    fe_square(r, &x);
    fe_mul(r, r, &x);

    /* Do a square-and-multiply step for the remaining 248 ones in the binary
     * representation of the exponent (p-2) */
    for (int i = 0; i < 248; i++) {

        fe_square(r, r);
        fe_mul(r, r, &x);
    }

    /* Zero in the exponent - just a squaring, with no multiplication */
    fe_square(r, r);

    /* One in the exponent - do both a squaring and a multiplication */
    fe_square(r, r);
    fe_mul(r, r, &x);
}

/**
 * @brief Encode a field element according to RFC 8032
 * @param[out] buffer Output buffer for the encoded field element
 * @param[in] a Field element to encode
 */
static inline void fe_encode(u8 * buffer, fe * a) {

    /* The field elements get encoded as little-endian byte strings according to RFC 8032 */
    u32 t0, t1, t2, t3, t4, t5, t6, t7;

    /* Canonicalize the element first */
    fe_strong_reduce(a, a);

    /* Store the temporary results of bit operations in separate variables (mapped to separate
     * registers) to allow for greater instruction-level parallelism. Let A[i] denote the limb
     * a->ed25519[i]. Recall that each limb has a different length according to the following
     * table:
     *
     *                              A[0]  ---   26 bits
     *                              A[1]  ---   25 bits
     *                              A[2]  ---   26 bits
     *                              A[3]  ---   25 bits
     *                              A[4]  ---   26 bits
     *                              A[5]  ---   25 bits
     *                              A[6]  ---   26 bits
     *                              A[7]  ---   25 bits
     *                              A[8]  ---   26 bits
     *                              A[9]  ---   25 bits
     */

    /* Store the lowest (26-bits wide) limb + whatever can fit (6 bits) of the second lowest limb */
    t0 = a->ed25519[0] | (a->ed25519[1] << 26);
    /* 6 bits of a->ed25519[1] are in t0, store the rest (19 bits) here + whatever can fit
     * (13 bits) of a->ed25519[2] */
    t1 = ( a->ed25519[1] >> 6 ) | ( a->ed25519[2] << 19 );
    /* 13 bits of a->ed25519[2] are in t1, store the rest (13 bits) here + whatever can fit
     * (19 bits) of a->ed25519[3] */
    t2 = ( a->ed25519[2] >> 13) | ( a->ed25519[3] << 13 );
    /* 19 bits of a->ed25519[3] are in t2, store the rest (6 bits) here + all of a->ed25519[4]
     * (all 26 bits) */
    t3 = ( a->ed25519[3] >> 19 ) | ( a->ed25519[4] << 6 );
    /* Store a->ed25519[5] + whatever can fit (7 bits) of a->ed25519[6] in t4 */
    t4 = a->ed25519[5] | (a->ed25519[6] << 25);
    /* 7 bits of a->ed25519[6] are in t0, store the rest (19 bits) here + whatever can fit
     * (13 bits) of a->ed25519[7] */
    t5 = ( a->ed25519[6] >> 7 ) | ( a->ed25519[7] << 19 );
    /* 13 bits of a->ed25519[7] are in t1, store the rest (12 bits) here + whatever can fit
     * (20 bits) of a->ed25519[8] */
    t6 = ( a->ed25519[7] >> 13) | ( a->ed25519[8] << 12 );
    /* 20 bits of a->ed25519[8] are in t2, store the rest (6 bits) here + all of a->ed25519[9]
     * (all 25 bits) */
    t7 = ( a->ed25519[8] >> 20 ) | ( a->ed25519[9] << 6 );

    _store_32(&buffer[0], t0);
    _store_32(&buffer[4], t1);
    _store_32(&buffer[8], t2);
    _store_32(&buffer[12], t3);
    _store_32(&buffer[16], t4);
    _store_32(&buffer[20], t5);
    _store_32(&buffer[24], t6);
    _store_32(&buffer[28], t7);
}

/**
 * @brief Decode a field element according to RFC 8032
 * @param[out] r Decoded field element
 * @param[out] buffer Encoding of the field element
 */
static inline void fe_decode(fe * r, const u8 * buffer) {

    r->ed25519[0] = ( _load_32(&buffer[0]) ) & LOW_26_BITS_MASK;
    /* Do not offset by 8 now (another 32 bits) since we have dropped 6 bits from buffer[3]. Offset
     * by 26 bits, i.e. 3 bytes of offset + 2 bits of shift*/
    r->ed25519[1] = ( _load_32(&buffer[3]) >> 2 ) & LOW_25_BITS_MASK;
    /* Offset by 25 bits this time (the odd-numbered limbs are 25-bits wide) */
    r->ed25519[2] = ( _load_32(&buffer[6]) >> 3 ) & LOW_26_BITS_MASK;
    r->ed25519[3] = ( _load_32(&buffer[9]) >> 5 ) & LOW_25_BITS_MASK;
    r->ed25519[4] = ( _load_32(&buffer[12]) >> 6 ) & LOW_26_BITS_MASK;
    /* At this point the shift bits have added up to a full byte */
    r->ed25519[5] = ( _load_32(&buffer[16]) ) & LOW_25_BITS_MASK;
    r->ed25519[6] = ( _load_32(&buffer[19]) >> 1 ) & LOW_26_BITS_MASK;
    r->ed25519[7] = ( _load_32(&buffer[22]) >> 3 ) & LOW_25_BITS_MASK;
    r->ed25519[8] = ( _load_32(&buffer[25]) >> 4 ) & LOW_26_BITS_MASK;
    /* Note that the last bit gets naturally ignored as required by RFC 8032 */
    r->ed25519[9] = ( _load_32(&buffer[28]) >> 6 ) & LOW_25_BITS_MASK;
}

/**
 * @brief Return the least-significant bit of the field element
 * @param a Field element
 * @return Least-significant bit of a
 */
static inline int fe_lsb(fe * a) {

    fe_strong_reduce(a, a);
    return a->ed25519[0] & 1;
}

#ifdef __cplusplus
}
#endif

#endif /* __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_ED25519_32_H */
