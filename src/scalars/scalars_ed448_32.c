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

    u64 t[33];
    u32 x[16];
    u32 mask;

    /* Parse the scalar into limbs */
    t[ 0] = ( load_32(&s[  0]) >> 0 ) & LOW_28_BITS_MASK;
    t[ 1] = ( load_32(&s[  3]) >> 4 ) & LOW_28_BITS_MASK;
    t[ 2] = ( load_32(&s[  7]) >> 0 ) & LOW_28_BITS_MASK;
    t[ 3] = ( load_32(&s[ 10]) >> 4 ) & LOW_28_BITS_MASK;
    t[ 4] = ( load_32(&s[ 14]) >> 0 ) & LOW_28_BITS_MASK;
    t[ 5] = ( load_32(&s[ 17]) >> 4 ) & LOW_28_BITS_MASK;
    t[ 6] = ( load_32(&s[ 21]) >> 0 ) & LOW_28_BITS_MASK;
    t[ 7] = ( load_32(&s[ 24]) >> 4 ) & LOW_28_BITS_MASK;
    t[ 8] = ( load_32(&s[ 28]) >> 0 ) & LOW_28_BITS_MASK;
    t[ 9] = ( load_32(&s[ 31]) >> 4 ) & LOW_28_BITS_MASK;
    t[10] = ( load_32(&s[ 35]) >> 0 ) & LOW_28_BITS_MASK;
    t[11] = ( load_32(&s[ 38]) >> 4 ) & LOW_28_BITS_MASK;
    t[12] = ( load_32(&s[ 42]) >> 0 ) & LOW_28_BITS_MASK;
    t[13] = ( load_32(&s[ 45]) >> 4 ) & LOW_28_BITS_MASK;
    t[14] = ( load_32(&s[ 49]) >> 0 ) & LOW_28_BITS_MASK;
    t[15] = ( load_32(&s[ 52]) >> 4 ) & LOW_28_BITS_MASK;
    t[16] = ( load_32(&s[ 56]) >> 0 ) & LOW_28_BITS_MASK;
    t[17] = ( load_32(&s[ 59]) >> 4 ) & LOW_28_BITS_MASK;
    t[18] = ( load_32(&s[ 63]) >> 0 ) & LOW_28_BITS_MASK;
    t[19] = ( load_32(&s[ 66]) >> 4 ) & LOW_28_BITS_MASK;
    t[20] = ( load_32(&s[ 70]) >> 0 ) & LOW_28_BITS_MASK;
    t[21] = ( load_32(&s[ 73]) >> 4 ) & LOW_28_BITS_MASK;
    t[22] = ( load_32(&s[ 77]) >> 0 ) & LOW_28_BITS_MASK;
    t[23] = ( load_32(&s[ 80]) >> 4 ) & LOW_28_BITS_MASK;
    t[24] = ( load_32(&s[ 84]) >> 0 ) & LOW_28_BITS_MASK;
    t[25] = ( load_32(&s[ 87]) >> 4 ) & LOW_28_BITS_MASK;
    t[26] = ( load_32(&s[ 91]) >> 0 ) & LOW_28_BITS_MASK;
    t[27] = ( load_32(&s[ 94]) >> 4 ) & LOW_28_BITS_MASK;
    t[28] = ( load_32(&s[ 98]) >> 0 ) & LOW_28_BITS_MASK;
    t[29] = ( load_32(&s[101]) >> 4 ) & LOW_28_BITS_MASK;
    t[30] = ( load_32(&s[105]) >> 0 ) & LOW_28_BITS_MASK;
    t[31] = ( load_32(&s[108]) >> 4 ) & LOW_28_BITS_MASK;
    /* Load the trailing 16 bits into t32 */
    t[32] = ( (u64) s[113] << 8 ) | s[112];

    /* Let L = 2^446 - c denote the group order. We shall split s into x + y 2^446
     * and use the identity:
     *
     *           x + y 2^446 = x + (2^446 - c) y + c y = x + c y (mod L)
     *
     * Note that the 2^446 boundary is in the middle of limb t15. Top two bits
     * of t15 are already above 2^446. For this reason limbs t16-t32 should actually
     * have larger weights. To compensate this we multiply them by four as we go.
     */

    t[ 8] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 9] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[10] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[11] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[12] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[13] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[14] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[15] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[ 9] += t[25] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[10] += t[25] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[11] += t[25] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[12] += t[25] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[13] += t[25] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[14] += t[25] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[15] += t[25] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[16] += t[25] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[10] += t[26] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[11] += t[26] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[12] += t[26] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[13] += t[26] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[14] += t[26] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[15] += t[26] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[16] += t[26] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[17] += t[26] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[11] += t[27] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[12] += t[27] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[13] += t[27] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[14] += t[27] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[15] += t[27] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[16] += t[27] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[17] += t[27] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[18] += t[27] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[12] += t[28] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[13] += t[28] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[14] += t[28] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[15] += t[28] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[16] += t[28] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[17] += t[28] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[18] += t[28] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[19] += t[28] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[13] += t[29] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[14] += t[29] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[15] += t[29] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[16] += t[29] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[17] += t[29] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[18] += t[29] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[19] += t[29] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[20] += t[29] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[14] += t[30] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[15] += t[30] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[16] += t[30] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[17] += t[30] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[18] += t[30] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[19] += t[30] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[20] += t[30] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[21] += t[30] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[15] += t[31] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[16] += t[31] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[17] += t[31] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[18] += t[31] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[19] += t[31] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[20] += t[31] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[21] += t[31] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[22] += t[31] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[16] += t[32] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[17] += t[32] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[18] += t[32] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[19] += t[32] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[20] += t[32] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[21] += t[32] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[22] += t[32] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[23] += t[32] * 4 * GROUP_ORDER_SUB_LIMB_7;

    /* Do a partial reduction of the limbs we have been working with above
     * to ensure no overflows occur */
    t[ 9] += t[ 8] >> 28;  t[ 8] &= LOW_28_BITS_MASK;
    t[10] += t[ 9] >> 28;  t[ 9] &= LOW_28_BITS_MASK;
    t[11] += t[10] >> 28;  t[10] &= LOW_28_BITS_MASK;
    t[12] += t[11] >> 28;  t[11] &= LOW_28_BITS_MASK;
    t[13] += t[12] >> 28;  t[12] &= LOW_28_BITS_MASK;
    t[14] += t[13] >> 28;  t[13] &= LOW_28_BITS_MASK;
    t[15] += t[14] >> 28;  t[14] &= LOW_28_BITS_MASK;
    t[16] += t[15] >> 28;  t[15] &= LOW_28_BITS_MASK;
    t[17] += t[16] >> 28;  t[16] &= LOW_28_BITS_MASK;
    t[18] += t[17] >> 28;  t[17] &= LOW_28_BITS_MASK;
    t[19] += t[18] >> 28;  t[18] &= LOW_28_BITS_MASK;
    t[20] += t[19] >> 28;  t[19] &= LOW_28_BITS_MASK;
    t[21] += t[20] >> 28;  t[20] &= LOW_28_BITS_MASK;
    t[22] += t[21] >> 28;  t[21] &= LOW_28_BITS_MASK;
    t[23] += t[22] >> 28;  t[22] &= LOW_28_BITS_MASK;
    /* Use t24 to store the overflow of t23 */
    t[24]  = t[23] >> 28;  t[23] &= LOW_28_BITS_MASK;

    t[ 0] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 1] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 2] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 3] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[ 4] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[ 5] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[ 6] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[ 7] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[ 1] += t[17] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 2] += t[17] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 3] += t[17] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 4] += t[17] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[ 5] += t[17] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[ 6] += t[17] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[ 7] += t[17] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[ 8] += t[17] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[ 2] += t[18] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 3] += t[18] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 4] += t[18] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 5] += t[18] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[ 6] += t[18] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[ 7] += t[18] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[ 8] += t[18] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[ 9] += t[18] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[ 3] += t[19] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 4] += t[19] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 5] += t[19] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 6] += t[19] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[ 7] += t[19] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[ 8] += t[19] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[ 9] += t[19] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[10] += t[19] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[ 4] += t[20] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 5] += t[20] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 6] += t[20] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 7] += t[20] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[ 8] += t[20] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[ 9] += t[20] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[10] += t[20] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[11] += t[20] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[ 5] += t[21] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 6] += t[21] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 7] += t[21] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 8] += t[21] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[ 9] += t[21] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[10] += t[21] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[11] += t[21] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[12] += t[21] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[ 6] += t[22] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 7] += t[22] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 8] += t[22] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 9] += t[22] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[10] += t[22] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[11] += t[22] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[12] += t[22] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[13] += t[22] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[ 7] += t[23] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 8] += t[23] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 9] += t[23] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[10] += t[23] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[11] += t[23] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[12] += t[23] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[13] += t[23] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[14] += t[23] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[ 8] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 9] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[10] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[11] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[12] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[13] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[14] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[15] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[ 1] += t[ 0] >> 28;  t[ 0] &= LOW_28_BITS_MASK;
    t[ 2] += t[ 1] >> 28;  t[ 1] &= LOW_28_BITS_MASK;
    t[ 3] += t[ 2] >> 28;  t[ 2] &= LOW_28_BITS_MASK;
    t[ 4] += t[ 3] >> 28;  t[ 3] &= LOW_28_BITS_MASK;
    t[ 5] += t[ 4] >> 28;  t[ 4] &= LOW_28_BITS_MASK;
    t[ 6] += t[ 5] >> 28;  t[ 5] &= LOW_28_BITS_MASK;
    t[ 7] += t[ 6] >> 28;  t[ 6] &= LOW_28_BITS_MASK;
    t[ 8] += t[ 7] >> 28;  t[ 7] &= LOW_28_BITS_MASK;
    t[ 9] += t[ 8] >> 28;  t[ 8] &= LOW_28_BITS_MASK;
    t[10] += t[ 9] >> 28;  t[ 9] &= LOW_28_BITS_MASK;
    t[11] += t[10] >> 28;  t[10] &= LOW_28_BITS_MASK;
    t[12] += t[11] >> 28;  t[11] &= LOW_28_BITS_MASK;
    t[13] += t[12] >> 28;  t[12] &= LOW_28_BITS_MASK;
    t[14] += t[13] >> 28;  t[13] &= LOW_28_BITS_MASK;
    t[15] += t[14] >> 28;  t[14] &= LOW_28_BITS_MASK;
    /* Use t16 to store the overflow of t15 */
    t[16]  = t[15] >> 28;  t[15] &= LOW_28_BITS_MASK;

    /* Bring down whatever has bubbled up into t16 */
    t[ 0] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 1] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 2] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 3] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[ 4] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[ 5] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[ 6] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[ 7] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[ 1] += t[ 0] >> 28;  t[ 0] &= LOW_28_BITS_MASK;
    t[ 2] += t[ 1] >> 28;  t[ 1] &= LOW_28_BITS_MASK;
    t[ 3] += t[ 2] >> 28;  t[ 2] &= LOW_28_BITS_MASK;
    t[ 4] += t[ 3] >> 28;  t[ 3] &= LOW_28_BITS_MASK;
    t[ 5] += t[ 4] >> 28;  t[ 4] &= LOW_28_BITS_MASK;
    t[ 6] += t[ 5] >> 28;  t[ 5] &= LOW_28_BITS_MASK;
    t[ 7] += t[ 6] >> 28;  t[ 6] &= LOW_28_BITS_MASK;
    t[ 8] += t[ 7] >> 28;  t[ 7] &= LOW_28_BITS_MASK;
    t[ 9] += t[ 8] >> 28;  t[ 8] &= LOW_28_BITS_MASK;
    t[10] += t[ 9] >> 28;  t[ 9] &= LOW_28_BITS_MASK;
    t[11] += t[10] >> 28;  t[10] &= LOW_28_BITS_MASK;
    t[12] += t[11] >> 28;  t[11] &= LOW_28_BITS_MASK;
    t[13] += t[12] >> 28;  t[12] &= LOW_28_BITS_MASK;
    t[14] += t[13] >> 28;  t[13] &= LOW_28_BITS_MASK;
    t[15] += t[14] >> 28;  t[14] &= LOW_28_BITS_MASK;
    /* Use t16 to store the overflow of t15. Note that at this point
     * we also set t15 to its canonical form which is 26-bits wide.  */
    t[16]  = t[15] >> 26;  t[15] &= LOW_26_BITS_MASK;

    /* Note that the "shift by two" has already been accounted for */
    t[ 0] += t[16] * GROUP_ORDER_SUB_LIMB_0;
    t[ 1] += t[16] * GROUP_ORDER_SUB_LIMB_1;
    t[ 2] += t[16] * GROUP_ORDER_SUB_LIMB_2;
    t[ 3] += t[16] * GROUP_ORDER_SUB_LIMB_3;
    t[ 4] += t[16] * GROUP_ORDER_SUB_LIMB_4;
    t[ 5] += t[16] * GROUP_ORDER_SUB_LIMB_5;
    t[ 6] += t[16] * GROUP_ORDER_SUB_LIMB_6;
    t[ 7] += t[16] * GROUP_ORDER_SUB_LIMB_7;

    /* Do one final weak reduction */
    t[ 1] += t[ 0] >> 28;  t[ 0] &= LOW_28_BITS_MASK;
    t[ 2] += t[ 1] >> 28;  t[ 1] &= LOW_28_BITS_MASK;
    t[ 3] += t[ 2] >> 28;  t[ 2] &= LOW_28_BITS_MASK;
    t[ 4] += t[ 3] >> 28;  t[ 3] &= LOW_28_BITS_MASK;
    t[ 5] += t[ 4] >> 28;  t[ 4] &= LOW_28_BITS_MASK;
    t[ 6] += t[ 5] >> 28;  t[ 5] &= LOW_28_BITS_MASK;
    t[ 7] += t[ 6] >> 28;  t[ 6] &= LOW_28_BITS_MASK;
    t[ 8] += t[ 7] >> 28;  t[ 7] &= LOW_28_BITS_MASK;
    t[ 9] += t[ 8] >> 28;  t[ 8] &= LOW_28_BITS_MASK;
    t[10] += t[ 9] >> 28;  t[ 9] &= LOW_28_BITS_MASK;
    t[11] += t[10] >> 28;  t[10] &= LOW_28_BITS_MASK;
    t[12] += t[11] >> 28;  t[11] &= LOW_28_BITS_MASK;
    t[13] += t[12] >> 28;  t[12] &= LOW_28_BITS_MASK;
    t[14] += t[13] >> 28;  t[13] &= LOW_28_BITS_MASK;
    t[15] += t[14] >> 28;  t[14] &= LOW_28_BITS_MASK;
    t[15] &= LOW_26_BITS_MASK;

    /* By this point we have done a "relaxed reduction", i.e. s is in the range
     * [0, 2*L). Compute s-L and conditionally use it as a result if s is still
     * larger than or equal to L. Start by computing s+c, where c = 2^446 - L.
     * To limit stack usage, reuse the t16-t31 limbs. */
    t[16] = GROUP_ORDER_SUB_LIMB_0 + t[ 0];
    t[17] = GROUP_ORDER_SUB_LIMB_1 + t[ 1];
    t[18] = GROUP_ORDER_SUB_LIMB_2 + t[ 2];
    t[19] = GROUP_ORDER_SUB_LIMB_3 + t[ 3];
    t[20] = GROUP_ORDER_SUB_LIMB_4 + t[ 4];
    t[21] = GROUP_ORDER_SUB_LIMB_5 + t[ 5];
    t[22] = GROUP_ORDER_SUB_LIMB_6 + t[ 6];
    t[23] = GROUP_ORDER_SUB_LIMB_7 + t[ 7];
    t[24] = t[ 8];
    t[25] = t[ 9];
    t[26] = t[10];
    t[27] = t[11];
    t[28] = t[12];
    t[29] = t[13];
    t[30] = t[14];
    t[31] = t[15];

    /* Normalize the result, i.e. propagate the overflows upwards */
    t[17] += t[16] >> 28;  t[16] &= LOW_28_BITS_MASK;
    t[18] += t[17] >> 28;  t[17] &= LOW_28_BITS_MASK;
    t[19] += t[18] >> 28;  t[18] &= LOW_28_BITS_MASK;
    t[20] += t[19] >> 28;  t[19] &= LOW_28_BITS_MASK;
    t[21] += t[20] >> 28;  t[20] &= LOW_28_BITS_MASK;
    t[22] += t[21] >> 28;  t[21] &= LOW_28_BITS_MASK;
    t[23] += t[22] >> 28;  t[22] &= LOW_28_BITS_MASK;
    t[24] += t[23] >> 28;  t[23] &= LOW_28_BITS_MASK;
    t[25] += t[24] >> 28;  t[24] &= LOW_28_BITS_MASK;
    t[26] += t[25] >> 28;  t[25] &= LOW_28_BITS_MASK;
    t[27] += t[26] >> 28;  t[26] &= LOW_28_BITS_MASK;
    t[28] += t[27] >> 28;  t[27] &= LOW_28_BITS_MASK;
    t[29] += t[28] >> 28;  t[28] &= LOW_28_BITS_MASK;
    t[30] += t[29] >> 28;  t[29] &= LOW_28_BITS_MASK;
    t[31] += t[30] >> 28;  t[30] &= LOW_28_BITS_MASK;
    /* At this point t31 contains the highest limb of s + c (extended to 64 bits).
     * Try subtracting 2^446. */
    t[31] -= 1 << 26;
    /* Check if the subtraction resulted in an underflow and use the result to create
     * a mask for a conditional move. */
    mask = (u32)( -(i32)( (t[31] >> 63) ^ 1 ) );
    /* If the subtraction underflowed, then mask is set to all zeroes and we will
     * discard limbs t16-t31. Otherwise we will use them as the result. */
    t[31] &= LOW_26_BITS_MASK;

    x[ 0] = t[ 0] ^ t[16];
    x[ 1] = t[ 1] ^ t[17];
    x[ 2] = t[ 2] ^ t[18];
    x[ 3] = t[ 3] ^ t[19];
    x[ 4] = t[ 4] ^ t[20];
    x[ 5] = t[ 5] ^ t[21];
    x[ 6] = t[ 6] ^ t[22];
    x[ 7] = t[ 7] ^ t[23];
    x[ 8] = t[ 8] ^ t[24];
    x[ 9] = t[ 9] ^ t[25];
    x[10] = t[10] ^ t[26];
    x[11] = t[11] ^ t[27];
    x[12] = t[12] ^ t[28];
    x[13] = t[13] ^ t[29];
    x[14] = t[14] ^ t[30];
    x[15] = t[15] ^ t[31];

    x[ 0] &= mask;
    x[ 1] &= mask;
    x[ 2] &= mask;
    x[ 3] &= mask;
    x[ 4] &= mask;
    x[ 5] &= mask;
    x[ 6] &= mask;
    x[ 7] &= mask;
    x[ 8] &= mask;
    x[ 9] &= mask;
    x[10] &= mask;
    x[11] &= mask;
    x[12] &= mask;
    x[13] &= mask;
    x[14] &= mask;
    x[15] &= mask;

    t[ 0] ^= x[ 0];
    t[ 1] ^= x[ 1];
    t[ 2] ^= x[ 2];
    t[ 3] ^= x[ 3];
    t[ 4] ^= x[ 4];
    t[ 5] ^= x[ 5];
    t[ 6] ^= x[ 6];
    t[ 7] ^= x[ 7];
    t[ 8] ^= x[ 8];
    t[ 9] ^= x[ 9];
    t[10] ^= x[10];
    t[11] ^= x[11];
    t[12] ^= x[12];
    t[13] ^= x[13];
    t[14] ^= x[14];
    t[15] ^= x[15];

    s[ 0] = ( t[ 0] >>  0 );
    s[ 1] = ( t[ 0] >>  8 );
    s[ 2] = ( t[ 0] >> 16 );
    /* In the first limb we only have 4 bits left for byte number 3,
     * the high 4 bits we must take from the next limb */
    s[ 3] = ( t[ 0] >> 24 ) | ( t[ 1] << 4 );
    /* Low 4 bits from t1 we have already encoded into s[3] */
    s[ 4] = ( t[ 1] >>  4 );
    s[ 5] = ( t[ 1] >> 12 );
    s[ 6] = ( t[ 1] >> 20 );
    /* At this point we have nicely fit the first two limbs into the
     * first seven bytes and the cycle shall repeat now */
    s[ 7] = ( t[ 2] >>  0 );
    s[ 8] = ( t[ 2] >>  8 );
    s[ 9] = ( t[ 2] >> 16 );
    s[10] = ( t[ 2] >> 24 ) | ( t[ 3] << 4 );
    s[11] = ( t[ 3] >>  4 );
    s[12] = ( t[ 3] >> 12 );
    s[13] = ( t[ 3] >> 20 );

    s[14] = ( t[ 4] >>  0 );
    s[15] = ( t[ 4] >>  8 );
    s[16] = ( t[ 4] >> 16 );
    s[17] = ( t[ 4] >> 24 ) | ( t[ 5] << 4 );
    s[18] = ( t[ 5] >>  4 );
    s[19] = ( t[ 5] >> 12 );
    s[20] = ( t[ 5] >> 20 );

    s[21] = ( t[ 6] >>  0 );
    s[22] = ( t[ 6] >>  8 );
    s[23] = ( t[ 6] >> 16 );
    s[24] = ( t[ 6] >> 24 ) | ( t[ 7] << 4 );
    s[25] = ( t[ 7] >>  4 );
    s[26] = ( t[ 7] >> 12 );
    s[27] = ( t[ 7] >> 20 );

    s[28] = ( t[ 8] >>  0 );
    s[29] = ( t[ 8] >>  8 );
    s[30] = ( t[ 8] >> 16 );
    s[31] = ( t[ 8] >> 24 ) | ( t[ 9] << 4 );
    s[32] = ( t[ 9] >>  4 );
    s[33] = ( t[ 9] >> 12 );
    s[34] = ( t[ 9] >> 20 );

    s[35] = ( t[10] >>  0 );
    s[36] = ( t[10] >>  8 );
    s[37] = ( t[10] >> 16 );
    s[38] = ( t[10] >> 24 ) | ( t[11] << 4 );
    s[39] = ( t[11] >>  4 );
    s[40] = ( t[11] >> 12 );
    s[41] = ( t[11] >> 20 );

    s[42] = ( t[12] >>  0 );
    s[43] = ( t[12] >>  8 );
    s[44] = ( t[12] >> 16 );
    s[45] = ( t[12] >> 24 ) | ( t[13] << 4 );
    s[46] = ( t[13] >>  4 );
    s[47] = ( t[13] >> 12 );
    s[48] = ( t[13] >> 20 );

    s[49] = ( t[14] >>  0 );
    s[50] = ( t[14] >>  8 );
    s[51] = ( t[14] >> 16 );
    s[52] = ( t[14] >> 24 ) | ( t[15] << 4 );
    s[53] = ( t[15] >>  4 );
    s[54] = ( t[15] >> 12 );
    s[55] = ( t[15] >> 20 );

    /* Set the last byte to 0 */
    s[56] = 0;

    purge_secrets(t, sizeof(t));
    purge_secrets(x, sizeof(x));
    purge_secrets(&mask, sizeof(mask));
}

static void ed448_scalars_muladd(u8 * r, const u8 * aa, const u8 * bb, const u8 * cc) {

    /* Ed448 multiplication+addition inputs must be 57 (56) bytes long. Use intermediate
     * 28-bit reduced-radix representation. */

    u64 a[16];
    u64 b[16];
    u64 c[16];
    u64 t[32];
    u32 x[16];
    u32 mask;

    a[ 0] = ( load_32(&aa[  0]) >> 0 ) & LOW_28_BITS_MASK;
    a[ 1] = ( load_32(&aa[  3]) >> 4 ) & LOW_28_BITS_MASK;
    a[ 2] = ( load_32(&aa[  7]) >> 0 ) & LOW_28_BITS_MASK;
    a[ 3] = ( load_32(&aa[ 10]) >> 4 ) & LOW_28_BITS_MASK;
    a[ 4] = ( load_32(&aa[ 14]) >> 0 ) & LOW_28_BITS_MASK;
    a[ 5] = ( load_32(&aa[ 17]) >> 4 ) & LOW_28_BITS_MASK;
    a[ 6] = ( load_32(&aa[ 21]) >> 0 ) & LOW_28_BITS_MASK;
    a[ 7] = ( load_32(&aa[ 24]) >> 4 ) & LOW_28_BITS_MASK;
    a[ 8] = ( load_32(&aa[ 28]) >> 0 ) & LOW_28_BITS_MASK;
    a[ 9] = ( load_32(&aa[ 31]) >> 4 ) & LOW_28_BITS_MASK;
    a[10] = ( load_32(&aa[ 35]) >> 0 ) & LOW_28_BITS_MASK;
    a[11] = ( load_32(&aa[ 38]) >> 4 ) & LOW_28_BITS_MASK;
    a[12] = ( load_32(&aa[ 42]) >> 0 ) & LOW_28_BITS_MASK;
    a[13] = ( load_32(&aa[ 45]) >> 4 ) & LOW_28_BITS_MASK;
    a[14] = ( load_32(&aa[ 49]) >> 0 ) & LOW_28_BITS_MASK;
    a[15] = ( load_32(&aa[ 52]) >> 4 ) & LOW_28_BITS_MASK;

    b[ 0] = ( load_32(&bb[  0]) >> 0 ) & LOW_28_BITS_MASK;
    b[ 1] = ( load_32(&bb[  3]) >> 4 ) & LOW_28_BITS_MASK;
    b[ 2] = ( load_32(&bb[  7]) >> 0 ) & LOW_28_BITS_MASK;
    b[ 3] = ( load_32(&bb[ 10]) >> 4 ) & LOW_28_BITS_MASK;
    b[ 4] = ( load_32(&bb[ 14]) >> 0 ) & LOW_28_BITS_MASK;
    b[ 5] = ( load_32(&bb[ 17]) >> 4 ) & LOW_28_BITS_MASK;
    b[ 6] = ( load_32(&bb[ 21]) >> 0 ) & LOW_28_BITS_MASK;
    b[ 7] = ( load_32(&bb[ 24]) >> 4 ) & LOW_28_BITS_MASK;
    b[ 8] = ( load_32(&bb[ 28]) >> 0 ) & LOW_28_BITS_MASK;
    b[ 9] = ( load_32(&bb[ 31]) >> 4 ) & LOW_28_BITS_MASK;
    b[10] = ( load_32(&bb[ 35]) >> 0 ) & LOW_28_BITS_MASK;
    b[11] = ( load_32(&bb[ 38]) >> 4 ) & LOW_28_BITS_MASK;
    b[12] = ( load_32(&bb[ 42]) >> 0 ) & LOW_28_BITS_MASK;
    b[13] = ( load_32(&bb[ 45]) >> 4 ) & LOW_28_BITS_MASK;
    b[14] = ( load_32(&bb[ 49]) >> 0 ) & LOW_28_BITS_MASK;
    b[15] = ( load_32(&bb[ 52]) >> 4 ) & LOW_28_BITS_MASK;

    c[ 0] = ( load_32(&cc[  0]) >> 0 ) & LOW_28_BITS_MASK;
    c[ 1] = ( load_32(&cc[  3]) >> 4 ) & LOW_28_BITS_MASK;
    c[ 2] = ( load_32(&cc[  7]) >> 0 ) & LOW_28_BITS_MASK;
    c[ 3] = ( load_32(&cc[ 10]) >> 4 ) & LOW_28_BITS_MASK;
    c[ 4] = ( load_32(&cc[ 14]) >> 0 ) & LOW_28_BITS_MASK;
    c[ 5] = ( load_32(&cc[ 17]) >> 4 ) & LOW_28_BITS_MASK;
    c[ 6] = ( load_32(&cc[ 21]) >> 0 ) & LOW_28_BITS_MASK;
    c[ 7] = ( load_32(&cc[ 24]) >> 4 ) & LOW_28_BITS_MASK;
    c[ 8] = ( load_32(&cc[ 28]) >> 0 ) & LOW_28_BITS_MASK;
    c[ 9] = ( load_32(&cc[ 31]) >> 4 ) & LOW_28_BITS_MASK;
    c[10] = ( load_32(&cc[ 35]) >> 0 ) & LOW_28_BITS_MASK;
    c[11] = ( load_32(&cc[ 38]) >> 4 ) & LOW_28_BITS_MASK;
    c[12] = ( load_32(&cc[ 42]) >> 0 ) & LOW_28_BITS_MASK;
    c[13] = ( load_32(&cc[ 45]) >> 4 ) & LOW_28_BITS_MASK;
    c[14] = ( load_32(&cc[ 49]) >> 0 ) & LOW_28_BITS_MASK;
    c[15] = ( load_32(&cc[ 52]) >> 4 ) & LOW_28_BITS_MASK;

    /* Do the naive schoolbook multiplication and offset the first 16 limbs
     * of the result by the limbs of c */
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
    t[12] = c[12] + a[0]*b[12] + a[1]*b[11] + a[2]*b[10] + a[3]*b[ 9] + a[4]*b[ 8] + a[5]*b[ 7] + a[6]*b[ 6] + a[7]*b[ 5] + a[8]*b[ 4] + a[9]*b[ 3] + a[10]*b[ 2] + a[11]*b[ 1] + a[12]*b[ 0];
    t[13] = c[13] + a[0]*b[13] + a[1]*b[12] + a[2]*b[11] + a[3]*b[10] + a[4]*b[ 9] + a[5]*b[ 8] + a[6]*b[ 7] + a[7]*b[ 6] + a[8]*b[ 5] + a[9]*b[ 4] + a[10]*b[ 3] + a[11]*b[ 2] + a[12]*b[ 1] + a[13]*b[ 0];
    t[14] = c[14] + a[0]*b[14] + a[1]*b[13] + a[2]*b[12] + a[3]*b[11] + a[4]*b[10] + a[5]*b[ 9] + a[6]*b[ 8] + a[7]*b[ 7] + a[8]*b[ 6] + a[9]*b[ 5] + a[10]*b[ 4] + a[11]*b[ 3] + a[12]*b[ 2] + a[13]*b[ 1] + a[14]*b[ 0];
    t[15] = c[15] + a[0]*b[15] + a[1]*b[14] + a[2]*b[13] + a[3]*b[12] + a[4]*b[11] + a[5]*b[10] + a[6]*b[ 9] + a[7]*b[ 8] + a[8]*b[ 7] + a[9]*b[ 6] + a[10]*b[ 5] + a[11]*b[ 4] + a[12]*b[ 3] + a[13]*b[ 2] + a[14]*b[ 1] + a[15]*b[ 0];
    t[16] =                      a[1]*b[15] + a[2]*b[14] + a[3]*b[13] + a[4]*b[12] + a[5]*b[11] + a[6]*b[10] + a[7]*b[ 9] + a[8]*b[ 8] + a[9]*b[ 7] + a[10]*b[ 6] + a[11]*b[ 5] + a[12]*b[ 4] + a[13]*b[ 3] + a[14]*b[ 2] + a[15]*b[ 1];
    t[17] =                                   a[2]*b[15] + a[3]*b[14] + a[4]*b[13] + a[5]*b[12] + a[6]*b[11] + a[7]*b[10] + a[8]*b[ 9] + a[9]*b[ 8] + a[10]*b[ 7] + a[11]*b[ 6] + a[12]*b[ 5] + a[13]*b[ 4] + a[14]*b[ 3] + a[15]*b[ 2];
    t[18] =                                                a[3]*b[15] + a[4]*b[14] + a[5]*b[13] + a[6]*b[12] + a[7]*b[11] + a[8]*b[10] + a[9]*b[ 9] + a[10]*b[ 8] + a[11]*b[ 7] + a[12]*b[ 6] + a[13]*b[ 5] + a[14]*b[ 4] + a[15]*b[ 3];
    t[19] =                                                             a[4]*b[15] + a[5]*b[14] + a[6]*b[13] + a[7]*b[12] + a[8]*b[11] + a[9]*b[10] + a[10]*b[ 9] + a[11]*b[ 8] + a[12]*b[ 7] + a[13]*b[ 6] + a[14]*b[ 5] + a[15]*b[ 4];
    t[20] =                                                                          a[5]*b[15] + a[6]*b[14] + a[7]*b[13] + a[8]*b[12] + a[9]*b[11] + a[10]*b[10] + a[11]*b[ 9] + a[12]*b[ 8] + a[13]*b[ 7] + a[14]*b[ 6] + a[15]*b[ 5];
    t[21] =                                                                                       a[6]*b[15] + a[7]*b[14] + a[8]*b[13] + a[9]*b[12] + a[10]*b[11] + a[11]*b[10] + a[12]*b[ 9] + a[13]*b[ 8] + a[14]*b[ 7] + a[15]*b[ 6];
    t[22] =                                                                                                    a[7]*b[15] + a[8]*b[14] + a[9]*b[13] + a[10]*b[12] + a[11]*b[11] + a[12]*b[10] + a[13]*b[ 9] + a[14]*b[ 8] + a[15]*b[ 7];
    t[23] =                                                                                                                 a[8]*b[15] + a[9]*b[14] + a[10]*b[13] + a[11]*b[12] + a[12]*b[11] + a[13]*b[10] + a[14]*b[ 9] + a[15]*b[ 8];
    t[24] =                                                                                                                              a[9]*b[15] + a[10]*b[14] + a[11]*b[13] + a[12]*b[12] + a[13]*b[11] + a[14]*b[10] + a[15]*b[ 9];
    t[25] =                                                                                                                                           a[10]*b[15] + a[11]*b[14] + a[12]*b[13] + a[13]*b[12] + a[14]*b[11] + a[15]*b[10];
    t[26] =                                                                                                                                                         a[11]*b[15] + a[12]*b[14] + a[13]*b[13] + a[14]*b[12] + a[15]*b[11];
    t[27] =                                                                                                                                                                       a[12]*b[15] + a[13]*b[14] + a[14]*b[13] + a[15]*b[12];
    t[28] =                                                                                                                                                                                     a[13]*b[15] + a[14]*b[14] + a[15]*b[13];
    t[29] =                                                                                                                                                                                                   a[14]*b[15] + a[15]*b[14];
    t[30] =                                                                                                                                                                                                                 a[15]*b[15];

    /* Normalize the limbs to identify the overflow */
    t[ 1] += t[ 0] >> 28;  t[ 0] &= LOW_28_BITS_MASK;
    t[ 2] += t[ 1] >> 28;  t[ 1] &= LOW_28_BITS_MASK;
    t[ 3] += t[ 2] >> 28;  t[ 2] &= LOW_28_BITS_MASK;
    t[ 4] += t[ 3] >> 28;  t[ 3] &= LOW_28_BITS_MASK;
    t[ 5] += t[ 4] >> 28;  t[ 4] &= LOW_28_BITS_MASK;
    t[ 6] += t[ 5] >> 28;  t[ 5] &= LOW_28_BITS_MASK;
    t[ 7] += t[ 6] >> 28;  t[ 6] &= LOW_28_BITS_MASK;
    t[ 8] += t[ 7] >> 28;  t[ 7] &= LOW_28_BITS_MASK;
    t[ 9] += t[ 8] >> 28;  t[ 8] &= LOW_28_BITS_MASK;
    t[10] += t[ 9] >> 28;  t[ 9] &= LOW_28_BITS_MASK;
    t[11] += t[10] >> 28;  t[10] &= LOW_28_BITS_MASK;
    t[12] += t[11] >> 28;  t[11] &= LOW_28_BITS_MASK;
    t[13] += t[12] >> 28;  t[12] &= LOW_28_BITS_MASK;
    t[14] += t[13] >> 28;  t[13] &= LOW_28_BITS_MASK;
    t[15] += t[14] >> 28;  t[14] &= LOW_28_BITS_MASK;
    t[16] += t[15] >> 28;  t[15] &= LOW_28_BITS_MASK;
    t[17] += t[16] >> 28;  t[16] &= LOW_28_BITS_MASK;
    t[18] += t[17] >> 28;  t[17] &= LOW_28_BITS_MASK;
    t[19] += t[18] >> 28;  t[18] &= LOW_28_BITS_MASK;
    t[20] += t[19] >> 28;  t[19] &= LOW_28_BITS_MASK;
    t[21] += t[20] >> 28;  t[20] &= LOW_28_BITS_MASK;
    t[22] += t[21] >> 28;  t[21] &= LOW_28_BITS_MASK;
    t[23] += t[22] >> 28;  t[22] &= LOW_28_BITS_MASK;
    t[24] += t[23] >> 28;  t[23] &= LOW_28_BITS_MASK;
    t[25] += t[24] >> 28;  t[24] &= LOW_28_BITS_MASK;
    t[26] += t[25] >> 28;  t[25] &= LOW_28_BITS_MASK;
    t[27] += t[26] >> 28;  t[26] &= LOW_28_BITS_MASK;
    t[28] += t[27] >> 28;  t[27] &= LOW_28_BITS_MASK;
    t[29] += t[28] >> 28;  t[28] &= LOW_28_BITS_MASK;
    t[30] += t[29] >> 28;  t[29] &= LOW_28_BITS_MASK;
    /* Use t31 to store the overflow of t30 */
    t[31]  = t[30] >> 28;  t[30] &= LOW_28_BITS_MASK;

    /* Bring down the limbs that exceed 2^446 according to the identity:
     *
     *   x + y 2^446 = x + (2^446 - u) y + u y = x + u y (mod L)
     *
     * where u = 2^446 - L. See ed448_scalar_reduce for a more detailed
     * description, in particular the reasoning behind the multiplication
     * by four (due to how the scalars were parsed and handled).
     */

    t[ 8] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 9] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[10] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[11] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[12] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[13] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[14] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[15] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[ 9] += t[25] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[10] += t[25] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[11] += t[25] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[12] += t[25] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[13] += t[25] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[14] += t[25] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[15] += t[25] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[16] += t[25] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[10] += t[26] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[11] += t[26] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[12] += t[26] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[13] += t[26] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[14] += t[26] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[15] += t[26] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[16] += t[26] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[17] += t[26] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[11] += t[27] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[12] += t[27] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[13] += t[27] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[14] += t[27] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[15] += t[27] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[16] += t[27] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[17] += t[27] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[18] += t[27] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[12] += t[28] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[13] += t[28] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[14] += t[28] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[15] += t[28] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[16] += t[28] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[17] += t[28] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[18] += t[28] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[19] += t[28] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[13] += t[29] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[14] += t[29] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[15] += t[29] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[16] += t[29] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[17] += t[29] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[18] += t[29] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[19] += t[29] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[20] += t[29] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[14] += t[30] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[15] += t[30] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[16] += t[30] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[17] += t[30] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[18] += t[30] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[19] += t[30] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[20] += t[30] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[21] += t[30] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[15] += t[31] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[16] += t[31] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[17] += t[31] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[18] += t[31] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[19] += t[31] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[20] += t[31] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[21] += t[31] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[22] += t[31] * 4 * GROUP_ORDER_SUB_LIMB_7;

    /* Do a partial reduction of the limbs we have been working with above
     * to ensure no overflows occur */
    t[ 9] += t[ 8] >> 28;  t[ 8] &= LOW_28_BITS_MASK;
    t[10] += t[ 9] >> 28;  t[ 9] &= LOW_28_BITS_MASK;
    t[11] += t[10] >> 28;  t[10] &= LOW_28_BITS_MASK;
    t[12] += t[11] >> 28;  t[11] &= LOW_28_BITS_MASK;
    t[13] += t[12] >> 28;  t[12] &= LOW_28_BITS_MASK;
    t[14] += t[13] >> 28;  t[13] &= LOW_28_BITS_MASK;
    t[15] += t[14] >> 28;  t[14] &= LOW_28_BITS_MASK;
    t[16] += t[15] >> 28;  t[15] &= LOW_28_BITS_MASK;
    t[17] += t[16] >> 28;  t[16] &= LOW_28_BITS_MASK;
    t[18] += t[17] >> 28;  t[17] &= LOW_28_BITS_MASK;
    t[19] += t[18] >> 28;  t[18] &= LOW_28_BITS_MASK;
    t[20] += t[19] >> 28;  t[19] &= LOW_28_BITS_MASK;
    t[21] += t[20] >> 28;  t[20] &= LOW_28_BITS_MASK;
    t[22] += t[21] >> 28;  t[21] &= LOW_28_BITS_MASK;
    t[23] += t[22] >> 28;  t[22] &= LOW_28_BITS_MASK;
    /* Use t24 to store the overflow of t23 */
    t[24]  = t[23] >> 28;  t[23] &= LOW_28_BITS_MASK;

    t[ 0] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 1] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 2] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 3] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[ 4] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[ 5] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[ 6] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[ 7] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[ 1] += t[17] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 2] += t[17] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 3] += t[17] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 4] += t[17] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[ 5] += t[17] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[ 6] += t[17] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[ 7] += t[17] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[ 8] += t[17] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[ 2] += t[18] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 3] += t[18] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 4] += t[18] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 5] += t[18] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[ 6] += t[18] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[ 7] += t[18] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[ 8] += t[18] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[ 9] += t[18] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[ 3] += t[19] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 4] += t[19] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 5] += t[19] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 6] += t[19] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[ 7] += t[19] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[ 8] += t[19] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[ 9] += t[19] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[10] += t[19] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[ 4] += t[20] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 5] += t[20] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 6] += t[20] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 7] += t[20] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[ 8] += t[20] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[ 9] += t[20] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[10] += t[20] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[11] += t[20] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[ 5] += t[21] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 6] += t[21] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 7] += t[21] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 8] += t[21] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[ 9] += t[21] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[10] += t[21] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[11] += t[21] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[12] += t[21] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[ 6] += t[22] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 7] += t[22] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 8] += t[22] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 9] += t[22] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[10] += t[22] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[11] += t[22] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[12] += t[22] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[13] += t[22] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[ 7] += t[23] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 8] += t[23] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 9] += t[23] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[10] += t[23] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[11] += t[23] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[12] += t[23] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[13] += t[23] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[14] += t[23] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[ 8] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 9] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[10] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[11] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[12] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[13] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[14] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[15] += t[24] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[ 1] += t[ 0] >> 28;  t[ 0] &= LOW_28_BITS_MASK;
    t[ 2] += t[ 1] >> 28;  t[ 1] &= LOW_28_BITS_MASK;
    t[ 3] += t[ 2] >> 28;  t[ 2] &= LOW_28_BITS_MASK;
    t[ 4] += t[ 3] >> 28;  t[ 3] &= LOW_28_BITS_MASK;
    t[ 5] += t[ 4] >> 28;  t[ 4] &= LOW_28_BITS_MASK;
    t[ 6] += t[ 5] >> 28;  t[ 5] &= LOW_28_BITS_MASK;
    t[ 7] += t[ 6] >> 28;  t[ 6] &= LOW_28_BITS_MASK;
    t[ 8] += t[ 7] >> 28;  t[ 7] &= LOW_28_BITS_MASK;
    t[ 9] += t[ 8] >> 28;  t[ 8] &= LOW_28_BITS_MASK;
    t[10] += t[ 9] >> 28;  t[ 9] &= LOW_28_BITS_MASK;
    t[11] += t[10] >> 28;  t[10] &= LOW_28_BITS_MASK;
    t[12] += t[11] >> 28;  t[11] &= LOW_28_BITS_MASK;
    t[13] += t[12] >> 28;  t[12] &= LOW_28_BITS_MASK;
    t[14] += t[13] >> 28;  t[13] &= LOW_28_BITS_MASK;
    t[15] += t[14] >> 28;  t[14] &= LOW_28_BITS_MASK;
    /* Use t16 to store the overflow of t15 */
    t[16]  = t[15] >> 28;  t[15] &= LOW_28_BITS_MASK;

    /* Bring down whatever has bubbled up into t16 */
    t[ 0] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 1] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 2] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 3] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_3;
    t[ 4] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_4;
    t[ 5] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_5;
    t[ 6] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_6;
    t[ 7] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_7;

    t[ 1] += t[ 0] >> 28;  t[ 0] &= LOW_28_BITS_MASK;
    t[ 2] += t[ 1] >> 28;  t[ 1] &= LOW_28_BITS_MASK;
    t[ 3] += t[ 2] >> 28;  t[ 2] &= LOW_28_BITS_MASK;
    t[ 4] += t[ 3] >> 28;  t[ 3] &= LOW_28_BITS_MASK;
    t[ 5] += t[ 4] >> 28;  t[ 4] &= LOW_28_BITS_MASK;
    t[ 6] += t[ 5] >> 28;  t[ 5] &= LOW_28_BITS_MASK;
    t[ 7] += t[ 6] >> 28;  t[ 6] &= LOW_28_BITS_MASK;
    t[ 8] += t[ 7] >> 28;  t[ 7] &= LOW_28_BITS_MASK;
    t[ 9] += t[ 8] >> 28;  t[ 8] &= LOW_28_BITS_MASK;
    t[10] += t[ 9] >> 28;  t[ 9] &= LOW_28_BITS_MASK;
    t[11] += t[10] >> 28;  t[10] &= LOW_28_BITS_MASK;
    t[12] += t[11] >> 28;  t[11] &= LOW_28_BITS_MASK;
    t[13] += t[12] >> 28;  t[12] &= LOW_28_BITS_MASK;
    t[14] += t[13] >> 28;  t[13] &= LOW_28_BITS_MASK;
    t[15] += t[14] >> 28;  t[14] &= LOW_28_BITS_MASK;
    /* Use t16 to store the overflow of t15. Note that at this point
     * we also set t15 to its canonical form which is 26-bits wide.  */
    t[16]  = t[15] >> 26;  t[15] &= LOW_26_BITS_MASK;

    /* Note that the "shift by two" has already been accounted for */
    t[ 0] += t[16] * GROUP_ORDER_SUB_LIMB_0;
    t[ 1] += t[16] * GROUP_ORDER_SUB_LIMB_1;
    t[ 2] += t[16] * GROUP_ORDER_SUB_LIMB_2;
    t[ 3] += t[16] * GROUP_ORDER_SUB_LIMB_3;
    t[ 4] += t[16] * GROUP_ORDER_SUB_LIMB_4;
    t[ 5] += t[16] * GROUP_ORDER_SUB_LIMB_5;
    t[ 6] += t[16] * GROUP_ORDER_SUB_LIMB_6;
    t[ 7] += t[16] * GROUP_ORDER_SUB_LIMB_7;

    /* Do one final weak reduction */
    t[ 1] += t[ 0] >> 28;  t[ 0] &= LOW_28_BITS_MASK;
    t[ 2] += t[ 1] >> 28;  t[ 1] &= LOW_28_BITS_MASK;
    t[ 3] += t[ 2] >> 28;  t[ 2] &= LOW_28_BITS_MASK;
    t[ 4] += t[ 3] >> 28;  t[ 3] &= LOW_28_BITS_MASK;
    t[ 5] += t[ 4] >> 28;  t[ 4] &= LOW_28_BITS_MASK;
    t[ 6] += t[ 5] >> 28;  t[ 5] &= LOW_28_BITS_MASK;
    t[ 7] += t[ 6] >> 28;  t[ 6] &= LOW_28_BITS_MASK;
    t[ 8] += t[ 7] >> 28;  t[ 7] &= LOW_28_BITS_MASK;
    t[ 9] += t[ 8] >> 28;  t[ 8] &= LOW_28_BITS_MASK;
    t[10] += t[ 9] >> 28;  t[ 9] &= LOW_28_BITS_MASK;
    t[11] += t[10] >> 28;  t[10] &= LOW_28_BITS_MASK;
    t[12] += t[11] >> 28;  t[11] &= LOW_28_BITS_MASK;
    t[13] += t[12] >> 28;  t[12] &= LOW_28_BITS_MASK;
    t[14] += t[13] >> 28;  t[13] &= LOW_28_BITS_MASK;
    t[15] += t[14] >> 28;  t[14] &= LOW_28_BITS_MASK;
    t[15] &= LOW_26_BITS_MASK;

    /* By this point we have done a "relaxed reduction", i.e. s is in the range
     * [0, 2*L). Compute s-L and conditionally use it as a result if s is still
     * larger than or equal to L. Start by computing s+c, where c = 2^446 - L.
     * To limit stack usage, reuse the t16-t31 limbs. */
    t[16] = t[ 0] + GROUP_ORDER_SUB_LIMB_0;
    t[17] = t[ 1] + GROUP_ORDER_SUB_LIMB_1;
    t[18] = t[ 2] + GROUP_ORDER_SUB_LIMB_2;
    t[19] = t[ 3] + GROUP_ORDER_SUB_LIMB_3;
    t[20] = t[ 4] + GROUP_ORDER_SUB_LIMB_4;
    t[21] = t[ 5] + GROUP_ORDER_SUB_LIMB_5;
    t[22] = t[ 6] + GROUP_ORDER_SUB_LIMB_6;
    t[23] = t[ 7] + GROUP_ORDER_SUB_LIMB_7;
    t[24] = t[ 8];
    t[25] = t[ 9];
    t[26] = t[10];
    t[27] = t[11];
    t[28] = t[12];
    t[29] = t[13];
    t[30] = t[14];
    t[31] = t[15];

    /* Normalize the result, i.e. propagate the overflows upwards */
    t[17] += t[16] >> 28;  t[16] &= LOW_28_BITS_MASK;
    t[18] += t[17] >> 28;  t[17] &= LOW_28_BITS_MASK;
    t[19] += t[18] >> 28;  t[18] &= LOW_28_BITS_MASK;
    t[20] += t[19] >> 28;  t[19] &= LOW_28_BITS_MASK;
    t[21] += t[20] >> 28;  t[20] &= LOW_28_BITS_MASK;
    t[22] += t[21] >> 28;  t[21] &= LOW_28_BITS_MASK;
    t[23] += t[22] >> 28;  t[22] &= LOW_28_BITS_MASK;
    t[24] += t[23] >> 28;  t[23] &= LOW_28_BITS_MASK;
    t[25] += t[24] >> 28;  t[24] &= LOW_28_BITS_MASK;
    t[26] += t[25] >> 28;  t[25] &= LOW_28_BITS_MASK;
    t[27] += t[26] >> 28;  t[26] &= LOW_28_BITS_MASK;
    t[28] += t[27] >> 28;  t[27] &= LOW_28_BITS_MASK;
    t[29] += t[28] >> 28;  t[28] &= LOW_28_BITS_MASK;
    t[30] += t[29] >> 28;  t[29] &= LOW_28_BITS_MASK;
    t[31] += t[30] >> 28;  t[30] &= LOW_28_BITS_MASK;
    /* At this point t31 contains the highest limb of s + c (extended to 64 bits).
     * Try subtracting 2^446. */
    t[31] -= 1 << 26;

    /* Check if the subtraction resulted in an underflow and use the result to create
     * a mask for a conditional move. */
    mask = (u32)( -(i32)( (t[31] >> 63) ^ 1 ) );
    /* If the subtraction underflowed, then mask is set to all zeroes and we will
     * discard limbs t16-t31. Otherwise we will use them as the result. */
    t[31] &= LOW_26_BITS_MASK;

    x[ 0] = t[ 0] ^ t[16];
    x[ 1] = t[ 1] ^ t[17];
    x[ 2] = t[ 2] ^ t[18];
    x[ 3] = t[ 3] ^ t[19];
    x[ 4] = t[ 4] ^ t[20];
    x[ 5] = t[ 5] ^ t[21];
    x[ 6] = t[ 6] ^ t[22];
    x[ 7] = t[ 7] ^ t[23];
    x[ 8] = t[ 8] ^ t[24];
    x[ 9] = t[ 9] ^ t[25];
    x[10] = t[10] ^ t[26];
    x[11] = t[11] ^ t[27];
    x[12] = t[12] ^ t[28];
    x[13] = t[13] ^ t[29];
    x[14] = t[14] ^ t[30];
    x[15] = t[15] ^ t[31];

    x[ 0] &= mask;
    x[ 1] &= mask;
    x[ 2] &= mask;
    x[ 3] &= mask;
    x[ 4] &= mask;
    x[ 5] &= mask;
    x[ 6] &= mask;
    x[ 7] &= mask;
    x[ 8] &= mask;
    x[ 9] &= mask;
    x[10] &= mask;
    x[11] &= mask;
    x[12] &= mask;
    x[13] &= mask;
    x[14] &= mask;
    x[15] &= mask;

    t[ 0] ^= x[ 0];
    t[ 1] ^= x[ 1];
    t[ 2] ^= x[ 2];
    t[ 3] ^= x[ 3];
    t[ 4] ^= x[ 4];
    t[ 5] ^= x[ 5];
    t[ 6] ^= x[ 6];
    t[ 7] ^= x[ 7];
    t[ 8] ^= x[ 8];
    t[ 9] ^= x[ 9];
    t[10] ^= x[10];
    t[11] ^= x[11];
    t[12] ^= x[12];
    t[13] ^= x[13];
    t[14] ^= x[14];
    t[15] ^= x[15];

    r[ 0] = ( t[ 0] >>  0 );
    r[ 1] = ( t[ 0] >>  8 );
    r[ 2] = ( t[ 0] >> 16 );
    /* In the first limb we only have 4 bits left for byte number 3,
     * the high 4 bits we must take from the next limb */
    r[ 3] = ( t[ 0] >> 24 ) | ( t[ 1] << 4 );
    /* Low 4 bits from t1 we have already encoded into r[3] */
    r[ 4] = ( t[ 1] >>  4 );
    r[ 5] = ( t[ 1] >> 12 );
    r[ 6] = ( t[ 1] >> 20 );
    /* At this point we have nicely fit the first two limbs into the
     * first seven bytes and the cycle shall repeat now */
    r[ 7] = ( t[ 2] >>  0 );
    r[ 8] = ( t[ 2] >>  8 );
    r[ 9] = ( t[ 2] >> 16 );
    r[10] = ( t[ 2] >> 24 ) | ( t[ 3] << 4 );
    r[11] = ( t[ 3] >>  4 );
    r[12] = ( t[ 3] >> 12 );
    r[13] = ( t[ 3] >> 20 );

    r[14] = ( t[ 4] >>  0 );
    r[15] = ( t[ 4] >>  8 );
    r[16] = ( t[ 4] >> 16 );
    r[17] = ( t[ 4] >> 24 ) | ( t[ 5] << 4 );
    r[18] = ( t[ 5] >>  4 );
    r[19] = ( t[ 5] >> 12 );
    r[20] = ( t[ 5] >> 20 );

    r[21] = ( t[ 6] >>  0 );
    r[22] = ( t[ 6] >>  8 );
    r[23] = ( t[ 6] >> 16 );
    r[24] = ( t[ 6] >> 24 ) | ( t[ 7] << 4 );
    r[25] = ( t[ 7] >>  4 );
    r[26] = ( t[ 7] >> 12 );
    r[27] = ( t[ 7] >> 20 );

    r[28] = ( t[ 8] >>  0 );
    r[29] = ( t[ 8] >>  8 );
    r[30] = ( t[ 8] >> 16 );
    r[31] = ( t[ 8] >> 24 ) | ( t[ 9] << 4 );
    r[32] = ( t[ 9] >>  4 );
    r[33] = ( t[ 9] >> 12 );
    r[34] = ( t[ 9] >> 20 );

    r[35] = ( t[10] >>  0 );
    r[36] = ( t[10] >>  8 );
    r[37] = ( t[10] >> 16 );
    r[38] = ( t[10] >> 24 ) | ( t[11] << 4 );
    r[39] = ( t[11] >>  4 );
    r[40] = ( t[11] >> 12 );
    r[41] = ( t[11] >> 20 );

    r[42] = ( t[12] >>  0 );
    r[43] = ( t[12] >>  8 );
    r[44] = ( t[12] >> 16 );
    r[45] = ( t[12] >> 24 ) | ( t[13] << 4 );
    r[46] = ( t[13] >>  4 );
    r[47] = ( t[13] >> 12 );
    r[48] = ( t[13] >> 20 );

    r[49] = ( t[14] >>  0 );
    r[50] = ( t[14] >>  8 );
    r[51] = ( t[14] >> 16 );
    r[52] = ( t[14] >> 24 ) | ( t[15] << 4 );
    r[53] = ( t[15] >>  4 );
    r[54] = ( t[15] >> 12 );
    r[55] = ( t[15] >> 20 );

    /* Set the last byte to 0 */
    r[56] = 0;

    /* In an EdDSA scheme the b and c scalars correspond to the long-term secret key and
     * the ephemeral mask, respectively. Both are secret so we zeroize their intermediate
     * representations. The scalar a corresponds to the public hash value so we do not
     * zeroize it. Neither do we zeroize the t array since its contents are perfectly
     * determined by the public output t0-t7 (response scalar S) as we have reused the
     * upper limbs of t to store ab+c-L. We do however, zeroize the x array and the mask
     * since they leak information about whether or not a strong reduction was needed. */
    purge_secrets(b, sizeof(b));
    purge_secrets(c, sizeof(c));
    purge_secrets(x, sizeof(x));
    purge_secrets(&mask, sizeof(mask));
}

scalar_ops ed448_scalar_ops = {
    .is_canonical = ed448_scalar_is_canonical,
    .reduce = ed448_scalar_reduce,
    .muladd = ed448_scalars_muladd
};
