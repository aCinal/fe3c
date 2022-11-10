#include <scalars/scalars.h>
#include <utils/utils.h>

#if !FE3C_64BIT
    #error "Build system inconsistency detected! scalars_ed25519_64.c in use despite FE3C_64BIT not being set"
#endif /* !FE3C_64BIT */

#define LOW_42_BITS_MASK  0x3ffffffffff

/* Order of the group as per RFC 7748 */
#define GROUP_ORDER_WORD_0  0x5812631a5cf5d3ed
#define GROUP_ORDER_WORD_1  0x14def9dea2f79cd6
#define GROUP_ORDER_WORD_2  0x0000000000000000
#define GROUP_ORDER_WORD_3  0x1000000000000000
/* Redefine the low half of the order, this time in 42-bit limbs instead of 64-bit words*/
#define GROUP_ORDER_LIMB_0  0x31a5cf5d3ed
#define GROUP_ORDER_LIMB_1  0x1e735960498
#define GROUP_ORDER_LIMB_2  0x14def9dea2f

static inline u64 load_64(const u8 src[8]) {

    /* Integer encoding is always little endian according to RFC 8032 */
#if FE3C_LILENDIAN_TARGET
    u64 qword;
    /* Target already little endian - copy the bytes with no shifts */
    (void) memcpy(&qword, src, 8);
#else
    /* Big-endian target or endianness unknown (take the safe route) */
    u64 qword = (u64) src[0];
    qword |= (u64) src[1] << 8;
    qword |= (u64) src[2] << 16;
    qword |= (u64) src[3] << 24;
    qword |= (u64) src[4] << 32;
    qword |= (u64) src[5] << 40;
    qword |= (u64) src[6] << 48;
    qword |= (u64) src[7] << 56;
#endif
    return qword;
}

static inline u64 load_48(const u8 src[8]) {

    /* Integer encoding is always little endian according to RFC 8032 */
#if FE3C_LILENDIAN_TARGET
    u64 qword;
    /* Target already little endian - copy the bytes with no shifts */
    (void) memcpy(&qword, src, 6);
#else
    /* Big-endian target or endianness unknown (take the safe route) */
    u64 qword = (u64) src[0];
    qword |= (u64) src[1] << 8;
    qword |= (u64) src[2] << 16;
    qword |= (u64) src[3] << 24;
    qword |= (u64) src[4] << 32;
    qword |= (u64) src[5] << 40;
#endif
    return qword;
}

static inline u64 load_56(const u8 src[8]) {

    /* Integer encoding is always little endian according to RFC 8032 */
#if FE3C_LILENDIAN_TARGET
    u64 qword;
    /* Target already little endian - copy the bytes with no shifts */
    (void) memcpy(&qword, src, 6);
#else
    /* Big-endian target or endianness unknown (take the safe route) */
    u64 qword = (u64) src[0];
    qword |= (u64) src[1] << 8;
    qword |= (u64) src[2] << 16;
    qword |= (u64) src[3] << 24;
    qword |= (u64) src[4] << 32;
    qword |= (u64) src[5] << 40;
    qword |= (u64) src[6] << 48;
#endif
    return qword;
}

static void ed25519_scalar_reduce(u8 * s) {

    /* Ed25519 reduction inputs can be 64 bytes long (outputs from SHA-512). Use intermediate
     * 42-bit reduced-radix representation during reduction - we shall use 12 limbs with the
     * highest limb holding 50 bits instead of 42. Note that we use SIGNED words as we rely
     * on sign extension in the algorithm. Recall that masking a signed variable with
     * LOW_42_BITS_MASK == ( (1 << 42) - 1 ) is equivalent to reduction modulo 2^42 (same as
     * for unsigned integers). Similarly shifting by 42 bits to the right is equivalent to
     * dividing by 2^42 while preserving the sign. */
    i128 t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11;

    /* Parse the scalar into limbs */
    t0  = ( load_48(&s[ 0]) >> 0 ) & LOW_42_BITS_MASK;
    /* Five full bytes were written into t0 as well as two
     * additional bits. Skip the five bytes by offsetting
     * into the array and skip the two bits by shifting. */
    t1  = ( load_48(&s[ 5]) >> 2 ) & LOW_42_BITS_MASK;
    t2  = ( load_48(&s[10]) >> 4 ) & LOW_42_BITS_MASK;
    t3  = ( load_48(&s[15]) >> 6 ) & LOW_42_BITS_MASK;
    /* At this point the 2-bit shifts have accumulated to
     * a full byte. Only do the offset but by six instead
     * of five bytes. */
    t4  = ( load_48(&s[21]) >> 0 ) & LOW_42_BITS_MASK;
    t5  = ( load_48(&s[26]) >> 2 ) & LOW_42_BITS_MASK;
    t6  = ( load_48(&s[31]) >> 4 ) & LOW_42_BITS_MASK;
    t7  = ( load_48(&s[36]) >> 6 ) & LOW_42_BITS_MASK;
    t8  = ( load_48(&s[42]) >> 0 ) & LOW_42_BITS_MASK;
    t9  = ( load_48(&s[47]) >> 2 ) & LOW_42_BITS_MASK;
    t10 = ( load_48(&s[52]) >> 4 ) & LOW_42_BITS_MASK;
    /* Load the last 50 bits into t11 */
    t11 = ( load_56(&s[57]) >> 6 );

    /* Note that limbs t0-t5 contain the value of s modulo 2^252. Let c denote the low
     * two 64-bit words of the group order L, namely let c = L - 2^252. In 42-bit limbs
     * c can be represented as (c2, c1, c0). Take the limbs t6-t11 and multiply them by
     * c and subtract the product from the low part (t0-t5). We rely here on the identity
     *
     *            s := x + 2^252 y = x + (2^252 + c) y - c y = x - c y (mod L)
     */

    /* Naive schoolbok multiplication of (c2, c1, c0) and (t11, t10, t9, t8, t7, t6) split
     * into two parts by a partial reduction in the middle. */
    t5 -= t11 * GROUP_ORDER_LIMB_0;
    t6 -= t11 * GROUP_ORDER_LIMB_1;
    t7 -= t11 * GROUP_ORDER_LIMB_2;

    t4 -= t10 * GROUP_ORDER_LIMB_0;
    t5 -= t10 * GROUP_ORDER_LIMB_1;
    t6 -= t10 * GROUP_ORDER_LIMB_2;

    t3 -= t9 * GROUP_ORDER_LIMB_0;
    t4 -= t9 * GROUP_ORDER_LIMB_1;
    t5 -= t9 * GROUP_ORDER_LIMB_2;

    /* Do a partial reduction at this point to support all possible inputs */
    t4 += t3 >> 42;  t3 &= LOW_42_BITS_MASK;
    t5 += t4 >> 42;  t4 &= LOW_42_BITS_MASK;
    t6 += t5 >> 42;  t5 &= LOW_42_BITS_MASK;
    t7 += t6 >> 42;  t6 &= LOW_42_BITS_MASK;
    t8 += t7 >> 42;  t7 &= LOW_42_BITS_MASK;

    t2 -= t8 * GROUP_ORDER_LIMB_0;
    t3 -= t8 * GROUP_ORDER_LIMB_1;
    t4 -= t8 * GROUP_ORDER_LIMB_2;

    t1 -= t7 * GROUP_ORDER_LIMB_0;
    t2 -= t7 * GROUP_ORDER_LIMB_1;
    t3 -= t7 * GROUP_ORDER_LIMB_2;

    t0 -= t6 * GROUP_ORDER_LIMB_0;
    t1 -= t6 * GROUP_ORDER_LIMB_1;
    t2 -= t6 * GROUP_ORDER_LIMB_2;
    /* Clear t6 - it has now been distributed among the lower limbs and shall now
     * be used to store any overflow from t5 */
    t6 = 0;
    t1 += t0 >> 42;  t0 &= LOW_42_BITS_MASK;
    t2 += t1 >> 42;  t1 &= LOW_42_BITS_MASK;
    t3 += t2 >> 42;  t2 &= LOW_42_BITS_MASK;
    t4 += t3 >> 42;  t3 &= LOW_42_BITS_MASK;
    t5 += t4 >> 42;  t4 &= LOW_42_BITS_MASK;
    t6 += t5 >> 42;  t5 &= LOW_42_BITS_MASK;

    /* Note that some overflow may have accumulated in t6 again. Reduce it the
     * same way again. */
    t0 -= t6 * GROUP_ORDER_LIMB_0;
    t1 -= t6 * GROUP_ORDER_LIMB_1;
    t2 -= t6 * GROUP_ORDER_LIMB_2;

    t1 += t0 >> 42;  t0 &= LOW_42_BITS_MASK;
    t2 += t1 >> 42;  t1 &= LOW_42_BITS_MASK;
    t3 += t2 >> 42;  t2 &= LOW_42_BITS_MASK;
    t4 += t3 >> 42;  t3 &= LOW_42_BITS_MASK;
    t5 += t4 >> 42;  t4 &= LOW_42_BITS_MASK;

    s[0]  = ( t0 >>  0 );
    s[1]  = ( t0 >>  8 );
    s[2]  = ( t0 >> 16 );
    s[3]  = ( t0 >> 24 );
    s[4]  = ( t0 >> 32 );
    /* In the first limb we only have 2 bits for byte number 5,
     * the high 6 bits we must take from the next limb */
    s[5]  = ( t0 >> 40 ) | ( t1 << 2 );
    /* Low 6 bits form t1 we have already encoded into s[5] */
    s[6]  = ( t1 >> 6 );
    s[7]  = ( t1 >> 14 );
    s[8]  = ( t1 >> 22 );
    s[9]  = ( t1 >> 30 );
    /* Only 4 bits do we have for byte 10 (top 4 bits of limb t1) */
    s[10] = ( t1 >> 38 ) | ( t2 << 4 );
    s[11] = ( t2 >> 4 );
    s[12] = ( t2 >> 12 );
    s[13] = ( t2 >> 20 );
    s[14] = ( t2 >> 28 );
    /* Put top 6 bits from t2 into s[15] and low 2 bits from the
     * next limb */
    s[15] = ( t2 >> 36 ) | ( t3 << 6);
    s[16] = ( t3 >> 2 );
    s[17] = ( t3 >> 10 );
    s[18] = ( t3 >> 18 );
    s[19] = ( t3 >> 26 );
    s[20] = ( t3 >> 34 );
    s[21] = ( t4 >> 0 );
    s[22] = ( t4 >> 8 );
    s[23] = ( t4 >> 16 );
    s[24] = ( t4 >> 24 );
    s[25] = ( t4 >> 32 );
    s[26] = ( t4 >> 40 ) | ( t5 << 2 );
    s[27] = ( t5 >> 6 );
    s[28] = ( t5 >> 14 );
    s[29] = ( t5 >> 22 );
    s[30] = ( t5 >> 30 );
    /* Recall that limb t5 contains 50 bits */
    s[31] = ( t5 >> 38 );
}

static int ed25519_scalar_is_canonical(const u8 s[32]) {

    /* Load the scalar into four 64-bit words */
    u128 s0 = load_64(&s[0 * 8]);
    u128 s1 = load_64(&s[1 * 8]);
    u128 s2 = load_64(&s[2 * 8]);
    u128 s3 = load_64(&s[3 * 8]);

    /* Let L[0]-L[3] denote the four words of L and let
     * S[0]-S[3] denote the four words of the scalar s
     * (both in little-endian order). If the following
     * subtraction underflows, then S[0] < L[0] */
    s0 -= GROUP_ORDER_WORD_0;
    /* Include the underflow in the next subtraction */
    s1 -= GROUP_ORDER_WORD_1 + (s0 >> 127);
    s2 -= GROUP_ORDER_WORD_2 + (s1 >> 127);
    s3 -= GROUP_ORDER_WORD_3 + (s2 >> 127);

    /* If the "borrow" made it all the way to the top,
     * then s is smaller than L and so is canonical. */
    return (s3 >> 127);
}

static void ed25519_scalars_muladd(u8 * r, const u8 * a, const u8 * b, const u8 * c) {

    /* Ed25519 multiplication+addition inputs must be 32 bytes long (outputs from SHA-512
     * subjected to reduction or the response scalar which is part of the signature). Use
     * intermediate 42-bit reduced-radix representation. */

    i128 a0, a1, a2, a3, a4, a5;
    i128 b0, b1, b2, b3, b4, b5;
    i128 c0, c1, c2, c3, c4, c5;
    i128 t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11;

    /* Parse the scalar a into limbs */
    a0  = ( load_48(&a[ 0]) >> 0 ) & LOW_42_BITS_MASK;
    /* Five full bytes were written into a0 as well as two
     * additional bits. Skip the five bytes by offsetting
     * into the array and skip the two bits by shifting. */
    a1  = ( load_48(&a[ 5]) >> 2 ) & LOW_42_BITS_MASK;
    a2  = ( load_48(&a[10]) >> 4 ) & LOW_42_BITS_MASK;
    a3  = ( load_48(&a[15]) >> 6 ) & LOW_42_BITS_MASK;
    /* At this point the 2-bit shifts have accumulated to
     * a full byte. Only do the offset but by six instead
     * of five bytes. */
    a4  = ( load_48(&a[21]) >> 0 ) & LOW_42_BITS_MASK;
    a5  = ( load_48(&a[26]) >> 2 );

    /* Repeat the same steps for b and c */
    b0  = ( load_48(&b[ 0]) >> 0 ) & LOW_42_BITS_MASK;
    b1  = ( load_48(&b[ 5]) >> 2 ) & LOW_42_BITS_MASK;
    b2  = ( load_48(&b[10]) >> 4 ) & LOW_42_BITS_MASK;
    b3  = ( load_48(&b[15]) >> 6 ) & LOW_42_BITS_MASK;
    b4  = ( load_48(&b[21]) >> 0 ) & LOW_42_BITS_MASK;
    b5  = ( load_48(&b[26]) >> 2 );

    c0  = ( load_48(&c[ 0]) >> 0 ) & LOW_42_BITS_MASK;
    c1  = ( load_48(&c[ 5]) >> 2 ) & LOW_42_BITS_MASK;
    c2  = ( load_48(&c[10]) >> 4 ) & LOW_42_BITS_MASK;
    c3  = ( load_48(&c[15]) >> 6 ) & LOW_42_BITS_MASK;
    c4  = ( load_48(&c[21]) >> 0 ) & LOW_42_BITS_MASK;
    c5  = ( load_48(&c[26]) >> 2 );

    /* Do the naive schoolbook multiplication - note that a*b takes 11 limbs (11 columns
     * in the multplication algorithm). Offset the first 6 limbs by the limbs of c
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

    t0  = c0 + a0*b0;
    t1  = c1 + a0*b1 + a1*b0;
    t2  = c2 + a0*b2 + a1*b1 + a2*b0;
    t3  = c3 + a0*b3 + a1*b2 + a2*b1 + a3*b0;
    t4  = c4 + a0*b4 + a1*b3 + a2*b2 + a3*b1 + a4*b0;
    t5  = c5 + a0*b5 + a1*b4 + a2*b3 + a3*b2 + a4*b1 + a5*b0;
    t6  =              a1*b5 + a2*b4 + a3*b3 + a4*b2 + a5*b1;
    t7  =                      a2*b5 + a3*b4 + a4*b3 + a5*b2;
    t8  =                              a3*b5 + a4*b4 + a5*b3;
    t9  =                                      a4*b5 + a5*b4;
    t10 =                                              a5*b5;
    /* Leave t11 for any overflows */
    t11 = 0;

    t1  +=  t0 >> 42;  t0  &= LOW_42_BITS_MASK;
    t2  +=  t1 >> 42;  t1  &= LOW_42_BITS_MASK;
    t3  +=  t2 >> 42;  t2  &= LOW_42_BITS_MASK;
    t4  +=  t3 >> 42;  t3  &= LOW_42_BITS_MASK;
    t5  +=  t4 >> 42;  t4  &= LOW_42_BITS_MASK;
    t6  +=  t5 >> 42;  t5  &= LOW_42_BITS_MASK;
    t7  +=  t6 >> 42;  t6  &= LOW_42_BITS_MASK;
    t8  +=  t7 >> 42;  t7  &= LOW_42_BITS_MASK;
    t9  +=  t8 >> 42;  t8  &= LOW_42_BITS_MASK;
    t10 +=  t9 >> 42;  t9  &= LOW_42_BITS_MASK;
    t11 += t10 >> 42;  t10 &= LOW_42_BITS_MASK;

    /* Bring down the limbs that exceed 2^252 according to the identity:
     *
     *   x + 2^252 y = x + (2^252 + c) y - c y = x - c y (mod L)
     *
     * where c = L - 2^252. See ed25519_scalar_reduce for a more detailed
     * description.
     */
    t5 -= t11 * GROUP_ORDER_LIMB_0;
    t6 -= t11 * GROUP_ORDER_LIMB_1;
    t7 -= t11 * GROUP_ORDER_LIMB_2;

    t4 -= t10 * GROUP_ORDER_LIMB_0;
    t5 -= t10 * GROUP_ORDER_LIMB_1;
    t6 -= t10 * GROUP_ORDER_LIMB_2;

    t3 -= t9 * GROUP_ORDER_LIMB_0;
    t4 -= t9 * GROUP_ORDER_LIMB_1;
    t5 -= t9 * GROUP_ORDER_LIMB_2;

    /* Do a partial reduction of the limbs we have been working with above
     * to ensure no overflows/underflows occur */
    t4 += t3 >> 42;  t3 &= LOW_42_BITS_MASK;
    t5 += t4 >> 42;  t4 &= LOW_42_BITS_MASK;
    t6 += t5 >> 42;  t5 &= LOW_42_BITS_MASK;
    t7 += t6 >> 42;  t6 &= LOW_42_BITS_MASK;
    t8 += t7 >> 42;  t7 &= LOW_42_BITS_MASK;

    t2 -= t8 * GROUP_ORDER_LIMB_0;
    t3 -= t8 * GROUP_ORDER_LIMB_1;
    t4 -= t8 * GROUP_ORDER_LIMB_2;

    t1 -= t7 * GROUP_ORDER_LIMB_0;
    t2 -= t7 * GROUP_ORDER_LIMB_1;
    t3 -= t7 * GROUP_ORDER_LIMB_2;

    t0 -= t6 * GROUP_ORDER_LIMB_0;
    t1 -= t6 * GROUP_ORDER_LIMB_1;
    t2 -= t6 * GROUP_ORDER_LIMB_2;
    /* t6 is fully distributed among lower limbs and we will need it
     * for handling overflow in t5. Clear it here to prepare it for
     * this purpose. */
    t6 = 0;

    t1 += t0 >> 42;  t0 &= LOW_42_BITS_MASK;
    t2 += t1 >> 42;  t1 &= LOW_42_BITS_MASK;
    t3 += t2 >> 42;  t2 &= LOW_42_BITS_MASK;
    t4 += t3 >> 42;  t3 &= LOW_42_BITS_MASK;
    t5 += t4 >> 42;  t4 &= LOW_42_BITS_MASK;
    t6 += t5 >> 42;  t5 &= LOW_42_BITS_MASK;

    /* Something may have bubbled up into t6 (overflow in t5) */
    t0 -= t6 * GROUP_ORDER_LIMB_0;
    t1 -= t6 * GROUP_ORDER_LIMB_1;
    t2 -= t6 * GROUP_ORDER_LIMB_2;

    /* Do the final normalization (normalize limbs to 42 bits) */
    t1 += t0 >> 42;  t0 &= LOW_42_BITS_MASK;
    t2 += t1 >> 42;  t1 &= LOW_42_BITS_MASK;
    t3 += t2 >> 42;  t2 &= LOW_42_BITS_MASK;
    t4 += t3 >> 42;  t3 &= LOW_42_BITS_MASK;
    t5 += t4 >> 42;  t4 &= LOW_42_BITS_MASK;

    /* Place the limbs in the output buffer, see ed25519_scalar_reduce for a
     * more detailed explanation of this */
    r[0]  = ( t0 >>  0 );
    r[1]  = ( t0 >>  8 );
    r[2]  = ( t0 >> 16 );
    r[3]  = ( t0 >> 24 );
    r[4]  = ( t0 >> 32 );
    r[5]  = ( t0 >> 40 ) | ( t1 << 2 );
    r[6]  = ( t1 >> 6 );
    r[7]  = ( t1 >> 14 );
    r[8]  = ( t1 >> 22 );
    r[9]  = ( t1 >> 30 );
    r[10] = ( t1 >> 38 ) | ( t2 << 4 );
    r[11] = ( t2 >> 4 );
    r[12] = ( t2 >> 12 );
    r[13] = ( t2 >> 20 );
    r[14] = ( t2 >> 28 );
    r[15] = ( t2 >> 36 ) | ( t3 << 6);
    r[16] = ( t3 >> 2 );
    r[17] = ( t3 >> 10 );
    r[18] = ( t3 >> 18 );
    r[19] = ( t3 >> 26 );
    r[20] = ( t3 >> 34 );
    r[21] = ( t4 >> 0 );
    r[22] = ( t4 >> 8 );
    r[23] = ( t4 >> 16 );
    r[24] = ( t4 >> 24 );
    r[25] = ( t4 >> 32 );
    r[26] = ( t4 >> 40 ) | ( t5 << 2 );
    r[27] = ( t5 >> 6 );
    r[28] = ( t5 >> 14 );
    r[29] = ( t5 >> 22 );
    r[30] = ( t5 >> 30 );
    r[31] = ( t5 >> 38 );
}

scalar_ops ed25519_scalar_ops = {
    .reduce = ed25519_scalar_reduce,
    .is_canonical = ed25519_scalar_is_canonical,
    .muladd = ed25519_scalars_muladd
};
