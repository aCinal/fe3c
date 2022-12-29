#include <scalars/scalars.h>
#include <utils/utils.h>

#if !FE3C_32BIT
    #error "Build system inconsistency detected! scalars_ed25519_32.c in use despite FE3C_32BIT not being set"
#endif /* !FE3C_32BIT */

#define LOW_21_BITS_MASK    0x1fffff

/* Order of the group as per RFC 7748 */
#define GROUP_ORDER_WORD_0  0x5cf5d3ed
#define GROUP_ORDER_WORD_1  0x5812631a
#define GROUP_ORDER_WORD_2  0xa2f79cd6
#define GROUP_ORDER_WORD_3  0x14def9de
#define GROUP_ORDER_WORD_4  0x00000000
#define GROUP_ORDER_WORD_5  0x00000000
#define GROUP_ORDER_WORD_6  0x00000000
#define GROUP_ORDER_WORD_7  0x10000000
/* Redefine the low half of the order, this time in 21-bit limbs instead of 32-bit words */
#define GROUP_ORDER_LIMB_0  0x15d3ed
#define GROUP_ORDER_LIMB_1  0x18d2e7
#define GROUP_ORDER_LIMB_2  0x160498
#define GROUP_ORDER_LIMB_3  0xf39ac
#define GROUP_ORDER_LIMB_4  0x1dea2f
#define GROUP_ORDER_LIMB_5  0xa6f7c

static inline u32 load_32(const u8 src[4]) {

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

static inline u32 load_24(const u8 src[3]) {

    /* Integer encoding is always little endian according to RFC 8032 */
    u32 dst;
#if FE3C_LILENDIAN_TARGET
    /* Target already little endian - copy the bytes with no shifts */
    dst = 0;
    (void) memcpy(&dst, src, 3);
#else
    /* Big-endian target or endianness unknown (take the safe route) */
    dst  = (u32) src[0];
    dst |= (u32) src[1] << 8;
    dst |= (u32) src[2] << 16;
#endif
    return dst;
}

static int ed25519_scalar_is_canonical(const u8 * s) {

    /* Load the scalar into eight 32-bit words (extended to 64 bits) */
    u64 s0 = load_32(&s[0 * 4]);
    u64 s1 = load_32(&s[1 * 4]);
    u64 s2 = load_32(&s[2 * 4]);
    u64 s3 = load_32(&s[3 * 4]);
    u64 s4 = load_32(&s[4 * 4]);
    u64 s5 = load_32(&s[5 * 4]);
    u64 s6 = load_32(&s[6 * 4]);
    u64 s7 = load_32(&s[7 * 4]);

    /* Let L[0]-L[7] denote the eight words of L and let
     * S[0]-S[7] denote the eight words of the scalar s
     * (both in little-endian order). If the following
     * subtraction underflows, then S < L. */
    s0 -= GROUP_ORDER_WORD_0;
    /* Include the underflow in the next subtraction */
    s1 -= GROUP_ORDER_WORD_1 + (s0 >> 63);
    s2 -= GROUP_ORDER_WORD_2 + (s1 >> 63);
    s3 -= GROUP_ORDER_WORD_3 + (s2 >> 63);
    s4 -= GROUP_ORDER_WORD_4 + (s3 >> 63);
    s5 -= GROUP_ORDER_WORD_5 + (s4 >> 63);
    s6 -= GROUP_ORDER_WORD_6 + (s5 >> 63);
    s7 -= GROUP_ORDER_WORD_7 + (s6 >> 63);

    /* If the "borrow" made it all the way to the top,
     * then s is smaller than L and so is canonical. */
    return (s7 >> 63);
}

static void ed25519_scalar_reduce(u8 * s) {

    /* Ed25519 reduction inputs can be 64 bytes long (outputs from SHA-512). Use intermediate
     * 21-bit reduced-radix representation during reduction - we shall use 24 limbs with the
     * highest limb holding 29 bits instead of 21. Note that we use SIGNED words as we rely
     * on sign extension in the algorithm. Recall that masking a signed variable with
     * LOW_21_BITS_MASK == ( (1 << 21) - 1 ) is equivalent to reduction modulo 2^21 (same as
     * for unsigned integers). Similarly shifting by 21 bits to the right is equivalent to
     * dividing by 2^21 while preserving the sign. */

    i64 t[24];

    /* Parse the scalar into limbs */
    t[ 0] = ( load_24(&s[ 0]) >> 0 ) & LOW_21_BITS_MASK;
    /* Two full bytes were written into t0 as well as five additional bits. Skip the two
     * bytes by offsetting into the array and skip the five bits by shifting (note that
     * we need to load four bytes since if we loaded only three, shift would rob us of
     * 5 bits and we would be left with only 19 bits). */
    t[ 1] = ( load_32(&s[ 2]) >> 5 ) & LOW_21_BITS_MASK;
    /* Offset by another 21 bits at each step, i.e. by two additional bytes and five bits
     * of shift (note that the number of shifts is modulo 8 - instead of shifting by 8 we
     * add 1 to the offset). */
    t[ 2] = ( load_24(&s[ 5]) >> 2 ) & LOW_21_BITS_MASK;
    /* Whenever shift is larger than 3 we need to load 32 bits */
    t[ 3] = ( load_32(&s[ 7]) >> 7 ) & LOW_21_BITS_MASK;
    t[ 4] = ( load_32(&s[10]) >> 4 ) & LOW_21_BITS_MASK;
    t[ 5] = ( load_24(&s[13]) >> 1 ) & LOW_21_BITS_MASK;
    t[ 6] = ( load_32(&s[15]) >> 6 ) & LOW_21_BITS_MASK;
    t[ 7] = ( load_24(&s[18]) >> 3 ) & LOW_21_BITS_MASK;
    t[ 8] = ( load_24(&s[21]) >> 0 ) & LOW_21_BITS_MASK;
    t[ 9] = ( load_32(&s[23]) >> 5 ) & LOW_21_BITS_MASK;
    t[10] = ( load_24(&s[26]) >> 2 ) & LOW_21_BITS_MASK;
    t[11] = ( load_32(&s[28]) >> 7 ) & LOW_21_BITS_MASK;
    t[12] = ( load_32(&s[31]) >> 4 ) & LOW_21_BITS_MASK;
    t[13] = ( load_24(&s[34]) >> 1 ) & LOW_21_BITS_MASK;
    t[14] = ( load_32(&s[36]) >> 6 ) & LOW_21_BITS_MASK;
    t[15] = ( load_24(&s[39]) >> 3 ) & LOW_21_BITS_MASK;
    t[16] = ( load_24(&s[42]) >> 0 ) & LOW_21_BITS_MASK;
    t[17] = ( load_32(&s[44]) >> 5 ) & LOW_21_BITS_MASK;
    t[18] = ( load_24(&s[47]) >> 2 ) & LOW_21_BITS_MASK;
    t[19] = ( load_32(&s[49]) >> 7 ) & LOW_21_BITS_MASK;
    t[20] = ( load_32(&s[52]) >> 4 ) & LOW_21_BITS_MASK;
    t[21] = ( load_24(&s[55]) >> 1 ) & LOW_21_BITS_MASK;
    t[22] = ( load_32(&s[57]) >> 6 ) & LOW_21_BITS_MASK;
    /* Load the last 29 bits into t23 */
    t[23] = ( load_32(&s[60]) >> 3 );

    /* Note that limbs t0-t11 contain the value of s modulo 2^252. Let c denote the low
     * four 32-bit words of the group order L, namely let c = L - 2^252. In 21-bit limbs
     * c can be represented as (c5, c4, c3, c2, c1, c0). Take the limbs t12-t23, multiply
     * them by c, and subtract the product from the low part (t0-t11). We rely here on
     * the identity:
     *
     *           s := x + 2^252 y = x + (2^252 + c) y - c y = x - c y (mod L)
     */

    t[11] -= t[23] * GROUP_ORDER_LIMB_0;
    t[12] -= t[23] * GROUP_ORDER_LIMB_1;
    t[13] -= t[23] * GROUP_ORDER_LIMB_2;
    t[14] -= t[23] * GROUP_ORDER_LIMB_3;
    t[15] -= t[23] * GROUP_ORDER_LIMB_4;
    t[16] -= t[23] * GROUP_ORDER_LIMB_5;

    t[10] -= t[22] * GROUP_ORDER_LIMB_0;
    t[11] -= t[22] * GROUP_ORDER_LIMB_1;
    t[12] -= t[22] * GROUP_ORDER_LIMB_2;
    t[13] -= t[22] * GROUP_ORDER_LIMB_3;
    t[14] -= t[22] * GROUP_ORDER_LIMB_4;
    t[15] -= t[22] * GROUP_ORDER_LIMB_5;

    t[ 9] -= t[21] * GROUP_ORDER_LIMB_0;
    t[10] -= t[21] * GROUP_ORDER_LIMB_1;
    t[11] -= t[21] * GROUP_ORDER_LIMB_2;
    t[12] -= t[21] * GROUP_ORDER_LIMB_3;
    t[13] -= t[21] * GROUP_ORDER_LIMB_4;
    t[14] -= t[21] * GROUP_ORDER_LIMB_5;

    t[ 8] -= t[20] * GROUP_ORDER_LIMB_0;
    t[ 9] -= t[20] * GROUP_ORDER_LIMB_1;
    t[10] -= t[20] * GROUP_ORDER_LIMB_2;
    t[11] -= t[20] * GROUP_ORDER_LIMB_3;
    t[12] -= t[20] * GROUP_ORDER_LIMB_4;
    t[13] -= t[20] * GROUP_ORDER_LIMB_5;

    t[ 7] -= t[19] * GROUP_ORDER_LIMB_0;
    t[ 8] -= t[19] * GROUP_ORDER_LIMB_1;
    t[ 9] -= t[19] * GROUP_ORDER_LIMB_2;
    t[10] -= t[19] * GROUP_ORDER_LIMB_3;
    t[11] -= t[19] * GROUP_ORDER_LIMB_4;
    t[12] -= t[19] * GROUP_ORDER_LIMB_5;

    t[ 6] -= t[18] * GROUP_ORDER_LIMB_0;
    t[ 7] -= t[18] * GROUP_ORDER_LIMB_1;
    t[ 8] -= t[18] * GROUP_ORDER_LIMB_2;
    t[ 9] -= t[18] * GROUP_ORDER_LIMB_3;
    t[10] -= t[18] * GROUP_ORDER_LIMB_4;
    t[11] -= t[18] * GROUP_ORDER_LIMB_5;

    /* Do a partial reduction at this point to support all possible inputs */
    t[ 7] += t[ 6] >> 21;  t[ 6] &= LOW_21_BITS_MASK;
    t[ 8] += t[ 7] >> 21;  t[ 7] &= LOW_21_BITS_MASK;
    t[ 9] += t[ 8] >> 21;  t[ 8] &= LOW_21_BITS_MASK;
    t[10] += t[ 9] >> 21;  t[ 9] &= LOW_21_BITS_MASK;
    t[11] += t[10] >> 21;  t[10] &= LOW_21_BITS_MASK;
    t[12] += t[11] >> 21;  t[11] &= LOW_21_BITS_MASK;
    t[13] += t[12] >> 21;  t[12] &= LOW_21_BITS_MASK;
    t[14] += t[13] >> 21;  t[13] &= LOW_21_BITS_MASK;
    t[15] += t[14] >> 21;  t[14] &= LOW_21_BITS_MASK;
    t[16] += t[15] >> 21;  t[15] &= LOW_21_BITS_MASK;
    t[17] += t[16] >> 21;  t[16] &= LOW_21_BITS_MASK;

    t[ 5] -= t[17] * GROUP_ORDER_LIMB_0;
    t[ 6] -= t[17] * GROUP_ORDER_LIMB_1;
    t[ 7] -= t[17] * GROUP_ORDER_LIMB_2;
    t[ 8] -= t[17] * GROUP_ORDER_LIMB_3;
    t[ 9] -= t[17] * GROUP_ORDER_LIMB_4;
    t[10] -= t[17] * GROUP_ORDER_LIMB_5;

    t[ 4] -= t[16] * GROUP_ORDER_LIMB_0;
    t[ 5] -= t[16] * GROUP_ORDER_LIMB_1;
    t[ 6] -= t[16] * GROUP_ORDER_LIMB_2;
    t[ 7] -= t[16] * GROUP_ORDER_LIMB_3;
    t[ 8] -= t[16] * GROUP_ORDER_LIMB_4;
    t[ 9] -= t[16] * GROUP_ORDER_LIMB_5;

    t[ 3] -= t[15] * GROUP_ORDER_LIMB_0;
    t[ 4] -= t[15] * GROUP_ORDER_LIMB_1;
    t[ 5] -= t[15] * GROUP_ORDER_LIMB_2;
    t[ 6] -= t[15] * GROUP_ORDER_LIMB_3;
    t[ 7] -= t[15] * GROUP_ORDER_LIMB_4;
    t[ 8] -= t[15] * GROUP_ORDER_LIMB_5;

    t[ 2] -= t[14] * GROUP_ORDER_LIMB_0;
    t[ 3] -= t[14] * GROUP_ORDER_LIMB_1;
    t[ 4] -= t[14] * GROUP_ORDER_LIMB_2;
    t[ 5] -= t[14] * GROUP_ORDER_LIMB_3;
    t[ 6] -= t[14] * GROUP_ORDER_LIMB_4;
    t[ 7] -= t[14] * GROUP_ORDER_LIMB_5;

    t[ 1] -= t[13] * GROUP_ORDER_LIMB_0;
    t[ 2] -= t[13] * GROUP_ORDER_LIMB_1;
    t[ 3] -= t[13] * GROUP_ORDER_LIMB_2;
    t[ 4] -= t[13] * GROUP_ORDER_LIMB_3;
    t[ 5] -= t[13] * GROUP_ORDER_LIMB_4;
    t[ 6] -= t[13] * GROUP_ORDER_LIMB_5;

    t[ 0] -= t[12] * GROUP_ORDER_LIMB_0;
    t[ 1] -= t[12] * GROUP_ORDER_LIMB_1;
    t[ 2] -= t[12] * GROUP_ORDER_LIMB_2;
    t[ 3] -= t[12] * GROUP_ORDER_LIMB_3;
    t[ 4] -= t[12] * GROUP_ORDER_LIMB_4;
    t[ 5] -= t[12] * GROUP_ORDER_LIMB_5;

    t[ 1] += t[ 0] >> 21;  t[ 0] &= LOW_21_BITS_MASK;
    t[ 2] += t[ 1] >> 21;  t[ 1] &= LOW_21_BITS_MASK;
    t[ 3] += t[ 2] >> 21;  t[ 2] &= LOW_21_BITS_MASK;
    t[ 4] += t[ 3] >> 21;  t[ 3] &= LOW_21_BITS_MASK;
    t[ 5] += t[ 4] >> 21;  t[ 4] &= LOW_21_BITS_MASK;
    t[ 6] += t[ 5] >> 21;  t[ 5] &= LOW_21_BITS_MASK;
    t[ 7] += t[ 6] >> 21;  t[ 6] &= LOW_21_BITS_MASK;
    t[ 8] += t[ 7] >> 21;  t[ 7] &= LOW_21_BITS_MASK;
    t[ 9] += t[ 8] >> 21;  t[ 8] &= LOW_21_BITS_MASK;
    t[10] += t[ 9] >> 21;  t[ 9] &= LOW_21_BITS_MASK;
    t[11] += t[10] >> 21;  t[10] &= LOW_21_BITS_MASK;
    /* Use t12 to store the overflow of t11 */
    t[12]  = t[11] >> 21;  t[11] &= LOW_21_BITS_MASK;

    /* Note that some overflow may have accumulated in t12 again. Reduce it the
     * same way again. */
    t[ 0] -= t[12] * GROUP_ORDER_LIMB_0;
    t[ 1] -= t[12] * GROUP_ORDER_LIMB_1;
    t[ 2] -= t[12] * GROUP_ORDER_LIMB_2;
    t[ 3] -= t[12] * GROUP_ORDER_LIMB_3;
    t[ 4] -= t[12] * GROUP_ORDER_LIMB_4;
    t[ 5] -= t[12] * GROUP_ORDER_LIMB_5;

    t[ 1] += t[ 0] >> 21;  t[ 0] &= LOW_21_BITS_MASK;
    t[ 2] += t[ 1] >> 21;  t[ 1] &= LOW_21_BITS_MASK;
    t[ 3] += t[ 2] >> 21;  t[ 2] &= LOW_21_BITS_MASK;
    t[ 4] += t[ 3] >> 21;  t[ 3] &= LOW_21_BITS_MASK;
    t[ 5] += t[ 4] >> 21;  t[ 4] &= LOW_21_BITS_MASK;
    t[ 6] += t[ 5] >> 21;  t[ 5] &= LOW_21_BITS_MASK;
    t[ 7] += t[ 6] >> 21;  t[ 6] &= LOW_21_BITS_MASK;
    t[ 8] += t[ 7] >> 21;  t[ 7] &= LOW_21_BITS_MASK;
    t[ 9] += t[ 8] >> 21;  t[ 8] &= LOW_21_BITS_MASK;
    t[10] += t[ 9] >> 21;  t[ 9] &= LOW_21_BITS_MASK;
    t[11] += t[10] >> 21;  t[10] &= LOW_21_BITS_MASK;

    s[0]  = ( t[ 0] >>  0 );
    s[1]  = ( t[ 0] >>  8 );
    /* In the first limb we only have 5 bits left for byte number 2,
     * the high 3 bits we must take from the next limb */
    s[2]  = ( t[ 0] >> 16 ) | ( t[ 1] << 5 );
    /* Low 3 bits from t1 we have already encoded into s[2] */
    s[3]  = ( t[ 1] >>  3 );
    s[4]  = ( t[ 1] >> 11 );
    /* Only 2 bits do we have for byte 5 in limb t1, the rest
     * must come from t2 */
    s[5]  = ( t[ 1] >> 19 ) | ( t[ 2] << 2 );
    s[6]  = ( t[ 2] >>  6 );
    s[7]  = ( t[ 2] >> 14 ) | ( t[ 3] << 7 );
    s[8]  = ( t[ 3] >>  1 );
    s[9]  = ( t[ 3] >>  9 );
    s[10] = ( t[ 3] >> 17 ) | ( t[ 4] << 4 );
    s[11] = ( t[ 4] >>  4 );
    s[12] = ( t[ 4] >> 12 );
    s[13] = ( t[ 4] >> 20 ) | ( t[ 5] << 1 );
    s[14] = ( t[ 5] >>  7 );
    s[15] = ( t[ 5] >> 15 ) | ( t[ 6] << 6 );
    s[16] = ( t[ 6] >>  2 );
    s[17] = ( t[ 6] >> 10 );
    s[18] = ( t[ 6] >> 18 ) | ( t[ 7] << 3 );
    s[19] = ( t[ 7] >>  5 );
    s[20] = ( t[ 7] >> 13 );
    /* t7 ended cleanly at byte boundary and so we have hit
     * a period of the limb packing. Starting from this point
     * we repeat the steps from the beginning. */
    s[21] = ( t[ 8] >>  0 );
    s[22] = ( t[ 8] >>  8 );
    s[23] = ( t[ 8] >> 16 ) | ( t[ 9] << 5 );
    s[24] = ( t[ 9] >>  3 );
    s[25] = ( t[ 9] >> 11 );
    s[26] = ( t[ 9] >> 19 ) | ( t[10] << 2 );
    s[27] = ( t[10] >>  6 );
    s[28] = ( t[10] >> 14 ) | ( t[11] << 7 );
    s[29] = ( t[11] >>  1 );
    s[30] = ( t[11] >>  9 );
    s[31] = ( t[11] >> 17 );

    purge_secrets(t, sizeof(t));
}

static void ed25519_scalars_muladd(u8 * r, const u8 * aa, const u8 * bb, const u8 * cc) {

    /* Ed25519 multiplication+addition inputs must be 32 bytes long (outputs from SHA-512
     * subjected to reduction or the response scalar which is part of the signature). Use
     * intermediate 21-bit reduced-radix representation. */

    i64 a[12];
    i64 b[12];
    i64 c[12];
    i64 t[24];

    /* Parse the scalar a into limbs */
    a[ 0] = ( load_24(&aa[ 0]) >> 0 ) & LOW_21_BITS_MASK;
    /* Two full bytes were written into t0 as well as five additional bits. Skip the two
     * bytes by offsetting into the array and skip the five bits by shifting (note that
     * we need to load four bytes since if we loaded only three, shift would rob us of
     * 5 bits and we would be left with only 19 bits). */
    a[ 1] = ( load_32(&aa[ 2]) >> 5 ) & LOW_21_BITS_MASK;
    /* Offset by another 21 bits at each step, i.e. by two additional bytes and five bits
     * of shift (note that the number of shifts is modulo 8 - instead of shifting by 8 we
     * add 1 to the offset). */
    a[ 2] = ( load_24(&aa[ 5]) >> 2 ) & LOW_21_BITS_MASK;
    /* Whenever shift is larger than 3 we need to load 32 bits */
    a[ 3] = ( load_32(&aa[ 7]) >> 7 ) & LOW_21_BITS_MASK;
    a[ 4] = ( load_32(&aa[10]) >> 4 ) & LOW_21_BITS_MASK;
    a[ 5] = ( load_24(&aa[13]) >> 1 ) & LOW_21_BITS_MASK;
    a[ 6] = ( load_32(&aa[15]) >> 6 ) & LOW_21_BITS_MASK;
    a[ 7] = ( load_24(&aa[18]) >> 3 ) & LOW_21_BITS_MASK;
    a[ 8] = ( load_24(&aa[21]) >> 0 ) & LOW_21_BITS_MASK;
    a[ 9] = ( load_32(&aa[23]) >> 5 ) & LOW_21_BITS_MASK;
    a[10] = ( load_24(&aa[26]) >> 2 ) & LOW_21_BITS_MASK;
    a[11] = ( load_32(&aa[28]) >> 7 );

    /* Repeat the same steps for b and c */
    b[ 0] = ( load_24(&bb[ 0]) >> 0 ) & LOW_21_BITS_MASK;
    b[ 1] = ( load_32(&bb[ 2]) >> 5 ) & LOW_21_BITS_MASK;
    b[ 2] = ( load_24(&bb[ 5]) >> 2 ) & LOW_21_BITS_MASK;
    b[ 3] = ( load_32(&bb[ 7]) >> 7 ) & LOW_21_BITS_MASK;
    b[ 4] = ( load_32(&bb[10]) >> 4 ) & LOW_21_BITS_MASK;
    b[ 5] = ( load_24(&bb[13]) >> 1 ) & LOW_21_BITS_MASK;
    b[ 6] = ( load_32(&bb[15]) >> 6 ) & LOW_21_BITS_MASK;
    b[ 7] = ( load_24(&bb[18]) >> 3 ) & LOW_21_BITS_MASK;
    b[ 8] = ( load_24(&bb[21]) >> 0 ) & LOW_21_BITS_MASK;
    b[ 9] = ( load_32(&bb[23]) >> 5 ) & LOW_21_BITS_MASK;
    b[10] = ( load_24(&bb[26]) >> 2 ) & LOW_21_BITS_MASK;
    b[11] = ( load_32(&bb[28]) >> 7 );

    c[ 0] = ( load_24(&cc[ 0]) >> 0 ) & LOW_21_BITS_MASK;
    c[ 1] = ( load_32(&cc[ 2]) >> 5 ) & LOW_21_BITS_MASK;
    c[ 2] = ( load_24(&cc[ 5]) >> 2 ) & LOW_21_BITS_MASK;
    c[ 3] = ( load_32(&cc[ 7]) >> 7 ) & LOW_21_BITS_MASK;
    c[ 4] = ( load_32(&cc[10]) >> 4 ) & LOW_21_BITS_MASK;
    c[ 5] = ( load_24(&cc[13]) >> 1 ) & LOW_21_BITS_MASK;
    c[ 6] = ( load_32(&cc[15]) >> 6 ) & LOW_21_BITS_MASK;
    c[ 7] = ( load_24(&cc[18]) >> 3 ) & LOW_21_BITS_MASK;
    c[ 8] = ( load_24(&cc[21]) >> 0 ) & LOW_21_BITS_MASK;
    c[ 9] = ( load_32(&cc[23]) >> 5 ) & LOW_21_BITS_MASK;
    c[10] = ( load_24(&cc[26]) >> 2 ) & LOW_21_BITS_MASK;
    c[11] = ( load_32(&cc[28]) >> 7 );

    /* Do the naive schoolbook multiplication - note that a*b takes 23 (24 with carry) limbs
     * (columns in the multplication algorithm). Offset the first 12 limbs by the limbs of c:
     *
     *                                                                        a11   a10    a9    a8    a7    a6    a5    a4    a3    a2    a1    a0
     *                                                                        b11   b10    b9    b8    b7    b6    b5    b4    b3    b2    b1    b0
     * -------------- -------------------------------------------------------------------------------------------------------------------------------
     *                                                                       a0b11 a0b10  a0b9  a0b8  a0b7  a0b6  a0b5  a0b4  a0b3  a0b2  a0b1  a0b0
     *                                                                 a1b11 a1b10  a1b9  a1b8  a1b7  a1b6  a1b5  a1b4  a1b3  a1b2  a1b1  a1b0
     *                                                           a2b11 a2b10  a2b9  a2b8  a2b7  a2b6  a2b5  a2b4  a2b3  a2b2  a2b1  a2b0
     *                                                     a3b11 a3b10  a3b9  a3b8  a3b7  a3b6  a3b5  a3b4  a3b3  a3b2  a3b1  a3b0
     *                                               a4b11 a4b10  a4b9  a4b8  a4b7  a4b6  a4b5  a4b4  a4b3  a4b2  a4b1  a4b0
     *                                         a5b11 a5b10  a5b9  a5b8  a5b7  a5b6  a5b5  a5b4  a5b3  a5b2  a5b1  a5b0
     *                                   a6b11 a6b10  a6b9  a6b8  a6b7  a6b6  a6b5  a6b4  a6b3  a6b2  a6b1  a6b0
     *                             a7b11 a7b10  a7b9  a7b8  a7b7  a7b6  a7b5  a7b4  a7b3  a7b2  a7b1  a7b0
     *                       a8b11 a8b10  a8b9  a8b8  a8b7  a8b6  a8b5  a8b4  a8b3  a8b2  a8b1  a8b0
     *                 a9b11 a9b10  a9b9  a9b8  a9b7  a9b6  a9b5  a9b4  a9b3  a9b2  a9b1  a9b0
     *         a10b11 a10b10  a9b9  a9b8  a9b7  a9b6  a9b5  a9b4  a9b3  a9b2  a9b1  a9b0
     *  a11b11 a11b10   a9b9  a9b8  a9b7  a9b6  a9b5  a9b4  a9b3  a9b2  a9b1  a9b0
     *                                                                        c11   c10    c9    c8    c7    c6    c5    c4    c3    c2    c1    c0
     */

    t[ 0] = c[ 0] + a[0]*b[ 0];
    t[ 1] = c[ 1] + a[0]*b[ 1] + a[1]*b[ 0];
    t[ 2] = c[ 2] + a[0]*b[ 2] + a[1]*b[ 1] + a[2]*b[ 0];
    t[ 3] = c[ 3] + a[0]*b[ 3] + a[1]*b[ 2] + a[2]*b[ 1] + a[3]*b[ 0];
    t[ 4] = c[ 4] + a[0]*b[ 4] + a[1]*b[ 3] + a[2]*b[ 2] + a[3]*b[ 1] + a[4]*b[ 0];
    t[ 5] = c[ 5] + a[0]*b[ 5] + a[1]*b[ 4] + a[2]*b[ 3] + a[3]*b[ 2] + a[4]*b[ 1] + a[5]*b[ 0];
    t[ 6] = c[ 6] + a[0]*b[ 6] + a[1]*b[ 5] + a[2]*b[ 4] + a[3]*b[ 3] + a[4]*b[ 2] + a[5]*b[ 1] + a[6]*b[ 0];
    t[ 7] = c[ 7] + a[0]*b[ 7] + a[1]*b[ 6] + a[2]*b[ 5] + a[3]*b[ 4] + a[4]*b[ 3] + a[5]*b[ 2] + a[6]*b[ 1] + a[7]*b[ 0];
    t[ 8] = c[ 8] + a[0]*b[ 8] + a[1]*b[ 7] + a[2]*b[ 6] + a[3]*b[ 5] + a[4]*b[ 4] + a[5]*b[ 3] + a[6]*b[ 2] + a[7]*b[ 1] + a[8]*b[ 0];
    t[ 9] = c[ 9] + a[0]*b[ 9] + a[1]*b[ 8] + a[2]*b[ 7] + a[3]*b[ 6] + a[4]*b[ 5] + a[5]*b[ 4] + a[6]*b[ 3] + a[7]*b[ 2] + a[8]*b[ 1] + a[9]*b[ 0];
    t[10] = c[10] + a[0]*b[10] + a[1]*b[ 9] + a[2]*b[ 8] + a[3]*b[ 7] + a[4]*b[ 6] + a[5]*b[ 5] + a[6]*b[ 4] + a[7]*b[ 3] + a[8]*b[ 2] + a[9]*b[ 1] + a[10]*b[ 0];
    t[11] = c[11] + a[0]*b[11] + a[1]*b[10] + a[2]*b[ 9] + a[3]*b[ 8] + a[4]*b[ 7] + a[5]*b[ 6] + a[6]*b[ 5] + a[7]*b[ 4] + a[8]*b[ 3] + a[9]*b[ 2] + a[10]*b[ 1] + a[11]*b[ 0];
    t[12] =                      a[1]*b[11] + a[2]*b[10] + a[3]*b[ 9] + a[4]*b[ 8] + a[5]*b[ 7] + a[6]*b[ 6] + a[7]*b[ 5] + a[8]*b[ 4] + a[9]*b[ 3] + a[10]*b[ 2] + a[11]*b[ 1];
    t[13] =                                   a[2]*b[11] + a[3]*b[10] + a[4]*b[ 9] + a[5]*b[ 8] + a[6]*b[ 7] + a[7]*b[ 6] + a[8]*b[ 5] + a[9]*b[ 4] + a[10]*b[ 3] + a[11]*b[ 2];
    t[14] =                                                a[3]*b[11] + a[4]*b[10] + a[5]*b[ 9] + a[6]*b[ 8] + a[7]*b[ 7] + a[8]*b[ 6] + a[9]*b[ 5] + a[10]*b[ 4] + a[11]*b[ 3];
    t[15] =                                                             a[4]*b[11] + a[5]*b[10] + a[6]*b[ 9] + a[7]*b[ 8] + a[8]*b[ 7] + a[9]*b[ 6] + a[10]*b[ 5] + a[11]*b[ 4];
    t[16] =                                                                          a[5]*b[11] + a[6]*b[10] + a[7]*b[ 9] + a[8]*b[ 8] + a[9]*b[ 7] + a[10]*b[ 6] + a[11]*b[ 5];
    t[17] =                                                                                       a[6]*b[11] + a[7]*b[10] + a[8]*b[ 9] + a[9]*b[ 8] + a[10]*b[ 7] + a[11]*b[ 6];
    t[18] =                                                                                                    a[7]*b[11] + a[8]*b[10] + a[9]*b[ 9] + a[10]*b[ 8] + a[11]*b[ 7];
    t[19] =                                                                                                                 a[8]*b[11] + a[9]*b[10] + a[10]*b[ 9] + a[11]*b[ 8];
    t[20] =                                                                                                                              a[9]*b[11] + a[10]*b[10] + a[11]*b[ 9];
    t[21] =                                                                                                                                           a[10]*b[11] + a[11]*b[10];
    t[22] =                                                                                                                                                         a[11]*b[11];

    t[ 1] += t[ 0] >> 21;  t[ 0] &= LOW_21_BITS_MASK;
    t[ 2] += t[ 1] >> 21;  t[ 1] &= LOW_21_BITS_MASK;
    t[ 3] += t[ 2] >> 21;  t[ 2] &= LOW_21_BITS_MASK;
    t[ 4] += t[ 3] >> 21;  t[ 3] &= LOW_21_BITS_MASK;
    t[ 5] += t[ 4] >> 21;  t[ 4] &= LOW_21_BITS_MASK;
    t[ 6] += t[ 5] >> 21;  t[ 5] &= LOW_21_BITS_MASK;
    t[ 7] += t[ 6] >> 21;  t[ 6] &= LOW_21_BITS_MASK;
    t[ 8] += t[ 7] >> 21;  t[ 7] &= LOW_21_BITS_MASK;
    t[ 9] += t[ 8] >> 21;  t[ 8] &= LOW_21_BITS_MASK;
    t[10] += t[ 9] >> 21;  t[ 9] &= LOW_21_BITS_MASK;
    t[11] += t[10] >> 21;  t[10] &= LOW_21_BITS_MASK;
    t[12] += t[11] >> 21;  t[11] &= LOW_21_BITS_MASK;
    t[13] += t[12] >> 21;  t[12] &= LOW_21_BITS_MASK;
    t[14] += t[13] >> 21;  t[13] &= LOW_21_BITS_MASK;
    t[15] += t[14] >> 21;  t[14] &= LOW_21_BITS_MASK;
    t[16] += t[15] >> 21;  t[15] &= LOW_21_BITS_MASK;
    t[17] += t[16] >> 21;  t[16] &= LOW_21_BITS_MASK;
    t[18] += t[17] >> 21;  t[17] &= LOW_21_BITS_MASK;
    t[19] += t[18] >> 21;  t[18] &= LOW_21_BITS_MASK;
    t[20] += t[19] >> 21;  t[19] &= LOW_21_BITS_MASK;
    t[21] += t[20] >> 21;  t[20] &= LOW_21_BITS_MASK;
    t[22] += t[21] >> 21;  t[21] &= LOW_21_BITS_MASK;
    /* Use t23 to store the overflow of t22 */
    t[23]  = t[22] >> 21;  t[22] &= LOW_21_BITS_MASK;

    /* Bring down the limbs that exceed 2^252 according to the identity:
     *
     *   x + 2^252 y = x + (2^252 + u) y - u y = x - u y (mod L)
     *
     * where u = L - 2^252. See ed25519_scalar_reduce for a more detailed
     * description.
     */
    t[11] -= t[23] * GROUP_ORDER_LIMB_0;
    t[12] -= t[23] * GROUP_ORDER_LIMB_1;
    t[13] -= t[23] * GROUP_ORDER_LIMB_2;
    t[14] -= t[23] * GROUP_ORDER_LIMB_3;
    t[15] -= t[23] * GROUP_ORDER_LIMB_4;
    t[16] -= t[23] * GROUP_ORDER_LIMB_5;

    t[10] -= t[22] * GROUP_ORDER_LIMB_0;
    t[11] -= t[22] * GROUP_ORDER_LIMB_1;
    t[12] -= t[22] * GROUP_ORDER_LIMB_2;
    t[13] -= t[22] * GROUP_ORDER_LIMB_3;
    t[14] -= t[22] * GROUP_ORDER_LIMB_4;
    t[15] -= t[22] * GROUP_ORDER_LIMB_5;

    t[ 9] -= t[21] * GROUP_ORDER_LIMB_0;
    t[10] -= t[21] * GROUP_ORDER_LIMB_1;
    t[11] -= t[21] * GROUP_ORDER_LIMB_2;
    t[12] -= t[21] * GROUP_ORDER_LIMB_3;
    t[13] -= t[21] * GROUP_ORDER_LIMB_4;
    t[14] -= t[21] * GROUP_ORDER_LIMB_5;

    t[ 8] -= t[20] * GROUP_ORDER_LIMB_0;
    t[ 9] -= t[20] * GROUP_ORDER_LIMB_1;
    t[10] -= t[20] * GROUP_ORDER_LIMB_2;
    t[11] -= t[20] * GROUP_ORDER_LIMB_3;
    t[12] -= t[20] * GROUP_ORDER_LIMB_4;
    t[13] -= t[20] * GROUP_ORDER_LIMB_5;

    t[ 7] -= t[19] * GROUP_ORDER_LIMB_0;
    t[ 8] -= t[19] * GROUP_ORDER_LIMB_1;
    t[ 9] -= t[19] * GROUP_ORDER_LIMB_2;
    t[10] -= t[19] * GROUP_ORDER_LIMB_3;
    t[11] -= t[19] * GROUP_ORDER_LIMB_4;
    t[12] -= t[19] * GROUP_ORDER_LIMB_5;

    t[ 6] -= t[18] * GROUP_ORDER_LIMB_0;
    t[ 7] -= t[18] * GROUP_ORDER_LIMB_1;
    t[ 8] -= t[18] * GROUP_ORDER_LIMB_2;
    t[ 9] -= t[18] * GROUP_ORDER_LIMB_3;
    t[10] -= t[18] * GROUP_ORDER_LIMB_4;
    t[11] -= t[18] * GROUP_ORDER_LIMB_5;

    /* Do a partial reduction of the limbs we have been working with above
     * to ensure no overflows/underflows occur */
    t[ 7] += t[ 6] >> 21;  t[ 6] &= LOW_21_BITS_MASK;
    t[ 8] += t[ 7] >> 21;  t[ 7] &= LOW_21_BITS_MASK;
    t[ 9] += t[ 8] >> 21;  t[ 8] &= LOW_21_BITS_MASK;
    t[10] += t[ 9] >> 21;  t[ 9] &= LOW_21_BITS_MASK;
    t[11] += t[10] >> 21;  t[10] &= LOW_21_BITS_MASK;
    t[12] += t[11] >> 21;  t[11] &= LOW_21_BITS_MASK;
    t[13] += t[12] >> 21;  t[12] &= LOW_21_BITS_MASK;
    t[14] += t[13] >> 21;  t[13] &= LOW_21_BITS_MASK;
    t[15] += t[14] >> 21;  t[14] &= LOW_21_BITS_MASK;
    t[16] += t[15] >> 21;  t[15] &= LOW_21_BITS_MASK;
    t[17] += t[16] >> 21;  t[16] &= LOW_21_BITS_MASK;

    t[ 5] -= t[17] * GROUP_ORDER_LIMB_0;
    t[ 6] -= t[17] * GROUP_ORDER_LIMB_1;
    t[ 7] -= t[17] * GROUP_ORDER_LIMB_2;
    t[ 8] -= t[17] * GROUP_ORDER_LIMB_3;
    t[ 9] -= t[17] * GROUP_ORDER_LIMB_4;
    t[10] -= t[17] * GROUP_ORDER_LIMB_5;

    t[ 4] -= t[16] * GROUP_ORDER_LIMB_0;
    t[ 5] -= t[16] * GROUP_ORDER_LIMB_1;
    t[ 6] -= t[16] * GROUP_ORDER_LIMB_2;
    t[ 7] -= t[16] * GROUP_ORDER_LIMB_3;
    t[ 8] -= t[16] * GROUP_ORDER_LIMB_4;
    t[ 9] -= t[16] * GROUP_ORDER_LIMB_5;

    t[ 3] -= t[15] * GROUP_ORDER_LIMB_0;
    t[ 4] -= t[15] * GROUP_ORDER_LIMB_1;
    t[ 5] -= t[15] * GROUP_ORDER_LIMB_2;
    t[ 6] -= t[15] * GROUP_ORDER_LIMB_3;
    t[ 7] -= t[15] * GROUP_ORDER_LIMB_4;
    t[ 8] -= t[15] * GROUP_ORDER_LIMB_5;

    t[ 2] -= t[14] * GROUP_ORDER_LIMB_0;
    t[ 3] -= t[14] * GROUP_ORDER_LIMB_1;
    t[ 4] -= t[14] * GROUP_ORDER_LIMB_2;
    t[ 5] -= t[14] * GROUP_ORDER_LIMB_3;
    t[ 6] -= t[14] * GROUP_ORDER_LIMB_4;
    t[ 7] -= t[14] * GROUP_ORDER_LIMB_5;

    t[ 1] -= t[13] * GROUP_ORDER_LIMB_0;
    t[ 2] -= t[13] * GROUP_ORDER_LIMB_1;
    t[ 3] -= t[13] * GROUP_ORDER_LIMB_2;
    t[ 4] -= t[13] * GROUP_ORDER_LIMB_3;
    t[ 5] -= t[13] * GROUP_ORDER_LIMB_4;
    t[ 6] -= t[13] * GROUP_ORDER_LIMB_5;

    t[ 0] -= t[12] * GROUP_ORDER_LIMB_0;
    t[ 1] -= t[12] * GROUP_ORDER_LIMB_1;
    t[ 2] -= t[12] * GROUP_ORDER_LIMB_2;
    t[ 3] -= t[12] * GROUP_ORDER_LIMB_3;
    t[ 4] -= t[12] * GROUP_ORDER_LIMB_4;
    t[ 5] -= t[12] * GROUP_ORDER_LIMB_5;

    t[ 1] += t[ 0] >> 21;  t[ 0] &= LOW_21_BITS_MASK;
    t[ 2] += t[ 1] >> 21;  t[ 1] &= LOW_21_BITS_MASK;
    t[ 3] += t[ 2] >> 21;  t[ 2] &= LOW_21_BITS_MASK;
    t[ 4] += t[ 3] >> 21;  t[ 3] &= LOW_21_BITS_MASK;
    t[ 5] += t[ 4] >> 21;  t[ 4] &= LOW_21_BITS_MASK;
    t[ 6] += t[ 5] >> 21;  t[ 5] &= LOW_21_BITS_MASK;
    t[ 7] += t[ 6] >> 21;  t[ 6] &= LOW_21_BITS_MASK;
    t[ 8] += t[ 7] >> 21;  t[ 7] &= LOW_21_BITS_MASK;
    t[ 9] += t[ 8] >> 21;  t[ 8] &= LOW_21_BITS_MASK;
    t[10] += t[ 9] >> 21;  t[ 9] &= LOW_21_BITS_MASK;
    t[11] += t[10] >> 21;  t[10] &= LOW_21_BITS_MASK;
    /* Use t12 to store the overflow of t11 */
    t[12]  = t[11] >> 21;  t[11] &= LOW_21_BITS_MASK;

    /* Something may have bubbled up into t12 (overflow in t11) */
    t[ 0] -= t[12] * GROUP_ORDER_LIMB_0;
    t[ 1] -= t[12] * GROUP_ORDER_LIMB_1;
    t[ 2] -= t[12] * GROUP_ORDER_LIMB_2;
    t[ 3] -= t[12] * GROUP_ORDER_LIMB_3;
    t[ 4] -= t[12] * GROUP_ORDER_LIMB_4;
    t[ 5] -= t[12] * GROUP_ORDER_LIMB_5;

    /* Do the final normalization (normalize limbs to 21 bits) */
    t[ 1] += t[ 0] >> 21;  t[ 0] &= LOW_21_BITS_MASK;
    t[ 2] += t[ 1] >> 21;  t[ 1] &= LOW_21_BITS_MASK;
    t[ 3] += t[ 2] >> 21;  t[ 2] &= LOW_21_BITS_MASK;
    t[ 4] += t[ 3] >> 21;  t[ 3] &= LOW_21_BITS_MASK;
    t[ 5] += t[ 4] >> 21;  t[ 4] &= LOW_21_BITS_MASK;
    t[ 6] += t[ 5] >> 21;  t[ 5] &= LOW_21_BITS_MASK;
    t[ 7] += t[ 6] >> 21;  t[ 6] &= LOW_21_BITS_MASK;
    t[ 8] += t[ 7] >> 21;  t[ 7] &= LOW_21_BITS_MASK;
    t[ 9] += t[ 8] >> 21;  t[ 8] &= LOW_21_BITS_MASK;
    t[10] += t[ 9] >> 21;  t[ 9] &= LOW_21_BITS_MASK;
    t[11] += t[10] >> 21;  t[10] &= LOW_21_BITS_MASK;

    /* Place the limbs in the output buffer, see ed25519_scalar_reduce for a
     * more detailed explanation of this */
    r[0]  = ( t[ 0] >>  0 );
    r[1]  = ( t[ 0] >>  8 );
    r[2]  = ( t[ 0] >> 16 ) | ( t[ 1] << 5 );
    r[3]  = ( t[ 1] >>  3 );
    r[4]  = ( t[ 1] >> 11 );
    r[5]  = ( t[ 1] >> 19 ) | ( t[ 2] << 2 );
    r[6]  = ( t[ 2] >>  6 );
    r[7]  = ( t[ 2] >> 14 ) | ( t[ 3] << 7 );
    r[8]  = ( t[ 3] >>  1 );
    r[9]  = ( t[ 3] >>  9 );
    r[10] = ( t[ 3] >> 17 ) | ( t[ 4] << 4 );
    r[11] = ( t[ 4] >>  4 );
    r[12] = ( t[ 4] >> 12 );
    r[13] = ( t[ 4] >> 20 ) | ( t[ 5] << 1 );
    r[14] = ( t[ 5] >>  7 );
    r[15] = ( t[ 5] >> 15 ) | ( t[ 6] << 6 );
    r[16] = ( t[ 6] >>  2 );
    r[17] = ( t[ 6] >> 10 );
    r[18] = ( t[ 6] >> 18 ) | ( t[ 7] << 3 );
    r[19] = ( t[ 7] >>  5 );
    r[20] = ( t[ 7] >> 13 );
    r[21] = ( t[ 8] >>  0 );
    r[22] = ( t[ 8] >>  8 );
    r[23] = ( t[ 8] >> 16 ) | ( t[ 9] << 5 );
    r[24] = ( t[ 9] >>  3 );
    r[25] = ( t[ 9] >> 11 );
    r[26] = ( t[ 9] >> 19 ) | ( t[10] << 2 );
    r[27] = ( t[10] >>  6 );
    r[28] = ( t[10] >> 14 ) | ( t[11] << 7 );
    r[29] = ( t[11] >>  1 );
    r[30] = ( t[11] >>  9 );
    r[31] = ( t[11] >> 17 );

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
