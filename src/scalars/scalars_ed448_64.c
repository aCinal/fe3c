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

static inline u64 load_64(const u8 src[8])
{
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

static inline u64 load_56(const u8 src[7])
{
    /* Integer encoding is always little endian according to RFC 8032 */
    u64 dst;
#if FE3C_LILENDIAN_TARGET
    dst = 0;
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

static inline void store_56(u8 dst[7], u64 src)
{
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

static int ed448_scalar_is_canonical(const u8 *s)
{
    /* Load the scalar into seven 64-bit words (extended to 128 bits) */
    u128 s0 = load_64(&s[0 * 8]);
    u128 s1 = load_64(&s[1 * 8]);
    u128 s2 = load_64(&s[2 * 8]);
    u128 s3 = load_64(&s[3 * 8]);
    u128 s4 = load_64(&s[4 * 8]);
    u128 s5 = load_64(&s[5 * 8]);
    u128 s6 = load_64(&s[6 * 8]);

    /* Let L[0]-L[6] denote the seven words of L and let
     * S[0]-S[7] denote the seven words of the scalar s
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

static void ed448_scalar_reduce(u8 *s)
{
    /* Ed448 reduction inputs can be 114 bytes long. Use intermediate 56-bit reduced-radix
     * representation during reduction. */

    u128 t[17];
    u64 x[8];
    u64 mask;

    /* Parse the scalar into limbs */
    t[ 0] = load_56(&s[ 0 * 7]);
    t[ 1] = load_56(&s[ 1 * 7]);
    t[ 2] = load_56(&s[ 2 * 7]);
    t[ 3] = load_56(&s[ 3 * 7]);
    t[ 4] = load_56(&s[ 4 * 7]);
    t[ 5] = load_56(&s[ 5 * 7]);
    t[ 6] = load_56(&s[ 6 * 7]);
    t[ 7] = load_56(&s[ 7 * 7]);
    t[ 8] = load_56(&s[ 8 * 7]);
    t[ 9] = load_56(&s[ 9 * 7]);
    t[10] = load_56(&s[10 * 7]);
    t[11] = load_56(&s[11 * 7]);
    t[12] = load_56(&s[12 * 7]);
    t[13] = load_56(&s[13 * 7]);
    t[14] = load_56(&s[14 * 7]);
    t[15] = load_56(&s[15 * 7]);
    /* Load the trailing 16 bits into t16 */
    t[16] = ( (u128) s[113] << 8 ) | s[112];

    /* Let L = 2^446 - c denote the group order. We shall split s into x + y 2^446
     * and use the identity:
     *
     *           x + y 2^446 = x + (2^446 - c) y + c y = x + c y (mod L)
     *
     * Note that the 2^446 boundary is in the middle of limb t7. Top two bits of t7
     * are already above 2^446. For this reason limbs t8-t16 should actually have
     * larger weights. To compensate this we multiply them by four as we go.
     */

    t[ 4] += t[12] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 5] += t[12] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 6] += t[12] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 7] += t[12] * 4 * GROUP_ORDER_SUB_LIMB_3;

    t[ 5] += t[13] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 6] += t[13] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 7] += t[13] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 8] += t[13] * 4 * GROUP_ORDER_SUB_LIMB_3;

    t[ 6] += t[14] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 7] += t[14] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 8] += t[14] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 9] += t[14] * 4 * GROUP_ORDER_SUB_LIMB_3;

    t[ 7] += t[15] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 8] += t[15] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 9] += t[15] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[10] += t[15] * 4 * GROUP_ORDER_SUB_LIMB_3;

    t[ 8] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 9] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[10] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[11] += t[16] * 4 * GROUP_ORDER_SUB_LIMB_3;

    /* Do a partial reduction of the limbs we have been working with above
     * to ensure no overflows occur */
    t[ 5] += t[ 4] >> 56;  t[ 4] &= LOW_56_BITS_MASK;
    t[ 6] += t[ 5] >> 56;  t[ 5] &= LOW_56_BITS_MASK;
    t[ 7] += t[ 6] >> 56;  t[ 6] &= LOW_56_BITS_MASK;
    t[ 8] += t[ 7] >> 56;  t[ 7] &= LOW_56_BITS_MASK;
    t[ 9] += t[ 8] >> 56;  t[ 8] &= LOW_56_BITS_MASK;
    t[10] += t[ 9] >> 56;  t[ 9] &= LOW_56_BITS_MASK;
    t[11] += t[10] >> 56;  t[10] &= LOW_56_BITS_MASK;
    /* Use t12 to store the overflow of t11 */
    t[12]  = t[11] >> 56;  t[11] &= LOW_56_BITS_MASK;

    t[ 0] += t[ 8] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 1] += t[ 8] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 2] += t[ 8] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 3] += t[ 8] * 4 * GROUP_ORDER_SUB_LIMB_3;

    t[ 1] += t[ 9] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 2] += t[ 9] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 3] += t[ 9] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 4] += t[ 9] * 4 * GROUP_ORDER_SUB_LIMB_3;

    t[ 2] += t[10] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 3] += t[10] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 4] += t[10] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 5] += t[10] * 4 * GROUP_ORDER_SUB_LIMB_3;

    t[ 3] += t[11] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 4] += t[11] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 5] += t[11] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 6] += t[11] * 4 * GROUP_ORDER_SUB_LIMB_3;

    t[ 4] += t[12] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 5] += t[12] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 6] += t[12] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 7] += t[12] * 4 * GROUP_ORDER_SUB_LIMB_3;

    t[ 1] += t[ 0] >> 56;  t[ 0] &= LOW_56_BITS_MASK;
    t[ 2] += t[ 1] >> 56;  t[ 1] &= LOW_56_BITS_MASK;
    t[ 3] += t[ 2] >> 56;  t[ 2] &= LOW_56_BITS_MASK;
    t[ 4] += t[ 3] >> 56;  t[ 3] &= LOW_56_BITS_MASK;
    t[ 5] += t[ 4] >> 56;  t[ 4] &= LOW_56_BITS_MASK;
    t[ 6] += t[ 5] >> 56;  t[ 5] &= LOW_56_BITS_MASK;
    t[ 7] += t[ 6] >> 56;  t[ 6] &= LOW_56_BITS_MASK;
    /* Use t8 to store the overflow of t7 */
    t[ 8]  = t[ 7] >> 56;  t[ 7] &= LOW_56_BITS_MASK;

    /* Bring down whatever has bubbled up into t8 */
    t[ 0] += t[ 8] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 1] += t[ 8] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 2] += t[ 8] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 3] += t[ 8] * 4 * GROUP_ORDER_SUB_LIMB_3;

    t[ 1] += t[ 0] >> 56;  t[ 0] &= LOW_56_BITS_MASK;
    t[ 2] += t[ 1] >> 56;  t[ 1] &= LOW_56_BITS_MASK;
    t[ 3] += t[ 2] >> 56;  t[ 2] &= LOW_56_BITS_MASK;
    t[ 4] += t[ 3] >> 56;  t[ 3] &= LOW_56_BITS_MASK;
    t[ 5] += t[ 4] >> 56;  t[ 4] &= LOW_56_BITS_MASK;
    t[ 6] += t[ 5] >> 56;  t[ 5] &= LOW_56_BITS_MASK;
    t[ 7] += t[ 6] >> 56;  t[ 6] &= LOW_56_BITS_MASK;
    /* Use t8 to store the overflow of t7. Note that at this point
     * we also set t7 to its canonical form which is 54-bits wide. */
    t[ 8]  = t[ 7] >> 54;  t[ 7]  &= LOW_54_BITS_MASK;

    /* Note that the "shift by two" has already been accounted for */
    t[ 0] += t[ 8] * GROUP_ORDER_SUB_LIMB_0;
    t[ 1] += t[ 8] * GROUP_ORDER_SUB_LIMB_1;
    t[ 2] += t[ 8] * GROUP_ORDER_SUB_LIMB_2;
    t[ 3] += t[ 8] * GROUP_ORDER_SUB_LIMB_3;

    /* Do one final weak reduction */
    t[ 1] += t[ 0] >> 56;  t[ 0] &= LOW_56_BITS_MASK;
    t[ 2] += t[ 1] >> 56;  t[ 1] &= LOW_56_BITS_MASK;
    t[ 3] += t[ 2] >> 56;  t[ 2] &= LOW_56_BITS_MASK;
    t[ 4] += t[ 3] >> 56;  t[ 3] &= LOW_56_BITS_MASK;
    t[ 5] += t[ 4] >> 56;  t[ 4] &= LOW_56_BITS_MASK;
    t[ 6] += t[ 5] >> 56;  t[ 5] &= LOW_56_BITS_MASK;
    t[ 7] += t[ 6] >> 56;  t[ 6] &= LOW_56_BITS_MASK;
    t[ 7] &= LOW_54_BITS_MASK;

    /* By this point we have done a "relaxed reduction", i.e. s is in the range
     * [0, 2*L). Compute s-L and conditionally use it as a result if s is still
     * larger than or equal to L. Start by computing s+c, where c = 2^446 - L.
     * To limit stack usage, reuse the t8-t15 limbs. */
    t[ 8] = t[ 0] + GROUP_ORDER_SUB_LIMB_0;
    t[ 9] = t[ 1] + GROUP_ORDER_SUB_LIMB_1;
    t[10] = t[ 2] + GROUP_ORDER_SUB_LIMB_2;
    t[11] = t[ 3] + GROUP_ORDER_SUB_LIMB_3;
    t[12] = t[ 4];
    t[13] = t[ 5];
    t[14] = t[ 6];
    t[15] = t[ 7];

    /* Normalize the result, i.e. propagate the overflows upwards */
    t[ 9] += t[ 8] >> 56;  t[ 8] &= LOW_56_BITS_MASK;
    t[10] += t[ 9] >> 56;  t[ 9] &= LOW_56_BITS_MASK;
    t[11] += t[10] >> 56;  t[10] &= LOW_56_BITS_MASK;
    t[12] += t[11] >> 56;  t[11] &= LOW_56_BITS_MASK;
    t[13] += t[12] >> 56;  t[12] &= LOW_56_BITS_MASK;
    t[14] += t[13] >> 56;  t[13] &= LOW_56_BITS_MASK;
    t[15] += t[14] >> 56;  t[14] &= LOW_56_BITS_MASK;
    /* At this point t15 contains the highest limb of s + c (extended to 128 bits).
     * Try subtracting 2^446. */
    t[15] -= 1ULL << 54;
    /* Check if the subtraction resulted in an underflow and use the result to create
     * a mask for a conditional move. */
    mask = (u64)( -(i64)( (t[15] >> 127) ^ 1 ) );
    /* If the subtraction underflowed, then mask is set to all zeroes and we will
     * discard limbs t9-t15. Otherwise we will use them as the result. */
    t[15] &= LOW_54_BITS_MASK;

    x[0] = t[0] ^ t[ 8];
    x[1] = t[1] ^ t[ 9];
    x[2] = t[2] ^ t[10];
    x[3] = t[3] ^ t[11];
    x[4] = t[4] ^ t[12];
    x[5] = t[5] ^ t[13];
    x[6] = t[6] ^ t[14];
    x[7] = t[7] ^ t[15];

    x[0] &= mask;
    x[1] &= mask;
    x[2] &= mask;
    x[3] &= mask;
    x[4] &= mask;
    x[5] &= mask;
    x[6] &= mask;
    x[7] &= mask;

    t[0] ^= x[0];
    t[1] ^= x[1];
    t[2] ^= x[2];
    t[3] ^= x[3];
    t[4] ^= x[4];
    t[5] ^= x[5];
    t[6] ^= x[6];
    t[7] ^= x[7];

    store_56(&s[0 * 7], t[0]);
    store_56(&s[1 * 7], t[1]);
    store_56(&s[2 * 7], t[2]);
    store_56(&s[3 * 7], t[3]);
    store_56(&s[4 * 7], t[4]);
    store_56(&s[5 * 7], t[5]);
    store_56(&s[6 * 7], t[6]);
    store_56(&s[7 * 7], t[7]);
    /* Set the last byte to 0 */
    s[56] = 0;

    purge_secrets(t, sizeof(t));
    purge_secrets(x, sizeof(x));
    purge_secrets(&mask, sizeof(mask));
}

static void ed448_scalars_muladd(u8 *r, const u8 *aa, const u8 *bb, const u8 *cc)
{
    /* Ed448 multiplication+addition inputs must be 57 (56) bytes long. Use intermediate
     * 56-bit reduced-radix representation. */

    u128 a[8];
    u128 b[8];
    u128 c[8];
    u128 t[16];
    u64 x[8];
    u64 mask;

    a[0] = load_56(&aa[0 * 7]);
    a[1] = load_56(&aa[1 * 7]);
    a[2] = load_56(&aa[2 * 7]);
    a[3] = load_56(&aa[3 * 7]);
    a[4] = load_56(&aa[4 * 7]);
    a[5] = load_56(&aa[5 * 7]);
    a[6] = load_56(&aa[6 * 7]);
    a[7] = load_56(&aa[7 * 7]);

    b[0] = load_56(&bb[0 * 7]);
    b[1] = load_56(&bb[1 * 7]);
    b[2] = load_56(&bb[2 * 7]);
    b[3] = load_56(&bb[3 * 7]);
    b[4] = load_56(&bb[4 * 7]);
    b[5] = load_56(&bb[5 * 7]);
    b[6] = load_56(&bb[6 * 7]);
    b[7] = load_56(&bb[7 * 7]);

    c[0] = load_56(&cc[0 * 7]);
    c[1] = load_56(&cc[1 * 7]);
    c[2] = load_56(&cc[2 * 7]);
    c[3] = load_56(&cc[3 * 7]);
    c[4] = load_56(&cc[4 * 7]);
    c[5] = load_56(&cc[5 * 7]);
    c[6] = load_56(&cc[6 * 7]);
    c[7] = load_56(&cc[7 * 7]);

    /* Do the naive schoolbook multiplication - note that a*b takes 15 (16 with carry) limbs
     * (columns in the multplication algorithm). Offset the first 8 limbs by the limbs of c
     *
     *                                                     a7     a6     a5     a4     a3     a2     a1     a0
     *                                                     b7     b6     b5     b4     b3     b2     b1     b0
     * ---------------------------------------------------------------------------------------------------------
     *                                                    a0b7   a0b6   a0b5   a0b4   a0b3   a0b2   a0b1   a0b0
     *                                             a1b7   a1b6   a1b5   a1b4   a1b3   a1b2   a1b1   a1b0
     *                                      a2b7   a2b6   a2b5   a2b4   a2b3   a2b2   a2b1   a2b0
     *                               a3b7   a3b6   a3b5   a3b4   a3b3   a3b2   a3b1   a3b0
     *                        a4b7   a4b6   a4b5   a4b4   a4b3   a4b2   a4b1   a4b0
     *                 a5b7   a5b6   a5b5   a5b4   a5b3   a5b2   a5b1   a5b0
     *          a6b7   a6b6   a6b5   a6b4   a6b3   a6b2   a6b1   a6b0
     *   a7b7   a7b6   a7b5   a7b4   a7b3   a7b2   a7b1   a7b0
     *                                                     c7     c6     c5     c4     c3     c2     c1     c0
     */

    t[ 0] = c[0] + a[0]*b[0];
    t[ 1] = c[1] + a[0]*b[1] + a[1]*b[0];
    t[ 2] = c[2] + a[0]*b[2] + a[1]*b[1] + a[2]*b[0];
    t[ 3] = c[3] + a[0]*b[3] + a[1]*b[2] + a[2]*b[1] + a[3]*b[0];
    t[ 4] = c[4] + a[0]*b[4] + a[1]*b[3] + a[2]*b[2] + a[3]*b[1] + a[4]*b[0];
    t[ 5] = c[5] + a[0]*b[5] + a[1]*b[4] + a[2]*b[3] + a[3]*b[2] + a[4]*b[1] + a[5]*b[0];
    t[ 6] = c[6] + a[0]*b[6] + a[1]*b[5] + a[2]*b[4] + a[3]*b[3] + a[4]*b[2] + a[5]*b[1] + a[6]*b[0];
    t[ 7] = c[7] + a[0]*b[7] + a[1]*b[6] + a[2]*b[5] + a[3]*b[4] + a[4]*b[3] + a[5]*b[2] + a[6]*b[1] + a[7]*b[0];
    t[ 8] =                    a[1]*b[7] + a[2]*b[6] + a[3]*b[5] + a[4]*b[4] + a[5]*b[3] + a[6]*b[2] + a[7]*b[1];
    t[ 9] =                                a[2]*b[7] + a[3]*b[6] + a[4]*b[5] + a[5]*b[4] + a[6]*b[3] + a[7]*b[2];
    t[10] =                                            a[3]*b[7] + a[4]*b[6] + a[5]*b[5] + a[6]*b[4] + a[7]*b[3];
    t[11] =                                                        a[4]*b[7] + a[5]*b[6] + a[6]*b[5] + a[7]*b[4];
    t[12] =                                                                    a[5]*b[7] + a[6]*b[6] + a[7]*b[5];
    t[13] =                                                                                a[6]*b[7] + a[7]*b[6];
    t[14] =                                                                                            a[7]*b[7];

    /* Normalize the limbs to identify the overflow */
    t[ 1] += t[ 0] >> 56;  t[ 0] &= LOW_56_BITS_MASK;
    t[ 2] += t[ 1] >> 56;  t[ 1] &= LOW_56_BITS_MASK;
    t[ 3] += t[ 2] >> 56;  t[ 2] &= LOW_56_BITS_MASK;
    t[ 4] += t[ 3] >> 56;  t[ 3] &= LOW_56_BITS_MASK;
    t[ 5] += t[ 4] >> 56;  t[ 4] &= LOW_56_BITS_MASK;
    t[ 6] += t[ 5] >> 56;  t[ 5] &= LOW_56_BITS_MASK;
    t[ 7] += t[ 6] >> 56;  t[ 6] &= LOW_56_BITS_MASK;
    t[ 8] += t[ 7] >> 56;  t[ 7] &= LOW_56_BITS_MASK;
    t[ 9] += t[ 8] >> 56;  t[ 8] &= LOW_56_BITS_MASK;
    t[10] += t[ 9] >> 56;  t[ 9] &= LOW_56_BITS_MASK;
    t[11] += t[10] >> 56;  t[10] &= LOW_56_BITS_MASK;
    t[12] += t[11] >> 56;  t[11] &= LOW_56_BITS_MASK;
    t[13] += t[12] >> 56;  t[12] &= LOW_56_BITS_MASK;
    t[14] += t[13] >> 56;  t[13] &= LOW_56_BITS_MASK;
    /* Use t15 to store the overflow of t14 */
    t[15]  = t[14] >> 56;  t[14] &= LOW_56_BITS_MASK;

    /* Bring down the limbs that exceed 2^446 according to the identity:
     *
     *   x + y 2^446 = x + (2^446 - u) y + u y = x + u y (mod L)
     *
     * where u = 2^446 - L. See ed448_scalar_reduce for a more detailed
     * description, in particular the reasoning behind the multiplication
     * by four (due to how the scalars were parsed and handled).
     */

    t[ 4] += t[12] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 5] += t[12] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 6] += t[12] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 7] += t[12] * 4 * GROUP_ORDER_SUB_LIMB_3;

    t[ 5] += t[13] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 6] += t[13] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 7] += t[13] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 8] += t[13] * 4 * GROUP_ORDER_SUB_LIMB_3;

    t[ 6] += t[14] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 7] += t[14] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 8] += t[14] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 9] += t[14] * 4 * GROUP_ORDER_SUB_LIMB_3;

    t[ 7] += t[15] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 8] += t[15] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 9] += t[15] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[10] += t[15] * 4 * GROUP_ORDER_SUB_LIMB_3;

    /* Do a partial reduction of the limbs we have been working with above
     * to ensure no overflows occur */
    t[ 5] += t[ 4] >> 56;  t[ 4] &= LOW_56_BITS_MASK;
    t[ 6] += t[ 5] >> 56;  t[ 5] &= LOW_56_BITS_MASK;
    t[ 7] += t[ 6] >> 56;  t[ 6] &= LOW_56_BITS_MASK;
    t[ 8] += t[ 7] >> 56;  t[ 7] &= LOW_56_BITS_MASK;
    t[ 9] += t[ 8] >> 56;  t[ 8] &= LOW_56_BITS_MASK;
    t[10] += t[ 9] >> 56;  t[ 9] &= LOW_56_BITS_MASK;
    t[11] += t[10] >> 56;  t[10] &= LOW_56_BITS_MASK;
    /* Use t12 to store the overflow of t11 */
    t[12]  = t[11] >> 56;  t[11] &= LOW_56_BITS_MASK;

    t[ 0] += t[ 8] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 1] += t[ 8] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 2] += t[ 8] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 3] += t[ 8] * 4 * GROUP_ORDER_SUB_LIMB_3;

    t[ 1] += t[ 9] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 2] += t[ 9] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 3] += t[ 9] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 4] += t[ 9] * 4 * GROUP_ORDER_SUB_LIMB_3;

    t[ 2] += t[10] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 3] += t[10] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 4] += t[10] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 5] += t[10] * 4 * GROUP_ORDER_SUB_LIMB_3;

    t[ 3] += t[11] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 4] += t[11] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 5] += t[11] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 6] += t[11] * 4 * GROUP_ORDER_SUB_LIMB_3;

    t[ 4] += t[12] * 4 * GROUP_ORDER_SUB_LIMB_0;
    t[ 5] += t[12] * 4 * GROUP_ORDER_SUB_LIMB_1;
    t[ 6] += t[12] * 4 * GROUP_ORDER_SUB_LIMB_2;
    t[ 7] += t[12] * 4 * GROUP_ORDER_SUB_LIMB_3;

    t[ 1] += t[ 0] >> 56;  t[ 0] &= LOW_56_BITS_MASK;
    t[ 2] += t[ 1] >> 56;  t[ 1] &= LOW_56_BITS_MASK;
    t[ 3] += t[ 2] >> 56;  t[ 2] &= LOW_56_BITS_MASK;
    t[ 4] += t[ 3] >> 56;  t[ 3] &= LOW_56_BITS_MASK;
    t[ 5] += t[ 4] >> 56;  t[ 4] &= LOW_56_BITS_MASK;
    t[ 6] += t[ 5] >> 56;  t[ 5] &= LOW_56_BITS_MASK;
    t[ 7] += t[ 6] >> 56;  t[ 6] &= LOW_56_BITS_MASK;
    /* Use t8 to store the overflow of t7. Note that at this point
     * we also set t7 to its canonical form which is 54-bits wide. */
    t[ 8]  = t[ 7] >> 54;  t[ 7] &= LOW_54_BITS_MASK;

    /* Note that the "shift by two" has already been accounted for */
    t[ 0] += t[ 8] * GROUP_ORDER_SUB_LIMB_0;
    t[ 1] += t[ 8] * GROUP_ORDER_SUB_LIMB_1;
    t[ 2] += t[ 8] * GROUP_ORDER_SUB_LIMB_2;
    t[ 3] += t[ 8] * GROUP_ORDER_SUB_LIMB_3;

    /* Do one final weak reduction */
    t[ 1] += t[ 0] >> 56;  t[ 0] &= LOW_56_BITS_MASK;
    t[ 2] += t[ 1] >> 56;  t[ 1] &= LOW_56_BITS_MASK;
    t[ 3] += t[ 2] >> 56;  t[ 2] &= LOW_56_BITS_MASK;
    t[ 4] += t[ 3] >> 56;  t[ 3] &= LOW_56_BITS_MASK;
    t[ 5] += t[ 4] >> 56;  t[ 4] &= LOW_56_BITS_MASK;
    t[ 6] += t[ 5] >> 56;  t[ 5] &= LOW_56_BITS_MASK;
    t[ 7] += t[ 6] >> 56;  t[ 6] &= LOW_56_BITS_MASK;
    t[ 7] &= LOW_54_BITS_MASK;

    /* By this point we have done a "relaxed reduction", i.e. the result is in
     * the range [0, 2*L). Compute ab+c-L and conditionally use it as a result if
     * the result is still larger than or equal to L. Start by computing ab+c+u,
     * where u = 2^446 - L. To limit stack usage, reuse the t8-t15 limbs. */
    t[ 8] = GROUP_ORDER_SUB_LIMB_0 + t[ 0];
    t[ 9] = GROUP_ORDER_SUB_LIMB_1 + t[ 1];
    t[10] = GROUP_ORDER_SUB_LIMB_2 + t[ 2];
    t[11] = GROUP_ORDER_SUB_LIMB_3 + t[ 3];
    t[12] = t[ 4];
    t[13] = t[ 5];
    t[14] = t[ 6];
    t[15] = t[ 7];

    /* Normalize the result, i.e. propagate the overflows upwards */
    t[ 9] += t[ 8] >> 56;  t[ 8] &= LOW_56_BITS_MASK;
    t[10] += t[ 9] >> 56;  t[ 9] &= LOW_56_BITS_MASK;
    t[11] += t[10] >> 56;  t[10] &= LOW_56_BITS_MASK;
    t[12] += t[11] >> 56;  t[11] &= LOW_56_BITS_MASK;
    t[13] += t[12] >> 56;  t[12] &= LOW_56_BITS_MASK;
    t[14] += t[13] >> 56;  t[13] &= LOW_56_BITS_MASK;
    t[15] += t[14] >> 56;  t[14] &= LOW_56_BITS_MASK;
    /* At this point t15 contains the highest limb of ab+c+u (extended to 128 bits).
     * Try subtracting 2^446 */
    t[15] -= (1ULL << 54);
    /* Check if the subtraction resulted in an underflow and use the result to create
     * a mask for a conditional move. */
    mask = (u64)( -(i64)( (t[15] >> 127) ^ 1 ) );
    /* If the subtraction underflowed, then mask is set to all zeroes and we will
     * discard limbs t9-t15. Otherwise we will use them as the result. */
    t[15] &= LOW_54_BITS_MASK;

    x[0] = t[ 0] ^ t[ 8];
    x[1] = t[ 1] ^ t[ 9];
    x[2] = t[ 2] ^ t[10];
    x[3] = t[ 3] ^ t[11];
    x[4] = t[ 4] ^ t[12];
    x[5] = t[ 5] ^ t[13];
    x[6] = t[ 6] ^ t[14];
    x[7] = t[ 7] ^ t[15];

    x[0] &= mask;
    x[1] &= mask;
    x[2] &= mask;
    x[3] &= mask;
    x[4] &= mask;
    x[5] &= mask;
    x[6] &= mask;
    x[7] &= mask;

    t[0] ^= x[0];
    t[1] ^= x[1];
    t[2] ^= x[2];
    t[3] ^= x[3];
    t[4] ^= x[4];
    t[5] ^= x[5];
    t[6] ^= x[6];
    t[7] ^= x[7];

    store_56(&r[0 * 7], t[0]);
    store_56(&r[1 * 7], t[1]);
    store_56(&r[2 * 7], t[2]);
    store_56(&r[3 * 7], t[3]);
    store_56(&r[4 * 7], t[4]);
    store_56(&r[5 * 7], t[5]);
    store_56(&r[6 * 7], t[6]);
    store_56(&r[7 * 7], t[7]);
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

const scalar_ops ed448_scalar_ops = {
    .is_canonical = ed448_scalar_is_canonical,
    .reduce = ed448_scalar_reduce,
    .muladd = ed448_scalars_muladd
};
