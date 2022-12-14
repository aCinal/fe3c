
#ifndef __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_ED448_64_H
#define __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_ED448_64_H

#ifdef __cplusplus
extern "C" {
#endif

#include <field_elements/field_elements.h>
#include <utils/utils.h>

#if !FE3C_64BIT
    #error "Build system inconsistency detected! field_elements_ed448_64.h in use despite FE3C_64BIT not being set"
#endif /* !FE3C_64BIT */

#define LOW_56_BITS_MASK  0xffffffffffffffULL

/* Elliptic curve constant d = -39081 */
static const fe ed448_d = { .ed448 = { 0xffffffffff6756, 0xffffffffffffff, 0xffffffffffffff, 0xffffffffffffff, 0xfffffffffffffe, 0xffffffffffffff, 0xffffffffffffff, 0xffffffffffffff } };
/* Additive identity in the field */
static const fe fe_zero = { .ed448 = { 0, 0, 0, 0, 0, 0, 0, 0 } };
/* Multiplicative identity in the field */
static const fe fe_one = { .ed448 = { 1, 0, 0, 0, 0, 0, 0, 0 } };

static inline u64 _load_56(const u8 src[7]) {

    /* Integer encoding is always little endian according to RFC 8032 */
    u64 dst;
#if FE3C_LILENDIAN_TARGET
    /* Target already little endian - copy the bytes with no shifts */
    dst = 0;
    (void) memcpy(&dst, src, 7);
#else
    /* Big-endian target or endianness unknown (take the safe route) */
    dst  = (u64) src[0];
    dst |= (u64) src[1] << 8;
    dst |= (u64) src[2] << 16;
    dst |= (u64) src[3] << 24;
    dst |= (u64) src[4] << 32;
    dst |= (u64) src[5] << 40;
    dst |= (u64) src[6] << 48;
#endif
    return dst;
}

static inline void _store_64(u8 dst[8], u64 src) {

    /* Integer encoding is always little endian according to RFC 8032 */
#if FE3C_LILENDIAN_TARGET
    (void) memcpy(dst, &src, 8);
#else
    /* After each write do a shift (src is a local variable) */
    dst[0] = (u8) src;  src >>= 8;
    dst[1] = (u8) src;  src >>= 8;
    dst[2] = (u8) src;  src >>= 8;
    dst[3] = (u8) src;  src >>= 8;
    dst[4] = (u8) src;  src >>= 8;
    dst[5] = (u8) src;  src >>= 8;
    dst[6] = (u8) src;  src >>= 8;
    dst[7] = (u8) src;
#endif
}

/**
 * @brief Check if an elements is in its canonical form, i.e. less than the modulus
 * @param a Field element to check
 * @return 1 if a is in canonical form, 0 otherwise
 */
static inline int fe_is_canonical(const fe * a) {

    int canonical = 1;
    canonical &= (a->ed448[0] <  0xffffffffffffffULL);
    canonical &= (a->ed448[1] <= 0xffffffffffffffULL);
    canonical &= (a->ed448[2] <= 0xffffffffffffffULL);
    canonical &= (a->ed448[3] <= 0xffffffffffffffULL);
    canonical &= (a->ed448[4] <= 0xfffffffffffffeULL);
    canonical &= (a->ed448[5] <= 0xffffffffffffffULL);
    canonical &= (a->ed448[6] <= 0xffffffffffffffULL);
    canonical &= (a->ed448[7] <= 0xffffffffffffffULL);
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
    sum |= a->ed448[0] ^ b->ed448[0];
    sum |= a->ed448[1] ^ b->ed448[1];
    sum |= a->ed448[2] ^ b->ed448[2];
    sum |= a->ed448[3] ^ b->ed448[3];
    sum |= a->ed448[4] ^ b->ed448[4];
    sum |= a->ed448[5] ^ b->ed448[5];
    sum |= a->ed448[6] ^ b->ed448[6];
    sum |= a->ed448[7] ^ b->ed448[7];

    /* Bring any 1's down to the least significant bit */
    sum |= (sum >> 32);
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
static inline void fe_conditional_move(volatile fe * r, const fe * a, int move) {

    /* Set the mask to 0x0000000000000000 if move is 0 or to 0xFFFFFFFFFFFFFFFF if it is 1 */
    const fe_limb_type mask = (fe_limb_type)( -(i64) move );

    /* The conditional move implementation relies on the identity (x ^ y) ^ y = x */

    /* Copy the current contents of r into the t array */
    fe_limb_type t0 = r->ed448[0];
    fe_limb_type t1 = r->ed448[1];
    fe_limb_type t2 = r->ed448[2];
    fe_limb_type t3 = r->ed448[3];
    fe_limb_type t4 = r->ed448[4];
    fe_limb_type t5 = r->ed448[5];
    fe_limb_type t6 = r->ed448[6];
    fe_limb_type t7 = r->ed448[7];

    /* XOR the t's with the limbs of a */
    fe_limb_type x0 = t0 ^ a->ed448[0];
    fe_limb_type x1 = t1 ^ a->ed448[1];
    fe_limb_type x2 = t2 ^ a->ed448[2];
    fe_limb_type x3 = t3 ^ a->ed448[3];
    fe_limb_type x4 = t4 ^ a->ed448[4];
    fe_limb_type x5 = t5 ^ a->ed448[5];
    fe_limb_type x6 = t6 ^ a->ed448[6];
    fe_limb_type x7 = t7 ^ a->ed448[7];

    /* Set the x array to all zeroes if move is zero or leave it be if move is one */
    x0 &= mask;
    x1 &= mask;
    x2 &= mask;
    x3 &= mask;
    x4 &= mask;
    x5 &= mask;
    x6 &= mask;
    x7 &= mask;

    /* If move=0 then x0-x7 are all zero and so we write back the limbs of r into r.
     * If move=1 then x0-x7 contain r XOR a and so by XORing back with the limbs of r we
     * write the limbs of a into r */
    r->ed448[0] = t0 ^ x0;
    r->ed448[1] = t1 ^ x1;
    r->ed448[2] = t2 ^ x2;
    r->ed448[3] = t3 ^ x3;
    r->ed448[4] = t4 ^ x4;
    r->ed448[5] = t5 ^ x5;
    r->ed448[6] = t6 ^ x6;
    r->ed448[7] = t7 ^ x7;
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

    /* Use 128-bit-wide auxiliary variables */
    u128 t0 = a->ed448[0];
    u128 t1 = a->ed448[1];
    u128 t2 = a->ed448[2];
    u128 t3 = a->ed448[3];
    u128 t4 = a->ed448[4];
    u128 t5 = a->ed448[5];
    u128 t6 = a->ed448[6];
    u128 t7 = a->ed448[7];

    /* The modulus can be expressed as p = u - v - 1, where u = 2^448 and v = 2^224.
     * Express (split) the value a as a = x + uy + vz. Then
     *
     *   a mod p = (x + uy + vz) mod (u - v - 1)
     *           = (x + (u - v - 1)y + vy + y + vz) mod (u - v - 1)
     *           = (x + vz + vy + y) mod p
     *
     * To split a into (x, y, z) we simply let the carries bubble up to the top
     * limb. Whatever has overflown the top limb is equal to y. Limbs 4-7 hold
     * the value of z and the low four limbs hold x.
     */

    /* Add whatever has overflown in t0 (a->ed448[0]) to t1 (a->ed448[1])
     * and mask out the overflow in t0 */
    t1 += t0 >> 56;  t0 &= LOW_56_BITS_MASK;
    /* Rinse and repeat for other limbs */
    t2 += t1 >> 56;  t1 &= LOW_56_BITS_MASK;
    t3 += t2 >> 56;  t2 &= LOW_56_BITS_MASK;
    t4 += t3 >> 56;  t3 &= LOW_56_BITS_MASK;
    t5 += t4 >> 56;  t4 &= LOW_56_BITS_MASK;
    t6 += t5 >> 56;  t5 &= LOW_56_BITS_MASK;
    t7 += t6 >> 56;  t6 &= LOW_56_BITS_MASK;

    /* At this point low 56 bits of each of limbs t0-t7 hold the value of a modulo
     * 2^448, i.e. x + vz, using the notation from above. Add vy and y to this
     * result. Note that y is stored in the "overflow" of t7, i.e. bits starting
     * from bit 56. Also note that adding vy is equivalent to adding y to the limbs
     * starting from limb 4 (since v = 2^{224} = 2^{4*56}). */

    /* Add y */
    t0 += t7 >> 56;
    /* Add y 2^224 */
    t4 += t7 >> 56;
    /* Clear the overflow in t7 */
    t7 &= LOW_56_BITS_MASK;

    /* Do this once more to ensure we correctly normalize
     * all integers possible to be represented in our system,
     * i.e. we don't get surprised by (t7 >> 56) leading
     * to overflow in t0 or t4 */
    t1 += t0 >> 56;  t0 &= LOW_56_BITS_MASK;
    t2 += t1 >> 56;  t1 &= LOW_56_BITS_MASK;
    t3 += t2 >> 56;  t2 &= LOW_56_BITS_MASK;
    t4 += t3 >> 56;  t3 &= LOW_56_BITS_MASK;
    t5 += t4 >> 56;  t4 &= LOW_56_BITS_MASK;
    t6 += t5 >> 56;  t5 &= LOW_56_BITS_MASK;
    t7 += t6 >> 56;  t6 &= LOW_56_BITS_MASK;
    t0 += t7 >> 56;
    t4 += t7 >> 56;
    t7 &= LOW_56_BITS_MASK;

    r->ed448[0] = t0;
    r->ed448[1] = t1;
    r->ed448[2] = t2;
    r->ed448[3] = t3;
    r->ed448[4] = t4;
    r->ed448[5] = t5;
    r->ed448[6] = t6;
    r->ed448[7] = t7;
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
    t.ed448[0] = r->ed448[0] + 1;
    t.ed448[1] = r->ed448[1];
    t.ed448[2] = r->ed448[2];
    t.ed448[3] = r->ed448[3];
    /* Add 2^224 to the mix */
    t.ed448[4] = r->ed448[4] + 1;
    t.ed448[5] = r->ed448[5];
    t.ed448[6] = r->ed448[6];
    t.ed448[7] = r->ed448[7];

    t.ed448[1] += t.ed448[0] >> 56;  t.ed448[0] &= LOW_56_BITS_MASK;
    t.ed448[2] += t.ed448[1] >> 56;  t.ed448[1] &= LOW_56_BITS_MASK;
    t.ed448[3] += t.ed448[2] >> 56;  t.ed448[2] &= LOW_56_BITS_MASK;
    t.ed448[4] += t.ed448[3] >> 56;  t.ed448[3] &= LOW_56_BITS_MASK;
    t.ed448[5] += t.ed448[4] >> 56;  t.ed448[4] &= LOW_56_BITS_MASK;
    t.ed448[6] += t.ed448[5] >> 56;  t.ed448[5] &= LOW_56_BITS_MASK;
    t.ed448[7] += t.ed448[6] >> 56;  t.ed448[6] &= LOW_56_BITS_MASK;

    /* At this point t.ed448[7] contains the highest limb of r + 2^224 + 1. Try subtracting
     * 2^448 - if we get an underflow this means that r < 2^448 - 2^224 - 1 and so r is the
     * final result. Otherwise we must return r - p (note that this includes the case where
     * r = p, as no underflow will occur then and t.ed448[4] will be equal to zero). */
    t.ed448[7] -= (1ULL << 56);
    /* Check the highest bit of t.ed448[7] for underflow. If the highest bit is set then
     * underflow occurred and so we return r, otherwise we set r ::= t and return that */
    fe_conditional_move(r, &t, (t.ed448[7] >> 63) ^ 1);
}

/**
 * @brief Negate an element in the field
 * @param[out] r The result of negation
 * @param[in] a Element to be negated
 */
static inline void fe_neg(fe * r, const fe * a) {

    /* Check against underflow */
    FE3C_SANITY_CHECK(a->ed448[0] <  0x1fffffffffffffeULL);
    FE3C_SANITY_CHECK(a->ed448[1] <= 0x1fffffffffffffeULL);
    FE3C_SANITY_CHECK(a->ed448[2] <= 0x1fffffffffffffeULL);
    FE3C_SANITY_CHECK(a->ed448[3] <= 0x1fffffffffffffeULL);
    FE3C_SANITY_CHECK(a->ed448[4] <= 0x1fffffffffffffcULL);
    FE3C_SANITY_CHECK(a->ed448[5] <= 0x1fffffffffffffeULL);
    FE3C_SANITY_CHECK(a->ed448[6] <= 0x1fffffffffffffeULL);
    FE3C_SANITY_CHECK(a->ed448[7] <= 0x1fffffffffffffeULL);

    /* Set r to 2p-a so as to not require strong reduction of a */
    r->ed448[0] = 0x1fffffffffffffeULL - a->ed448[0];
    r->ed448[1] = 0x1fffffffffffffeULL - a->ed448[1];
    r->ed448[2] = 0x1fffffffffffffeULL - a->ed448[2];
    r->ed448[3] = 0x1fffffffffffffeULL - a->ed448[3];
    r->ed448[4] = 0x1fffffffffffffcULL - a->ed448[4];
    r->ed448[5] = 0x1fffffffffffffeULL - a->ed448[5];
    r->ed448[6] = 0x1fffffffffffffeULL - a->ed448[6];
    r->ed448[7] = 0x1fffffffffffffeULL - a->ed448[7];
}

/**
 * @brief Add two field elements
 * @param[out] r Result of the addition, i.e. the sum r = a + b
 * @param[in] a Operand
 * @param[in] b Operand
 */
static inline void fe_add(fe * r, const fe * a, const fe * b) {

    r->ed448[0] = a->ed448[0] + b->ed448[0];
    r->ed448[1] = a->ed448[1] + b->ed448[1];
    r->ed448[2] = a->ed448[2] + b->ed448[2];
    r->ed448[3] = a->ed448[3] + b->ed448[3];
    r->ed448[4] = a->ed448[4] + b->ed448[4];
    r->ed448[5] = a->ed448[5] + b->ed448[5];
    r->ed448[6] = a->ed448[6] + b->ed448[6];
    r->ed448[7] = a->ed448[7] + b->ed448[7];
}

/**
 * @brief Subtract one field element from another
 * @param[out] r Result of the subtraction, i.e. the difference r = a - b
 * @param[in] a Minuend
 * @param[in] b Subtrahend
 */
static inline void fe_sub(fe * r, const fe * a, const fe * b) {

    /* Compute a + 2p - b so as to not risk underflow */
    r->ed448[0] = a->ed448[0] + 0x1fffffffffffffeULL - b->ed448[0];
    r->ed448[1] = a->ed448[1] + 0x1fffffffffffffeULL - b->ed448[1];
    r->ed448[2] = a->ed448[2] + 0x1fffffffffffffeULL - b->ed448[2];
    r->ed448[3] = a->ed448[3] + 0x1fffffffffffffeULL - b->ed448[3];
    r->ed448[4] = a->ed448[4] + 0x1fffffffffffffcULL - b->ed448[4];
    r->ed448[5] = a->ed448[5] + 0x1fffffffffffffeULL - b->ed448[5];
    r->ed448[6] = a->ed448[6] + 0x1fffffffffffffeULL - b->ed448[6];
    r->ed448[7] = a->ed448[7] + 0x1fffffffffffffeULL - b->ed448[7];
    /* We could also call fe_neg() followed by fe_add(), but this would require
     * an intermediate fe variable to support aliasing */
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
     * since p = S^2 - S - 1. Note that in our representation the terms x and z (u and v)
     * correspond to limbs 0-3 and 4-7, respectively.
     */

    /* TODO: Optimize stack usage */

    u128 a0 = a->ed448[0];
    u128 a1 = a->ed448[1];
    u128 a2 = a->ed448[2];
    u128 a3 = a->ed448[3];
    u128 a4 = a->ed448[4];
    u128 a5 = a->ed448[5];
    u128 a6 = a->ed448[6];
    u128 a7 = a->ed448[7];

    u128 b0 = b->ed448[0];
    u128 b1 = b->ed448[1];
    u128 b2 = b->ed448[2];
    u128 b3 = b->ed448[3];
    u128 b4 = b->ed448[4];
    u128 b5 = b->ed448[5];
    u128 b6 = b->ed448[6];
    u128 b7 = b->ed448[7];

    /* Do a simple schoolbook multiplication of (x+z)(u+v). Let:
     *
     *   m = (x+z) = (m3, m2, m1, m0) and k = (u+v) = (k3, k2, k1, k0)
     *
     * where mi, ki are all 56-digits for i=0,1,2,3. We shall compute simple
     * products and add them together column by column:
     *
     *                              m3     m2     m1     m0
     *                              k3     k2     k1     k0
     * -----------------------------------------------------------
     *                           | m0k3   m0k2   m0k1   m0k0
     *                      m1k3 | m1k2   m1k1   m1k0
     *               m2k3   m2k2 | m2k1   m2k0
     *        m3k3   m3k2   m3k1 | m3k0
     *                           |
     *                           |
     *        2^224 cutoff_______|
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
     * partial products mikj where i+j>3.
     */

    u128 m0 = a0 + a4;
    u128 m1 = a1 + a5;
    u128 m2 = a2 + a6;
    u128 m3 = a3 + a7;

    u128 k0 = b0 + b4;
    u128 k1 = b1 + b5;
    u128 k2 = b2 + b6;
    u128 k3 = b3 + b7;

    u128 r0 =             m1*k3 + m2*k2 + m3*k1;
    u128 r1 =                     m2*k3 + m3*k2;
    u128 r2 =                             m3*k3;
    u128 r3 = 0;
    /* Add at the 2^224 "level" the terms which exceeded 2^448 (e in the above identity),
     * which are currently held in r0-r3 */
    u128 r4 = r0 + m0*k0;
    u128 r5 = r1 + m0*k1 + m1*k0;
    u128 r6 = r2 + m0*k2 + m1*k1 + m2*k0;
    u128 r7 =      m0*k3 + m1*k2 + m2*k1 + m3*k0;

    /* Do a schoolbook multiplication for x*u and z*v as well. Here, however, we do not risk
     * exceeding 2^448 since all inputs are bound by 2^224. */
    u128 xu0 = a0*b0;
    u128 xu1 = a0*b1 + a1*b0;
    u128 xu2 = a0*b2 + a1*b1 + a2*b0;
    u128 xu3 = a0*b3 + a1*b2 + a2*b1 + a3*b0;
    u128 xu4 =         a1*b3 + a2*b2 + a3*b1;
    u128 xu5 =                 a2*b3 + a3*b2;
    u128 xu6 =                         a3*b3;

    u128 zv0 = a4*b4;
    u128 zv1 = a4*b5 + a5*b4;
    u128 zv2 = a4*b6 + a5*b5 + a6*b4;
    u128 zv3 = a4*b7 + a5*b6 + a6*b5 + a7*b4;
    u128 zv4 =         a5*b7 + a6*b6 + a7*b5;
    u128 zv5 =                 a6*b7 + a7*b6;
    u128 zv6 =                         a7*b7;

    /* Subtract xu 2^224 */
    r0 -= xu4;
    r1 -= xu5;
    r2 -= xu6;
    r3 -= 0;
    r4 -= xu0 + xu4;
    r5 -= xu1 + xu5;
    r6 -= xu2 + xu6;
    r7 -= xu3;

    r1 += r0 >> 56;  r0 &= LOW_56_BITS_MASK;
    r2 += r1 >> 56;  r1 &= LOW_56_BITS_MASK;
    r3 += r2 >> 56;  r2 &= LOW_56_BITS_MASK;
    r4 += r3 >> 56;  r3 &= LOW_56_BITS_MASK;
    r5 += r4 >> 56;  r4 &= LOW_56_BITS_MASK;
    r6 += r5 >> 56;  r5 &= LOW_56_BITS_MASK;
    r7 += r6 >> 56;  r6 &= LOW_56_BITS_MASK;
    r0 += r7 >> 56;
    r4 += r7 >> 56;
    r7 &= LOW_56_BITS_MASK;

    /* Add (xu + zv) */
    r0 += xu0 + zv0;
    r1 += xu1 + zv1;
    r2 += xu2 + zv2;
    r3 += xu3 + zv3;
    r4 += xu4 + zv4;
    r5 += xu5 + zv5;
    r6 += xu6 + zv6;

    r1 += r0 >> 56;  r0 &= LOW_56_BITS_MASK;
    r2 += r1 >> 56;  r1 &= LOW_56_BITS_MASK;
    r3 += r2 >> 56;  r2 &= LOW_56_BITS_MASK;
    r4 += r3 >> 56;  r3 &= LOW_56_BITS_MASK;
    r5 += r4 >> 56;  r4 &= LOW_56_BITS_MASK;
    r6 += r5 >> 56;  r5 &= LOW_56_BITS_MASK;
    r7 += r6 >> 56;  r6 &= LOW_56_BITS_MASK;
    r0 += r7 >> 56;
    r4 += r7 >> 56;
    r7 &= LOW_56_BITS_MASK;

    r->ed448[0] = r0;
    r->ed448[1] = r1;
    r->ed448[2] = r2;
    r->ed448[3] = r3;
    r->ed448[4] = r4;
    r->ed448[5] = r5;
    r->ed448[6] = r6;
    r->ed448[7] = r7;
}

/**
 * @brief Multiply an element by itself
 * @param[out] r Result of the squaring, i.e. the product r = a a
 * @param[in] a Field element to square
 */
static inline void fe_square(fe * r, const fe * a) {

#if !FE3C_OPTIMIZATION_FAST_SQUARING
    fe_mul(r, a, a);
#else
    u128 a0 = a->ed448[0];
    u128 a1 = a->ed448[1];
    u128 a2 = a->ed448[2];
    u128 a3 = a->ed448[3];
    u128 a4 = a->ed448[4];
    u128 a5 = a->ed448[5];
    u128 a6 = a->ed448[6];
    u128 a7 = a->ed448[7];

    /* Do the Karatsuba algorithm leveraging the form of the Goldilocks prime, but
     * let the compiler group together like terms whenever possible. Partial single-
     * precision products aibj are equivalent to ajbi when a=b, i.e. when squaring.
     * Leverage this to halve the number of single-precision multiplications.
     *
     * Let S = 2^224. Then:
     *
     *   (x + zS)^2 = x^2 + 2xzS + z^2S^2
     *              = x^2 + z^2 + (2xz + z^2)S (mod p)
     *              = x^2 + z^2 + ((x+z)^2 - x^2)S (mod p)
     *
     * since p = S^2 - S - 1. Note that in our representation the terms x and z
     * correspond to limbs 0-3 and 4-7, respectively. Also note that the reason we gain
     * anything by using Karatsuba's trick in the last line of the above identity is
     * that squaring can be optimized by grouping like terms.
     */

    u128 m0 = a0 + a4;
    u128 m1 = a1 + a5;
    u128 m2 = a2 + a6;
    u128 m3 = a3 + a7;

    /* Use the simple schoolbook multiplication algorithm to square (x+z) */
    u128 r0 =             m1*m3 + m2*m2 + m3*m1;
    u128 r1 =                     m2*m3 + m3*m2;
    u128 r2 =                             m3*m3;
    u128 r3 = 0;
    /* Add at the 2^224 "level" the terms which exceeded 2^448 which are currently held in r0-r3.
     * See comments in fe_mul() for a more detailed explanation. */
    u128 r4 = r0 + m0*m0;
    u128 r5 = r1 + m0*m1 + m1*m0;
    u128 r6 = r2 + m0*m2 + m1*m1 + m2*m0;
    u128 r7 =      m0*m3 + m1*m2 + m2*m1 + m3*m0;

    /* Use schoolbook multiplication to square x and z as well, this time without risk of
     * exceeding 2^448 since all inputs are bound by 2^224. */
    u128 xx0 = a0*a0;
    u128 xx1 = a0*a1 + a1*a0;
    u128 xx2 = a0*a2 + a1*a1 + a2*a0;
    u128 xx3 = a0*a3 + a1*a2 + a2*a1 + a3*a0;
    u128 xx4 =         a1*a3 + a2*a2 + a3*a1;
    u128 xx5 =                 a2*a3 + a3*a2;
    u128 xx6 =                         a3*a3;

    u128 zz0 = a4*a4;
    u128 zz1 = a4*a5 + a5*a4;
    u128 zz2 = a4*a6 + a5*a5 + a6*a4;
    u128 zz3 = a4*a7 + a5*a6 + a6*a5 + a7*a4;
    u128 zz4 =         a5*a7 + a6*a6 + a7*a5;
    u128 zz5 =                 a6*a7 + a7*a6;
    u128 zz6 =                         a7*a7;

    /* Subtract x^2 2^224 */
    r0 -= xx4;
    r1 -= xx5;
    r2 -= xx6;
    r3 -= 0;
    r4 -= xx0 + xx4;
    r5 -= xx1 + xx5;
    r6 -= xx2 + xx6;
    r7 -= xx3;

    r1 += r0 >> 56;  r0 &= LOW_56_BITS_MASK;
    r2 += r1 >> 56;  r1 &= LOW_56_BITS_MASK;
    r3 += r2 >> 56;  r2 &= LOW_56_BITS_MASK;
    r4 += r3 >> 56;  r3 &= LOW_56_BITS_MASK;
    r5 += r4 >> 56;  r4 &= LOW_56_BITS_MASK;
    r6 += r5 >> 56;  r5 &= LOW_56_BITS_MASK;
    r7 += r6 >> 56;  r6 &= LOW_56_BITS_MASK;
    r0 += r7 >> 56;
    r4 += r7 >> 56;
    r7 &= LOW_56_BITS_MASK;

    /* Add (x^2 + z^2) */
    r0 += xx0 + zz0;
    r1 += xx1 + zz1;
    r2 += xx2 + zz2;
    r3 += xx3 + zz3;
    r4 += xx4 + zz4;
    r5 += xx5 + zz5;
    r6 += xx6 + zz6;

    r1 += r0 >> 56;  r0 &= LOW_56_BITS_MASK;
    r2 += r1 >> 56;  r1 &= LOW_56_BITS_MASK;
    r3 += r2 >> 56;  r2 &= LOW_56_BITS_MASK;
    r4 += r3 >> 56;  r3 &= LOW_56_BITS_MASK;
    r5 += r4 >> 56;  r4 &= LOW_56_BITS_MASK;
    r6 += r5 >> 56;  r5 &= LOW_56_BITS_MASK;
    r7 += r6 >> 56;  r6 &= LOW_56_BITS_MASK;
    r0 += r7 >> 56;
    r4 += r7 >> 56;
    r7 &= LOW_56_BITS_MASK;

    r->ed448[0] = r0;
    r->ed448[1] = r1;
    r->ed448[2] = r2;
    r->ed448[3] = r3;
    r->ed448[4] = r4;
    r->ed448[5] = r5;
    r->ed448[6] = r6;
    r->ed448[7] = r7;
#endif /* !FE3C_OPTIMIZATION_FAST_SQUARING */
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

    /* Store the lowest limb + whatever can fit (8 bits) of the second lowest limb */
    u64 t0 = ( a->ed448[0] >>  0 ) | ( a->ed448[1] << 56 );
    /* One byte of a->ed448[1] is in t0, store the rest (48 bits) here + whatever can fit
     * (16 bits) of a->ed448[2] */
    u64 t1 = ( a->ed448[1] >>  8 ) | ( a->ed448[2] << 48 );
    /* 16 bits of a->ed448[2] are in t1, store the rest (40 bits) here + whatever can fit
     * (24 bits) of a->ed448[3] */
    u64 t2 = ( a->ed448[2] >> 16 ) | ( a->ed448[3] << 40 );
    /* 24 bits of a->ed448[3] are in t2, store the rest (32 bits) here + whatever can fit
     * (32 bits) of a->ed448[4] */
    u64 t3 = ( a->ed448[3] >> 24 ) | ( a->ed448[4] << 32 );
    /* 32 bits of a->ed448[4] are in t3, store the rest (24 bits) here + whatever can fit
     * (40 bits) of a->ed448[5] */
    u64 t4 = ( a->ed448[4] >> 32 ) | ( a->ed448[5] << 24 );
    /* 40 bits of a->ed448[5] are in t4, store the rest (16 bits) here + whatever can fit
     * (48 bits) of a->ed448[6] */
    u64 t5 = ( a->ed448[5] >> 40 ) | ( a->ed448[6] << 16 );
    /* Store the missing byte of a->ed448[6] and all of a->ed448[7] here */
    u64 t6 = ( a->ed448[6] >> 48 ) | ( a->ed448[7] << 8 );

    /* The field elements get encoded as little-endian byte strings according to RFC 8032 */
    _store_64(&buffer[0 * 8], t0);
    _store_64(&buffer[1 * 8], t1);
    _store_64(&buffer[2 * 8], t2);
    _store_64(&buffer[3 * 8], t3);
    _store_64(&buffer[4 * 8], t4);
    _store_64(&buffer[5 * 8], t5);
    _store_64(&buffer[6 * 8], t6);
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

    r->ed448[0] = _load_56(&buffer[0 * 7]);
    r->ed448[1] = _load_56(&buffer[1 * 7]);
    r->ed448[2] = _load_56(&buffer[2 * 7]);
    r->ed448[3] = _load_56(&buffer[3 * 7]);
    r->ed448[4] = _load_56(&buffer[4 * 7]);
    r->ed448[5] = _load_56(&buffer[5 * 7]);
    r->ed448[6] = _load_56(&buffer[6 * 7]);
    r->ed448[7] = _load_56(&buffer[7 * 7]);

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

#endif /* __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_ED448_64_H */
