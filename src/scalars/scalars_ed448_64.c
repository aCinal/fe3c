#include <scalars/scalars.h>
#include <utils/utils.h>

#if !FE3C_64BIT
    #error "Build system inconsistency detected! scalars_ed448_64.c in use despite FE3C_64BIT not being set"
#endif /* !FE3C_64BIT */

#define LOW_54_BITS_MASK        0x3fffffffffffff
#define LOW_56_BITS_MASK        0xffffffffffffff

/* Order of the group as per RFC 7748 */
#define GROUP_ORDER_WORD_0      0x2378c292ab5844f3
#define GROUP_ORDER_WORD_1      0x216cc2728dc58f55
#define GROUP_ORDER_WORD_2      0xc44edb49aed63690
#define GROUP_ORDER_WORD_3      0xffffffff7cca23e9
#define GROUP_ORDER_WORD_4      0xffffffffffffffff
#define GROUP_ORDER_WORD_5      0xffffffffffffffff
#define GROUP_ORDER_WORD_6      0x3fffffffffffffff
/* Redefine the subtrahend of the order (the c in L = 2^446 - c) in terms of 56-bit limbs */
#define GROUP_ORDER_SUB_LIMB_0  0x873d6d54a7bb0d
#define GROUP_ORDER_SUB_LIMB_1  0x3d8d723a70aadc
#define GROUP_ORDER_SUB_LIMB_2  0xb65129c96fde93
#define GROUP_ORDER_SUB_LIMB_3  0x8335dc163bb124

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

static inline u64 load_56(const u8 src[7]) {

    /* Integer encoding is always little endian according to RFC 8032 */
    u64 dst;
#if FE3C_LILENDIAN_TARGET
    /* Target already little endian - copy the bytes with no shifts */
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

static inline void store_56(u8 dst[7], u64 src) {

    /* Integer encoding is always little endian according to RFC 8032 */
#if FE3C_LILENDIAN_TARGET
    (void) memcpy(dst, &src, 7);
#else
    /* After each write do a shift (src is a local variable) */
    dst[0] = (u8) src;  src >>= 8;
    dst[1] = (u8) src;  src >>= 8;
    dst[2] = (u8) src;  src >>= 8;
    dst[3] = (u8) src;  src >>= 8;
    dst[4] = (u8) src;  src >>= 8;
    dst[5] = (u8) src;  src >>= 8;
    dst[6] = (u8) src;
#endif
}

static void ed448_scalar_reduce(u8 * s) {

    /* Ed448 reduction inputs can be 114 bytes long. Use intermediate 56-bit reduced-radix
     * representation during reduction. */

    /* Parse the scalar into limbs */
    u128 t0  = load_56(&s[ 0 * 7]);
    u128 t1  = load_56(&s[ 1 * 7]);
    u128 t2  = load_56(&s[ 2 * 7]);
    u128 t3  = load_56(&s[ 3 * 7]);
    u128 t4  = load_56(&s[ 4 * 7]);
    u128 t5  = load_56(&s[ 5 * 7]);
    u128 t6  = load_56(&s[ 6 * 7]);
    u128 t7  = load_56(&s[ 7 * 7]);
    u128 t8  = load_56(&s[ 8 * 7]);
    u128 t9  = load_56(&s[ 9 * 7]);
    u128 t10 = load_56(&s[10 * 7]);
    u128 t11 = load_56(&s[11 * 7]);
    u128 t12 = load_56(&s[12 * 7]);
    u128 t13 = load_56(&s[13 * 7]);
    u128 t14 = load_56(&s[14 * 7]);
    u128 t15 = load_56(&s[15 * 7]);
    /* Load the trailing 16 bits into t16 */
    u128 t16 = ( (u128) s[113] << 8 ) | s[112];

    /* Let L = 2^446 - c denote the group order. We shall split s into x + y 2^446
     * and use the identity:
     *
     *           x + y 2^446 = x + (2^446 - c) y + c y = x + c y (mod L)
     *
     * Note that the 2^446 boundary is in the middle of the t7 limb. Top two bits
     * of t7 are already above 2^446. For this reason limbs t8-t16 should actually
     * have larger weights. To compensate this we multiply them by four as we go.
     */

    t4  += t12 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t5  += t12 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t6  += t12 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t7  += t12 * 4 * GROUP_ORDER_SUB_LIMB_3;

    t5  += t13 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t6  += t13 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t7  += t13 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t8  += t13 * 4 * GROUP_ORDER_SUB_LIMB_3;

    t6  += t14 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t7  += t14 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t8  += t14 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t9  += t14 * 4 * GROUP_ORDER_SUB_LIMB_3;

    t7  += t15 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t8  += t15 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t9  += t15 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t10 += t15 * 4 * GROUP_ORDER_SUB_LIMB_3;

    t8  += t16 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t9  += t16 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t10 += t16 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t11 += t16 * 4 * GROUP_ORDER_SUB_LIMB_3;

    /* Do a partial reduction of the limbs we have been working with above
     * to ensure no overflows occur */
    t5  += t4  >> 56;  t4  &= LOW_56_BITS_MASK;
    t6  += t5  >> 56;  t5  &= LOW_56_BITS_MASK;
    t7  += t6  >> 56;  t6  &= LOW_56_BITS_MASK;
    t8  += t7  >> 56;  t7  &= LOW_56_BITS_MASK;
    t9  += t8  >> 56;  t8  &= LOW_56_BITS_MASK;
    t10 += t9  >> 56;  t9  &= LOW_56_BITS_MASK;
    t11 += t10 >> 56;  t10 &= LOW_56_BITS_MASK;
    /* Use t12 to store the overflow of t11 */
    t12  = t11 >> 56;  t11 &= LOW_56_BITS_MASK;

    t0  += t8  * 4 * GROUP_ORDER_SUB_LIMB_0;
    t1  += t8  * 4 * GROUP_ORDER_SUB_LIMB_1;
    t2  += t8  * 4 * GROUP_ORDER_SUB_LIMB_2;
    t3  += t8  * 4 * GROUP_ORDER_SUB_LIMB_3;

    t1  += t9  * 4 * GROUP_ORDER_SUB_LIMB_0;
    t2  += t9  * 4 * GROUP_ORDER_SUB_LIMB_1;
    t3  += t9  * 4 * GROUP_ORDER_SUB_LIMB_2;
    t4  += t9  * 4 * GROUP_ORDER_SUB_LIMB_3;

    t2  += t10 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t3  += t10 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t4  += t10 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t5  += t10 * 4 * GROUP_ORDER_SUB_LIMB_3;

    t3  += t11 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t4  += t11 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t5  += t11 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t6  += t11 * 4 * GROUP_ORDER_SUB_LIMB_3;

    t4  += t12 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t5  += t12 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t6  += t12 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t7  += t12 * 4 * GROUP_ORDER_SUB_LIMB_3;

    t1  += t0  >> 56;  t0  &= LOW_56_BITS_MASK;
    t2  += t1  >> 56;  t1  &= LOW_56_BITS_MASK;
    t3  += t2  >> 56;  t2  &= LOW_56_BITS_MASK;
    t4  += t3  >> 56;  t3  &= LOW_56_BITS_MASK;
    t5  += t4  >> 56;  t4  &= LOW_56_BITS_MASK;
    t6  += t5  >> 56;  t5  &= LOW_56_BITS_MASK;
    t7  += t6  >> 56;  t6  &= LOW_56_BITS_MASK;
    /* Use t8 to store the overflow of t7 */
    t8   = t7  >> 56;  t7  &= LOW_56_BITS_MASK;

    /* Bring down whatever has bubbled up into t8 */
    t0  += t8  * 4 * GROUP_ORDER_SUB_LIMB_0;
    t1  += t8  * 4 * GROUP_ORDER_SUB_LIMB_1;
    t2  += t8  * 4 * GROUP_ORDER_SUB_LIMB_2;
    t3  += t8  * 4 * GROUP_ORDER_SUB_LIMB_3;

    t1  += t0  >> 56;  t0  &= LOW_56_BITS_MASK;
    t2  += t1  >> 56;  t1  &= LOW_56_BITS_MASK;
    t3  += t2  >> 56;  t2  &= LOW_56_BITS_MASK;
    t4  += t3  >> 56;  t3  &= LOW_56_BITS_MASK;
    t5  += t4  >> 56;  t4  &= LOW_56_BITS_MASK;
    t6  += t5  >> 56;  t5  &= LOW_56_BITS_MASK;
    t7  += t6  >> 56;  t6  &= LOW_56_BITS_MASK;
    /* Use t8 to store the overflow of t7. Note that at this point
     * we also set t7 to its canonical form which is 54-bits wide. */
    t8   = t7  >> 54;  t7  &= LOW_54_BITS_MASK;

    /* Note that the "shift by two" has already been accounted for */
    t0  += t8  * GROUP_ORDER_SUB_LIMB_0;
    t1  += t8  * GROUP_ORDER_SUB_LIMB_1;
    t2  += t8  * GROUP_ORDER_SUB_LIMB_2;
    t3  += t8  * GROUP_ORDER_SUB_LIMB_3;

    /* Do one final weak reduction */
    t1  += t0  >> 56;  t0  &= LOW_56_BITS_MASK;
    t2  += t1  >> 56;  t1  &= LOW_56_BITS_MASK;
    t3  += t2  >> 56;  t2  &= LOW_56_BITS_MASK;
    t4  += t3  >> 56;  t3  &= LOW_56_BITS_MASK;
    t5  += t4  >> 56;  t4  &= LOW_56_BITS_MASK;
    t6  += t5  >> 56;  t5  &= LOW_56_BITS_MASK;
    t7  += t6  >> 56;  t6  &= LOW_56_BITS_MASK;
    t7  &= LOW_54_BITS_MASK;

    /* By this point we have done a "relaxed reduction", i.e. s is in the range
     * [0, 2*L). Compute s-L and conditionally use it as a result if s is still
     * larger than or equal to L. Start by computing s+c, where c = 2^446 - L.
     * To limit stack usage, reuse the t8-t15 limbs. */
    t8  = GROUP_ORDER_SUB_LIMB_0 + t0;
    t9  = GROUP_ORDER_SUB_LIMB_1 + t1;
    t10 = GROUP_ORDER_SUB_LIMB_2 + t2;
    t11 = GROUP_ORDER_SUB_LIMB_3 + t3;
    t12 = t4;
    t13 = t5;
    t14 = t6;
    t15 = t7;

    /* Normalize the result, i.e. propagate the overflows upwards */
    t9  += t8  >> 56;  t8  &= LOW_56_BITS_MASK;
    t10 += t9  >> 56;  t9  &= LOW_56_BITS_MASK;
    t11 += t10 >> 56;  t10 &= LOW_56_BITS_MASK;
    t12 += t11 >> 56;  t11 &= LOW_56_BITS_MASK;
    t13 += t12 >> 56;  t12 &= LOW_56_BITS_MASK;
    t14 += t13 >> 56;  t13 &= LOW_56_BITS_MASK;
    t15 += t14 >> 56;  t14 &= LOW_56_BITS_MASK;
    /* At this point t15 contains the highest limb of s + c (extended to 128 bits).
     * Try subtracting 2^446 */
    t15 -= (1ULL << 54);
    /* Check if the subtraction resulted in an underflow and use the result to create
     * a mask for a conditional move. */
    u64 mask = (u64)( -(i64)( (t15 >> 127) ^ 1 ) );
    /* If the subtraction underflowed, then mask is set to all zeroes and we will
     * discard limbs t9-t15. Otherwise we will use them as the result. */
    t15 &= LOW_54_BITS_MASK;

    u64 x0 = t0 ^ t8;
    u64 x1 = t1 ^ t9;
    u64 x2 = t2 ^ t10;
    u64 x3 = t3 ^ t11;
    u64 x4 = t4 ^ t12;
    u64 x5 = t5 ^ t13;
    u64 x6 = t6 ^ t14;
    u64 x7 = t7 ^ t15;
    x0 &= mask;
    x1 &= mask;
    x2 &= mask;
    x3 &= mask;
    x4 &= mask;
    x5 &= mask;
    x6 &= mask;
    x7 &= mask;

    t0 ^= x0;
    t1 ^= x1;
    t2 ^= x2;
    t3 ^= x3;
    t4 ^= x4;
    t5 ^= x5;
    t6 ^= x6;
    t7 ^= x7;

    store_56(&s[0 * 7], t0);
    store_56(&s[1 * 7], t1);
    store_56(&s[2 * 7], t2);
    store_56(&s[3 * 7], t3);
    store_56(&s[4 * 7], t4);
    store_56(&s[5 * 7], t5);
    store_56(&s[6 * 7], t6);
    store_56(&s[7 * 7], t7);
    /* Set the last byte to 0 */
    s[56] = 0;
}

static int ed448_scalar_is_canonical(const u8 * s) {

    /* Load the scalar into seven 64-bit words (extended to 128 bits) */
    u128 s0 = load_64(&s[0 * 8]);
    u128 s1 = load_64(&s[1 * 8]);
    u128 s2 = load_64(&s[2 * 8]);
    u128 s3 = load_64(&s[3 * 8]);
    u128 s4 = load_64(&s[4 * 8]);
    u128 s5 = load_64(&s[5 * 8]);
    u128 s6 = load_64(&s[6 * 8]);

    /* Let L[0]-L[6] denote the seven words of L and let
     * S[0]-S[7] denote the four words of the scalar s
     * (both in little-endian order). If the following
     * subtraction underflows and s[56] = 0x00, then
     * S < L. */
    s0 -= GROUP_ORDER_WORD_0;
    /* Include the underflow in the next subtraction */
    s1 -= GROUP_ORDER_WORD_1 + (s0 >> 127);
    s2 -= GROUP_ORDER_WORD_2 + (s1 >> 127);
    s3 -= GROUP_ORDER_WORD_3 + (s2 >> 127);
    s4 -= GROUP_ORDER_WORD_4 + (s3 >> 127);
    s5 -= GROUP_ORDER_WORD_5 + (s4 >> 127);
    s6 -= GROUP_ORDER_WORD_6 + (s5 >> 127);

    /* We separately check if the "borrow" made it all
     * the way to the top and that the last byte of the
     * scalar is cleared (both need to be the case for
     * the scalar to be canonical). */
    return (s6 >> 127) & (s[56] == 0);
}

static void ed448_scalars_muladd(u8 * r, const u8 * a, const u8 * b, const u8 * c) {

    /* Ed448 multiplication+addition inputs must be 57 (56) bytes long. Use intermediate
     * 56-bit reduced-radix representation. */

    u128 a0 = load_56(&a[0 * 7]);
    u128 a1 = load_56(&a[1 * 7]);
    u128 a2 = load_56(&a[2 * 7]);
    u128 a3 = load_56(&a[3 * 7]);
    u128 a4 = load_56(&a[4 * 7]);
    u128 a5 = load_56(&a[5 * 7]);
    u128 a6 = load_56(&a[6 * 7]);
    u128 a7 = load_56(&a[7 * 7]);

    u128 b0 = load_56(&b[0 * 7]);
    u128 b1 = load_56(&b[1 * 7]);
    u128 b2 = load_56(&b[2 * 7]);
    u128 b3 = load_56(&b[3 * 7]);
    u128 b4 = load_56(&b[4 * 7]);
    u128 b5 = load_56(&b[5 * 7]);
    u128 b6 = load_56(&b[6 * 7]);
    u128 b7 = load_56(&b[7 * 7]);

    u128 c0 = load_56(&c[0 * 7]);
    u128 c1 = load_56(&c[1 * 7]);
    u128 c2 = load_56(&c[2 * 7]);
    u128 c3 = load_56(&c[3 * 7]);
    u128 c4 = load_56(&c[4 * 7]);
    u128 c5 = load_56(&c[5 * 7]);
    u128 c6 = load_56(&c[6 * 7]);
    u128 c7 = load_56(&c[7 * 7]);

    /* Do the naive schoolbook multiplication - note that a*b takes 15 (16 with carry) limbs
     * (columns in the multplication algorithm). Offset the first 8 limbs by the limbs of c
     *
     *                                                      a7     a6     a5     a4     a3     a2     a1     a0
     *                                                      b7     b6     b5     b4     b3     b2     b1     b0
     * ---------------------------------------------------------------------------------------------------------
     *                                                     a0b7   a0b6   a0b5   a0b4   a0b3   a0b2   a0b1   a0b0
     *                                              a1b7   a1b6   a1b5   a1b4   a1b3   a1b2   a1b1   a1b0
     *                                       a2b7   a2b6   a2b5   a2b4   a2b3   a2b2   a2b1   a2b0
     *                                a3b7   a3b6   a3b5   a3b4   a3b3   a3b2   a3b1   a3b0
     *                         a4b7   a4b6   a4b5   a4b4   a4b3   a4b2   a4b1   a4b0
     *                  a5b7   a5b6   a5b5   a5b4   a5b3   a5b2   a5b1   a5b0
     *            a6b7  a6b6   a6b5   a6b4   a6b3   a6b2   a6b1   a6b0
     *      a7b7  a7b6  a7b5   a7b4   a7b3   a7b2   a7b1   a7b0
     *                                                      c7     c6     c5     c4     c3     c2     c1     c0
     */

    u128 t0  = c0 + a0*b0;
    u128 t1  = c1 + a0*b1 + a1*b0;
    u128 t2  = c2 + a0*b2 + a1*b1 + a2*b0;
    u128 t3  = c3 + a0*b3 + a1*b2 + a2*b1 + a3*b0;
    u128 t4  = c4 + a0*b4 + a1*b3 + a2*b2 + a3*b1 + a4*b0;
    u128 t5  = c5 + a0*b5 + a1*b4 + a2*b3 + a3*b2 + a4*b1 + a5*b0;
    u128 t6  = c6 + a0*b6 + a1*b5 + a2*b4 + a3*b3 + a4*b2 + a5*b1 + a6*b0;
    u128 t7  = c7 + a0*b7 + a1*b6 + a2*b5 + a3*b4 + a4*b3 + a5*b2 + a6*b1 + a7*b0;
    u128 t8  =              a1*b7 + a2*b6 + a3*b5 + a4*b4 + a5*b3 + a6*b2 + a7*b1;
    u128 t9  =                      a2*b7 + a3*b6 + a4*b5 + a5*b4 + a6*b3 + a7*b2;
    u128 t10 =                              a3*b7 + a4*b6 + a5*b5 + a6*b4 + a7*b3;
    u128 t11 =                                      a4*b7 + a5*b6 + a6*b5 + a7*b4;
    u128 t12 =                                              a5*b7 + a6*b6 + a7*b5;
    u128 t13 =                                                      a6*b7 + a7*b6;
    u128 t14 =                                                              a7*b7;
    u128 t15;

    /* Normalize the limbs to identify the overflow */
    t1  += t0  >> 56;  t0  &= LOW_56_BITS_MASK;
    t2  += t1  >> 56;  t1  &= LOW_56_BITS_MASK;
    t3  += t2  >> 56;  t2  &= LOW_56_BITS_MASK;
    t4  += t3  >> 56;  t3  &= LOW_56_BITS_MASK;
    t5  += t4  >> 56;  t4  &= LOW_56_BITS_MASK;
    t6  += t5  >> 56;  t5  &= LOW_56_BITS_MASK;
    t7  += t6  >> 56;  t6  &= LOW_56_BITS_MASK;
    t8  += t7  >> 56;  t7  &= LOW_56_BITS_MASK;
    t9  += t8  >> 56;  t8  &= LOW_56_BITS_MASK;
    t10 += t9  >> 56;  t9  &= LOW_56_BITS_MASK;
    t11 += t10 >> 56;  t10 &= LOW_56_BITS_MASK;
    t12 += t11 >> 56;  t11 &= LOW_56_BITS_MASK;
    t13 += t12 >> 56;  t12 &= LOW_56_BITS_MASK;
    t14 += t13 >> 56;  t13 &= LOW_56_BITS_MASK;
    /* Use t15 to store the overflow of t14 */
    t15  = t14 >> 56;  t14 &= LOW_56_BITS_MASK;

    /* Bring down the limbs that exceed 2^446 according to the identity:
     *
     *   x + y 2^446 = x + (2^446 - u) y + u y = x + u y (mod L)
     *
     * where u = 2^446 - L. See ed448_scalar_reduce for a more detailed
     * description, in particular the reasoning behind the multiplication
     * by four (due to how the scalars were parsed and handled).
     */

    t4  += t12 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t5  += t12 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t6  += t12 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t7  += t12 * 4 * GROUP_ORDER_SUB_LIMB_3;

    t5  += t13 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t6  += t13 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t7  += t13 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t8  += t13 * 4 * GROUP_ORDER_SUB_LIMB_3;

    t6  += t14 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t7  += t14 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t8  += t14 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t9  += t14 * 4 * GROUP_ORDER_SUB_LIMB_3;

    t7  += t15 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t8  += t15 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t9  += t15 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t10 += t15 * 4 * GROUP_ORDER_SUB_LIMB_3;

    /* Do a partial reduction of the limbs we have been working with above
     * to ensure no overflows occur */
    t5  += t4  >> 56;  t4  &= LOW_56_BITS_MASK;
    t6  += t5  >> 56;  t5  &= LOW_56_BITS_MASK;
    t7  += t6  >> 56;  t6  &= LOW_56_BITS_MASK;
    t8  += t7  >> 56;  t7  &= LOW_56_BITS_MASK;
    t9  += t8  >> 56;  t8  &= LOW_56_BITS_MASK;
    t10 += t9  >> 56;  t9  &= LOW_56_BITS_MASK;
    t11 += t10 >> 56;  t10 &= LOW_56_BITS_MASK;

    t0  += t8  * 4 * GROUP_ORDER_SUB_LIMB_0;
    t1  += t8  * 4 * GROUP_ORDER_SUB_LIMB_1;
    t2  += t8  * 4 * GROUP_ORDER_SUB_LIMB_2;
    t3  += t8  * 4 * GROUP_ORDER_SUB_LIMB_3;

    t1  += t9  * 4 * GROUP_ORDER_SUB_LIMB_0;
    t2  += t9  * 4 * GROUP_ORDER_SUB_LIMB_1;
    t3  += t9  * 4 * GROUP_ORDER_SUB_LIMB_2;
    t4  += t9  * 4 * GROUP_ORDER_SUB_LIMB_3;

    t2  += t10 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t3  += t10 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t4  += t10 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t5  += t10 * 4 * GROUP_ORDER_SUB_LIMB_3;

    t3  += t11 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t4  += t11 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t5  += t11 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t6  += t11 * 4 * GROUP_ORDER_SUB_LIMB_3;

    t1  += t0  >> 56;  t0  &= LOW_56_BITS_MASK;
    t2  += t1  >> 56;  t1  &= LOW_56_BITS_MASK;
    t3  += t2  >> 56;  t2  &= LOW_56_BITS_MASK;
    t4  += t3  >> 56;  t3  &= LOW_56_BITS_MASK;
    t5  += t4  >> 56;  t4  &= LOW_56_BITS_MASK;
    t6  += t5  >> 56;  t5  &= LOW_56_BITS_MASK;
    t7  += t6  >> 56;  t6  &= LOW_56_BITS_MASK;
    /* Use t8 to store the overflow of t7. Note that at this point
     * we also set t7 to its canonical form which is 54-bits wide. */
    t8   = t7  >> 54;  t7  &= LOW_54_BITS_MASK;

    /* Note that the "shift by two" has already been accounted for */
    t0  += t8  * GROUP_ORDER_SUB_LIMB_0;
    t1  += t8  * GROUP_ORDER_SUB_LIMB_1;
    t2  += t8  * GROUP_ORDER_SUB_LIMB_2;
    t3  += t8  * GROUP_ORDER_SUB_LIMB_3;

    /* Do one final weak reduction */
    t1  += t0  >> 56;  t0  &= LOW_56_BITS_MASK;
    t2  += t1  >> 56;  t1  &= LOW_56_BITS_MASK;
    t3  += t2  >> 56;  t2  &= LOW_56_BITS_MASK;
    t4  += t3  >> 56;  t3  &= LOW_56_BITS_MASK;
    t5  += t4  >> 56;  t4  &= LOW_56_BITS_MASK;
    t6  += t5  >> 56;  t5  &= LOW_56_BITS_MASK;
    t7  += t6  >> 56;  t6  &= LOW_56_BITS_MASK;
    t7  &= LOW_54_BITS_MASK;

    /* By this point we have done a "relaxed reduction", i.e. the result is in
     * the range [0, 2*L). Compute ab+c-L and conditionally use it as a result if
     * the result is still larger than or equal to L. Start by computing ab+c+u,
     * where u = 2^446 - L. To limit stack usage, reuse the t8-t15 limbs. */
    t8  = GROUP_ORDER_SUB_LIMB_0 + t0;
    t9  = GROUP_ORDER_SUB_LIMB_1 + t1;
    t10 = GROUP_ORDER_SUB_LIMB_2 + t2;
    t11 = GROUP_ORDER_SUB_LIMB_3 + t3;
    t12 = t4;
    t13 = t5;
    t14 = t6;
    t15 = t7;

    /* Normalize the result, i.e. propagate the overflows upwards */
    t9  += t8  >> 56;  t8  &= LOW_56_BITS_MASK;
    t10 += t9  >> 56;  t9  &= LOW_56_BITS_MASK;
    t11 += t10 >> 56;  t10 &= LOW_56_BITS_MASK;
    t12 += t11 >> 56;  t11 &= LOW_56_BITS_MASK;
    t13 += t12 >> 56;  t12 &= LOW_56_BITS_MASK;
    t14 += t13 >> 56;  t13 &= LOW_56_BITS_MASK;
    t15 += t14 >> 56;  t14 &= LOW_56_BITS_MASK;
    /* At this point t15 contains the highest limb of ab+c+u (extended to 128 bits).
     * Try subtracting 2^446 */
    t15 -= (1ULL << 54);
    /* Check if the subtraction resulted in an underflow and use the result to create
     * a mask for a conditional move. */
    u64 mask = (u64)( -(i64)( (t15 >> 127) ^ 1 ) );
    /* If the subtraction underflowed, then mask is set to all zeroes and we will
     * discard limbs t9-t15. Otherwise we will use them as the result. */
    t15 &= LOW_54_BITS_MASK;

    u64 x0 = t0 ^ t8;
    u64 x1 = t1 ^ t9;
    u64 x2 = t2 ^ t10;
    u64 x3 = t3 ^ t11;
    u64 x4 = t4 ^ t12;
    u64 x5 = t5 ^ t13;
    u64 x6 = t6 ^ t14;
    u64 x7 = t7 ^ t15;
    x0 &= mask;
    x1 &= mask;
    x2 &= mask;
    x3 &= mask;
    x4 &= mask;
    x5 &= mask;
    x6 &= mask;
    x7 &= mask;

    t0 ^= x0;
    t1 ^= x1;
    t2 ^= x2;
    t3 ^= x3;
    t4 ^= x4;
    t5 ^= x5;
    t6 ^= x6;
    t7 ^= x7;

    store_56(&r[0 * 7], t0);
    store_56(&r[1 * 7], t1);
    store_56(&r[2 * 7], t2);
    store_56(&r[3 * 7], t3);
    store_56(&r[4 * 7], t4);
    store_56(&r[5 * 7], t5);
    store_56(&r[6 * 7], t6);
    store_56(&r[7 * 7], t7);
    /* Set the last byte to 0 */
    r[56] = 0;
}

scalar_ops ed448_scalar_ops = {
    .reduce = ed448_scalar_reduce,
    .is_canonical = ed448_scalar_is_canonical,
    .muladd = ed448_scalars_muladd
};
