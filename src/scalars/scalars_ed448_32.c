#include <scalars/scalars.h>
#include <utils/utils.h>

#if !FE3C_32BIT
    #error "Build system inconsistency detected! scalars_ed448_32.c in use despite FE3C_32BIT not being set"
#endif /* !FE3C_32BIT */

#define LOW_26_BITS_MASK  0x3ffffff
#define LOW_28_BITS_MASK  0xfffffff

/* Order of the group as per RFC 7748 */
#define GROUP_ORDER_WORD_0      0xab5844f3
#define GROUP_ORDER_WORD_1      0x2378c292
#define GROUP_ORDER_WORD_2      0x8dc58f55
#define GROUP_ORDER_WORD_3      0x216cc272
#define GROUP_ORDER_WORD_4      0xaed63690
#define GROUP_ORDER_WORD_5      0xc44edb49
#define GROUP_ORDER_WORD_6      0x7cca23e9
#define GROUP_ORDER_WORD_7      0xffffffff
#define GROUP_ORDER_WORD_8      0xffffffff
#define GROUP_ORDER_WORD_9      0xffffffff
#define GROUP_ORDER_WORD_10     0xffffffff
#define GROUP_ORDER_WORD_11     0xffffffff
#define GROUP_ORDER_WORD_12     0xffffffff
#define GROUP_ORDER_WORD_13     0x3fffffff
/* Redefine the subtrahend of the order (the c in L = 2^446 - c) in terms of 28-bit limbs */
#define GROUP_ORDER_SUB_LIMB_0  0x4a7bb0d
#define GROUP_ORDER_SUB_LIMB_1  0x873d6d5
#define GROUP_ORDER_SUB_LIMB_2  0xa70aadc
#define GROUP_ORDER_SUB_LIMB_3  0x3d8d723
#define GROUP_ORDER_SUB_LIMB_4  0x96fde93
#define GROUP_ORDER_SUB_LIMB_5  0xb65129c
#define GROUP_ORDER_SUB_LIMB_6  0x63bb124
#define GROUP_ORDER_SUB_LIMB_7  0x8335dc1

static inline u32 load_32(const u8 src[8]) {

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

static int ed448_scalar_is_canonical(const u8 * s) {

    /* Load the scalar into fourteen 32-bit words (extended to 64 bits) */
    u64 s0  = load_32(&s[ 0 * 4]);
    u64 s1  = load_32(&s[ 1 * 4]);
    u64 s2  = load_32(&s[ 2 * 4]);
    u64 s3  = load_32(&s[ 3 * 4]);
    u64 s4  = load_32(&s[ 4 * 4]);
    u64 s5  = load_32(&s[ 5 * 4]);
    u64 s6  = load_32(&s[ 6 * 4]);
    u64 s7  = load_32(&s[ 7 * 4]);
    u64 s8  = load_32(&s[ 8 * 4]);
    u64 s9  = load_32(&s[ 9 * 4]);
    u64 s10 = load_32(&s[10 * 4]);
    u64 s11 = load_32(&s[11 * 4]);
    u64 s12 = load_32(&s[12 * 4]);
    u64 s13 = load_32(&s[13 * 4]);

    /* Let L[0]-L[13] denote the fourteen words of L and let
     * S[0]-S[13] denote the fourteen words of the scalar s
     * (both in little-endian order). If the following
     * subtraction underflows and s[56] = 0x00, then
     * S < L. */
    s0  -= GROUP_ORDER_WORD_0;
    /* Include the underflow in the next subtraction */
    s1  -= GROUP_ORDER_WORD_1  + (s0  >> 63);
    s2  -= GROUP_ORDER_WORD_2  + (s1  >> 63);
    s3  -= GROUP_ORDER_WORD_3  + (s2  >> 63);
    s4  -= GROUP_ORDER_WORD_4  + (s3  >> 63);
    s5  -= GROUP_ORDER_WORD_5  + (s4  >> 63);
    s6  -= GROUP_ORDER_WORD_6  + (s5  >> 63);
    s7  -= GROUP_ORDER_WORD_7  + (s6  >> 63);
    s8  -= GROUP_ORDER_WORD_8  + (s7  >> 63);
    s9  -= GROUP_ORDER_WORD_9  + (s8  >> 63);
    s10 -= GROUP_ORDER_WORD_10 + (s9  >> 63);
    s11 -= GROUP_ORDER_WORD_11 + (s10 >> 63);
    s12 -= GROUP_ORDER_WORD_12 + (s11 >> 63);
    s13 -= GROUP_ORDER_WORD_13 + (s12 >> 63);

    /* We separately check if the "borrow" made it all
     * the way to the top and that the last byte of the
     * scalar is cleared (both need to be the case for
     * the scalar to be canonical). */
    return (s13 >> 63) & (s[56] == 0);
}

static void ed448_scalar_reduce(u8 * s) {

    /* Ed448 reduction inputs can be 114 bytes long. Use intermediate 28-bit reduced-radix
     * representation during reduction. */

    /* Parse the scalar into limbs */
    u64 t0  = ( load_32(&s[  0]) >> 0 ) & LOW_28_BITS_MASK;
    u64 t1  = ( load_32(&s[  3]) >> 4 ) & LOW_28_BITS_MASK;
    u64 t2  = ( load_32(&s[  7]) >> 0 ) & LOW_28_BITS_MASK;
    u64 t3  = ( load_32(&s[ 10]) >> 4 ) & LOW_28_BITS_MASK;
    u64 t4  = ( load_32(&s[ 14]) >> 0 ) & LOW_28_BITS_MASK;
    u64 t5  = ( load_32(&s[ 17]) >> 4 ) & LOW_28_BITS_MASK;
    u64 t6  = ( load_32(&s[ 21]) >> 0 ) & LOW_28_BITS_MASK;
    u64 t7  = ( load_32(&s[ 24]) >> 4 ) & LOW_28_BITS_MASK;
    u64 t8  = ( load_32(&s[ 28]) >> 0 ) & LOW_28_BITS_MASK;
    u64 t9  = ( load_32(&s[ 31]) >> 4 ) & LOW_28_BITS_MASK;
    u64 t10 = ( load_32(&s[ 35]) >> 0 ) & LOW_28_BITS_MASK;
    u64 t11 = ( load_32(&s[ 38]) >> 4 ) & LOW_28_BITS_MASK;
    u64 t12 = ( load_32(&s[ 42]) >> 0 ) & LOW_28_BITS_MASK;
    u64 t13 = ( load_32(&s[ 45]) >> 4 ) & LOW_28_BITS_MASK;
    u64 t14 = ( load_32(&s[ 49]) >> 0 ) & LOW_28_BITS_MASK;
    u64 t15 = ( load_32(&s[ 52]) >> 4 ) & LOW_28_BITS_MASK;
    u64 t16 = ( load_32(&s[ 56]) >> 0 ) & LOW_28_BITS_MASK;
    u64 t17 = ( load_32(&s[ 59]) >> 4 ) & LOW_28_BITS_MASK;
    u64 t18 = ( load_32(&s[ 63]) >> 0 ) & LOW_28_BITS_MASK;
    u64 t19 = ( load_32(&s[ 66]) >> 4 ) & LOW_28_BITS_MASK;
    u64 t20 = ( load_32(&s[ 70]) >> 0 ) & LOW_28_BITS_MASK;
    u64 t21 = ( load_32(&s[ 73]) >> 4 ) & LOW_28_BITS_MASK;
    u64 t22 = ( load_32(&s[ 77]) >> 0 ) & LOW_28_BITS_MASK;
    u64 t23 = ( load_32(&s[ 80]) >> 4 ) & LOW_28_BITS_MASK;
    u64 t24 = ( load_32(&s[ 84]) >> 0 ) & LOW_28_BITS_MASK;
    u64 t25 = ( load_32(&s[ 87]) >> 4 ) & LOW_28_BITS_MASK;
    u64 t26 = ( load_32(&s[ 91]) >> 0 ) & LOW_28_BITS_MASK;
    u64 t27 = ( load_32(&s[ 94]) >> 4 ) & LOW_28_BITS_MASK;
    u64 t28 = ( load_32(&s[ 98]) >> 0 ) & LOW_28_BITS_MASK;
    u64 t29 = ( load_32(&s[101]) >> 4 ) & LOW_28_BITS_MASK;
    u64 t30 = ( load_32(&s[105]) >> 0 ) & LOW_28_BITS_MASK;
    u64 t31 = ( load_32(&s[108]) >> 4 ) & LOW_28_BITS_MASK;
    /* Load the trailing 16 bits into t32 */
    u64 t32 = ( (u64) s[113] << 8 ) | s[112];

    /* Let L = 2^446 - c denote the group order. We shall split s into x + y 2^446
     * and use the identity:
     *
     *           x + y 2^446 = x + (2^446 - c) y + c y = x + c y (mod L)
     *
     * Note that the 2^446 boundary is in the middle of limb t15. Top two bits
     * of t15 are already above 2^446. For this reason limbs t16-t32 should actually
     * have larger weights. To compensate this we multiply them by four as we go.
     */

    t8  += t24 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t9  += t24 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t10 += t24 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t11 += t24 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t12 += t24 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t13 += t24 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t14 += t24 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t15 += t24 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t9  += t25 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t10 += t25 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t11 += t25 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t12 += t25 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t13 += t25 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t14 += t25 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t15 += t25 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t16 += t25 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t10 += t26 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t11 += t26 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t12 += t26 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t13 += t26 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t14 += t26 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t15 += t26 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t16 += t26 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t17 += t26 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t11 += t27 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t12 += t27 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t13 += t27 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t14 += t27 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t15 += t27 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t16 += t27 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t17 += t27 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t18 += t27 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t12 += t28 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t13 += t28 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t14 += t28 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t15 += t28 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t16 += t28 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t17 += t28 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t18 += t28 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t19 += t28 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t13 += t29 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t14 += t29 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t15 += t29 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t16 += t29 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t17 += t29 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t18 += t29 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t19 += t29 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t20 += t29 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t14 += t30 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t15 += t30 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t16 += t30 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t17 += t30 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t18 += t30 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t19 += t30 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t20 += t30 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t21 += t30 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t15 += t31 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t16 += t31 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t17 += t31 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t18 += t31 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t19 += t31 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t20 += t31 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t21 += t31 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t22 += t31 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t16 += t32 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t17 += t32 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t18 += t32 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t19 += t32 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t20 += t32 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t21 += t32 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t22 += t32 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t23 += t32 * 4 * GROUP_ORDER_SUB_LIMB_7;

    /* Do a partial reduction of the limbs we have been working with above
     * to ensure no overflows occur */
    t9  += t8  >> 28;  t8  &= LOW_28_BITS_MASK;
    t10 += t9  >> 28;  t9  &= LOW_28_BITS_MASK;
    t11 += t10 >> 28;  t10 &= LOW_28_BITS_MASK;
    t12 += t11 >> 28;  t11 &= LOW_28_BITS_MASK;
    t13 += t12 >> 28;  t12 &= LOW_28_BITS_MASK;
    t14 += t13 >> 28;  t13 &= LOW_28_BITS_MASK;
    t15 += t14 >> 28;  t14 &= LOW_28_BITS_MASK;
    t16 += t15 >> 28;  t15 &= LOW_28_BITS_MASK;
    t17 += t16 >> 28;  t16 &= LOW_28_BITS_MASK;
    t18 += t17 >> 28;  t17 &= LOW_28_BITS_MASK;
    t19 += t18 >> 28;  t18 &= LOW_28_BITS_MASK;
    t20 += t19 >> 28;  t19 &= LOW_28_BITS_MASK;
    t21 += t20 >> 28;  t20 &= LOW_28_BITS_MASK;
    t22 += t21 >> 28;  t21 &= LOW_28_BITS_MASK;
    t23 += t22 >> 28;  t22 &= LOW_28_BITS_MASK;
    /* Use t24 to store the overflow of t23 */
    t24  = t23 >> 28;  t23 &= LOW_28_BITS_MASK;

    t0  += t16 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t1  += t16 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t2  += t16 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t3  += t16 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t4  += t16 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t5  += t16 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t6  += t16 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t7  += t16 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t1  += t17 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t2  += t17 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t3  += t17 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t4  += t17 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t5  += t17 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t6  += t17 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t7  += t17 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t8  += t17 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t2  += t18 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t3  += t18 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t4  += t18 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t5  += t18 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t6  += t18 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t7  += t18 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t8  += t18 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t9  += t18 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t3  += t19 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t4  += t19 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t5  += t19 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t6  += t19 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t7  += t19 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t8  += t19 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t9  += t19 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t10 += t19 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t4  += t20 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t5  += t20 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t6  += t20 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t7  += t20 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t8  += t20 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t9  += t20 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t10 += t20 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t11 += t20 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t5  += t21 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t6  += t21 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t7  += t21 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t8  += t21 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t9  += t21 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t10 += t21 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t11 += t21 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t12 += t21 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t6  += t22 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t7  += t22 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t8  += t22 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t9  += t22 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t10 += t22 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t11 += t22 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t12 += t22 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t13 += t22 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t7  += t23 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t8  += t23 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t9  += t23 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t10 += t23 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t11 += t23 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t12 += t23 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t13 += t23 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t14 += t23 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t8  += t24 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t9  += t24 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t10 += t24 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t11 += t24 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t12 += t24 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t13 += t24 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t14 += t24 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t15 += t24 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t1  += t0  >> 28;  t0  &= LOW_28_BITS_MASK;
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
    /* Use t16 to store the overflow of t15 */
    t16  = t15 >> 28;  t15 &= LOW_28_BITS_MASK;

    /* Bring down whatever has bubbled up into t16 */
    t0  += t16 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t1  += t16 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t2  += t16 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t3  += t16 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t4  += t16 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t5  += t16 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t6  += t16 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t7  += t16 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t1  += t0  >> 28;  t0  &= LOW_28_BITS_MASK;
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
    /* Use t16 to store the overflow of t15. Note that at this point
     * we also set t15 to its canonical form which is 26-bits wide.  */
    t16  = t15 >> 26;  t15 &= LOW_26_BITS_MASK;

    /* Note that the "shift by two" has already been accounted for */
    t0  += t16 * GROUP_ORDER_SUB_LIMB_0;
    t1  += t16 * GROUP_ORDER_SUB_LIMB_1;
    t2  += t16 * GROUP_ORDER_SUB_LIMB_2;
    t3  += t16 * GROUP_ORDER_SUB_LIMB_3;
    t4  += t16 * GROUP_ORDER_SUB_LIMB_4;
    t5  += t16 * GROUP_ORDER_SUB_LIMB_5;
    t6  += t16 * GROUP_ORDER_SUB_LIMB_6;
    t7  += t16 * GROUP_ORDER_SUB_LIMB_7;

    /* Do one final weak reduction */
    t1  += t0  >> 28;  t0  &= LOW_28_BITS_MASK;
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
    t15 &= LOW_26_BITS_MASK;

    /* By this point we have done a "relaxed reduction", i.e. s is in the range
     * [0, 2*L). Compute s-L and conditionally use it as a result if s is still
     * larger than or equal to L. Start by computing s+c, where c = 2^446 - L.
     * To limit stack usage, reuse the t16-t31 limbs. */
    t16 = GROUP_ORDER_SUB_LIMB_0 + t0;
    t17 = GROUP_ORDER_SUB_LIMB_1 + t1;
    t18 = GROUP_ORDER_SUB_LIMB_2 + t2;
    t19 = GROUP_ORDER_SUB_LIMB_3 + t3;
    t20 = GROUP_ORDER_SUB_LIMB_4 + t4;
    t21 = GROUP_ORDER_SUB_LIMB_5 + t5;
    t22 = GROUP_ORDER_SUB_LIMB_6 + t6;
    t23 = GROUP_ORDER_SUB_LIMB_7 + t7;
    t24 = t8;
    t25 = t9;
    t26 = t10;
    t27 = t11;
    t28 = t12;
    t29 = t13;
    t30 = t14;
    t31 = t15;

    /* Normalize the result, i.e. propagate the overflows upwards */
    t17 += t16 >> 28;  t16 &= LOW_28_BITS_MASK;
    t18 += t17 >> 28;  t17 &= LOW_28_BITS_MASK;
    t19 += t18 >> 28;  t18 &= LOW_28_BITS_MASK;
    t20 += t19 >> 28;  t19 &= LOW_28_BITS_MASK;
    t21 += t20 >> 28;  t20 &= LOW_28_BITS_MASK;
    t22 += t21 >> 28;  t21 &= LOW_28_BITS_MASK;
    t23 += t22 >> 28;  t22 &= LOW_28_BITS_MASK;
    t24 += t23 >> 28;  t23 &= LOW_28_BITS_MASK;
    t25 += t24 >> 28;  t24 &= LOW_28_BITS_MASK;
    t26 += t25 >> 28;  t25 &= LOW_28_BITS_MASK;
    t27 += t26 >> 28;  t26 &= LOW_28_BITS_MASK;
    t28 += t27 >> 28;  t27 &= LOW_28_BITS_MASK;
    t29 += t28 >> 28;  t28 &= LOW_28_BITS_MASK;
    t30 += t29 >> 28;  t29 &= LOW_28_BITS_MASK;
    t31 += t30 >> 28;  t30 &= LOW_28_BITS_MASK;
    /* At this point t31 contains the highest limb of s + c (extended to 64 bits).
     * Try subtracting 2^446. */
    t31 -= 1 << 26;
    /* Check if the subtraction resulted in an underflow and use the result to create
     * a mask for a conditional move. */
    u32 mask = (u32)( -(i32)( (t31 >> 63) ^ 1 ) );
    /* If the subtraction underflowed, then mask is set to all zeroes and we will
     * discard limbs t16-t31. Otherwise we will use them as the result. */
    t31 &= LOW_26_BITS_MASK;

    u32 x0  = t0  ^ t16;
    u32 x1  = t1  ^ t17;
    u32 x2  = t2  ^ t18;
    u32 x3  = t3  ^ t19;
    u32 x4  = t4  ^ t20;
    u32 x5  = t5  ^ t21;
    u32 x6  = t6  ^ t22;
    u32 x7  = t7  ^ t23;
    u32 x8  = t8  ^ t24;
    u32 x9  = t9  ^ t25;
    u32 x10 = t10 ^ t26;
    u32 x11 = t11 ^ t27;
    u32 x12 = t12 ^ t28;
    u32 x13 = t13 ^ t29;
    u32 x14 = t14 ^ t30;
    u32 x15 = t15 ^ t31;

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

    t0  ^= x0;
    t1  ^= x1;
    t2  ^= x2;
    t3  ^= x3;
    t4  ^= x4;
    t5  ^= x5;
    t6  ^= x6;
    t7  ^= x7;
    t8  ^= x8;
    t9  ^= x9;
    t10 ^= x10;
    t11 ^= x11;
    t12 ^= x12;
    t13 ^= x13;
    t14 ^= x14;
    t15 ^= x15;

    s[ 0] = ( t0 >>  0 );
    s[ 1] = ( t0 >>  8 );
    s[ 2] = ( t0 >> 16 );
    /* In the first limb we only have 4 bits left for byte number 3,
     * the high 4 bits we must take from the next limb */
    s[ 3] = ( t0 >> 24 ) | ( t1 << 4 );
    /* Low 4 bits from t1 we have already encoded into s[3] */
    s[ 4] = ( t1 >>  4 );
    s[ 5] = ( t1 >> 12 );
    s[ 6] = ( t1 >> 20 );
    /* At this point we have nicely fit the first two limbs into the
     * first seven bytes and the cycle shall repeat now */
    s[ 7] = ( t2  >>  0 );
    s[ 8] = ( t2  >>  8 );
    s[ 9] = ( t2  >> 16 );
    s[10] = ( t2  >> 24 ) | ( t3  << 4 );
    s[11] = ( t3  >>  4 );
    s[12] = ( t3  >> 12 );
    s[13] = ( t3  >> 20 );

    s[14] = ( t4  >>  0 );
    s[15] = ( t4  >>  8 );
    s[16] = ( t4  >> 16 );
    s[17] = ( t4  >> 24 ) | ( t5  << 4 );
    s[18] = ( t5  >>  4 );
    s[19] = ( t5  >> 12 );
    s[20] = ( t5  >> 20 );

    s[21] = ( t6  >>  0 );
    s[22] = ( t6  >>  8 );
    s[23] = ( t6  >> 16 );
    s[24] = ( t6  >> 24 ) | ( t7  << 4 );
    s[25] = ( t7  >>  4 );
    s[26] = ( t7  >> 12 );
    s[27] = ( t7  >> 20 );

    s[28] = ( t8  >>  0 );
    s[29] = ( t8  >>  8 );
    s[30] = ( t8  >> 16 );
    s[31] = ( t8  >> 24 ) | ( t9  << 4 );
    s[32] = ( t9  >>  4 );
    s[33] = ( t9  >> 12 );
    s[34] = ( t9  >> 20 );

    s[35] = ( t10 >>  0 );
    s[36] = ( t10 >>  8 );
    s[37] = ( t10 >> 16 );
    s[38] = ( t10 >> 24 ) | ( t11 << 4 );
    s[39] = ( t11 >>  4 );
    s[40] = ( t11 >> 12 );
    s[41] = ( t11 >> 20 );

    s[42] = ( t12 >>  0 );
    s[43] = ( t12 >>  8 );
    s[44] = ( t12 >> 16 );
    s[45] = ( t12 >> 24 ) | ( t13 << 4 );
    s[46] = ( t13 >>  4 );
    s[47] = ( t13 >> 12 );
    s[48] = ( t13 >> 20 );

    s[49] = ( t14 >>  0 );
    s[50] = ( t14 >>  8 );
    s[51] = ( t14 >> 16 );
    s[52] = ( t14 >> 24 ) | ( t15 << 4 );
    s[53] = ( t15 >>  4 );
    s[54] = ( t15 >> 12 );
    s[55] = ( t15 >> 20 );

    /* Set the last byte to 0 */
    s[56] = 0;
}

static void ed448_scalars_muladd(u8 * r, const u8 * a, const u8 * b, const u8 * c) {

    /* Ed448 multiplication+addition inputs must be 57 (56) bytes long. Use intermediate
     * 28-bit reduced-radix representation. */

    u64 a0  = ( load_32(&a[  0]) >> 0 ) & LOW_28_BITS_MASK;
    u64 a1  = ( load_32(&a[  3]) >> 4 ) & LOW_28_BITS_MASK;
    u64 a2  = ( load_32(&a[  7]) >> 0 ) & LOW_28_BITS_MASK;
    u64 a3  = ( load_32(&a[ 10]) >> 4 ) & LOW_28_BITS_MASK;
    u64 a4  = ( load_32(&a[ 14]) >> 0 ) & LOW_28_BITS_MASK;
    u64 a5  = ( load_32(&a[ 17]) >> 4 ) & LOW_28_BITS_MASK;
    u64 a6  = ( load_32(&a[ 21]) >> 0 ) & LOW_28_BITS_MASK;
    u64 a7  = ( load_32(&a[ 24]) >> 4 ) & LOW_28_BITS_MASK;
    u64 a8  = ( load_32(&a[ 28]) >> 0 ) & LOW_28_BITS_MASK;
    u64 a9  = ( load_32(&a[ 31]) >> 4 ) & LOW_28_BITS_MASK;
    u64 a10 = ( load_32(&a[ 35]) >> 0 ) & LOW_28_BITS_MASK;
    u64 a11 = ( load_32(&a[ 38]) >> 4 ) & LOW_28_BITS_MASK;
    u64 a12 = ( load_32(&a[ 42]) >> 0 ) & LOW_28_BITS_MASK;
    u64 a13 = ( load_32(&a[ 45]) >> 4 ) & LOW_28_BITS_MASK;
    u64 a14 = ( load_32(&a[ 49]) >> 0 ) & LOW_28_BITS_MASK;
    u64 a15 = ( load_32(&a[ 52]) >> 4 ) & LOW_28_BITS_MASK;

    u64 b0  = ( load_32(&b[  0]) >> 0 ) & LOW_28_BITS_MASK;
    u64 b1  = ( load_32(&b[  3]) >> 4 ) & LOW_28_BITS_MASK;
    u64 b2  = ( load_32(&b[  7]) >> 0 ) & LOW_28_BITS_MASK;
    u64 b3  = ( load_32(&b[ 10]) >> 4 ) & LOW_28_BITS_MASK;
    u64 b4  = ( load_32(&b[ 14]) >> 0 ) & LOW_28_BITS_MASK;
    u64 b5  = ( load_32(&b[ 17]) >> 4 ) & LOW_28_BITS_MASK;
    u64 b6  = ( load_32(&b[ 21]) >> 0 ) & LOW_28_BITS_MASK;
    u64 b7  = ( load_32(&b[ 24]) >> 4 ) & LOW_28_BITS_MASK;
    u64 b8  = ( load_32(&b[ 28]) >> 0 ) & LOW_28_BITS_MASK;
    u64 b9  = ( load_32(&b[ 31]) >> 4 ) & LOW_28_BITS_MASK;
    u64 b10 = ( load_32(&b[ 35]) >> 0 ) & LOW_28_BITS_MASK;
    u64 b11 = ( load_32(&b[ 38]) >> 4 ) & LOW_28_BITS_MASK;
    u64 b12 = ( load_32(&b[ 42]) >> 0 ) & LOW_28_BITS_MASK;
    u64 b13 = ( load_32(&b[ 45]) >> 4 ) & LOW_28_BITS_MASK;
    u64 b14 = ( load_32(&b[ 49]) >> 0 ) & LOW_28_BITS_MASK;
    u64 b15 = ( load_32(&b[ 52]) >> 4 ) & LOW_28_BITS_MASK;

    u64 c0  = ( load_32(&c[  0]) >> 0 ) & LOW_28_BITS_MASK;
    u64 c1  = ( load_32(&c[  3]) >> 4 ) & LOW_28_BITS_MASK;
    u64 c2  = ( load_32(&c[  7]) >> 0 ) & LOW_28_BITS_MASK;
    u64 c3  = ( load_32(&c[ 10]) >> 4 ) & LOW_28_BITS_MASK;
    u64 c4  = ( load_32(&c[ 14]) >> 0 ) & LOW_28_BITS_MASK;
    u64 c5  = ( load_32(&c[ 17]) >> 4 ) & LOW_28_BITS_MASK;
    u64 c6  = ( load_32(&c[ 21]) >> 0 ) & LOW_28_BITS_MASK;
    u64 c7  = ( load_32(&c[ 24]) >> 4 ) & LOW_28_BITS_MASK;
    u64 c8  = ( load_32(&c[ 28]) >> 0 ) & LOW_28_BITS_MASK;
    u64 c9  = ( load_32(&c[ 31]) >> 4 ) & LOW_28_BITS_MASK;
    u64 c10 = ( load_32(&c[ 35]) >> 0 ) & LOW_28_BITS_MASK;
    u64 c11 = ( load_32(&c[ 38]) >> 4 ) & LOW_28_BITS_MASK;
    u64 c12 = ( load_32(&c[ 42]) >> 0 ) & LOW_28_BITS_MASK;
    u64 c13 = ( load_32(&c[ 45]) >> 4 ) & LOW_28_BITS_MASK;
    u64 c14 = ( load_32(&c[ 49]) >> 0 ) & LOW_28_BITS_MASK;
    u64 c15 = ( load_32(&c[ 52]) >> 4 ) & LOW_28_BITS_MASK;

    /* Do the naive schoolbook multiplication and offset the first 16 limbs
     * of the result by the limbs of c */
    u64 t0  = c0  + a0*b0;
    u64 t1  = c1  + a0*b1  + a1*b0;
    u64 t2  = c2  + a0*b2  + a1*b1  + a2*b0;
    u64 t3  = c3  + a0*b3  + a1*b2  + a2*b1 + a3*b0;
    u64 t4  = c4  + a0*b4  + a1*b3  + a2*b2 + a3*b1  + a4*b0;
    u64 t5  = c5  + a0*b5  + a1*b4  + a2*b3 + a3*b2  + a4*b1  + a5*b0;
    u64 t6  = c6  + a0*b6  + a1*b5  + a2*b4 + a3*b3  + a4*b2  + a5*b1  + a6*b0;
    u64 t7  = c7  + a0*b7  + a1*b6  + a2*b5 + a3*b4  + a4*b3  + a5*b2  + a6*b1  + a7*b0;
    u64 t8  = c8  + a0*b8  + a1*b7  + a2*b6 + a3*b5  + a4*b4  + a5*b3  + a6*b2  + a7*b1  + a8*b0;
    u64 t9  = c9  + a0*b9  + a1*b8  + a2*b7 + a3*b6  + a4*b5  + a5*b4  + a6*b3  + a7*b2  + a8*b1  + a9*b0;
    u64 t10 = c10 + a0*b10 + a1*b9  + a2*b8 + a3*b7  + a4*b6  + a5*b5  + a6*b4  + a7*b3  + a8*b2  + a9*b1  + a10*b0;
    u64 t11 = c11 + a0*b11 + a1*b10 + a2*b9 + a3*b8  + a4*b7  + a5*b6  + a6*b5  + a7*b4  + a8*b3  + a9*b2  + a10*b1  + a11*b0;
    u64 t12 = c12 + a0*b12 + a1*b11 + a2*b10+ a3*b9  + a4*b8  + a5*b7  + a6*b6  + a7*b5  + a8*b4  + a9*b3  + a10*b2  + a11*b1  + a12*b0;
    u64 t13 = c13 + a0*b13 + a1*b12 + a2*b11+ a3*b10 + a4*b9  + a5*b8  + a6*b7  + a7*b6  + a8*b5  + a9*b4  + a10*b3  + a11*b2  + a12*b1  + a13*b0;
    u64 t14 = c14 + a0*b14 + a1*b13 + a2*b12+ a3*b11 + a4*b10 + a5*b9  + a6*b8  + a7*b7  + a8*b6  + a9*b5  + a10*b4  + a11*b3  + a12*b2  + a13*b1  + a14*b0;
    u64 t15 = c15 + a0*b15 + a1*b14 + a2*b13+ a3*b12 + a4*b11 + a5*b10 + a6*b9  + a7*b8  + a8*b7  + a9*b6  + a10*b5  + a11*b4  + a12*b3  + a13*b2  + a14*b1  + a15*b0;
    u64 t16 =                a1*b15 + a2*b14+ a3*b13 + a4*b12 + a5*b11 + a6*b10 + a7*b9  + a8*b8  + a9*b7  + a10*b6  + a11*b5  + a12*b4  + a13*b3  + a14*b2  + a15*b1;
    u64 t17 =                         a2*b15+ a3*b14 + a4*b13 + a5*b12 + a6*b11 + a7*b10 + a8*b9  + a9*b8  + a10*b7  + a11*b6  + a12*b5  + a13*b4  + a14*b3  + a15*b2;
    u64 t18 =                                 a3*b15 + a4*b14 + a5*b13 + a6*b12 + a7*b11 + a8*b10 + a9*b9  + a10*b8  + a11*b7  + a12*b6  + a13*b5  + a14*b4  + a15*b3;
    u64 t19 =                                          a4*b15 + a5*b14 + a6*b13 + a7*b12 + a8*b11 + a9*b10 + a10*b9  + a11*b8  + a12*b7  + a13*b6  + a14*b5  + a15*b4;
    u64 t20 =                                                   a5*b15 + a6*b14 + a7*b13 + a8*b12 + a9*b11 + a10*b10 + a11*b9  + a12*b8  + a13*b7  + a14*b6  + a15*b5;
    u64 t21 =                                                            a6*b15 + a7*b14 + a8*b13 + a9*b12 + a10*b11 + a11*b10 + a12*b9  + a13*b8  + a14*b7  + a15*b6;
    u64 t22 =                                                                     a7*b15 + a8*b14 + a9*b13 + a10*b12 + a11*b11 + a12*b10 + a13*b9  + a14*b8  + a15*b7;
    u64 t23 =                                                                              a8*b15 + a9*b14 + a10*b13 + a11*b12 + a12*b11 + a13*b10 + a14*b9  + a15*b8;
    u64 t24 =                                                                                       a9*b15 + a10*b14 + a11*b13 + a12*b12 + a13*b11 + a14*b10 + a15*b9;
    u64 t25 =                                                                                                a10*b15 + a11*b14 + a12*b13 + a13*b12 + a14*b11 + a15*b10;
    u64 t26 =                                                                                                          a11*b15 + a12*b14 + a13*b13 + a14*b12 + a15*b11;
    u64 t27 =                                                                                                                    a12*b15 + a13*b14 + a14*b13 + a15*b12;
    u64 t28 =                                                                                                                              a13*b15 + a14*b14 + a15*b13;
    u64 t29 =                                                                                                                                        a14*b15 + a15*b14;
    u64 t30 =                                                                                                                                                  a15*b15;
    u64 t31;

    /* Normalize the limbs to identify the overflow */
    t1  += t0  >> 28;  t0  &= LOW_28_BITS_MASK;
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
    t16 += t15 >> 28;  t15 &= LOW_28_BITS_MASK;
    t17 += t16 >> 28;  t16 &= LOW_28_BITS_MASK;
    t18 += t17 >> 28;  t17 &= LOW_28_BITS_MASK;
    t19 += t18 >> 28;  t18 &= LOW_28_BITS_MASK;
    t20 += t19 >> 28;  t19 &= LOW_28_BITS_MASK;
    t21 += t20 >> 28;  t20 &= LOW_28_BITS_MASK;
    t22 += t21 >> 28;  t21 &= LOW_28_BITS_MASK;
    t23 += t22 >> 28;  t22 &= LOW_28_BITS_MASK;
    t24 += t23 >> 28;  t23 &= LOW_28_BITS_MASK;
    t25 += t24 >> 28;  t24 &= LOW_28_BITS_MASK;
    t26 += t25 >> 28;  t25 &= LOW_28_BITS_MASK;
    t27 += t26 >> 28;  t26 &= LOW_28_BITS_MASK;
    t28 += t27 >> 28;  t27 &= LOW_28_BITS_MASK;
    t29 += t28 >> 28;  t28 &= LOW_28_BITS_MASK;
    t30 += t29 >> 28;  t29 &= LOW_28_BITS_MASK;
    /* Use t31 to store the overflow of t30 */
    t31  = t30 >> 28;  t30 &= LOW_28_BITS_MASK;

    /* Bring down the limbs that exceed 2^446 according to the identity:
     *
     *   x + y 2^446 = x + (2^446 - u) y + u y = x + u y (mod L)
     *
     * where u = 2^446 - L. See ed448_scalar_reduce for a more detailed
     * description, in particular the reasoning behind the multiplication
     * by four (due to how the scalars were parsed and handled).
     */

    t8  += t24 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t9  += t24 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t10 += t24 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t11 += t24 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t12 += t24 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t13 += t24 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t14 += t24 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t15 += t24 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t9  += t25 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t10 += t25 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t11 += t25 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t12 += t25 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t13 += t25 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t14 += t25 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t15 += t25 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t16 += t25 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t10 += t26 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t11 += t26 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t12 += t26 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t13 += t26 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t14 += t26 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t15 += t26 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t16 += t26 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t17 += t26 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t11 += t27 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t12 += t27 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t13 += t27 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t14 += t27 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t15 += t27 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t16 += t27 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t17 += t27 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t18 += t27 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t12 += t28 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t13 += t28 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t14 += t28 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t15 += t28 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t16 += t28 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t17 += t28 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t18 += t28 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t19 += t28 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t13 += t29 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t14 += t29 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t15 += t29 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t16 += t29 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t17 += t29 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t18 += t29 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t19 += t29 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t20 += t29 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t14 += t30 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t15 += t30 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t16 += t30 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t17 += t30 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t18 += t30 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t19 += t30 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t20 += t30 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t21 += t30 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t15 += t31 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t16 += t31 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t17 += t31 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t18 += t31 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t19 += t31 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t20 += t31 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t21 += t31 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t22 += t31 * 4 * GROUP_ORDER_SUB_LIMB_7;

    /* Do a partial reduction of the limbs we have been working with above
     * to ensure no overflows occur */
    t9  += t8  >> 28;  t8  &= LOW_28_BITS_MASK;
    t10 += t9  >> 28;  t9  &= LOW_28_BITS_MASK;
    t11 += t10 >> 28;  t10 &= LOW_28_BITS_MASK;
    t12 += t11 >> 28;  t11 &= LOW_28_BITS_MASK;
    t13 += t12 >> 28;  t12 &= LOW_28_BITS_MASK;
    t14 += t13 >> 28;  t13 &= LOW_28_BITS_MASK;
    t15 += t14 >> 28;  t14 &= LOW_28_BITS_MASK;
    t16 += t15 >> 28;  t15 &= LOW_28_BITS_MASK;
    t17 += t16 >> 28;  t16 &= LOW_28_BITS_MASK;
    t18 += t17 >> 28;  t17 &= LOW_28_BITS_MASK;
    t19 += t18 >> 28;  t18 &= LOW_28_BITS_MASK;
    t20 += t19 >> 28;  t19 &= LOW_28_BITS_MASK;
    t21 += t20 >> 28;  t20 &= LOW_28_BITS_MASK;
    t22 += t21 >> 28;  t21 &= LOW_28_BITS_MASK;
    t23 += t22 >> 28;  t22 &= LOW_28_BITS_MASK;
    /* Use t24 to store the overflow of t23 */
    t24  = t23 >> 28;  t23 &= LOW_28_BITS_MASK;

    t0  += t16 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t1  += t16 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t2  += t16 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t3  += t16 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t4  += t16 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t5  += t16 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t6  += t16 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t7  += t16 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t1  += t17 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t2  += t17 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t3  += t17 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t4  += t17 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t5  += t17 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t6  += t17 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t7  += t17 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t8  += t17 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t2  += t18 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t3  += t18 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t4  += t18 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t5  += t18 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t6  += t18 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t7  += t18 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t8  += t18 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t9  += t18 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t3  += t19 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t4  += t19 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t5  += t19 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t6  += t19 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t7  += t19 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t8  += t19 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t9  += t19 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t10 += t19 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t4  += t20 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t5  += t20 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t6  += t20 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t7  += t20 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t8  += t20 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t9  += t20 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t10 += t20 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t11 += t20 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t5  += t21 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t6  += t21 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t7  += t21 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t8  += t21 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t9  += t21 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t10 += t21 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t11 += t21 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t12 += t21 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t6  += t22 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t7  += t22 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t8  += t22 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t9  += t22 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t10 += t22 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t11 += t22 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t12 += t22 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t13 += t22 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t7  += t23 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t8  += t23 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t9  += t23 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t10 += t23 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t11 += t23 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t12 += t23 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t13 += t23 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t14 += t23 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t8  += t24 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t9  += t24 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t10 += t24 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t11 += t24 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t12 += t24 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t13 += t24 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t14 += t24 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t15 += t24 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t1  += t0  >> 28;  t0  &= LOW_28_BITS_MASK;
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
    /* Use t16 to store the overflow of t15 */
    t16  = t15 >> 28;  t15 &= LOW_28_BITS_MASK;

    /* Bring down whatever has bubbled up into t16 */
    t0  += t16 * 4 * GROUP_ORDER_SUB_LIMB_0;
    t1  += t16 * 4 * GROUP_ORDER_SUB_LIMB_1;
    t2  += t16 * 4 * GROUP_ORDER_SUB_LIMB_2;
    t3  += t16 * 4 * GROUP_ORDER_SUB_LIMB_3;
    t4  += t16 * 4 * GROUP_ORDER_SUB_LIMB_4;
    t5  += t16 * 4 * GROUP_ORDER_SUB_LIMB_5;
    t6  += t16 * 4 * GROUP_ORDER_SUB_LIMB_6;
    t7  += t16 * 4 * GROUP_ORDER_SUB_LIMB_7;

    t1  += t0  >> 28;  t0  &= LOW_28_BITS_MASK;
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
    /* Use t16 to store the overflow of t15. Note that at this point
     * we also set t15 to its canonical form which is 26-bits wide.  */
    t16  = t15 >> 26;  t15 &= LOW_26_BITS_MASK;

    /* Note that the "shift by two" has already been accounted for */
    t0  += t16 * GROUP_ORDER_SUB_LIMB_0;
    t1  += t16 * GROUP_ORDER_SUB_LIMB_1;
    t2  += t16 * GROUP_ORDER_SUB_LIMB_2;
    t3  += t16 * GROUP_ORDER_SUB_LIMB_3;
    t4  += t16 * GROUP_ORDER_SUB_LIMB_4;
    t5  += t16 * GROUP_ORDER_SUB_LIMB_5;
    t6  += t16 * GROUP_ORDER_SUB_LIMB_6;
    t7  += t16 * GROUP_ORDER_SUB_LIMB_7;

    /* Do one final weak reduction */
    t1  += t0  >> 28;  t0  &= LOW_28_BITS_MASK;
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
    t15 &= LOW_26_BITS_MASK;

    /* By this point we have done a "relaxed reduction", i.e. s is in the range
     * [0, 2*L). Compute s-L and conditionally use it as a result if s is still
     * larger than or equal to L. Start by computing s+c, where c = 2^446 - L.
     * To limit stack usage, reuse the t16-t31 limbs. */
    t16 = t0 + GROUP_ORDER_SUB_LIMB_0;
    t17 = t1 + GROUP_ORDER_SUB_LIMB_1;
    t18 = t2 + GROUP_ORDER_SUB_LIMB_2;
    t19 = t3 + GROUP_ORDER_SUB_LIMB_3;
    t20 = t4 + GROUP_ORDER_SUB_LIMB_4;
    t21 = t5 + GROUP_ORDER_SUB_LIMB_5;
    t22 = t6 + GROUP_ORDER_SUB_LIMB_6;
    t23 = t7 + GROUP_ORDER_SUB_LIMB_7;
    t24 = t8;
    t25 = t9;
    t26 = t10;
    t27 = t11;
    t28 = t12;
    t29 = t13;
    t30 = t14;
    t31 = t15;

    /* Normalize the result, i.e. propagate the overflows upwards */
    t17 += t16 >> 28;  t16 &= LOW_28_BITS_MASK;
    t18 += t17 >> 28;  t17 &= LOW_28_BITS_MASK;
    t19 += t18 >> 28;  t18 &= LOW_28_BITS_MASK;
    t20 += t19 >> 28;  t19 &= LOW_28_BITS_MASK;
    t21 += t20 >> 28;  t20 &= LOW_28_BITS_MASK;
    t22 += t21 >> 28;  t21 &= LOW_28_BITS_MASK;
    t23 += t22 >> 28;  t22 &= LOW_28_BITS_MASK;
    t24 += t23 >> 28;  t23 &= LOW_28_BITS_MASK;
    t25 += t24 >> 28;  t24 &= LOW_28_BITS_MASK;
    t26 += t25 >> 28;  t25 &= LOW_28_BITS_MASK;
    t27 += t26 >> 28;  t26 &= LOW_28_BITS_MASK;
    t28 += t27 >> 28;  t27 &= LOW_28_BITS_MASK;
    t29 += t28 >> 28;  t28 &= LOW_28_BITS_MASK;
    t30 += t29 >> 28;  t29 &= LOW_28_BITS_MASK;
    t31 += t30 >> 28;  t30 &= LOW_28_BITS_MASK;
    /* At this point t31 contains the highest limb of s + c (extended to 64 bits).
     * Try subtracting 2^446. */
    t31 -= 1 << 26;

    /* Check if the subtraction resulted in an underflow and use the result to create
     * a mask for a conditional move. */
    u32 mask = (u32)( -(i32)( (t31 >> 63) ^ 1 ) );
    /* If the subtraction underflowed, then mask is set to all zeroes and we will
     * discard limbs t16-t31. Otherwise we will use them as the result. */
    t31 &= LOW_26_BITS_MASK;

    u32 x0  = t0  ^ t16;
    u32 x1  = t1  ^ t17;
    u32 x2  = t2  ^ t18;
    u32 x3  = t3  ^ t19;
    u32 x4  = t4  ^ t20;
    u32 x5  = t5  ^ t21;
    u32 x6  = t6  ^ t22;
    u32 x7  = t7  ^ t23;
    u32 x8  = t8  ^ t24;
    u32 x9  = t9  ^ t25;
    u32 x10 = t10 ^ t26;
    u32 x11 = t11 ^ t27;
    u32 x12 = t12 ^ t28;
    u32 x13 = t13 ^ t29;
    u32 x14 = t14 ^ t30;
    u32 x15 = t15 ^ t31;

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

    t0  ^= x0;
    t1  ^= x1;
    t2  ^= x2;
    t3  ^= x3;
    t4  ^= x4;
    t5  ^= x5;
    t6  ^= x6;
    t7  ^= x7;
    t8  ^= x8;
    t9  ^= x9;
    t10 ^= x10;
    t11 ^= x11;
    t12 ^= x12;
    t13 ^= x13;
    t14 ^= x14;
    t15 ^= x15;

    r[ 0] = ( t0 >>  0 );
    r[ 1] = ( t0 >>  8 );
    r[ 2] = ( t0 >> 16 );
    /* In the first limb we only have 4 bits left for byte number 3,
     * the high 4 bits we must take from the next limb */
    r[ 3] = ( t0 >> 24 ) | ( t1 << 4 );
    /* Low 4 bits from t1 we have already encoded into r[3] */
    r[ 4] = ( t1 >>  4 );
    r[ 5] = ( t1 >> 12 );
    r[ 6] = ( t1 >> 20 );
    /* At this point we have nicely fit the first two limbs into the
     * first seven bytes and the cycle shall repeat now */
    r[ 7] = ( t2  >>  0 );
    r[ 8] = ( t2  >>  8 );
    r[ 9] = ( t2  >> 16 );
    r[10] = ( t2  >> 24 ) | ( t3  << 4 );
    r[11] = ( t3  >>  4 );
    r[12] = ( t3  >> 12 );
    r[13] = ( t3  >> 20 );

    r[14] = ( t4  >>  0 );
    r[15] = ( t4  >>  8 );
    r[16] = ( t4  >> 16 );
    r[17] = ( t4  >> 24 ) | ( t5  << 4 );
    r[18] = ( t5  >>  4 );
    r[19] = ( t5  >> 12 );
    r[20] = ( t5  >> 20 );

    r[21] = ( t6  >>  0 );
    r[22] = ( t6  >>  8 );
    r[23] = ( t6  >> 16 );
    r[24] = ( t6  >> 24 ) | ( t7  << 4 );
    r[25] = ( t7  >>  4 );
    r[26] = ( t7  >> 12 );
    r[27] = ( t7  >> 20 );

    r[28] = ( t8  >>  0 );
    r[29] = ( t8  >>  8 );
    r[30] = ( t8  >> 16 );
    r[31] = ( t8  >> 24 ) | ( t9  << 4 );
    r[32] = ( t9  >>  4 );
    r[33] = ( t9  >> 12 );
    r[34] = ( t9  >> 20 );

    r[35] = ( t10 >>  0 );
    r[36] = ( t10 >>  8 );
    r[37] = ( t10 >> 16 );
    r[38] = ( t10 >> 24 ) | ( t11 << 4 );
    r[39] = ( t11 >>  4 );
    r[40] = ( t11 >> 12 );
    r[41] = ( t11 >> 20 );

    r[42] = ( t12 >>  0 );
    r[43] = ( t12 >>  8 );
    r[44] = ( t12 >> 16 );
    r[45] = ( t12 >> 24 ) | ( t13 << 4 );
    r[46] = ( t13 >>  4 );
    r[47] = ( t13 >> 12 );
    r[48] = ( t13 >> 20 );

    r[49] = ( t14 >>  0 );
    r[50] = ( t14 >>  8 );
    r[51] = ( t14 >> 16 );
    r[52] = ( t14 >> 24 ) | ( t15 << 4 );
    r[53] = ( t15 >>  4 );
    r[54] = ( t15 >> 12 );
    r[55] = ( t15 >> 20 );

    /* Set the last byte to 0 */
    r[56] = 0;
}

scalar_ops ed448_scalar_ops = {
    .is_canonical = ed448_scalar_is_canonical,
    .reduce = ed448_scalar_reduce,
    .muladd = ed448_scalars_muladd
};
