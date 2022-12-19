
#ifndef __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_ED25519_64_H
#define __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_ED25519_64_H

#ifdef __cplusplus
extern "C" {
#endif

#include <field_elements/field_elements.h>
#include <utils/utils.h>

#if !FE3C_64BIT
    #error "Build system inconsistency detected! field_elements_ed25519_64.h in use despite FE3C_64BIT not being set"
#endif /* !FE3C_64BIT */

#define LOW_51_BITS_MASK  0x7ffffffffffffULL
#define FE25519_STR "0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx"
#define FE25519_TO_STR(x) \
    x[0], x[1], x[2], x[3], x[4]

/* Elliptic curve constant d = -121665/121666 */
static const fe25519 ed25519_d = { 0x34dca135978a3, 0x1a8283b156ebd, 0x5e7a26001c029, 0x739c663a03cbb, 0x52036cee2b6ff };
/* Additive identity in the field */
static const fe25519 fe_zero = { 0, 0, 0, 0, 0 };
/* Multiplicative identity in the field */
static const fe25519 fe_one = { 1, 0, 0, 0, 0 };
/* "Positive" (even) square root of -1 in the field */
static const fe25519 fe_i = { 0x61b274a0ea0b0, 0xd5a5fc8f189d, 0x7ef5e9cbd0c60, 0x78595a6804c9e, 0x2b8324804fc1d };

static inline u64 _load_64(const u8 src[8]) {

    /* Integer encoding is always little endian according to RFC 8032 */
    u64 dst;
#if FE3C_LILENDIAN_TARGET
    /* Target already little endian - copy the bytes with no shifts */
    (void) memcpy(&dst, src, 8);
#else
    /* Big-endian target or endianness unknown (take the safe route) */
    dst  = (u64) src[0];
    dst |= (u64) src[1] << 8;
    dst |= (u64) src[2] << 16;
    dst |= (u64) src[3] << 24;
    dst |= (u64) src[4] << 32;
    dst |= (u64) src[5] << 40;
    dst |= (u64) src[6] << 48;
    dst |= (u64) src[7] << 56;
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
 * @brief Uncondionally copy a field element
 * @param r Destination field element
 * @param a Source field element
 */
static inline void fe_copy(fe25519 r, const fe25519 a) {

    r[0] = a[0];
    r[1] = a[1];
    r[2] = a[2];
    r[3] = a[3];
    r[4] = a[4];
}

/**
 * @brief Check if an elements is in its canonical form, i.e. less than the modulus
 * @param a Field element to check
 * @return 1 if a is in canonical form, 0 otherwise
 */
static inline int fe_is_canonical(const fe25519 a) {

    int canonical = 1;
    canonical &= (a[0] <  0x7ffffffffffedULL);
    canonical &= (a[1] <= 0x7ffffffffffffULL);
    canonical &= (a[2] <= 0x7ffffffffffffULL);
    canonical &= (a[3] <= 0x7ffffffffffffULL);
    canonical &= (a[4] <= 0x7ffffffffffffULL);
    return canonical;
}

/**
 * @brief Do a constant-time equality check
 * @param[in] a The first element to check
 * @param[in] b The second element to check
 * @return 1 if a = b, 0 otherwise
 * @note The elements should be reduced by the caller first
 */
static inline int fe_equal(const fe25519 a, const fe25519 b) {

    fe_limb_type sum = 0;

    /* Do an XOR between the two elements, if they are equal this should amount
     * to zero */
    sum |= a[0] ^ b[0];
    sum |= a[1] ^ b[1];
    sum |= a[2] ^ b[2];
    sum |= a[3] ^ b[3];
    sum |= a[4] ^ b[4];

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
static inline void fe_conditional_move(fe25519 r, const fe25519 a, int move) {

    /* Set the mask to 0x0000000000000000 if move is 0 or to 0xFFFFFFFFFFFFFFFF if it is 1 */
    const fe_limb_type mask = (fe_limb_type)( -(i64) move );

    /* The conditional move implementation relies on the identity (x ^ y) ^ y = x */

    /* Copy the current contents of r into the t array */
    fe_limb_type t0 = r[0];
    fe_limb_type t1 = r[1];
    fe_limb_type t2 = r[2];
    fe_limb_type t3 = r[3];
    fe_limb_type t4 = r[4];

    /* XOR the t's with the limbs of a */
    fe_limb_type x0 = t0 ^ a[0];
    fe_limb_type x1 = t1 ^ a[1];
    fe_limb_type x2 = t2 ^ a[2];
    fe_limb_type x3 = t3 ^ a[3];
    fe_limb_type x4 = t4 ^ a[4];

    /* Set the x array to all zeroes if move is zero or leave it be if move is one */
    x0 &= mask;
    x1 &= mask;
    x2 &= mask;
    x3 &= mask;
    x4 &= mask;

    /* If move=0 then x0-x4 are all zero and so we write back the limbs of r into r.
     * If move=1 then x0-x4 contain r XOR a and so by XORing back with the limbs of r we
     * write the limbs of a into r */
    r[0] = t0 ^ x0;
    r[1] = t1 ^ x1;
    r[2] = t2 ^ x2;
    r[3] = t3 ^ x3;
    r[4] = t4 ^ x4;
}

/**
 * @brief Do a weak modular reduction modulo the underlying field's characteristic p
 * @param[out] r Result of the reduction
 * @param[in] a Field element to be reduced
 * @note Note that the result need to be in canonical form, i.e. between 0 and p-1, it need only be less than 2p
 */
static inline void fe_weak_reduce(fe25519 r, const fe25519 a) {

    /* Do a "relaxed" reduction (to borrow terminology form Michael Scott's "Slothful reduction" paper)
     * - this ensures the result is less than 2p (where p = 2^255 - 19) */

    /* Use 128-bit-wide auxiliary variables */
    u128 t0 = a[0];
    u128 t1 = a[1];
    u128 t2 = a[2];
    u128 t3 = a[3];
    u128 t4 = a[4];

    /* The modulus can be expressed as p = s - c, where s = 2^255 and c = 19.
     * Express (split) the value a as a = x + sy. Then
     *
     *     a mod p = (x + sy) mod (s - c)
     *             = (x + (s - c)y + cy) mod (s - c)
     *             = (x + cy) mod p
     *
     * To split a into (x, y) we simply let the carries bubble up to the top
     * limb. Whatever has overflown the top limb is equal to y and in the limbs
     * we have the value x.
     */

    /* Add whatever has overflown in t0 (a[0]) to t1 (a[1])
     * and mask out the overflow in t0 */
    t1 += t0 >> 51;  t0 &= LOW_51_BITS_MASK;
    /* Rinse and repeat for the other limbs */
    t2 += t1 >> 51;  t1 &= LOW_51_BITS_MASK;
    t3 += t2 >> 51;  t2 &= LOW_51_BITS_MASK;
    t4 += t3 >> 51;  t3 &= LOW_51_BITS_MASK;
    /* Add c*y to x */
    t0 += 19 * (t4 >> 51);
    t4 &= LOW_51_BITS_MASK;

    /* Do this once more to ensure we correctly normalize
     * all integers possible to be represented in our system,
     * i.e. we don't get surprised by 19 * (t4 >> 51) leading
     * to overflow in t0 */
    t1 += t0 >> 51;  t0 &= LOW_51_BITS_MASK;
    t2 += t1 >> 51;  t1 &= LOW_51_BITS_MASK;
    t3 += t2 >> 51;  t2 &= LOW_51_BITS_MASK;
    t4 += t3 >> 51;  t3 &= LOW_51_BITS_MASK;
    t0 += 19 * (t4 >> 51);
    t4 &= LOW_51_BITS_MASK;

    r[0] = t0;
    r[1] = t1;
    r[2] = t2;
    r[3] = t3;
    r[4] = t4;
}

/**
 * @brief Do a strong modular reduction modulo the underlying field's characteristic p
 * @param[out] r Result of the reduction
 * @param[in] a Field element to be reduced
 * @note The result is guaranteed to be in canonical form, i.e. between 0 and p-1
 */
static inline void fe_strong_reduce(fe25519 r, const fe25519 a) {

    fe_weak_reduce(r, a);
    /* After the weak reduction r is congruent to a and less than 2p */

    /* Compute r-p and conditionally use it as a result if r is larger than p */
    fe25519 t;
    /* Store r + 19 in t (we will subtract 2^255 from it, resulting in t = r - p) */
    t[0] = r[0] + 19;
    t[1] = r[1];
    t[2] = r[2];
    t[3] = r[3];
    t[4] = r[4];

    t[1] += t[0] >> 51;  t[0] &= LOW_51_BITS_MASK;
    t[2] += t[1] >> 51;  t[1] &= LOW_51_BITS_MASK;
    t[3] += t[2] >> 51;  t[2] &= LOW_51_BITS_MASK;
    t[4] += t[3] >> 51;  t[3] &= LOW_51_BITS_MASK;

    /* At this point t[4] contains the highest limb of r + 19. Try subtracting
     * 2^255 - if we get an underflow this means that r < 2^255 - 19 and so r is the
     * final result. Otherwise we must return r - p (note that this includes the case
     * where r = p, as no underflow will occur then and t[4] will be equal to
     * zero). */
    t[4] -= (1ULL << 51);

    /* Check the highest bit of t[4] for underflow. If the highest bit is set then
     * underflow occurred and so we return r, otherwise we set r ::= t and return that */
    fe_conditional_move(r, t, (t[4] >> 63) ^ 1);
}

/**
 * @brief Negate an element in the field
 * @param[out] r The result of negation
 * @param[in] a Element to be negated
 */
static inline void fe_neg(fe25519 r, const fe25519 a) {

    /* Check against underflow */
    FE3C_SANITY_CHECK(a[0] <= 0xfffffffffffdaULL, FE25519_STR, FE25519_TO_STR(a));
    FE3C_SANITY_CHECK(a[1] <= 0xffffffffffffeULL, FE25519_STR, FE25519_TO_STR(a));
    FE3C_SANITY_CHECK(a[2] <= 0xffffffffffffeULL, FE25519_STR, FE25519_TO_STR(a));
    FE3C_SANITY_CHECK(a[3] <= 0xffffffffffffeULL, FE25519_STR, FE25519_TO_STR(a));
    FE3C_SANITY_CHECK(a[4] <= 0xffffffffffffeULL, FE25519_STR, FE25519_TO_STR(a));

    /* Set r to 2p-a so as to not require strong reduction of a */
    r[0] = 0xfffffffffffdaULL - a[0];
    r[1] = 0xffffffffffffeULL - a[1];
    r[2] = 0xffffffffffffeULL - a[2];
    r[3] = 0xffffffffffffeULL - a[3];
    r[4] = 0xffffffffffffeULL - a[4];
}

/**
 * @brief Add two field elements
 * @param[out] r Result of the addition, i.e. the sum r = a + b
 * @param[in] a Operand
 * @param[in] b Operand
 */
static inline void fe_add(fe25519 r, const fe25519 a, const fe25519 b) {

    r[0] = a[0] + b[0];
    r[1] = a[1] + b[1];
    r[2] = a[2] + b[2];
    r[3] = a[3] + b[3];
    r[4] = a[4] + b[4];
}

/**
 * @brief Subtract one field element from another
 * @param[out] r Result of the subtraction, i.e. the difference r = a - b
 * @param[in] a Minuend
 * @param[in] b Subtrahend
 */
static inline void fe_sub(fe25519 r, const fe25519 a, const fe25519 b) {

    /* Compute a + 2p - b so as to not risk underflow */
    r[0] = a[0] + 0xfffffffffffdaULL - b[0];
    r[1] = a[1] + 0xffffffffffffeULL - b[1];
    r[2] = a[2] + 0xffffffffffffeULL - b[2];
    r[3] = a[3] + 0xffffffffffffeULL - b[3];
    r[4] = a[4] + 0xffffffffffffeULL - b[4];
    /* We could also call fe_neg() followed by fe_add(), but this would require
     * an intermediate fe variable to support aliasing */
}

/**
 * @brief Multiply two field elements
 * @param[out] r Result of the multiplication, i.e. the product r = a b
 * @param[in] a Operand
 * @param[in] b Operand
 */
static inline void fe_mul(fe25519 r, const fe25519 a, const fe25519 b) {

    u128 r0, r1, r2, r3, r4;

    u128 a0 = a[0];
    u128 a1 = a[1];
    u128 a2 = a[2];
    u128 a3 = a[3];
    u128 a4 = a[4];

    u128 b0 = b[0];
    u128 b1 = b[1];
    u128 b2 = b[2];
    u128 b3 = b[3];
    u128 b4 = b[4];

    /* TODO: Try using Karatsuba here */

    /* Fuse the multiplication with (weak) reduction using the identity:
     *
     *     a + b 2^255 = a + (2^255 - 19) b + 19 b = a + 19 b (mod p)
     *
     * We use the schoolbook multiplication algorithm (for now) which multiplies
     * a = (a4, a3, a2, a1, a0), b = (b4, b3, b2, b1, b0) (where ai, bi are 51-bit
     * digits) by computing the simple products and adding them together in columns:
     *
     *                                  a4     a3     a2     a1     a0
     *                                  b4     b3     b2     b1     b0
     * ------------------------------------------------------------------------------
     *                               | a0b4   a0b3   a0b2   a0b1   a0b0
     *                          a1b4 | a1b3   a1b2   a1b1   a1b0
     *                   a2b4   a2b3 | a2b2   a2b1   a2b0
     *            a3b4   a3b3   a3b2 | a3b1   a3b0
     *     a4b4   a4b3   a4b2   a4b1 | a4b0
     *                               |
     *                               |
     *      2^255 cutoff_____________|
     *
     * All the partial products to the left of the 2^255 cutoff point correspond to
     * the b term in the identity above and so are shifted down by 2^255 after
     * multiplying by 19.
     */
    r0 = a0*b0 + 19 * ( a1*b4 + a2*b3 + a3*b2 + a4*b1 );

    /* Store overflow in r1 and mask it out in r0 */
    r1 = r0 >> 51;  r0 &= LOW_51_BITS_MASK;
    /* Add the current partial products */
    r1 += a0*b1 + a1*b0 + 19 * ( a2*b4 + a3*b3 + a4*b2 );

    /* Rinse and repeat */
    r2 = r1 >> 51;  r1 &= LOW_51_BITS_MASK;
    r2 += a0*b2 + a1*b1 + a2*b0 + 19 * ( a3*b4 + a4*b3 );

    r3 = r2 >> 51;  r2 &= LOW_51_BITS_MASK;
    r3 += a0*b3 + a1*b2 + a2*b1 + a3*b0 + 19 * ( a4*b4 );

    r4 = r3 >> 51;  r3 &= LOW_51_BITS_MASK;
    r4 += a0*b4 + a1*b3 + a2*b2 + a3*b1 + a4*b0;
    /* The result may have exceeded 2^255, do another round just as in
     * fe_weak_reduce() */
    r0 += 19 * (r4 >> 51);
    r4 &= LOW_51_BITS_MASK;

    r1 += r0 >> 51;  r0 &= LOW_51_BITS_MASK;
    r2 += r1 >> 51;  r1 &= LOW_51_BITS_MASK;
    r3 += r2 >> 51;  r2 &= LOW_51_BITS_MASK;
    r4 += r3 >> 51;  r3 &= LOW_51_BITS_MASK;
    r0 += 19 * (r4 >> 51);
    r4 &= LOW_51_BITS_MASK;

    r[0] = r0;
    r[1] = r1;
    r[2] = r2;
    r[3] = r3;
    r[4] = r4;
}

/**
 * @brief Multiply an element by itself
 * @param[out] r Result of the squaring, i.e. the product r = a a
 * @param[in] a Field element to square
 */
static inline void fe_square(fe25519 r, const fe25519 a) {

#if !FE3C_OPTIMIZATION_FAST_SQUARING
    fe_mul(r, a, a);
#else
    u128 r0, r1, r2, r3, r4;

    u128 a0 = a[0];
    u128 a1 = a[1];
    u128 a2 = a[2];
    u128 a3 = a[3];
    u128 a4 = a[4];
    /* Do the naive schoolbook multiplication but allow the compiler to group together
     * like terms. Partial single-precision products in the schoolbook multiplication
     * are of the form aibj for different i and j. When squaring the term aibj is
     * equivalent to ajbi which we can levarage to halve the number of single-precision
     * multiplications. */

    r0 = a0*a0 + 19 * ( a1*a4 + a2*a3 + a3*a2 + a4*a1 );

    /* Store overflow in r1 and mask it out in r0 */
    r1 = r0 >> 51;  r0 &= LOW_51_BITS_MASK;
    /* Add the current partial products */
    r1 += a0*a1 + a1*a0 + 19 * ( a2*a4 + a3*a3 + a4*a2 );

    /* Rinse and repeat */
    r2 = r1 >> 51;  r1 &= LOW_51_BITS_MASK;
    r2 += a0*a2 + a1*a1 + a2*a0 + 19 * ( a3*a4 + a4*a3 );

    r3 = r2 >> 51;  r2 &= LOW_51_BITS_MASK;
    r3 += a0*a3 + a1*a2 + a2*a1 + a3*a0 + 19 * ( a4*a4 );

    r4 = r3 >> 51;  r3 &= LOW_51_BITS_MASK;
    r4 += a0*a4 + a1*a3 + a2*a2 + a3*a1 + a4*a0;
    /* The result may have exceeded 2^255, do another round just as in
     * fe_weak_reduce() */
    r0 += 19 * (r4 >> 51);
    r4 &= LOW_51_BITS_MASK;

    r1 += r0 >> 51;  r0 &= LOW_51_BITS_MASK;
    r2 += r1 >> 51;  r1 &= LOW_51_BITS_MASK;
    r3 += r2 >> 51;  r2 &= LOW_51_BITS_MASK;
    r4 += r3 >> 51;  r3 &= LOW_51_BITS_MASK;
    r0 += 19 * (r4 >> 51);
    r4 &= LOW_51_BITS_MASK;

    r[0] = r0;
    r[1] = r1;
    r[2] = r2;
    r[3] = r3;
    r[4] = r4;
#endif /* FE3C_OPTIMIZATION_FAST_SQUARING */
}

/**
 * @brief Encode a field element according to RFC 8032
 * @param[out] buffer Output buffer for the encoded field element
 * @param[in] a Field element to encode
 */
static inline void fe_encode(u8 * buffer, fe25519 a) {

    /* Canonicalize the element first */
    fe_strong_reduce(a, a);

    /* Store the temporary results of bit operations in separate variables (mapped to separate
     * registers) to allow for greater instruction-level parallelism */

    /* Store the lowest limb + whatever can fit (13 bits) of the second lowest limb */
    u64 t0 = ( a[0] >>  0 ) | ( a[1] << 51 );
    /* 13 bits of a[1] are in t0, store the rest (38 bits) here + whatever can fit
     * (26 bits) of a[2] */
    u64 t1 = ( a[1] >> 13 ) | ( a[2] << 38 );
    /* 26 bits of a[2] are in t1, store the rest (25 bits) here + whatever can fit
     * (39 bits) of a[3] */
    u64 t2 = ( a[2] >> 26 ) | ( a[3] << 25 );
    /* Store the top 51-39=12 bits of a[3] and all of a[4] */
    u64 t3 = ( a[3] >> 39 ) | ( a[4] << 12 );

    /* The field elements get encoded as little-endian byte strings according to RFC 8032 */
    _store_64(&buffer[0 * 8], t0);
    _store_64(&buffer[1 * 8], t1);
    _store_64(&buffer[2 * 8], t2);
    _store_64(&buffer[3 * 8], t3);
}

/**
 * @brief Decode a field element according to RFC 8032
 * @param[out] r Decoded field element
 * @param[out] buffer Encoding of the field element
 * @return 1 if decoding succeeded, 0 otherwise
 */
__attribute__((warn_unused_result))
static inline int fe_decode(fe25519 r, const u8 * buffer) {

    r[0] = ( _load_64(&buffer[ 0]) >> 0 ) & LOW_51_BITS_MASK;
    /* Do not offset by 8 now since we have dropped the top byte
     * and a few more bits from the first word by masking. Offset
     * by 51 bits (6 bytes + 3 bits of shift): */
    r[1] = ( _load_64(&buffer[ 6]) >> 3 ) & LOW_51_BITS_MASK;
    /* Same spiel - offset by another 51 bits */
    r[2] = ( _load_64(&buffer[12]) >> 6 ) & LOW_51_BITS_MASK;
    /* Now the "shift" bits have added up to over a byte, and so
     * we offset by 3*6+1 bytes and the leftover bit */
    r[3] = ( _load_64(&buffer[19]) >> 1 ) & LOW_51_BITS_MASK;
    /* Load the last limb (note that the last bit gets naturally ignored
     * as required by RFC 8032) */
    r[4] = ( _load_64(&buffer[25]) >> 4 ) & LOW_51_BITS_MASK;

    return fe_is_canonical(r);
}

#ifdef __cplusplus
}
#endif

#endif /* __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_ED25519_64_H */
