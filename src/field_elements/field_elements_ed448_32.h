
#ifndef __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_ED448_32_H
#define __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_ED448_32_H

#ifdef __cplusplus
extern "C" {
#endif

#include <field_elements/field_elements.h>
#include <utils/utils.h>

#if !FE3C_32BIT
    #error "Build system inconsistency detected! field_elements_ed448_32.h in use despite FE3C_32BIT not being set"
#endif /* !FE3C_32BIT */

#define LOW_28_BITS_MASK  0xfffffff

/* Elliptic curve constant d = -39081 */
static const fe ed448_d = { .ed448 = {
        0xfff6756, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff,
        0xffffffe, 0xfffffff, 0xfffffff, 0xfffffff,
        0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff
    }
};
/* Additive identity in the field */
static const fe fe_zero = { .ed448 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
/* Multiplicative identity in the field */
static const fe fe_one = { .ed448 = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };

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
    canonical &= (a->ed448[ 0] <  0xfffffff);
    canonical &= (a->ed448[ 1] <= 0xfffffff);
    canonical &= (a->ed448[ 2] <= 0xfffffff);
    canonical &= (a->ed448[ 3] <= 0xfffffff);
    canonical &= (a->ed448[ 4] <= 0xfffffff);
    canonical &= (a->ed448[ 5] <= 0xfffffff);
    canonical &= (a->ed448[ 6] <= 0xfffffff);
    canonical &= (a->ed448[ 7] <= 0xfffffff);
    canonical &= (a->ed448[ 8] <= 0xffffffe);
    canonical &= (a->ed448[ 9] <= 0xfffffff);
    canonical &= (a->ed448[10] <= 0xfffffff);
    canonical &= (a->ed448[11] <= 0xfffffff);
    canonical &= (a->ed448[12] <= 0xfffffff);
    canonical &= (a->ed448[13] <= 0xfffffff);
    canonical &= (a->ed448[14] <= 0xfffffff);
    canonical &= (a->ed448[15] <= 0xfffffff);
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
    sum |= a->ed448[ 0] ^ b->ed448[ 0];
    sum |= a->ed448[ 1] ^ b->ed448[ 1];
    sum |= a->ed448[ 2] ^ b->ed448[ 2];
    sum |= a->ed448[ 3] ^ b->ed448[ 3];
    sum |= a->ed448[ 4] ^ b->ed448[ 4];
    sum |= a->ed448[ 5] ^ b->ed448[ 5];
    sum |= a->ed448[ 6] ^ b->ed448[ 6];
    sum |= a->ed448[ 7] ^ b->ed448[ 7];
    sum |= a->ed448[ 8] ^ b->ed448[ 8];
    sum |= a->ed448[ 9] ^ b->ed448[ 9];
    sum |= a->ed448[10] ^ b->ed448[10];
    sum |= a->ed448[11] ^ b->ed448[11];
    sum |= a->ed448[12] ^ b->ed448[12];
    sum |= a->ed448[13] ^ b->ed448[13];
    sum |= a->ed448[14] ^ b->ed448[14];
    sum |= a->ed448[15] ^ b->ed448[15];

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
    fe_limb_type t0  = r->ed448[ 0];
    fe_limb_type t1  = r->ed448[ 1];
    fe_limb_type t2  = r->ed448[ 2];
    fe_limb_type t3  = r->ed448[ 3];
    fe_limb_type t4  = r->ed448[ 4];
    fe_limb_type t5  = r->ed448[ 5];
    fe_limb_type t6  = r->ed448[ 6];
    fe_limb_type t7  = r->ed448[ 7];
    fe_limb_type t8  = r->ed448[ 8];
    fe_limb_type t9  = r->ed448[ 9];
    fe_limb_type t10 = r->ed448[10];
    fe_limb_type t11 = r->ed448[11];
    fe_limb_type t12 = r->ed448[12];
    fe_limb_type t13 = r->ed448[13];
    fe_limb_type t14 = r->ed448[14];
    fe_limb_type t15 = r->ed448[15];

    /* XOR the t's with the limbs of a */
    fe_limb_type x0  =  t0 ^ a->ed448[ 0];
    fe_limb_type x1  =  t1 ^ a->ed448[ 1];
    fe_limb_type x2  =  t2 ^ a->ed448[ 2];
    fe_limb_type x3  =  t3 ^ a->ed448[ 3];
    fe_limb_type x4  =  t4 ^ a->ed448[ 4];
    fe_limb_type x5  =  t5 ^ a->ed448[ 5];
    fe_limb_type x6  =  t6 ^ a->ed448[ 6];
    fe_limb_type x7  =  t7 ^ a->ed448[ 7];

    fe_limb_type x8  =  t8 ^ a->ed448[ 8];
    fe_limb_type x9  =  t9 ^ a->ed448[ 9];
    fe_limb_type x10 = t10 ^ a->ed448[10];
    fe_limb_type x11 = t11 ^ a->ed448[11];
    fe_limb_type x12 = t12 ^ a->ed448[12];
    fe_limb_type x13 = t13 ^ a->ed448[13];
    fe_limb_type x14 = t14 ^ a->ed448[14];
    fe_limb_type x15 = t15 ^ a->ed448[15];

    /* Set the x array to all zeroes if move is zero or leave it be if move is one */
    x0  &= mask;
    x1  &= mask;
    x2  &= mask;
    x3  &= mask;
    x4  &= mask;
    x5  &= mask;
    x6  &= mask;
    x7  &= mask;
    x8  &= mask;
    x9  &= mask;
    x10 &= mask;
    x11 &= mask;
    x12 &= mask;
    x13 &= mask;
    x14 &= mask;
    x15 &= mask;

    /* If move=0 then x0-x15 are all zero and so we write back the limbs of r into r.
     * If move=1 then x0-x15 contain r XOR a and so by XORing back with the limbs of r we
     * write the limbs of a into r */
    r->ed448[ 0] =  t0 ^ x0;
    r->ed448[ 1] =  t1 ^ x1;
    r->ed448[ 2] =  t2 ^ x2;
    r->ed448[ 3] =  t3 ^ x3;
    r->ed448[ 4] =  t4 ^ x4;
    r->ed448[ 5] =  t5 ^ x5;
    r->ed448[ 6] =  t6 ^ x6;
    r->ed448[ 7] =  t7 ^ x7;
    r->ed448[ 8] =  t8 ^ x8;
    r->ed448[ 9] =  t9 ^ x9;
    r->ed448[10] = t10 ^ x10;
    r->ed448[11] = t11 ^ x11;
    r->ed448[12] = t12 ^ x12;
    r->ed448[13] = t13 ^ x13;
    r->ed448[14] = t14 ^ x14;
    r->ed448[15] = t15 ^ x15;
}

/**
 * @brief Do a weak modular reduction modulo the underlying field's characteristic p
 * @param[out] r Result of the reduction
 * @param[in] a Field element to be reduced
 * @note Note that the result need to be in canonical form, i.e. between 0 and p-1, it need only be less than 2p
 */
static inline void fe_weak_reduce(fe * r, const fe * a) {

    /* Do a "relaxed" reduction (to borrow terminology form Michael Scott's "Slothful reduction" paper)
     * - this ensures the result is less than 2p (where p = 2^448 - 2^224 - 1) */

    /* Use 64-bit auxiliary variables */
    u64 t0  = a->ed448[ 0];
    u64 t1  = a->ed448[ 1];
    u64 t2  = a->ed448[ 2];
    u64 t3  = a->ed448[ 3];
    u64 t4  = a->ed448[ 4];
    u64 t5  = a->ed448[ 5];
    u64 t6  = a->ed448[ 6];
    u64 t7  = a->ed448[ 7];
    u64 t8  = a->ed448[ 8];
    u64 t9  = a->ed448[ 9];
    u64 t10 = a->ed448[10];
    u64 t11 = a->ed448[11];
    u64 t12 = a->ed448[12];
    u64 t13 = a->ed448[13];
    u64 t14 = a->ed448[14];
    u64 t15 = a->ed448[15];

    /* The modulus can be expressed as p = u - v - 1, where u = 2^448 and v = 2^224.
     * Express (split) the value a as a = x + uy + vz. Then
     *
     *   a mod p = (x + uy + vz) mod (u - v - 1)
     *           = (x + (u - v - 1)y + vy + y + vz) mod (u - v - 1)
     *           = (x + vz + vy + y) mod p
     *
     * To split a into (x, y, z) we simply let the carries bubble up to the top
     * limb. Whatever has overflown the top limb is equal to y. Limbs 8-15 hold
     * the value of z and the low eight limbs hold x.
     */

    /* Add whatever has overflown in t0 (a->ed448[0]) to t1 (a->ed448[1])
     * and mask out the overflow in t0 */
    t1  += t0  >> 28;  t0  &= LOW_28_BITS_MASK;
    /* Rinse and repeat for other limbs */
    t2  += t1  >> 28;  t1  &= LOW_28_BITS_MASK;
    t3  += t2  >> 28;  t2  &= LOW_28_BITS_MASK;
    t4  += t3  >> 28;  t3  &= LOW_28_BITS_MASK;
    t5  += t4  >> 28;  t4  &= LOW_28_BITS_MASK;
    t6  += t5  >> 28;  t5  &= LOW_28_BITS_MASK;
    t7  += t6  >> 28;  t6  &= LOW_28_BITS_MASK;
    t8  += t7  >> 28;  t7  &= LOW_28_BITS_MASK;
    t9  += t8  >> 28;  t8  &= LOW_28_BITS_MASK;
    t10 += t9  >> 28;  t9  &= LOW_28_BITS_MASK;
    t11 += t10 >> 28;  t10 &= LOW_28_BITS_MASK;
    t12 += t11 >> 28;  t11 &= LOW_28_BITS_MASK;
    t13 += t12 >> 28;  t12 &= LOW_28_BITS_MASK;
    t14 += t13 >> 28;  t13 &= LOW_28_BITS_MASK;
    t15 += t14 >> 28;  t14 &= LOW_28_BITS_MASK;

    /* At this point low 28 bits of each of limbs t0-t15 hold the value of a modulo
     * 2^448, i.e. x + vz, using the notation from above. Add vy and y to this
     * result. Note that y is stored in the "overflow" of t15, i.e. bits starting
     * from bit 28. Also note that adding vy is equivalent to adding y to the limbs
     * starting from limb 8 (since v = 2^{224} = 2^{8*28}). */

    /* Add y */
    t0 += t15 >> 28;
    /* Add y 2^224 */
    t8 += t15 >> 28;
    /* Clear the overflow in t15 */
    t15 &= LOW_28_BITS_MASK;

    /* Do this once more to ensure we correctly normalize
     * all integers possible to be represented in our system,
     * i.e. we don't get surprised by (t15 >> 28) leading
     * to overflow in t0 or t8 */
    t1  += t0  >> 28;  t0  &= LOW_28_BITS_MASK;
    /* Rinse and repeat for other limbs */
    t2  += t1  >> 28;  t1  &= LOW_28_BITS_MASK;
    t3  += t2  >> 28;  t2  &= LOW_28_BITS_MASK;
    t4  += t3  >> 28;  t3  &= LOW_28_BITS_MASK;
    t5  += t4  >> 28;  t4  &= LOW_28_BITS_MASK;
    t6  += t5  >> 28;  t5  &= LOW_28_BITS_MASK;
    t7  += t6  >> 28;  t6  &= LOW_28_BITS_MASK;
    t8  += t7  >> 28;  t7  &= LOW_28_BITS_MASK;
    t9  += t8  >> 28;  t8  &= LOW_28_BITS_MASK;
    t10 += t9  >> 28;  t9  &= LOW_28_BITS_MASK;
    t11 += t10 >> 28;  t10 &= LOW_28_BITS_MASK;
    t12 += t11 >> 28;  t11 &= LOW_28_BITS_MASK;
    t13 += t12 >> 28;  t12 &= LOW_28_BITS_MASK;
    t14 += t13 >> 28;  t13 &= LOW_28_BITS_MASK;
    t15 += t14 >> 28;  t14 &= LOW_28_BITS_MASK;
    t0  += t15 >> 28;
    t8  += t15 >> 28;
    t15 &= LOW_28_BITS_MASK;

    r->ed448[ 0] = t0;
    r->ed448[ 1] = t1;
    r->ed448[ 2] = t2;
    r->ed448[ 3] = t3;
    r->ed448[ 4] = t4;
    r->ed448[ 5] = t5;
    r->ed448[ 6] = t6;
    r->ed448[ 7] = t7;
    r->ed448[ 8] = t8;
    r->ed448[ 9] = t9;
    r->ed448[10] = t10;
    r->ed448[11] = t11;
    r->ed448[12] = t12;
    r->ed448[13] = t13;
    r->ed448[14] = t14;
    r->ed448[15] = t15;
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
    /* Store r + 2^224 + 1 in t (we will subtract 2^448 from it, resulting in
     * t = r - p)*/
    t.ed448[ 0] = r->ed448[ 0] + 1;
    t.ed448[ 1] = r->ed448[ 1];
    t.ed448[ 2] = r->ed448[ 2];
    t.ed448[ 3] = r->ed448[ 3];
    t.ed448[ 4] = r->ed448[ 4];
    t.ed448[ 5] = r->ed448[ 5];
    t.ed448[ 6] = r->ed448[ 6];
    t.ed448[ 7] = r->ed448[ 7];
    /* Add 2^224 to the mix */
    t.ed448[ 8] = r->ed448[ 8] + 1;
    t.ed448[ 9] = r->ed448[ 9];
    t.ed448[10] = r->ed448[10];
    t.ed448[11] = r->ed448[11];
    t.ed448[12] = r->ed448[12];
    t.ed448[13] = r->ed448[13];
    t.ed448[14] = r->ed448[14];
    t.ed448[15] = r->ed448[15];

    t.ed448[ 1] += t.ed448[ 0] >> 28;  t.ed448[ 0] &= LOW_28_BITS_MASK;
    t.ed448[ 2] += t.ed448[ 1] >> 28;  t.ed448[ 1] &= LOW_28_BITS_MASK;
    t.ed448[ 3] += t.ed448[ 2] >> 28;  t.ed448[ 2] &= LOW_28_BITS_MASK;
    t.ed448[ 4] += t.ed448[ 3] >> 28;  t.ed448[ 3] &= LOW_28_BITS_MASK;
    t.ed448[ 5] += t.ed448[ 4] >> 28;  t.ed448[ 4] &= LOW_28_BITS_MASK;
    t.ed448[ 6] += t.ed448[ 5] >> 28;  t.ed448[ 5] &= LOW_28_BITS_MASK;
    t.ed448[ 7] += t.ed448[ 6] >> 28;  t.ed448[ 6] &= LOW_28_BITS_MASK;
    t.ed448[ 8] += t.ed448[ 7] >> 28;  t.ed448[ 7] &= LOW_28_BITS_MASK;
    t.ed448[ 9] += t.ed448[ 8] >> 28;  t.ed448[ 8] &= LOW_28_BITS_MASK;
    t.ed448[10] += t.ed448[ 9] >> 28;  t.ed448[ 9] &= LOW_28_BITS_MASK;
    t.ed448[11] += t.ed448[10] >> 28;  t.ed448[10] &= LOW_28_BITS_MASK;
    t.ed448[12] += t.ed448[11] >> 28;  t.ed448[11] &= LOW_28_BITS_MASK;
    t.ed448[13] += t.ed448[12] >> 28;  t.ed448[12] &= LOW_28_BITS_MASK;
    t.ed448[14] += t.ed448[13] >> 28;  t.ed448[13] &= LOW_28_BITS_MASK;
    t.ed448[15] += t.ed448[14] >> 28;  t.ed448[14] &= LOW_28_BITS_MASK;

    /* At this point t.ed448[15] contains the highest limb of r + 2^224 + 1. Try subtracting
     * 2^448 - if we get an underflow this means that r < 2^448 - 2^224 - 1 and so r is the
     * final result. Otherwise we must return r - p (note that this includes the case
     * where r = p, as no underflow will occur then and t.ed448[15] will be equal to
     * zero). */
    t.ed448[15] -= (1ULL << 28);

    /* Check the highest bit of t.ed448[15] for underflow. If the highest bit is set then
     * underflow occurred and so we return r, otherwise we set r ::= t and return that */
    fe_conditional_move(r, &t, (t.ed448[15] >> 31) ^ 1);
}

/**
 * @brief Negate an element in the field
 * @param[out] r The result of negation
 * @param[in] a Element to be negated
 */
static inline void fe_neg(fe * r, const fe * a) {

    /* Check against underflow */
    FE3C_SANITY_CHECK(a->ed448[ 0] <  0x1ffffffe);
    FE3C_SANITY_CHECK(a->ed448[ 1] <= 0x1ffffffe);
    FE3C_SANITY_CHECK(a->ed448[ 2] <= 0x1ffffffe);
    FE3C_SANITY_CHECK(a->ed448[ 3] <= 0x1ffffffe);
    FE3C_SANITY_CHECK(a->ed448[ 4] <= 0x1ffffffe);
    FE3C_SANITY_CHECK(a->ed448[ 5] <= 0x1ffffffe);
    FE3C_SANITY_CHECK(a->ed448[ 6] <= 0x1ffffffe);
    FE3C_SANITY_CHECK(a->ed448[ 7] <= 0x1ffffffe);
    FE3C_SANITY_CHECK(a->ed448[ 8] <= 0x1ffffffc);
    FE3C_SANITY_CHECK(a->ed448[ 9] <= 0x1ffffffe);
    FE3C_SANITY_CHECK(a->ed448[10] <= 0x1ffffffe);
    FE3C_SANITY_CHECK(a->ed448[11] <= 0x1ffffffe);
    FE3C_SANITY_CHECK(a->ed448[12] <= 0x1ffffffe);
    FE3C_SANITY_CHECK(a->ed448[13] <= 0x1ffffffe);
    FE3C_SANITY_CHECK(a->ed448[14] <= 0x1ffffffe);
    FE3C_SANITY_CHECK(a->ed448[15] <= 0x1ffffffe);

    /* Set r to 2p-a so as to not require strong reduction of a */
    r->ed448[ 0] = 0x1ffffffe - a->ed448[ 0];
    r->ed448[ 1] = 0x1ffffffe - a->ed448[ 1];
    r->ed448[ 2] = 0x1ffffffe - a->ed448[ 2];
    r->ed448[ 3] = 0x1ffffffe - a->ed448[ 3];
    r->ed448[ 4] = 0x1ffffffe - a->ed448[ 4];
    r->ed448[ 5] = 0x1ffffffe - a->ed448[ 5];
    r->ed448[ 6] = 0x1ffffffe - a->ed448[ 6];
    r->ed448[ 7] = 0x1ffffffe - a->ed448[ 7];
    r->ed448[ 8] = 0x1ffffffc - a->ed448[ 8];
    r->ed448[ 9] = 0x1ffffffe - a->ed448[ 9];
    r->ed448[10] = 0x1ffffffe - a->ed448[10];
    r->ed448[11] = 0x1ffffffe - a->ed448[11];
    r->ed448[12] = 0x1ffffffe - a->ed448[12];
    r->ed448[13] = 0x1ffffffe - a->ed448[13];
    r->ed448[14] = 0x1ffffffe - a->ed448[14];
    r->ed448[15] = 0x1ffffffe - a->ed448[15];
}

/**
 * @brief Add two field elements
 * @param[out] r Result of the addition, i.e. the sum r = a + b
 * @param[in] a Operand
 * @param[in] b Operand
 */
static inline void fe_add(fe * r, const fe * a, const fe * b) {

    r->ed448[ 0] = a->ed448[ 0] + b->ed448[ 0];
    r->ed448[ 1] = a->ed448[ 1] + b->ed448[ 1];
    r->ed448[ 2] = a->ed448[ 2] + b->ed448[ 2];
    r->ed448[ 3] = a->ed448[ 3] + b->ed448[ 3];
    r->ed448[ 4] = a->ed448[ 4] + b->ed448[ 4];
    r->ed448[ 5] = a->ed448[ 5] + b->ed448[ 5];
    r->ed448[ 6] = a->ed448[ 6] + b->ed448[ 6];
    r->ed448[ 7] = a->ed448[ 7] + b->ed448[ 7];
    r->ed448[ 8] = a->ed448[ 8] + b->ed448[ 8];
    r->ed448[ 9] = a->ed448[ 9] + b->ed448[ 9];
    r->ed448[10] = a->ed448[10] + b->ed448[10];
    r->ed448[11] = a->ed448[11] + b->ed448[11];
    r->ed448[12] = a->ed448[12] + b->ed448[12];
    r->ed448[13] = a->ed448[13] + b->ed448[13];
    r->ed448[14] = a->ed448[14] + b->ed448[14];
    r->ed448[15] = a->ed448[15] + b->ed448[15];
}

/**
 * @brief Subtract one field element from another
 * @param[out] r Result of the subtraction, i.e. the difference r = a - b
 * @param[in] a Minuend
 * @param[in] b Subtrahend
 */
static inline void fe_sub(fe * r, const fe * a, const fe * b) {

    /* Set r to 2p-a so as to not require strong reduction of a */
    r->ed448[ 0] = a->ed448[ 0] + 0x1ffffffe - b->ed448[ 0];
    r->ed448[ 1] = a->ed448[ 1] + 0x1ffffffe - b->ed448[ 1];
    r->ed448[ 2] = a->ed448[ 2] + 0x1ffffffe - b->ed448[ 2];
    r->ed448[ 3] = a->ed448[ 3] + 0x1ffffffe - b->ed448[ 3];
    r->ed448[ 4] = a->ed448[ 4] + 0x1ffffffe - b->ed448[ 4];
    r->ed448[ 5] = a->ed448[ 5] + 0x1ffffffe - b->ed448[ 5];
    r->ed448[ 6] = a->ed448[ 6] + 0x1ffffffe - b->ed448[ 6];
    r->ed448[ 7] = a->ed448[ 7] + 0x1ffffffe - b->ed448[ 7];
    r->ed448[ 8] = a->ed448[ 8] + 0x1ffffffc - b->ed448[ 8];
    r->ed448[ 9] = a->ed448[ 9] + 0x1ffffffe - b->ed448[ 9];
    r->ed448[10] = a->ed448[10] + 0x1ffffffe - b->ed448[10];
    r->ed448[11] = a->ed448[11] + 0x1ffffffe - b->ed448[11];
    r->ed448[12] = a->ed448[12] + 0x1ffffffe - b->ed448[12];
    r->ed448[13] = a->ed448[13] + 0x1ffffffe - b->ed448[13];
    r->ed448[14] = a->ed448[14] + 0x1ffffffe - b->ed448[14];
    r->ed448[15] = a->ed448[15] + 0x1ffffffe - b->ed448[15];
    /* We could also call fe_neg() followed by fe_add(), but this would require
     * an intermediate fe variable to support aliasing */
}

/**
 * @brief Do a quick doubling of a point by shifting the bits
 * @param[out] r Result of the doubling, i.e. 2a
 * @param[in] a The element to double
 */
static inline void fe_double(fe * r, const fe * a) {

    r->ed448[ 0] = a->ed448[ 0] << 1;
    r->ed448[ 1] = a->ed448[ 1] << 1;
    r->ed448[ 2] = a->ed448[ 2] << 1;
    r->ed448[ 3] = a->ed448[ 3] << 1;
    r->ed448[ 4] = a->ed448[ 4] << 1;
    r->ed448[ 5] = a->ed448[ 5] << 1;
    r->ed448[ 6] = a->ed448[ 6] << 1;
    r->ed448[ 7] = a->ed448[ 7] << 1;
    r->ed448[ 8] = a->ed448[ 8] << 1;
    r->ed448[ 9] = a->ed448[ 9] << 1;
    r->ed448[10] = a->ed448[10] << 1;
    r->ed448[11] = a->ed448[11] << 1;
    r->ed448[12] = a->ed448[12] << 1;
    r->ed448[13] = a->ed448[13] << 1;
    r->ed448[14] = a->ed448[14] << 1;
    r->ed448[15] = a->ed448[15] << 1;
}

/**
 * @brief Multiply two field elements
 * @param[out] r Result of the multiplication, i.e. the product r = a b
 * @param[in] a Operand
 * @param[in] b Operand
 */
static inline void fe_mul(fe * r, const fe * a, const fe * b) {

    /* Note that we are using the so-called "golden-ratio prime" which facilitates fast
     * Karatsuba multiplication. Let S = 2^224. Then:
     *
     *   (x + zS)(u + vS) = xu + (xv + zu)S + zvS^2 =
     *                    = (xu + zv) + (xv + zu + zv)S (mod p)
     *                    = (xu + zv) + ((x+z)(u+v) - xu)S (mod p)
     *
     * since p = X^2 - X - 1. Note that in our representation the terms x and z (u and v)
     * correspond to limbs 0-7 and 8-15, respectively.
     */

    u64 a0  = a->ed448[ 0];
    u64 a1  = a->ed448[ 1];
    u64 a2  = a->ed448[ 2];
    u64 a3  = a->ed448[ 3];
    u64 a4  = a->ed448[ 4];
    u64 a5  = a->ed448[ 5];
    u64 a6  = a->ed448[ 6];
    u64 a7  = a->ed448[ 7];
    u64 a8  = a->ed448[ 8];
    u64 a9  = a->ed448[ 9];
    u64 a10 = a->ed448[10];
    u64 a11 = a->ed448[11];
    u64 a12 = a->ed448[12];
    u64 a13 = a->ed448[13];
    u64 a14 = a->ed448[14];
    u64 a15 = a->ed448[15];

    u64 b0  = b->ed448[ 0];
    u64 b1  = b->ed448[ 1];
    u64 b2  = b->ed448[ 2];
    u64 b3  = b->ed448[ 3];
    u64 b4  = b->ed448[ 4];
    u64 b5  = b->ed448[ 5];
    u64 b6  = b->ed448[ 6];
    u64 b7  = b->ed448[ 7];
    u64 b8  = b->ed448[ 8];
    u64 b9  = b->ed448[ 9];
    u64 b10 = b->ed448[10];
    u64 b11 = b->ed448[11];
    u64 b12 = b->ed448[12];
    u64 b13 = b->ed448[13];
    u64 b14 = b->ed448[14];
    u64 b15 = b->ed448[15];

    /* Do a simple schoolbook multiplication of (x+z)(u+v). Let:
     *
     *   m = (x+z) = (m7, m6, m5, m4, m3, m2, m1, m0) and k = (u+v) = (k7, k6, k5, k4, k3, k2, k1, k0)
     *
     * where mi, ki are all 56-digits for i=0,1,2,3,4,5,6,7. We shall compute simple products and add
     * them together column by column:
     *
     *                                         m7   m6   m5   m4   m3   m2   m1   m0
     *                                         k7   k6   k5   k4   k3   k2   k1   k0
     * -------------------------------------------------------------------------------
     *                                      | m0k7 m0k6 m0k5 m0k4 m0k3 m0k2 m0k1 m0k0
     *                                 m1k7 | m1k6 m1k5 m1k4 m1k3 m1k2 m1k1 m1k0
     *                            m2k7 m2k6 | m2k5 m2k4 m2k3 m2k2 m2k1 m2k0
     *                       m3k7 m3k6 m3k5 | m3k4 m3k3 m3k2 m3k1 m3k0
     *                  m4k7 m4k6 m4k5 m4k4 | m4k3 m4k2 m4k1 m4k0
     *             m5k7 m5k6 m5k5 m5k4 m5k3 | m5k2 m5k1 m5k0
     *        m6k7 m6k6 m6k5 m6k4 m6k3 m6k2 | m6k1 m6k0
     *   m7k7 m7k6 m7k5 m7k4 m7k3 m7k2 m7k1 | m7k0
     *                                      |
     *                   2^224 cutoff_______|
     *
     * Now recall that we are interested in the expression ((x+z)(u+v) - xu)2^224
     * and so the terms to the left of the 2^224 cutoff point above will ultimately
     * exceed 2^448 and so must be brought down using the identity:
     *
     *   n mod p = (2^448 e + 2^224 f + g) mod (2^448 - 2^224 - 1)
     *           = (g + (2^448 - 2^224 - 1)e + 2^224 e + e + 2^224 f) mod (2^448 - 2^224 - 1)
     *           = (g + 2^224 (e + f) + e) mod (2^448 - 2^224 - 1)
     *
     * (cf. comments in fe_weak_reduce()). In our case the e term corresponds to the
     * partial products mikj where i+j>7.
     */

    u64 m0 = a0 + a8;
    u64 m1 = a1 + a9;
    u64 m2 = a2 + a10;
    u64 m3 = a3 + a11;
    u64 m4 = a4 + a12;
    u64 m5 = a5 + a13;
    u64 m6 = a6 + a14;
    u64 m7 = a7 + a15;

    u64 k0 = b0 + b8;
    u64 k1 = b1 + b9;
    u64 k2 = b2 + b10;
    u64 k3 = b3 + b11;
    u64 k4 = b4 + b12;
    u64 k5 = b5 + b13;
    u64 k6 = b6 + b14;
    u64 k7 = b7 + b15;

    u64 r0  =         m1*k7 + m2*k6 + m3*k5 + m4*k4 + m5*k3 + m6*k2 + m7*k1;
    u64 r1  =                 m2*k7 + m3*k6 + m4*k5 + m5*k4 + m6*k3 + m7*k2;
    u64 r2  =                         m3*k7 + m4*k6 + m5*k5 + m6*k4 + m7*k3;
    u64 r3  =                                 m4*k7 + m5*k6 + m6*k5 + m7*k4;
    u64 r4  =                                         m5*k7 + m6*k6 + m7*k5;
    u64 r5  =                                                 m6*k7 + m7*k6;
    u64 r6  =                                                         m7*k7;
    u64 r7  = 0;
    /* Add at the 2^224 "level" the terms which exceeded 2^448 (e in the above identity),
     * which are currently held in r0-r7 */
    u64 r8  = r0 + m0*k0;
    u64 r9  = r1 + m0*k1 + m1*k0;
    u64 r10 = r2 + m0*k2 + m1*k1 + m2*k0;
    u64 r11 = r3 + m0*k3 + m1*k2 + m2*k1 + m3*k0;
    u64 r12 = r4 + m0*k4 + m1*k3 + m2*k2 + m3*k1 + m4*k0;
    u64 r13 = r5 + m0*k5 + m1*k4 + m2*k3 + m3*k2 + m4*k1 + m5*k0;
    u64 r14 = r6 + m0*k6 + m1*k5 + m2*k4 + m3*k3 + m4*k2 + m5*k1 + m6*k0;
    u64 r15 =      m0*k7 + m1*k6 + m2*k5 + m3*k4 + m4*k3 + m5*k2 + m6*k1 + m7*k0;

    /* Do a schoolbook multiplication for x*u and z*v as well. Here, however, we do not risk
     * exceeding 2^448 since all inputs are bound by 2^224. */
    u64 xu0  = a0*b0;
    u64 xu1  = a0*b1 + a1*b0;
    u64 xu2  = a0*b2 + a1*b1 + a2*b0;
    u64 xu3  = a0*b3 + a1*b2 + a2*b1 + a3*b0;
    u64 xu4  = a0*b4 + a1*b3 + a2*b2 + a3*b1 + a4*b0;
    u64 xu5  = a0*b5 + a1*b4 + a2*b3 + a3*b2 + a4*b1 + a5*b0;
    u64 xu6  = a0*b6 + a1*b5 + a2*b4 + a3*b3 + a4*b2 + a5*b1 + a6*b0;
    u64 xu7  = a0*b7 + a1*b6 + a2*b5 + a3*b4 + a4*b3 + a5*b2 + a6*b1 + a7*b0;
    u64 xu8  =         a1*b7 + a2*b6 + a3*b5 + a4*b4 + a5*b3 + a6*b2 + a7*b1;
    u64 xu9  =                 a2*b7 + a3*b6 + a4*b5 + a5*b4 + a6*b3 + a7*b2;
    u64 xu10 =                         a3*b7 + a4*b6 + a5*b5 + a6*b4 + a7*b3;
    u64 xu11 =                                 a4*b7 + a5*b6 + a6*b5 + a7*b4;
    u64 xu12 =                                         a5*b7 + a6*b6 + a7*b5;
    u64 xu13 =                                                 a6*b7 + a7*b6;
    u64 xu14 =                                                         a7*b7;

    u64 zv0  = a8*b8;
    u64 zv1  = a8*b9  + a9*b8;
    u64 zv2  = a8*b10 + a9*b9 +  a10*b8;
    u64 zv3  = a8*b11 + a9*b10 + a10*b9  + a11*b8;
    u64 zv4  = a8*b12 + a9*b11 + a10*b10 + a11*b9  + a12*b8;
    u64 zv5  = a8*b13 + a9*b12 + a10*b11 + a11*b10 + a12*b9  + a13*b8;
    u64 zv6  = a8*b14 + a9*b13 + a10*b12 + a11*b11 + a12*b10 + a13*b9  + a14*b8;
    u64 zv7  = a8*b15 + a9*b14 + a10*b13 + a11*b12 + a12*b11 + a13*b10 + a14*b9  + a15*b8;
    u64 zv8  =          a9*b15 + a10*b14 + a11*b13 + a12*b12 + a13*b11 + a14*b10 + a15*b9;
    u64 zv9  =                   a10*b15 + a11*b14 + a12*b13 + a13*b12 + a14*b11 + a15*b10;
    u64 zv10 =                             a11*b15 + a12*b14 + a13*b13 + a14*b12 + a15*b11;
    u64 zv11 =                                       a12*b15 + a13*b14 + a14*b13 + a15*b12;
    u64 zv12 =                                                 a13*b15 + a14*b14 + a15*b13;
    u64 zv13 =                                                           a14*b15 + a15*b14;
    u64 zv14 =                                                                     a15*b15;

    /* Subtract xu 2^224 */
    r0  -= xu8;
    r1  -= xu9;
    r2  -= xu10;
    r3  -= xu11;
    r4  -= xu12;
    r5  -= xu13;
    r6  -= xu14;
    r7  -= 0;
    r8  -= xu0 + xu8;
    r9  -= xu1 + xu9;
    r10 -= xu2 + xu10;
    r11 -= xu3 + xu11;
    r12 -= xu4 + xu12;
    r13 -= xu5 + xu13;
    r14 -= xu6 + xu14;
    r15 -= xu7;

    r1  += r0  >> 28;  r0  &= LOW_28_BITS_MASK;
    r2  += r1  >> 28;  r1  &= LOW_28_BITS_MASK;
    r3  += r2  >> 28;  r2  &= LOW_28_BITS_MASK;
    r4  += r3  >> 28;  r3  &= LOW_28_BITS_MASK;
    r5  += r4  >> 28;  r4  &= LOW_28_BITS_MASK;
    r6  += r5  >> 28;  r5  &= LOW_28_BITS_MASK;
    r7  += r6  >> 28;  r6  &= LOW_28_BITS_MASK;
    r8  += r7  >> 28;  r7  &= LOW_28_BITS_MASK;
    r9  += r8  >> 28;  r8  &= LOW_28_BITS_MASK;
    r10 += r9  >> 28;  r9  &= LOW_28_BITS_MASK;
    r11 += r10 >> 28;  r10 &= LOW_28_BITS_MASK;
    r12 += r11 >> 28;  r11 &= LOW_28_BITS_MASK;
    r13 += r12 >> 28;  r12 &= LOW_28_BITS_MASK;
    r14 += r13 >> 28;  r13 &= LOW_28_BITS_MASK;
    r15 += r14 >> 28;  r14 &= LOW_28_BITS_MASK;
    r0  += r15 >> 28;
    r8  += r15 >> 28;
    r15 &= LOW_28_BITS_MASK;

    /* Add (xu + zv) */
    r0  +=  xu0 + zv0;
    r1  +=  xu1 + zv1;
    r2  +=  xu2 + zv2;
    r3  +=  xu3 + zv3;
    r4  +=  xu4 + zv4;
    r5  +=  xu5 + zv5;
    r6  +=  xu6 + zv6;
    r7  +=  xu7 + zv7;
    r8  +=  xu8 + zv8;
    r9  +=  xu9 + zv9;
    r10 += xu10 + zv10;
    r11 += xu11 + zv11;
    r12 += xu12 + zv12;
    r13 += xu13 + zv13;
    r14 += xu14 + zv14;

    r1  += r0  >> 28;  r0  &= LOW_28_BITS_MASK;
    r2  += r1  >> 28;  r1  &= LOW_28_BITS_MASK;
    r3  += r2  >> 28;  r2  &= LOW_28_BITS_MASK;
    r4  += r3  >> 28;  r3  &= LOW_28_BITS_MASK;
    r5  += r4  >> 28;  r4  &= LOW_28_BITS_MASK;
    r6  += r5  >> 28;  r5  &= LOW_28_BITS_MASK;
    r7  += r6  >> 28;  r6  &= LOW_28_BITS_MASK;
    r8  += r7  >> 28;  r7  &= LOW_28_BITS_MASK;
    r9  += r8  >> 28;  r8  &= LOW_28_BITS_MASK;
    r10 += r9  >> 28;  r9  &= LOW_28_BITS_MASK;
    r11 += r10 >> 28;  r10 &= LOW_28_BITS_MASK;
    r12 += r11 >> 28;  r11 &= LOW_28_BITS_MASK;
    r13 += r12 >> 28;  r12 &= LOW_28_BITS_MASK;
    r14 += r13 >> 28;  r13 &= LOW_28_BITS_MASK;
    r15 += r14 >> 28;  r14 &= LOW_28_BITS_MASK;
    r0  += r15 >> 28;
    r8  += r15 >> 28;
    r15 &= LOW_28_BITS_MASK;

    r->ed448[ 0] = r0;
    r->ed448[ 1] = r1;
    r->ed448[ 2] = r2;
    r->ed448[ 3] = r3;
    r->ed448[ 4] = r4;
    r->ed448[ 5] = r5;
    r->ed448[ 6] = r6;
    r->ed448[ 7] = r7;
    r->ed448[ 8] = r8;
    r->ed448[ 9] = r9;
    r->ed448[10] = r10;
    r->ed448[11] = r11;
    r->ed448[12] = r12;
    r->ed448[13] = r13;
    r->ed448[14] = r14;
    r->ed448[15] = r15;
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
    fe_square(r, &x);
    fe_mul(r, r, &x);

    /* Do a square-and-multiply step for the remaining 221 ones in the binary
     * representation of the exponent (p-2) */
    for (int i = 0; i < 221; i++) {

        fe_square(r, r);
        fe_mul(r, r, &x);
    }

    /* Zero in the exponent - just a squaring, with no multiplication */
    fe_square(r, r);

    /* Another 222 ones in the exponent */
    for (int i = 0; i < 222; i++) {

        fe_square(r, r);
        fe_mul(r, r, &x);
    }

    /* Zero in the exponent - only square */
    fe_square(r, r);

    /* And finally a one - do both a squaring and a multiplication */
    fe_square(r, r);
    fe_mul(r, r, &x);
}

/**
 * @brief Raise an element to the power (p-3)/4 where p = 2^448 - 2^224 - 1 is the field modulus
 * @param[out] r Result of the exponentation
 * @param[in] a The element to exponentiate
 */
static inline void fe_exp_p_minus_3_over_4(fe * r, const fe * a) {

    /* Make a local copy to support aliasing */
    fe x = *a;

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
    fe_square(r, &x);
    fe_mul(r, r, &x);

    /* Do a square-and-multiply step for the remaining 221 ones in the binary
     * representation of the exponent */
    for (int i = 0; i < 221; i++) {

        fe_square(r, r);
        fe_mul(r, r, &x);
    }

    /* Zero in the exponent - just a squaring, with no multiplication */
    fe_square(r, r);

    /* Do a square-and-multiply step for the final 222 ones in the binary
     * representation of the exponent */
    for (int i = 0; i < 222; i++) {

        fe_square(r, r);
        fe_mul(r, r, &x);
    }
}

/**
 * @brief Encode a field element according to RFC 8032
 * @param[out] buffer Output buffer for the encoded field element
 * @param[in] a Field element to encode
 */
static inline void fe_encode(u8 * buffer, fe * a) {

    /* Canonicalize the element first */
    fe_strong_reduce(a, a);

    /* Store the temporary results of bit operations in separate variables (mapped to separate
     * registers) to allow for greater instruction-level parallelism */

    /* Store the lowest limb + whatever can fit (4 bits) of the second lowest limb */
    u32 t0  = ( a->ed448[ 0] >>  0 ) | ( a->ed448[ 1] << 28 );
    /* 4 bits of a->ed448[1] are in t0, store the rest (24 bits) here + whatever can fit
     * (8 bits) of a->ed448[2] */
    u32 t1  = ( a->ed448[ 1] >>  4 ) | ( a->ed448[ 2] << 24 );
    /* 8 bits of a->ed448[2] are in t1, store the rest (20 bits) here + whatever can fit
     * (12 bits) of a->ed448[3] */
    u32 t2  = ( a->ed448[ 2] >>  8 ) | ( a->ed448[ 3] << 20 );
    /* 12 bits of a->ed448[3] are in t2, store the rest (16 bits) here + whatever can fit
     * (16 bits) of a->ed448[4] */
    u32 t3  = ( a->ed448[ 3] >> 12 ) | ( a->ed448[ 4] << 16 );
    /* 16 bits of a->ed448[4] are in t3, store the rest (12 bits) here + whatever can fit
     * (20 bits) of a->ed448[5] */
    u32 t4  = ( a->ed448[ 4] >> 16 ) | ( a->ed448[ 5] << 12 );
    /* 20 bits of a->ed448[5] are in t4, store the rest (8 bits) here + whatever can fit
     * (24 bits) of a->ed448[6] */
    u32 t5  = ( a->ed448[ 5] >> 20 ) | ( a->ed448[ 6] <<  8 );
    /* 24 bits of a->ed448[6] are in t5, store the rest (4 bits) here + all of a->ed448[7] */
    u32 t6  = ( a->ed448[ 6] >> 24 ) | ( a->ed448[ 7] <<  4 );
    /* Repeat the steps from above */
    u32 t7  = ( a->ed448[ 8] >>  0 ) | ( a->ed448[ 9] << 28 );
    u32 t8  = ( a->ed448[ 9] >>  4 ) | ( a->ed448[10] << 24 );
    u32 t9  = ( a->ed448[10] >>  8 ) | ( a->ed448[11] << 20 );
    u32 t10 = ( a->ed448[11] >> 12 ) | ( a->ed448[12] << 16 );
    u32 t11 = ( a->ed448[12] >> 16 ) | ( a->ed448[13] << 12 );
    u32 t12 = ( a->ed448[13] >> 20 ) | ( a->ed448[14] <<  8 );
    u32 t13 = ( a->ed448[14] >> 24 ) | ( a->ed448[15] <<  4 );

    /* The field elements get encoded as little-endian byte strings according to RFC 8032 */
    _store_32(&buffer[ 0 * 4], t0);
    _store_32(&buffer[ 1 * 4], t1);
    _store_32(&buffer[ 2 * 4], t2);
    _store_32(&buffer[ 3 * 4], t3);
    _store_32(&buffer[ 4 * 4], t4);
    _store_32(&buffer[ 5 * 4], t5);
    _store_32(&buffer[ 6 * 4], t6);
    _store_32(&buffer[ 7 * 4], t7);
    _store_32(&buffer[ 8 * 4], t8);
    _store_32(&buffer[ 9 * 4], t9);
    _store_32(&buffer[10 * 4], t10);
    _store_32(&buffer[11 * 4], t11);
    _store_32(&buffer[12 * 4], t12);
    _store_32(&buffer[13 * 4], t13);
    /* Set the last byte to 0 */
    buffer[7 * 8] = 0;
}

/**
 * @brief Decode a field element according to RFC 8032
 * @param[out] r Decoded field element
 * @param[out] buffer Encoding of the field element
 * @return 1 if decoding succeeded, 0 otherwise
 */
__attribute__((warn_unused_result))
static inline int fe_decode(fe * r, const u8 * buffer) {

    r->ed448[ 0] = ( _load_32(&buffer[ 0]) >>  0 ) & LOW_28_BITS_MASK;
    /* Offset by 28 bits, i.e. three full bytes and a shift of four bits */
    r->ed448[ 1] = ( _load_32(&buffer[ 3]) >>  4 ) & LOW_28_BITS_MASK;
    /* Note that at this point the two shifts by four have added up to a
     * full byte which we incorporate into the offset (array index) */
    r->ed448[ 2] = ( _load_32(&buffer[ 7]) >>  0 ) & LOW_28_BITS_MASK;
    /* Rinse and repeat */
    r->ed448[ 3] = ( _load_32(&buffer[10]) >>  4 ) & LOW_28_BITS_MASK;
    r->ed448[ 4] = ( _load_32(&buffer[14]) >>  0 ) & LOW_28_BITS_MASK;
    r->ed448[ 5] = ( _load_32(&buffer[17]) >>  4 ) & LOW_28_BITS_MASK;
    r->ed448[ 6] = ( _load_32(&buffer[21]) >>  0 ) & LOW_28_BITS_MASK;
    r->ed448[ 7] = ( _load_32(&buffer[24]) >>  4 ) & LOW_28_BITS_MASK;
    r->ed448[ 8] = ( _load_32(&buffer[28]) >>  0 ) & LOW_28_BITS_MASK;
    r->ed448[ 9] = ( _load_32(&buffer[31]) >>  4 ) & LOW_28_BITS_MASK;
    r->ed448[10] = ( _load_32(&buffer[35]) >>  0 ) & LOW_28_BITS_MASK;
    r->ed448[11] = ( _load_32(&buffer[38]) >>  4 ) & LOW_28_BITS_MASK;
    r->ed448[12] = ( _load_32(&buffer[42]) >>  0 ) & LOW_28_BITS_MASK;
    r->ed448[13] = ( _load_32(&buffer[45]) >>  4 ) & LOW_28_BITS_MASK;
    r->ed448[14] = ( _load_32(&buffer[49]) >>  0 ) & LOW_28_BITS_MASK;
    r->ed448[15] = ( _load_32(&buffer[52]) >>  4 ) & LOW_28_BITS_MASK;

    /* Check that the last byte is cleared (except for possibly the highest bit)
     * and that the rest of the bytes (which we have just parsed into limbs)
     * encode a canonical integer (i.e. smaller than p) */
    return fe_is_canonical(r) & ( (buffer[56] & 0x7F) == 0 );
}

/**
 * @brief Return the least-significant bit of the field element
 * @param a Field element
 * @return Least-significant bit of a
 */
static inline int fe_lsb(fe * a) {

    fe_strong_reduce(a, a);
    return a->ed448[0] & 1;
}

#ifdef __cplusplus
}
#endif

#endif /* __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_ED448_32_H */
