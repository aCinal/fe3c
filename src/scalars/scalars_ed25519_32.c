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

static void ed25519_scalar_reduce(u8 * s) {

    /* Ed25519 reduction inputs can be 64 bytes long (outputs from SHA-512). Use intermediate
     * 21-bit reduced-radix representation during reduction - we shall use 24 limbs with the
     * highest limb holding 29 bits instead of 21. Note that we use SIGNED words as we rely
     * on sign extension in the algorithm. Recall that masking a signed variable with
     * LOW_21_BITS_MASK == ( (1 << 21) - 1 ) is equivalent to reduction modulo 2^21 (same as
     * for unsigned integers). Similarly shifting by 21 bits to the right is equivalent to
     * dividing by 2^21 while preserving the sign. */

    /* Parse the scalar into limbs */
    i64 t0  = ( load_24(&s[ 0]) >> 0 ) & LOW_21_BITS_MASK;
    /* Two full bytes were written into t0 as well as five additional bits. Skip the two
     * bytes by offsetting into the array and skip the five bits by shifting (note that
     * we need to load four bytes since if we loaded only three, shift would rob us of
     * 5 bits and we would be left with only 19 bits). */
    i64 t1  = ( load_32(&s[ 2]) >> 5 ) & LOW_21_BITS_MASK;
    /* Offset by another 21 bits at each step, i.e. by two additional bytes and five bits
     * of shift (note that the number of shifts is modulo 8 - instead of shifting by 8 we
     * add 1 to the offset). */
    i64 t2  = ( load_24(&s[ 5]) >> 2 ) & LOW_21_BITS_MASK;
    /* Whenever shift is larger than 3 we need to load 32 bits */
    i64 t3  = ( load_32(&s[ 7]) >> 7 ) & LOW_21_BITS_MASK;
    i64 t4  = ( load_32(&s[10]) >> 4 ) & LOW_21_BITS_MASK;
    i64 t5  = ( load_24(&s[13]) >> 1 ) & LOW_21_BITS_MASK;
    i64 t6  = ( load_32(&s[15]) >> 6 ) & LOW_21_BITS_MASK;
    i64 t7  = ( load_24(&s[18]) >> 3 ) & LOW_21_BITS_MASK;
    i64 t8  = ( load_24(&s[21]) >> 0 ) & LOW_21_BITS_MASK;
    i64 t9  = ( load_32(&s[23]) >> 5 ) & LOW_21_BITS_MASK;
    i64 t10 = ( load_24(&s[26]) >> 2 ) & LOW_21_BITS_MASK;
    i64 t11 = ( load_32(&s[28]) >> 7 ) & LOW_21_BITS_MASK;
    i64 t12 = ( load_32(&s[31]) >> 4 ) & LOW_21_BITS_MASK;
    i64 t13 = ( load_24(&s[34]) >> 1 ) & LOW_21_BITS_MASK;
    i64 t14 = ( load_32(&s[36]) >> 6 ) & LOW_21_BITS_MASK;
    i64 t15 = ( load_24(&s[39]) >> 3 ) & LOW_21_BITS_MASK;
    i64 t16 = ( load_24(&s[42]) >> 0 ) & LOW_21_BITS_MASK;
    i64 t17 = ( load_32(&s[44]) >> 5 ) & LOW_21_BITS_MASK;
    i64 t18 = ( load_24(&s[47]) >> 2 ) & LOW_21_BITS_MASK;
    i64 t19 = ( load_32(&s[49]) >> 7 ) & LOW_21_BITS_MASK;
    i64 t20 = ( load_32(&s[52]) >> 4 ) & LOW_21_BITS_MASK;
    i64 t21 = ( load_24(&s[55]) >> 1 ) & LOW_21_BITS_MASK;
    i64 t22 = ( load_32(&s[57]) >> 6 ) & LOW_21_BITS_MASK;
    /* Load the last 29 bits into t23 */
    i64 t23 = ( load_32(&s[60]) >> 3 );

    /* Note that limbs t0-t11 contain the value of s modulo 2^252. Let c denote the low
     * four 32-bit words of the group order L, namely let c = L - 2^252. In 21-bit limbs
     * c can be represented as (c5, c4, c3, c2, c1, c0). Take the limbs t12-t23, multiply
     * them by c, and subtract the product from the low part (t0-t11). We rely here on
     * the identity:
     *
     *           s := x + 2^252 y = x + (2^252 + c) y - c y = x - c y (mod L)
     */

    t11 -= t23 * GROUP_ORDER_LIMB_0;
    t12 -= t23 * GROUP_ORDER_LIMB_1;
    t13 -= t23 * GROUP_ORDER_LIMB_2;
    t14 -= t23 * GROUP_ORDER_LIMB_3;
    t15 -= t23 * GROUP_ORDER_LIMB_4;
    t16 -= t23 * GROUP_ORDER_LIMB_5;

    t10 -= t22 * GROUP_ORDER_LIMB_0;
    t11 -= t22 * GROUP_ORDER_LIMB_1;
    t12 -= t22 * GROUP_ORDER_LIMB_2;
    t13 -= t22 * GROUP_ORDER_LIMB_3;
    t14 -= t22 * GROUP_ORDER_LIMB_4;
    t15 -= t22 * GROUP_ORDER_LIMB_5;

    t9  -= t21 * GROUP_ORDER_LIMB_0;
    t10 -= t21 * GROUP_ORDER_LIMB_1;
    t11 -= t21 * GROUP_ORDER_LIMB_2;
    t12 -= t21 * GROUP_ORDER_LIMB_3;
    t13 -= t21 * GROUP_ORDER_LIMB_4;
    t14 -= t21 * GROUP_ORDER_LIMB_5;

    t8  -= t20 * GROUP_ORDER_LIMB_0;
    t9  -= t20 * GROUP_ORDER_LIMB_1;
    t10 -= t20 * GROUP_ORDER_LIMB_2;
    t11 -= t20 * GROUP_ORDER_LIMB_3;
    t12 -= t20 * GROUP_ORDER_LIMB_4;
    t13 -= t20 * GROUP_ORDER_LIMB_5;

    t7  -= t19 * GROUP_ORDER_LIMB_0;
    t8  -= t19 * GROUP_ORDER_LIMB_1;
    t9  -= t19 * GROUP_ORDER_LIMB_2;
    t10 -= t19 * GROUP_ORDER_LIMB_3;
    t11 -= t19 * GROUP_ORDER_LIMB_4;
    t12 -= t19 * GROUP_ORDER_LIMB_5;

    t6  -= t18 * GROUP_ORDER_LIMB_0;
    t7  -= t18 * GROUP_ORDER_LIMB_1;
    t8  -= t18 * GROUP_ORDER_LIMB_2;
    t9  -= t18 * GROUP_ORDER_LIMB_3;
    t10 -= t18 * GROUP_ORDER_LIMB_4;
    t11 -= t18 * GROUP_ORDER_LIMB_5;

    /* Do a partial reduction at this point to support all possible inputs */
    t7  += t6  >> 21;  t6  &= LOW_21_BITS_MASK;
    t8  += t7  >> 21;  t7  &= LOW_21_BITS_MASK;
    t9  += t8  >> 21;  t8  &= LOW_21_BITS_MASK;
    t10 += t9  >> 21;  t9  &= LOW_21_BITS_MASK;
    t11 += t10 >> 21;  t10 &= LOW_21_BITS_MASK;
    t12 += t11 >> 21;  t11 &= LOW_21_BITS_MASK;
    t13 += t12 >> 21;  t12 &= LOW_21_BITS_MASK;
    t14 += t13 >> 21;  t13 &= LOW_21_BITS_MASK;
    t15 += t14 >> 21;  t14 &= LOW_21_BITS_MASK;
    t16 += t15 >> 21;  t15 &= LOW_21_BITS_MASK;
    t17 += t16 >> 21;  t16 &= LOW_21_BITS_MASK;

    t5  -= t17 * GROUP_ORDER_LIMB_0;
    t6  -= t17 * GROUP_ORDER_LIMB_1;
    t7  -= t17 * GROUP_ORDER_LIMB_2;
    t8  -= t17 * GROUP_ORDER_LIMB_3;
    t9  -= t17 * GROUP_ORDER_LIMB_4;
    t10 -= t17 * GROUP_ORDER_LIMB_5;

    t4  -= t16 * GROUP_ORDER_LIMB_0;
    t5  -= t16 * GROUP_ORDER_LIMB_1;
    t6  -= t16 * GROUP_ORDER_LIMB_2;
    t7  -= t16 * GROUP_ORDER_LIMB_3;
    t8  -= t16 * GROUP_ORDER_LIMB_4;
    t9  -= t16 * GROUP_ORDER_LIMB_5;

    t3  -= t15 * GROUP_ORDER_LIMB_0;
    t4  -= t15 * GROUP_ORDER_LIMB_1;
    t5  -= t15 * GROUP_ORDER_LIMB_2;
    t6  -= t15 * GROUP_ORDER_LIMB_3;
    t7  -= t15 * GROUP_ORDER_LIMB_4;
    t8  -= t15 * GROUP_ORDER_LIMB_5;

    t2  -= t14 * GROUP_ORDER_LIMB_0;
    t3  -= t14 * GROUP_ORDER_LIMB_1;
    t4  -= t14 * GROUP_ORDER_LIMB_2;
    t5  -= t14 * GROUP_ORDER_LIMB_3;
    t6  -= t14 * GROUP_ORDER_LIMB_4;
    t7  -= t14 * GROUP_ORDER_LIMB_5;

    t1  -= t13 * GROUP_ORDER_LIMB_0;
    t2  -= t13 * GROUP_ORDER_LIMB_1;
    t3  -= t13 * GROUP_ORDER_LIMB_2;
    t4  -= t13 * GROUP_ORDER_LIMB_3;
    t5  -= t13 * GROUP_ORDER_LIMB_4;
    t6  -= t13 * GROUP_ORDER_LIMB_5;

    t0  -= t12 * GROUP_ORDER_LIMB_0;
    t1  -= t12 * GROUP_ORDER_LIMB_1;
    t2  -= t12 * GROUP_ORDER_LIMB_2;
    t3  -= t12 * GROUP_ORDER_LIMB_3;
    t4  -= t12 * GROUP_ORDER_LIMB_4;
    t5  -= t12 * GROUP_ORDER_LIMB_5;

    t1  += t0  >> 21;  t0  &= LOW_21_BITS_MASK;
    t2  += t1  >> 21;  t1  &= LOW_21_BITS_MASK;
    t3  += t2  >> 21;  t2  &= LOW_21_BITS_MASK;
    t4  += t3  >> 21;  t3  &= LOW_21_BITS_MASK;
    t5  += t4  >> 21;  t4  &= LOW_21_BITS_MASK;
    t6  += t5  >> 21;  t5  &= LOW_21_BITS_MASK;
    t7  += t6  >> 21;  t6  &= LOW_21_BITS_MASK;
    t8  += t7  >> 21;  t7  &= LOW_21_BITS_MASK;
    t9  += t8  >> 21;  t8  &= LOW_21_BITS_MASK;
    t10 += t9  >> 21;  t9  &= LOW_21_BITS_MASK;
    t11 += t10 >> 21;  t10 &= LOW_21_BITS_MASK;
    /* Use t12 to store the overflow of t11 */
    t12  = t11 >> 21;  t11 &= LOW_21_BITS_MASK;

    /* Note that some overflow may have accumulated in t12 again. Reduce it the
     * same way again. */
    t0  -= t12 * GROUP_ORDER_LIMB_0;
    t1  -= t12 * GROUP_ORDER_LIMB_1;
    t2  -= t12 * GROUP_ORDER_LIMB_2;
    t3  -= t12 * GROUP_ORDER_LIMB_3;
    t4  -= t12 * GROUP_ORDER_LIMB_4;
    t5  -= t12 * GROUP_ORDER_LIMB_5;

    t1  += t0  >> 21;  t0  &= LOW_21_BITS_MASK;
    t2  += t1  >> 21;  t1  &= LOW_21_BITS_MASK;
    t3  += t2  >> 21;  t2  &= LOW_21_BITS_MASK;
    t4  += t3  >> 21;  t3  &= LOW_21_BITS_MASK;
    t5  += t4  >> 21;  t4  &= LOW_21_BITS_MASK;
    t6  += t5  >> 21;  t5  &= LOW_21_BITS_MASK;
    t7  += t6  >> 21;  t6  &= LOW_21_BITS_MASK;
    t8  += t7  >> 21;  t7  &= LOW_21_BITS_MASK;
    t9  += t8  >> 21;  t8  &= LOW_21_BITS_MASK;
    t10 += t9  >> 21;  t9  &= LOW_21_BITS_MASK;
    t11 += t10 >> 21;  t10 &= LOW_21_BITS_MASK;

    s[0]  = ( t0 >> 0 );
    s[1]  = ( t0 >> 8 );
    /* In the first limb we only have 5 bits left for byte number 2,
     * the high 3 bits we must take from the next limb */
    s[2]  = ( t0 >> 16 ) | ( t1 << 5 );
    /* Low 3 bits from t1 we have already encoded into s[2] */
    s[3]  = ( t1 >> 3 );
    s[4]  = ( t1 >> 11 );
    /* Only 2 bits do we have for byte 5 in limb t1, the rest
     * must come from t2 */
    s[5]  = ( t1 >> 19 ) | ( t2 << 2 );
    s[6]  = ( t2 >> 6 );
    s[7]  = ( t2 >> 14 ) | ( t3 << 7 );
    s[8]  = ( t3 >> 1 );
    s[9]  = ( t3 >> 9 );
    s[10] = ( t3 >> 17 ) | ( t4 << 4 );
    s[11] = ( t4 >> 4 );
    s[12] = ( t4 >> 12 );
    s[13] = ( t4 >> 20 ) | ( t5 << 1 );
    s[14] = ( t5 >> 7 );
    s[15] = ( t5 >> 15 ) | ( t6 << 6 );
    s[16] = ( t6 >> 2 );
    s[17] = ( t6 >> 10 );
    s[18] = ( t6 >> 18 ) | ( t7 << 3 );
    s[19] = ( t7 >> 5 );
    s[20] = ( t7 >> 13 );
    /* t7 ended cleanly at byte boundary and so we have hit
     * a period of the limb packing. Starting from this point
     * we repeat the steps from the beginning. */
    s[21] = ( t8 >> 0 );
    s[22] = ( t8 >> 8 );
    s[23] = ( t8 >> 16 ) | ( t9 << 5 );
    s[24] = ( t9 >> 3 );
    s[25] = ( t9 >> 11 );
    s[26] = ( t9 >> 19 ) | ( t10 << 2 );
    s[27] = ( t10 >> 6 );
    s[28] = ( t10 >> 14 ) | ( t11 << 7 );
    s[29] = ( t11 >> 1 );
    s[30] = ( t11 >> 9 );
    s[31] = ( t11 >> 17 );
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

static void ed25519_scalars_muladd(u8 * r, const u8 * a, const u8 * b, const u8 * c) {

    /* Ed25519 multiplication+addition inputs must be 32 bytes long (outputs from SHA-512
     * subjected to reduction or the response scalar which is part of the signature). Use
     * intermediate 21-bit reduced-radix representation. */

    /* Parse the scalar a into limbs */
    i64 a0  = ( load_24(&a[ 0]) >> 0 ) & LOW_21_BITS_MASK;
    /* Two full bytes were written into t0 as well as five additional bits. Skip the two
     * bytes by offsetting into the array and skip the five bits by shifting (note that
     * we need to load four bytes since if we loaded only three, shift would rob us of
     * 5 bits and we would be left with only 19 bits). */
    i64 a1  = ( load_32(&a[ 2]) >> 5 ) & LOW_21_BITS_MASK;
    /* Offset by another 21 bits at each step, i.e. by two additional bytes and five bits
     * of shift (note that the number of shifts is modulo 8 - instead of shifting by 8 we
     * add 1 to the offset). */
    i64 a2  = ( load_24(&a[ 5]) >> 2 ) & LOW_21_BITS_MASK;
    /* Whenever shift is larger than 3 we need to load 32 bits */
    i64 a3  = ( load_32(&a[ 7]) >> 7 ) & LOW_21_BITS_MASK;
    i64 a4  = ( load_32(&a[10]) >> 4 ) & LOW_21_BITS_MASK;
    i64 a5  = ( load_24(&a[13]) >> 1 ) & LOW_21_BITS_MASK;
    i64 a6  = ( load_32(&a[15]) >> 6 ) & LOW_21_BITS_MASK;
    i64 a7  = ( load_24(&a[18]) >> 3 ) & LOW_21_BITS_MASK;
    i64 a8  = ( load_24(&a[21]) >> 0 ) & LOW_21_BITS_MASK;
    i64 a9  = ( load_32(&a[23]) >> 5 ) & LOW_21_BITS_MASK;
    i64 a10 = ( load_24(&a[26]) >> 2 ) & LOW_21_BITS_MASK;
    i64 a11 = ( load_32(&a[28]) >> 7 );

    /* Repeat the same steps for b and c */
    i64 b0  = ( load_24(&b[ 0]) >> 0 ) & LOW_21_BITS_MASK;
    i64 b1  = ( load_32(&b[ 2]) >> 5 ) & LOW_21_BITS_MASK;
    i64 b2  = ( load_24(&b[ 5]) >> 2 ) & LOW_21_BITS_MASK;
    i64 b3  = ( load_32(&b[ 7]) >> 7 ) & LOW_21_BITS_MASK;
    i64 b4  = ( load_32(&b[10]) >> 4 ) & LOW_21_BITS_MASK;
    i64 b5  = ( load_24(&b[13]) >> 1 ) & LOW_21_BITS_MASK;
    i64 b6  = ( load_32(&b[15]) >> 6 ) & LOW_21_BITS_MASK;
    i64 b7  = ( load_24(&b[18]) >> 3 ) & LOW_21_BITS_MASK;
    i64 b8  = ( load_24(&b[21]) >> 0 ) & LOW_21_BITS_MASK;
    i64 b9  = ( load_32(&b[23]) >> 5 ) & LOW_21_BITS_MASK;
    i64 b10 = ( load_24(&b[26]) >> 2 ) & LOW_21_BITS_MASK;
    i64 b11 = ( load_32(&b[28]) >> 7 );

    i64 c0  = ( load_24(&c[ 0]) >> 0 ) & LOW_21_BITS_MASK;
    i64 c1  = ( load_32(&c[ 2]) >> 5 ) & LOW_21_BITS_MASK;
    i64 c2  = ( load_24(&c[ 5]) >> 2 ) & LOW_21_BITS_MASK;
    i64 c3  = ( load_32(&c[ 7]) >> 7 ) & LOW_21_BITS_MASK;
    i64 c4  = ( load_32(&c[10]) >> 4 ) & LOW_21_BITS_MASK;
    i64 c5  = ( load_24(&c[13]) >> 1 ) & LOW_21_BITS_MASK;
    i64 c6  = ( load_32(&c[15]) >> 6 ) & LOW_21_BITS_MASK;
    i64 c7  = ( load_24(&c[18]) >> 3 ) & LOW_21_BITS_MASK;
    i64 c8  = ( load_24(&c[21]) >> 0 ) & LOW_21_BITS_MASK;
    i64 c9  = ( load_32(&c[23]) >> 5 ) & LOW_21_BITS_MASK;
    i64 c10 = ( load_24(&c[26]) >> 2 ) & LOW_21_BITS_MASK;
    i64 c11 = ( load_32(&c[28]) >> 7 );

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

    i64 t0  = c0  + a0*b0;
    i64 t1  = c1  + a0*b1  + a1*b0;
    i64 t2  = c2  + a0*b2  + a1*b1  + a2*b0;
    i64 t3  = c3  + a0*b3  + a1*b2  + a2*b1  + a3*b0;
    i64 t4  = c4  + a0*b4  + a1*b3  + a2*b2  + a3*b1  + a4*b0;
    i64 t5  = c5  + a0*b5  + a1*b4  + a2*b3  + a3*b2  + a4*b1  + a5*b0;
    i64 t6  = c6  + a0*b6  + a1*b5  + a2*b4  + a3*b3  + a4*b2  + a5*b1  + a6*b0;
    i64 t7  = c7  + a0*b7  + a1*b6  + a2*b5  + a3*b4  + a4*b3  + a5*b2  + a6*b1  + a7*b0;
    i64 t8  = c8  + a0*b8  + a1*b7  + a2*b6  + a3*b5  + a4*b4  + a5*b3  + a6*b2  + a7*b1  + a8*b0;
    i64 t9  = c9  + a0*b9  + a1*b8  + a2*b7  + a3*b6  + a4*b5  + a5*b4  + a6*b3  + a7*b2  + a8*b1  + a9*b0;
    i64 t10 = c10 + a0*b10 + a1*b9  + a2*b8  + a3*b7  + a4*b6  + a5*b5  + a6*b4  + a7*b3  + a8*b2  + a9*b1  + a10*b0;
    i64 t11 = c11 + a0*b11 + a1*b10 + a2*b9  + a3*b8  + a4*b7  + a5*b6  + a6*b5  + a7*b4  + a8*b3  + a9*b2  + a10*b1  + a11*b0;
    i64 t12 =                a1*b11 + a2*b10 + a3*b9  + a4*b8  + a5*b7  + a6*b6  + a7*b5  + a8*b4  + a9*b3  + a10*b2  + a11*b1;
    i64 t13 =                         a2*b11 + a3*b10 + a4*b9  + a5*b8  + a6*b7  + a7*b6  + a8*b5  + a9*b4  + a10*b3  + a11*b2;
    i64 t14 =                                  a3*b11 + a4*b10 + a5*b9  + a6*b8  + a7*b7  + a8*b6  + a9*b5  + a10*b4  + a11*b3;
    i64 t15 =                                           a4*b11 + a5*b10 + a6*b9  + a7*b8  + a8*b7  + a9*b6  + a10*b5  + a11*b4;
    i64 t16 =                                                    a5*b11 + a6*b10 + a7*b9  + a8*b8  + a9*b7  + a10*b6  + a11*b5;
    i64 t17 =                                                             a6*b11 + a7*b10 + a8*b9  + a9*b8  + a10*b7  + a11*b6;
    i64 t18 =                                                                      a7*b11 + a8*b10 + a9*b9  + a10*b8  + a11*b7;
    i64 t19 =                                                                               a8*b11 + a9*b10 + a10*b9  + a11*b8;
    i64 t20 =                                                                                        a9*b11 + a10*b10 + a11*b9;
    i64 t21 =                                                                                                 a10*b11 + a11*b10;
    i64 t22 =                                                                                                           a11*b11;
    i64 t23;

    t1  += t0  >> 21;  t0  &= LOW_21_BITS_MASK;
    t2  += t1  >> 21;  t1  &= LOW_21_BITS_MASK;
    t3  += t2  >> 21;  t2  &= LOW_21_BITS_MASK;
    t4  += t3  >> 21;  t3  &= LOW_21_BITS_MASK;
    t5  += t4  >> 21;  t4  &= LOW_21_BITS_MASK;
    t6  += t5  >> 21;  t5  &= LOW_21_BITS_MASK;
    t7  += t6  >> 21;  t6  &= LOW_21_BITS_MASK;
    t8  += t7  >> 21;  t7  &= LOW_21_BITS_MASK;
    t9  += t8  >> 21;  t8  &= LOW_21_BITS_MASK;
    t10 += t9  >> 21;  t9  &= LOW_21_BITS_MASK;
    t11 += t10 >> 21;  t10 &= LOW_21_BITS_MASK;
    t12 += t11 >> 21;  t11 &= LOW_21_BITS_MASK;
    t13 += t12 >> 21;  t12 &= LOW_21_BITS_MASK;
    t14 += t13 >> 21;  t13 &= LOW_21_BITS_MASK;
    t15 += t14 >> 21;  t14 &= LOW_21_BITS_MASK;
    t16 += t15 >> 21;  t15 &= LOW_21_BITS_MASK;
    t17 += t16 >> 21;  t16 &= LOW_21_BITS_MASK;
    t18 += t17 >> 21;  t17 &= LOW_21_BITS_MASK;
    t19 += t18 >> 21;  t18 &= LOW_21_BITS_MASK;
    t20 += t19 >> 21;  t19 &= LOW_21_BITS_MASK;
    t21 += t20 >> 21;  t20 &= LOW_21_BITS_MASK;
    t22 += t21 >> 21;  t21 &= LOW_21_BITS_MASK;
    /* Use t23 to store the overflow of t22 */
    t23  = t22 >> 21;  t22 &= LOW_21_BITS_MASK;

    /* Bring down the limbs that exceed 2^252 according to the identity:
     *
     *   x + 2^252 y = x + (2^252 + u) y - u y = x - u y (mod L)
     *
     * where u = L - 2^252. See ed25519_scalar_reduce for a more detailed
     * description.
     */
    t11 -= t23 * GROUP_ORDER_LIMB_0;
    t12 -= t23 * GROUP_ORDER_LIMB_1;
    t13 -= t23 * GROUP_ORDER_LIMB_2;
    t14 -= t23 * GROUP_ORDER_LIMB_3;
    t15 -= t23 * GROUP_ORDER_LIMB_4;
    t16 -= t23 * GROUP_ORDER_LIMB_5;

    t10 -= t22 * GROUP_ORDER_LIMB_0;
    t11 -= t22 * GROUP_ORDER_LIMB_1;
    t12 -= t22 * GROUP_ORDER_LIMB_2;
    t13 -= t22 * GROUP_ORDER_LIMB_3;
    t14 -= t22 * GROUP_ORDER_LIMB_4;
    t15 -= t22 * GROUP_ORDER_LIMB_5;

    t9  -= t21 * GROUP_ORDER_LIMB_0;
    t10 -= t21 * GROUP_ORDER_LIMB_1;
    t11 -= t21 * GROUP_ORDER_LIMB_2;
    t12 -= t21 * GROUP_ORDER_LIMB_3;
    t13 -= t21 * GROUP_ORDER_LIMB_4;
    t14 -= t21 * GROUP_ORDER_LIMB_5;

    t8  -= t20 * GROUP_ORDER_LIMB_0;
    t9  -= t20 * GROUP_ORDER_LIMB_1;
    t10 -= t20 * GROUP_ORDER_LIMB_2;
    t11 -= t20 * GROUP_ORDER_LIMB_3;
    t12 -= t20 * GROUP_ORDER_LIMB_4;
    t13 -= t20 * GROUP_ORDER_LIMB_5;

    t7  -= t19 * GROUP_ORDER_LIMB_0;
    t8  -= t19 * GROUP_ORDER_LIMB_1;
    t9  -= t19 * GROUP_ORDER_LIMB_2;
    t10 -= t19 * GROUP_ORDER_LIMB_3;
    t11 -= t19 * GROUP_ORDER_LIMB_4;
    t12 -= t19 * GROUP_ORDER_LIMB_5;

    t6  -= t18 * GROUP_ORDER_LIMB_0;
    t7  -= t18 * GROUP_ORDER_LIMB_1;
    t8  -= t18 * GROUP_ORDER_LIMB_2;
    t9  -= t18 * GROUP_ORDER_LIMB_3;
    t10 -= t18 * GROUP_ORDER_LIMB_4;
    t11 -= t18 * GROUP_ORDER_LIMB_5;

    /* Do a partial reduction of the limbs we have been working with above
     * to ensure no overflows/underflows occur */
    t7  += t6  >> 21;  t6  &= LOW_21_BITS_MASK;
    t8  += t7  >> 21;  t7  &= LOW_21_BITS_MASK;
    t9  += t8  >> 21;  t8  &= LOW_21_BITS_MASK;
    t10 += t9  >> 21;  t9  &= LOW_21_BITS_MASK;
    t11 += t10 >> 21;  t10 &= LOW_21_BITS_MASK;
    t12 += t11 >> 21;  t11 &= LOW_21_BITS_MASK;
    t13 += t12 >> 21;  t12 &= LOW_21_BITS_MASK;
    t14 += t13 >> 21;  t13 &= LOW_21_BITS_MASK;
    t15 += t14 >> 21;  t14 &= LOW_21_BITS_MASK;
    t16 += t15 >> 21;  t15 &= LOW_21_BITS_MASK;
    t17 += t16 >> 21;  t16 &= LOW_21_BITS_MASK;

    t5  -= t17 * GROUP_ORDER_LIMB_0;
    t6  -= t17 * GROUP_ORDER_LIMB_1;
    t7  -= t17 * GROUP_ORDER_LIMB_2;
    t8  -= t17 * GROUP_ORDER_LIMB_3;
    t9  -= t17 * GROUP_ORDER_LIMB_4;
    t10 -= t17 * GROUP_ORDER_LIMB_5;

    t4  -= t16 * GROUP_ORDER_LIMB_0;
    t5  -= t16 * GROUP_ORDER_LIMB_1;
    t6  -= t16 * GROUP_ORDER_LIMB_2;
    t7  -= t16 * GROUP_ORDER_LIMB_3;
    t8  -= t16 * GROUP_ORDER_LIMB_4;
    t9  -= t16 * GROUP_ORDER_LIMB_5;

    t3  -= t15 * GROUP_ORDER_LIMB_0;
    t4  -= t15 * GROUP_ORDER_LIMB_1;
    t5  -= t15 * GROUP_ORDER_LIMB_2;
    t6  -= t15 * GROUP_ORDER_LIMB_3;
    t7  -= t15 * GROUP_ORDER_LIMB_4;
    t8  -= t15 * GROUP_ORDER_LIMB_5;

    t2  -= t14 * GROUP_ORDER_LIMB_0;
    t3  -= t14 * GROUP_ORDER_LIMB_1;
    t4  -= t14 * GROUP_ORDER_LIMB_2;
    t5  -= t14 * GROUP_ORDER_LIMB_3;
    t6  -= t14 * GROUP_ORDER_LIMB_4;
    t7  -= t14 * GROUP_ORDER_LIMB_5;

    t1  -= t13 * GROUP_ORDER_LIMB_0;
    t2  -= t13 * GROUP_ORDER_LIMB_1;
    t3  -= t13 * GROUP_ORDER_LIMB_2;
    t4  -= t13 * GROUP_ORDER_LIMB_3;
    t5  -= t13 * GROUP_ORDER_LIMB_4;
    t6  -= t13 * GROUP_ORDER_LIMB_5;

    t0  -= t12 * GROUP_ORDER_LIMB_0;
    t1  -= t12 * GROUP_ORDER_LIMB_1;
    t2  -= t12 * GROUP_ORDER_LIMB_2;
    t3  -= t12 * GROUP_ORDER_LIMB_3;
    t4  -= t12 * GROUP_ORDER_LIMB_4;
    t5  -= t12 * GROUP_ORDER_LIMB_5;

    t1  += t0  >> 21;  t0  &= LOW_21_BITS_MASK;
    t2  += t1  >> 21;  t1  &= LOW_21_BITS_MASK;
    t3  += t2  >> 21;  t2  &= LOW_21_BITS_MASK;
    t4  += t3  >> 21;  t3  &= LOW_21_BITS_MASK;
    t5  += t4  >> 21;  t4  &= LOW_21_BITS_MASK;
    t6  += t5  >> 21;  t5  &= LOW_21_BITS_MASK;
    t7  += t6  >> 21;  t6  &= LOW_21_BITS_MASK;
    t8  += t7  >> 21;  t7  &= LOW_21_BITS_MASK;
    t9  += t8  >> 21;  t8  &= LOW_21_BITS_MASK;
    t10 += t9  >> 21;  t9  &= LOW_21_BITS_MASK;
    t11 += t10 >> 21;  t10 &= LOW_21_BITS_MASK;
    /* Use t12 to store the overflow of t11 */
    t12  = t11 >> 21;  t11 &= LOW_21_BITS_MASK;

    /* Something may have bubbled up into t12 (overflow in t11) */
    t0  -= t12 * GROUP_ORDER_LIMB_0;
    t1  -= t12 * GROUP_ORDER_LIMB_1;
    t2  -= t12 * GROUP_ORDER_LIMB_2;
    t3  -= t12 * GROUP_ORDER_LIMB_3;
    t4  -= t12 * GROUP_ORDER_LIMB_4;
    t5  -= t12 * GROUP_ORDER_LIMB_5;

    /* Do the final normalization (normalize limbs to 21 bits) */
    t1  += t0  >> 21;  t0  &= LOW_21_BITS_MASK;
    t2  += t1  >> 21;  t1  &= LOW_21_BITS_MASK;
    t3  += t2  >> 21;  t2  &= LOW_21_BITS_MASK;
    t4  += t3  >> 21;  t3  &= LOW_21_BITS_MASK;
    t5  += t4  >> 21;  t4  &= LOW_21_BITS_MASK;
    t6  += t5  >> 21;  t5  &= LOW_21_BITS_MASK;
    t7  += t6  >> 21;  t6  &= LOW_21_BITS_MASK;
    t8  += t7  >> 21;  t7  &= LOW_21_BITS_MASK;
    t9  += t8  >> 21;  t8  &= LOW_21_BITS_MASK;
    t10 += t9  >> 21;  t9  &= LOW_21_BITS_MASK;
    t11 += t10 >> 21;  t10 &= LOW_21_BITS_MASK;

    /* Place the limbs in the output buffer, see ed25519_scalar_reduce for a
     * more detailed explanation of this */
    r[0]  = ( t0 >> 0 );
    r[1]  = ( t0 >> 8 );
    r[2]  = ( t0 >> 16 ) | ( t1 << 5 );
    r[3]  = ( t1 >> 3 );
    r[4]  = ( t1 >> 11 );
    r[5]  = ( t1 >> 19 ) | ( t2 << 2 );
    r[6]  = ( t2 >> 6 );
    r[7]  = ( t2 >> 14 ) | ( t3 << 7 );
    r[8]  = ( t3 >> 1 );
    r[9]  = ( t3 >> 9 );
    r[10] = ( t3 >> 17 ) | ( t4 << 4 );
    r[11] = ( t4 >> 4 );
    r[12] = ( t4 >> 12 );
    r[13] = ( t4 >> 20 ) | ( t5 << 1 );
    r[14] = ( t5 >> 7 );
    r[15] = ( t5 >> 15 ) | ( t6 << 6 );
    r[16] = ( t6 >> 2 );
    r[17] = ( t6 >> 10 );
    r[18] = ( t6 >> 18 ) | ( t7 << 3 );
    r[19] = ( t7 >> 5 );
    r[20] = ( t7 >> 13 );
    r[21] = ( t8 >> 0 );
    r[22] = ( t8 >> 8 );
    r[23] = ( t8 >> 16 ) | ( t9 << 5 );
    r[24] = ( t9 >> 3 );
    r[25] = ( t9 >> 11 );
    r[26] = ( t9 >> 19 ) | ( t10 << 2 );
    r[27] = ( t10 >> 6 );
    r[28] = ( t10 >> 14 ) | ( t11 << 7 );
    r[29] = ( t11 >> 1 );
    r[30] = ( t11 >> 9 );
    r[31] = ( t11 >> 17 );
}

scalar_ops ed25519_scalar_ops = {
    .reduce = ed25519_scalar_reduce,
    .is_canonical = ed25519_scalar_is_canonical,
    .muladd = ed25519_scalars_muladd
};
