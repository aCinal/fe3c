#include <scalars/scalars.h>
#include <utils/utils.h>

#if !FE3C_64BIT
    #error "Build system inconsistency detected! scalars_ed25519_64.c in use despite FE3C_64BIT not being set"
#endif /* !FE3C_64BIT */

#define LOW_42_BITS_MASK    0x3ffffffffff

/* Order of the group as per RFC 7748 */
#define GROUP_ORDER_WORD_0  0x5812631a5cf5d3ed
#define GROUP_ORDER_WORD_1  0x14def9dea2f79cd6
#define GROUP_ORDER_WORD_2  0x0000000000000000
#define GROUP_ORDER_WORD_3  0x1000000000000000
/* Redefine the low half of the order, this time in 42-bit limbs instead of 64-bit words */
#define GROUP_ORDER_LIMB_0  0x31a5cf5d3ed
#define GROUP_ORDER_LIMB_1  0x1e735960498
#define GROUP_ORDER_LIMB_2  0x14def9dea2f

static inline u64 load_64(const u8 src[8]) {

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

static inline u64 load_48(const u8 src[6]) {

    /* Integer encoding is always little endian according to RFC 8032 */
    u64 dst;
#if FE3C_LILENDIAN_TARGET
    /* Target already little endian - copy the bytes with no shifts */
    dst = 0;
    (void) memcpy(&dst, src, 6);
#else
    /* Big-endian target or endianness unknown (take the safe route) */
    dst  = (u64) src[0];
    dst |= (u64) src[1] << 8;
    dst |= (u64) src[2] << 16;
    dst |= (u64) src[3] << 24;
    dst |= (u64) src[4] << 32;
    dst |= (u64) src[5] << 40;
#endif
    return dst;
}

static inline u64 load_56(const u8 src[7]) {

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

static int ed25519_scalar_is_canonical(const u8 * s) {

    /* Load the scalar into four 64-bit words (extended to 128 bits) */
    u128 s0 = load_64(&s[0 * 8]);
    u128 s1 = load_64(&s[1 * 8]);
    u128 s2 = load_64(&s[2 * 8]);
    u128 s3 = load_64(&s[3 * 8]);

    /* Let L[0]-L[3] denote the four words of L and let
     * S[0]-S[3] denote the four words of the scalar s
     * (both in little-endian order). If the following
     * subtraction underflows, then S < L. */
    s0 -= GROUP_ORDER_WORD_0;
    /* Include the underflow in the next subtraction */
    s1 -= GROUP_ORDER_WORD_1 + (s0 >> 127);
    s2 -= GROUP_ORDER_WORD_2 + (s1 >> 127);
    s3 -= GROUP_ORDER_WORD_3 + (s2 >> 127);

    /* If the "borrow" made it all the way to the top,
     * then s is smaller than L and so is canonical. */
    return (s3 >> 127);
}

static void ed25519_scalar_reduce(u8 * s) {

    /* Ed25519 reduction inputs can be 64 bytes long (outputs from SHA-512). Use intermediate
     * 42-bit reduced-radix representation during reduction - we shall use 12 limbs with the
     * highest limb holding 50 bits instead of 42. Note that we use SIGNED words as we rely
     * on sign extension in the algorithm. Recall that masking a signed variable with
     * LOW_42_BITS_MASK == ( (1 << 42) - 1 ) is equivalent to reduction modulo 2^42 (same as
     * for unsigned integers). Similarly shifting by 42 bits to the right is equivalent to
     * dividing by 2^42 while preserving the sign. */

    i128 t[12];

    /* Parse the scalar into limbs */
    t[ 0]  = ( load_48(&s[ 0]) >> 0 ) & LOW_42_BITS_MASK;
    /* Five full bytes were written into t0 as well as two
     * additional bits. Skip the five bytes by offsetting
     * into the array and skip the two bits by shifting. */
    t[ 1]  = ( load_48(&s[ 5]) >> 2 ) & LOW_42_BITS_MASK;
    t[ 2]  = ( load_48(&s[10]) >> 4 ) & LOW_42_BITS_MASK;
    t[ 3]  = ( load_48(&s[15]) >> 6 ) & LOW_42_BITS_MASK;
    /* At this point the 2-bit shifts have accumulated to
     * a full byte. Only do the offset but by six instead
     * of five bytes. */
    t[ 4] = ( load_48(&s[21]) >> 0 ) & LOW_42_BITS_MASK;
    t[ 5] = ( load_48(&s[26]) >> 2 ) & LOW_42_BITS_MASK;
    t[ 6] = ( load_48(&s[31]) >> 4 ) & LOW_42_BITS_MASK;
    t[ 7] = ( load_48(&s[36]) >> 6 ) & LOW_42_BITS_MASK;
    t[ 8] = ( load_48(&s[42]) >> 0 ) & LOW_42_BITS_MASK;
    t[ 9] = ( load_48(&s[47]) >> 2 ) & LOW_42_BITS_MASK;
    t[10] = ( load_48(&s[52]) >> 4 ) & LOW_42_BITS_MASK;
    /* Load the last 50 bits into t11 */
    t[11] = ( load_56(&s[57]) >> 6 );

    /* Note that limbs t0-t5 contain the value of s modulo 2^252. Let c denote the low
     * two 64-bit words of the group order L, namely let c = L - 2^252. In 42-bit limbs
     * c can be represented as (c2, c1, c0). Take the limbs t6-t11, multiply them by c,
     * and subtract the product from the low part (t0-t5). We rely here on the identity:
     *
     *           s := x + 2^252 y = x + (2^252 + c) y - c y = x - c y (mod L)
     */

    /* Naive schoolbok multiplication of (c2, c1, c0) and (t11, t10, t9, t8, t7, t6) split
     * into two parts by a partial reduction in the middle. */
    t[5] -= t[11] * GROUP_ORDER_LIMB_0;
    t[6] -= t[11] * GROUP_ORDER_LIMB_1;
    t[7] -= t[11] * GROUP_ORDER_LIMB_2;

    t[4] -= t[10] * GROUP_ORDER_LIMB_0;
    t[5] -= t[10] * GROUP_ORDER_LIMB_1;
    t[6] -= t[10] * GROUP_ORDER_LIMB_2;

    t[3] -= t[ 9] * GROUP_ORDER_LIMB_0;
    t[4] -= t[ 9] * GROUP_ORDER_LIMB_1;
    t[5] -= t[ 9] * GROUP_ORDER_LIMB_2;

    /* Do a partial reduction at this point to support all possible inputs */
    t[4] += t[3] >> 42;  t[3] &= LOW_42_BITS_MASK;
    t[5] += t[4] >> 42;  t[4] &= LOW_42_BITS_MASK;
    t[6] += t[5] >> 42;  t[5] &= LOW_42_BITS_MASK;
    t[7] += t[6] >> 42;  t[6] &= LOW_42_BITS_MASK;
    t[8] += t[7] >> 42;  t[7] &= LOW_42_BITS_MASK;

    t[2] -= t[8] * GROUP_ORDER_LIMB_0;
    t[3] -= t[8] * GROUP_ORDER_LIMB_1;
    t[4] -= t[8] * GROUP_ORDER_LIMB_2;

    t[1] -= t[7] * GROUP_ORDER_LIMB_0;
    t[2] -= t[7] * GROUP_ORDER_LIMB_1;
    t[3] -= t[7] * GROUP_ORDER_LIMB_2;

    t[0] -= t[6] * GROUP_ORDER_LIMB_0;
    t[1] -= t[6] * GROUP_ORDER_LIMB_1;
    t[2] -= t[6] * GROUP_ORDER_LIMB_2;

    t[1] += t[0] >> 42;  t[0] &= LOW_42_BITS_MASK;
    t[2] += t[1] >> 42;  t[1] &= LOW_42_BITS_MASK;
    t[3] += t[2] >> 42;  t[2] &= LOW_42_BITS_MASK;
    t[4] += t[3] >> 42;  t[3] &= LOW_42_BITS_MASK;
    t[5] += t[4] >> 42;  t[4] &= LOW_42_BITS_MASK;
    /* Use t6 to store the overflow of t5 */
    t[6]  = t[5] >> 42;  t[5] &= LOW_42_BITS_MASK;

    /* Note that some overflow may have accumulated in t6 again. Reduce it the
     * same way again. */
    t[0] -= t[6] * GROUP_ORDER_LIMB_0;
    t[1] -= t[6] * GROUP_ORDER_LIMB_1;
    t[2] -= t[6] * GROUP_ORDER_LIMB_2;

    t[1] += t[0] >> 42;  t[0] &= LOW_42_BITS_MASK;
    t[2] += t[1] >> 42;  t[1] &= LOW_42_BITS_MASK;
    t[3] += t[2] >> 42;  t[2] &= LOW_42_BITS_MASK;
    t[4] += t[3] >> 42;  t[3] &= LOW_42_BITS_MASK;
    t[5] += t[4] >> 42;  t[4] &= LOW_42_BITS_MASK;

    s[ 0] = ( t[0] >>  0 );
    s[ 1] = ( t[0] >>  8 );
    s[ 2] = ( t[0] >> 16 );
    s[ 3] = ( t[0] >> 24 );
    s[ 4] = ( t[0] >> 32 );
    /* In the first limb we only have 2 bits left for byte number 5,
     * the high 6 bits we must take from the next limb */
    s[ 5] = ( t[0] >> 40 ) | ( t[1] << 2 );
    /* Low 6 bits from t1 we have already encoded into s[5] */
    s[ 6] = ( t[1] >>  6 );
    s[ 7] = ( t[1] >> 14 );
    s[ 8] = ( t[1] >> 22 );
    s[ 9] = ( t[1] >> 30 );
    /* Only 4 bits do we have for byte 10 in limb t1, the rest
     * must come from t2 */
    s[10] = ( t[1] >> 38 ) | ( t[2] << 4 );
    s[11] = ( t[2] >>  4 );
    s[12] = ( t[2] >> 12 );
    s[13] = ( t[2] >> 20 );
    s[14] = ( t[2] >> 28 );
    /* Put top 6 bits from t2 into s[15] and low 2 bits from the
     * next limb */
    s[15] = ( t[2] >> 36 ) | ( t[3] << 6);
    s[16] = ( t[3] >>  2 );
    s[17] = ( t[3] >> 10 );
    s[18] = ( t[3] >> 18 );
    s[19] = ( t[3] >> 26 );
    s[20] = ( t[3] >> 34 );
    s[21] = ( t[4] >>  0 );
    s[22] = ( t[4] >>  8 );
    s[23] = ( t[4] >> 16 );
    s[24] = ( t[4] >> 24 );
    s[25] = ( t[4] >> 32 );
    s[26] = ( t[4] >> 40 ) | ( t[5] << 2 );
    s[27] = ( t[5] >>  6 );
    s[28] = ( t[5] >> 14 );
    s[29] = ( t[5] >> 22 );
    s[30] = ( t[5] >> 30 );
    s[31] = ( t[5] >> 38 );

    purge_secrets(t, sizeof(t));
}

static void ed25519_scalars_muladd(u8 * r, const u8 * aa, const u8 * bb, const u8 * cc) {

    /* Ed25519 multiplication+addition inputs must be 32 bytes long (outputs from SHA-512
     * subjected to reduction or the response scalar which is part of the signature). Use
     * intermediate 42-bit reduced-radix representation. */

    i128 a[6];
    i128 b[6];
    i128 c[6];
    i128 t[12];

    /* Parse the scalar a into limbs */
    a[0]  = ( load_48(&aa[ 0]) >> 0 ) & LOW_42_BITS_MASK;
    /* Five full bytes were written into a0 as well as two
     * additional bits. Skip the five bytes by offsetting
     * into the array and skip the two bits by shifting. */
    a[1]  = ( load_48(&aa[ 5]) >> 2 ) & LOW_42_BITS_MASK;
    a[2]  = ( load_48(&aa[10]) >> 4 ) & LOW_42_BITS_MASK;
    a[3]  = ( load_48(&aa[15]) >> 6 ) & LOW_42_BITS_MASK;
    /* At this point the 2-bit shifts have accumulated to
     * a full byte. Only do the offset but by six instead
     * of five bytes. */
    a[4]  = ( load_48(&aa[21]) >> 0 ) & LOW_42_BITS_MASK;
    a[5]  = ( load_48(&aa[26]) >> 2 );

    /* Repeat the same steps for b and c */
    b[0]  = ( load_48(&bb[ 0]) >> 0 ) & LOW_42_BITS_MASK;
    b[1]  = ( load_48(&bb[ 5]) >> 2 ) & LOW_42_BITS_MASK;
    b[2]  = ( load_48(&bb[10]) >> 4 ) & LOW_42_BITS_MASK;
    b[3]  = ( load_48(&bb[15]) >> 6 ) & LOW_42_BITS_MASK;
    b[4]  = ( load_48(&bb[21]) >> 0 ) & LOW_42_BITS_MASK;
    b[5]  = ( load_48(&bb[26]) >> 2 );

    c[0]  = ( load_48(&cc[ 0]) >> 0 ) & LOW_42_BITS_MASK;
    c[1]  = ( load_48(&cc[ 5]) >> 2 ) & LOW_42_BITS_MASK;
    c[2]  = ( load_48(&cc[10]) >> 4 ) & LOW_42_BITS_MASK;
    c[3]  = ( load_48(&cc[15]) >> 6 ) & LOW_42_BITS_MASK;
    c[4]  = ( load_48(&cc[21]) >> 0 ) & LOW_42_BITS_MASK;
    c[5]  = ( load_48(&cc[26]) >> 2 );

    /* Do the naive schoolbook multiplication - note that a*b takes 11 (12 with carry) limbs
     * (columns in the multplication algorithm). Offset the first 6 limbs by the limbs of c
     *
     *                                         a5     a4     a3     a2     a1     a0
     *                                         b5     b4     b3     b2     b1     b0
     * ----------------------------------------------------------------------------------
     *                                        a0b5   a0b4   a0b3   a0b2   a0b1   a0b0
     *                                 a1b5   a1b4   a1b3   a1b2   a1b1   a1b0
     *                          a2b5   a2b4   a2b3   a2b2   a2b1   a2b0
     *                   a3b5   a3b4   a3b3   a3b2   a3b1   a3b0
     *            a4b5   a4b4   a4b3   a4b2   a4b1   a4b0
     *     a5b5   a5b4   a5b3   a5b2   a5b1   a5b0
     *                                         c5     c4     c3     c2     c1     c0
     */

    t[ 0] = c[0] + a[0]*b[0];
    t[ 1] = c[1] + a[0]*b[1] + a[1]*b[0];
    t[ 2] = c[2] + a[0]*b[2] + a[1]*b[1] + a[2]*b[0];
    t[ 3] = c[3] + a[0]*b[3] + a[1]*b[2] + a[2]*b[1] + a[3]*b[0];
    t[ 4] = c[4] + a[0]*b[4] + a[1]*b[3] + a[2]*b[2] + a[3]*b[1] + a[4]*b[0];
    t[ 5] = c[5] + a[0]*b[5] + a[1]*b[4] + a[2]*b[3] + a[3]*b[2] + a[4]*b[1] + a[5]*b[0];
    t[ 6] =                    a[1]*b[5] + a[2]*b[4] + a[3]*b[3] + a[4]*b[2] + a[5]*b[1];
    t[ 7] =                                a[2]*b[5] + a[3]*b[4] + a[4]*b[3] + a[5]*b[2];
    t[ 8] =                                            a[3]*b[5] + a[4]*b[4] + a[5]*b[3];
    t[ 9] =                                                        a[4]*b[5] + a[5]*b[4];
    t[10] =                                                                    a[5]*b[5];

    t[ 1] += t[ 0] >> 42;  t[ 0] &= LOW_42_BITS_MASK;
    t[ 2] += t[ 1] >> 42;  t[ 1] &= LOW_42_BITS_MASK;
    t[ 3] += t[ 2] >> 42;  t[ 2] &= LOW_42_BITS_MASK;
    t[ 4] += t[ 3] >> 42;  t[ 3] &= LOW_42_BITS_MASK;
    t[ 5] += t[ 4] >> 42;  t[ 4] &= LOW_42_BITS_MASK;
    t[ 6] += t[ 5] >> 42;  t[ 5] &= LOW_42_BITS_MASK;
    t[ 7] += t[ 6] >> 42;  t[ 6] &= LOW_42_BITS_MASK;
    t[ 8] += t[ 7] >> 42;  t[ 7] &= LOW_42_BITS_MASK;
    t[ 9] += t[ 8] >> 42;  t[ 8] &= LOW_42_BITS_MASK;
    t[10] += t[ 9] >> 42;  t[ 9] &= LOW_42_BITS_MASK;
    /* Use t11 to store the overflow of t10 */
    t[11]  = t[10] >> 42;  t[10] &= LOW_42_BITS_MASK;

    /* Bring down the limbs that exceed 2^252 according to the identity:
     *
     *   x + 2^252 y = x + (2^252 + u) y - u y = x - u y (mod L)
     *
     * where u = L - 2^252. See ed25519_scalar_reduce for a more detailed
     * description.
     */
    t[5] -= t[11] * GROUP_ORDER_LIMB_0;
    t[6] -= t[11] * GROUP_ORDER_LIMB_1;
    t[7] -= t[11] * GROUP_ORDER_LIMB_2;

    t[4] -= t[10] * GROUP_ORDER_LIMB_0;
    t[5] -= t[10] * GROUP_ORDER_LIMB_1;
    t[6] -= t[10] * GROUP_ORDER_LIMB_2;

    t[3] -= t[ 9] * GROUP_ORDER_LIMB_0;
    t[4] -= t[ 9] * GROUP_ORDER_LIMB_1;
    t[5] -= t[ 9] * GROUP_ORDER_LIMB_2;

    /* Do a partial reduction of the limbs we have been working with above
     * to ensure no overflows/underflows occur */
    t[4] += t[3] >> 42;  t[3] &= LOW_42_BITS_MASK;
    t[5] += t[4] >> 42;  t[4] &= LOW_42_BITS_MASK;
    t[6] += t[5] >> 42;  t[5] &= LOW_42_BITS_MASK;
    t[7] += t[6] >> 42;  t[6] &= LOW_42_BITS_MASK;
    t[8] += t[7] >> 42;  t[7] &= LOW_42_BITS_MASK;

    t[2] -= t[8] * GROUP_ORDER_LIMB_0;
    t[3] -= t[8] * GROUP_ORDER_LIMB_1;
    t[4] -= t[8] * GROUP_ORDER_LIMB_2;

    t[1] -= t[7] * GROUP_ORDER_LIMB_0;
    t[2] -= t[7] * GROUP_ORDER_LIMB_1;
    t[3] -= t[7] * GROUP_ORDER_LIMB_2;

    t[0] -= t[6] * GROUP_ORDER_LIMB_0;
    t[1] -= t[6] * GROUP_ORDER_LIMB_1;
    t[2] -= t[6] * GROUP_ORDER_LIMB_2;

    t[1] += t[0] >> 42;  t[0] &= LOW_42_BITS_MASK;
    t[2] += t[1] >> 42;  t[1] &= LOW_42_BITS_MASK;
    t[3] += t[2] >> 42;  t[2] &= LOW_42_BITS_MASK;
    t[4] += t[3] >> 42;  t[3] &= LOW_42_BITS_MASK;
    t[5] += t[4] >> 42;  t[4] &= LOW_42_BITS_MASK;
    /* Use t6 to store the overflow of t5 */
    t[6]  = t[5] >> 42;  t[5] &= LOW_42_BITS_MASK;

    /* Something may have bubbled up into t6 (overflow in t5) */
    t[0] -= t[6] * GROUP_ORDER_LIMB_0;
    t[1] -= t[6] * GROUP_ORDER_LIMB_1;
    t[2] -= t[6] * GROUP_ORDER_LIMB_2;

    /* Do the final normalization (normalize limbs to 42 bits) */
    t[1] += t[0] >> 42;  t[0] &= LOW_42_BITS_MASK;
    t[2] += t[1] >> 42;  t[1] &= LOW_42_BITS_MASK;
    t[3] += t[2] >> 42;  t[2] &= LOW_42_BITS_MASK;
    t[4] += t[3] >> 42;  t[3] &= LOW_42_BITS_MASK;
    t[5] += t[4] >> 42;  t[4] &= LOW_42_BITS_MASK;

    /* Place the limbs in the output buffer, see ed25519_scalar_reduce for a
     * more detailed explanation of this */
    r[0]  = ( t[0] >>  0 );
    r[1]  = ( t[0] >>  8 );
    r[2]  = ( t[0] >> 16 );
    r[3]  = ( t[0] >> 24 );
    r[4]  = ( t[0] >> 32 );
    r[5]  = ( t[0] >> 40 ) | ( t[1] << 2 );
    r[6]  = ( t[1] >>  6 );
    r[7]  = ( t[1] >> 14 );
    r[8]  = ( t[1] >> 22 );
    r[9]  = ( t[1] >> 30 );
    r[10] = ( t[1] >> 38 ) | ( t[2] << 4 );
    r[11] = ( t[2] >>  4 );
    r[12] = ( t[2] >> 12 );
    r[13] = ( t[2] >> 20 );
    r[14] = ( t[2] >> 28 );
    r[15] = ( t[2] >> 36 ) | ( t[3] << 6);
    r[16] = ( t[3] >>  2 );
    r[17] = ( t[3] >> 10 );
    r[18] = ( t[3] >> 18 );
    r[19] = ( t[3] >> 26 );
    r[20] = ( t[3] >> 34 );
    r[21] = ( t[4] >>  0 );
    r[22] = ( t[4] >>  8 );
    r[23] = ( t[4] >> 16 );
    r[24] = ( t[4] >> 24 );
    r[25] = ( t[4] >> 32 );
    r[26] = ( t[4] >> 40 ) | ( t[5] << 2 );
    r[27] = ( t[5] >>  6 );
    r[28] = ( t[5] >> 14 );
    r[29] = ( t[5] >> 22 );
    r[30] = ( t[5] >> 30 );
    r[31] = ( t[5] >> 38 );

    /* In an EdDSA scheme the b and c scalars correspond to the long-term secret key and
     * the ephemeral mask, respectively. Both are secret so we zeroize their intermediate
     * representations. The scalar a corresponds to the public hash value so we do not
     * zeroize it, but the output we do since its unreduced representation (limbs t6-t11)
     * may leak information about the operands b and c. */
    purge_secrets(b, sizeof(b));
    purge_secrets(c, sizeof(c));
    purge_secrets(t, sizeof(t));
}

scalar_ops ed25519_scalar_ops = {
    .is_canonical = ed25519_scalar_is_canonical,
    .reduce = ed25519_scalar_reduce,
    .muladd = ed25519_scalars_muladd
};
