#include <field_elements/field_elements_ed448.h>
#include <utils/utils.h>

#if !FE3C_32BIT
    #error "Build system inconsistency detected! field_elements_ed448_32.c in use despite FE3C_32BIT not being set"
#endif /* !FE3C_32BIT */

#define LOW_28_BITS_MASK  0xfffffff

/* Elliptic curve constant d = -39081 */
const fe448 ed448_d = {
    0xfff6756, 0xfffffff, 0xfffffff, 0xfffffff,
    0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff,
    0xffffffe, 0xfffffff, 0xfffffff, 0xfffffff,
    0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff
};
/* Twisted elliptic curve constant d' = d-1 */
const fe448 ed448twist_d = {
    0xfff6755, 0xfffffff, 0xfffffff, 0xfffffff,
    0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff,
    0xffffffe, 0xfffffff, 0xfffffff, 0xfffffff,
    0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff
};
/* Additive identity in the field */
const fe448 fe448_zero = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
/* Multiplicative identity in the field */
const fe448 fe448_one = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static inline u32 _load_32(const u8 src[4])
{
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

static inline void store_32(u8 dst[4], u32 src)
{
    /* Integer encoding is always little endian according to RFC 8032 */
#if FE3C_LILENDIAN_TARGET
    (void) memcpy(dst, &src, 4);
#else
    /* After each write do a shift (src is a local variable) */
    dst[0] = (u8) src;  src >>= 8;
    dst[1] = (u8) src;  src >>= 8;
    dst[2] = (u8) src;  src >>= 8;
    dst[3] = (u8) src;
#endif
}

/**
 * @brief Uncondionally copy a field element
 * @param r Destination field element
 * @param a Source field element
 */
void fe448_copy(fe448 r, const fe448 a)
{
    r[ 0] = a[ 0];
    r[ 1] = a[ 1];
    r[ 2] = a[ 2];
    r[ 3] = a[ 3];
    r[ 4] = a[ 4];
    r[ 5] = a[ 5];
    r[ 6] = a[ 6];
    r[ 7] = a[ 7];
    r[ 8] = a[ 8];
    r[ 9] = a[ 9];
    r[10] = a[10];
    r[11] = a[11];
    r[12] = a[12];
    r[13] = a[13];
    r[14] = a[14];
    r[15] = a[15];
}

/**
 * @brief Check if an elements is in its canonical form, i.e. less than the modulus
 * @param a Field element to check
 * @return 1 if a is in canonical form, 0 otherwise
 */
static inline int fe448_is_canonical(const fe448 a)
{
    int canonical = 1;
    canonical &= (a[ 0] <  0xfffffff);
    canonical &= (a[ 1] <= 0xfffffff);
    canonical &= (a[ 2] <= 0xfffffff);
    canonical &= (a[ 3] <= 0xfffffff);
    canonical &= (a[ 4] <= 0xfffffff);
    canonical &= (a[ 5] <= 0xfffffff);
    canonical &= (a[ 6] <= 0xfffffff);
    canonical &= (a[ 7] <= 0xfffffff);
    canonical &= (a[ 8] <= 0xffffffe);
    canonical &= (a[ 9] <= 0xfffffff);
    canonical &= (a[10] <= 0xfffffff);
    canonical &= (a[11] <= 0xfffffff);
    canonical &= (a[12] <= 0xfffffff);
    canonical &= (a[13] <= 0xfffffff);
    canonical &= (a[14] <= 0xfffffff);
    canonical &= (a[15] <= 0xfffffff);
    return canonical;
}

/**
 * @brief Do a constant-time equality check
 * @param[in] a The first element to check
 * @param[in] b The second element to check
 * @return 1 if a = b, 0 otherwise
 * @note The elements should be reduced by the caller first
 */
int fe448_equal(const fe448 a, const fe448 b)
{
    fe_limb_type sum = 0;

    /* Do an XOR between the two elements, if they are equal this should amount
     * to zero */
    sum |= a[ 0] ^ b[ 0];
    sum |= a[ 1] ^ b[ 1];
    sum |= a[ 2] ^ b[ 2];
    sum |= a[ 3] ^ b[ 3];
    sum |= a[ 4] ^ b[ 4];
    sum |= a[ 5] ^ b[ 5];
    sum |= a[ 6] ^ b[ 6];
    sum |= a[ 7] ^ b[ 7];
    sum |= a[ 8] ^ b[ 8];
    sum |= a[ 9] ^ b[ 9];
    sum |= a[10] ^ b[10];
    sum |= a[11] ^ b[11];
    sum |= a[12] ^ b[12];
    sum |= a[13] ^ b[13];
    sum |= a[14] ^ b[14];
    sum |= a[15] ^ b[15];

    /* Bring any 1's down to the least significant bit */
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
void fe448_conditional_move(fe448 r, const fe448 a, int move)
{
    /* Set the mask to 0x00000000 if move is 0 or to 0xFFFFFFFF if it is 1 */
    const fe_limb_type mask = (fe_limb_type)( -(i32) move );

    /* The conditional move implementation relies on the identity (x ^ y) ^ y = x */

    /* Copy the current contents of r into the t array */
    fe_limb_type t0  = r[ 0];
    fe_limb_type t1  = r[ 1];
    fe_limb_type t2  = r[ 2];
    fe_limb_type t3  = r[ 3];
    fe_limb_type t4  = r[ 4];
    fe_limb_type t5  = r[ 5];
    fe_limb_type t6  = r[ 6];
    fe_limb_type t7  = r[ 7];
    fe_limb_type t8  = r[ 8];
    fe_limb_type t9  = r[ 9];
    fe_limb_type t10 = r[10];
    fe_limb_type t11 = r[11];
    fe_limb_type t12 = r[12];
    fe_limb_type t13 = r[13];
    fe_limb_type t14 = r[14];
    fe_limb_type t15 = r[15];

    /* XOR the t's with the limbs of a */
    fe_limb_type x0  =  t0 ^ a[ 0];
    fe_limb_type x1  =  t1 ^ a[ 1];
    fe_limb_type x2  =  t2 ^ a[ 2];
    fe_limb_type x3  =  t3 ^ a[ 3];
    fe_limb_type x4  =  t4 ^ a[ 4];
    fe_limb_type x5  =  t5 ^ a[ 5];
    fe_limb_type x6  =  t6 ^ a[ 6];
    fe_limb_type x7  =  t7 ^ a[ 7];

    fe_limb_type x8  =  t8 ^ a[ 8];
    fe_limb_type x9  =  t9 ^ a[ 9];
    fe_limb_type x10 = t10 ^ a[10];
    fe_limb_type x11 = t11 ^ a[11];
    fe_limb_type x12 = t12 ^ a[12];
    fe_limb_type x13 = t13 ^ a[13];
    fe_limb_type x14 = t14 ^ a[14];
    fe_limb_type x15 = t15 ^ a[15];

    /* Set the x array to all zeroes if move is zero or leave it be if move is one */
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

    /* If move=0 then x0-x15 are all zero and so we write back the limbs of r into r.
     * If move=1 then x0-x15 contain r XOR a and so by XORing back with the limbs of r we
     * write the limbs of a into r */
    r[ 0] =  t0 ^ x0;
    r[ 1] =  t1 ^ x1;
    r[ 2] =  t2 ^ x2;
    r[ 3] =  t3 ^ x3;
    r[ 4] =  t4 ^ x4;
    r[ 5] =  t5 ^ x5;
    r[ 6] =  t6 ^ x6;
    r[ 7] =  t7 ^ x7;
    r[ 8] =  t8 ^ x8;
    r[ 9] =  t9 ^ x9;
    r[10] = t10 ^ x10;
    r[11] = t11 ^ x11;
    r[12] = t12 ^ x12;
    r[13] = t13 ^ x13;
    r[14] = t14 ^ x14;
    r[15] = t15 ^ x15;
}

/**
 * @brief Do a weak modular reduction modulo the underlying field's characteristic p
 * @param[out] r Result of the reduction
 * @param[in] a Field element to be reduced
 * @note Note that the result need to be in canonical form, i.e. between 0 and p-1, it need only be less than 2p
 */
void fe448_weak_reduce(fe448 r, const fe448 a)
{
    /* Do a "relaxed" reduction (to borrow terminology form Michael Scott's "Slothful reduction" paper)
     * - this ensures the result is less than 2p (where p = 2^448 - 2^224 - 1) */

    /* Use 64-bit auxiliary variables */
    u64 t0  = a[ 0];
    u64 t1  = a[ 1];
    u64 t2  = a[ 2];
    u64 t3  = a[ 3];
    u64 t4  = a[ 4];
    u64 t5  = a[ 5];
    u64 t6  = a[ 6];
    u64 t7  = a[ 7];
    u64 t8  = a[ 8];
    u64 t9  = a[ 9];
    u64 t10 = a[10];
    u64 t11 = a[11];
    u64 t12 = a[12];
    u64 t13 = a[13];
    u64 t14 = a[14];
    u64 t15 = a[15];

    /* The modulus can be expressed as p = u - v - 1, where u = 2^448 and v = 2^224.
     * Express (split) the value a as a = x + uy + vz. Then
     *
     *   a mod p = (x + uy + vz) mod (u - v - 1)
     *           = (x + (u - v - 1)y + vy + y + vz) mod (u - v - 1)
     *           = (x + vz + vy + y) mod p
     *
     * To split a into (x, y, z) we simply let the carries bubble up to the top
     * limb. Whatever has overflown the top limb is equal to y. Limbs 8-15 hold
     * the value of z and the low eight limbs hold x.
     */

    /* Add whatever has overflown in t0 (a[0]) to t1 (a[1])
     * and mask out the overflow in t0 */
    t1  += t0  >> 28;  t0  &= LOW_28_BITS_MASK;
    /* Rinse and repeat for other limbs */
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

    /* At this point low 28 bits of each of limbs t0-t15 hold the value of a modulo
     * 2^448, i.e. x + vz, using the notation from above. Add vy and y to this
     * result. Note that y is stored in the "overflow" of t15, i.e. bits starting
     * from bit 28. Also note that adding vy is equivalent to adding y to the limbs
     * starting from limb 8 (since v = 2^{224} = 2^{8*28}). */

    /* Add y */
    t0 += t15 >> 28;
    /* Add y 2^224 */
    t8 += t15 >> 28;
    /* Clear the overflow in t15 */
    t15 &= LOW_28_BITS_MASK;

    /* Do this once more to ensure we correctly normalize
     * all integers possible to be represented in our system,
     * i.e. we don't get surprised by (t15 >> 28) leading
     * to overflow in t0 or t8 */
    t1  += t0  >> 28;  t0  &= LOW_28_BITS_MASK;
    /* Rinse and repeat for other limbs */
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
    t0  += t15 >> 28;
    t8  += t15 >> 28;
    t15 &= LOW_28_BITS_MASK;

    r[ 0] = t0;
    r[ 1] = t1;
    r[ 2] = t2;
    r[ 3] = t3;
    r[ 4] = t4;
    r[ 5] = t5;
    r[ 6] = t6;
    r[ 7] = t7;
    r[ 8] = t8;
    r[ 9] = t9;
    r[10] = t10;
    r[11] = t11;
    r[12] = t12;
    r[13] = t13;
    r[14] = t14;
    r[15] = t15;
}

/**
 * @brief Do a strong modular reduction modulo the underlying field's characteristic p
 * @param[out] r Result of the reduction
 * @param[in] a Field element to be reduced
 * @note The result is guaranteed to be in canonical form, i.e. between 0 and p-1
 */
void fe448_strong_reduce(fe448 r, const fe448 a)
{
    fe448_weak_reduce(r, a);
    /* After the weak reduction r is congruent to a and less than 2p */

    /* Compute r-p and conditionally use it as a result if r is larger than p */
    fe448 t;
    /* Store r + 2^224 + 1 in t (we will subtract 2^448 from it, resulting in
     * t = r - p)*/
    t[ 0] = r[ 0] + 1;
    t[ 1] = r[ 1];
    t[ 2] = r[ 2];
    t[ 3] = r[ 3];
    t[ 4] = r[ 4];
    t[ 5] = r[ 5];
    t[ 6] = r[ 6];
    t[ 7] = r[ 7];
    /* Add 2^224 to the mix */
    t[ 8] = r[ 8] + 1;
    t[ 9] = r[ 9];
    t[10] = r[10];
    t[11] = r[11];
    t[12] = r[12];
    t[13] = r[13];
    t[14] = r[14];
    t[15] = r[15];

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

    /* At this point t[15] contains the highest limb of r + 2^224 + 1. Try subtracting
     * 2^448 - if we get an underflow this means that r < 2^448 - 2^224 - 1 and so r is the
     * final result. Otherwise we must return r - p (note that this includes the case
     * where r = p, as no underflow will occur then and t[15] will be equal to
     * zero). */
    t[15] -= (1ULL << 28);

    /* Check the highest bit of t[15] for underflow. If the highest bit is set then
     * underflow occurred and so we return r, otherwise we set r ::= t and return that */
    fe448_conditional_move(r, t, (t[15] >> 31) ^ 1);
}

/**
 * @brief Negate an element in the field
 * @param[out] r The result of negation
 * @param[in] a Element to be negated
 */
void fe448_neg(fe448 r, const fe448 a)
{
    /* Check against underflow */
    FE3C_SANITY_CHECK(a[ 0] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(a));
    FE3C_SANITY_CHECK(a[ 1] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(a));
    FE3C_SANITY_CHECK(a[ 2] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(a));
    FE3C_SANITY_CHECK(a[ 3] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(a));
    FE3C_SANITY_CHECK(a[ 4] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(a));
    FE3C_SANITY_CHECK(a[ 5] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(a));
    FE3C_SANITY_CHECK(a[ 6] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(a));
    FE3C_SANITY_CHECK(a[ 7] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(a));
    FE3C_SANITY_CHECK(a[ 8] <= 0x1ffffffc, FE448_STR, FE448_TO_STR(a));
    FE3C_SANITY_CHECK(a[ 9] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(a));
    FE3C_SANITY_CHECK(a[10] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(a));
    FE3C_SANITY_CHECK(a[11] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(a));
    FE3C_SANITY_CHECK(a[12] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(a));
    FE3C_SANITY_CHECK(a[13] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(a));
    FE3C_SANITY_CHECK(a[14] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(a));
    FE3C_SANITY_CHECK(a[15] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(a));

    /* Set r to 2p-a so as to not require strong reduction of a */
    r[ 0] = 0x1ffffffe - a[ 0];
    r[ 1] = 0x1ffffffe - a[ 1];
    r[ 2] = 0x1ffffffe - a[ 2];
    r[ 3] = 0x1ffffffe - a[ 3];
    r[ 4] = 0x1ffffffe - a[ 4];
    r[ 5] = 0x1ffffffe - a[ 5];
    r[ 6] = 0x1ffffffe - a[ 6];
    r[ 7] = 0x1ffffffe - a[ 7];
    r[ 8] = 0x1ffffffc - a[ 8];
    r[ 9] = 0x1ffffffe - a[ 9];
    r[10] = 0x1ffffffe - a[10];
    r[11] = 0x1ffffffe - a[11];
    r[12] = 0x1ffffffe - a[12];
    r[13] = 0x1ffffffe - a[13];
    r[14] = 0x1ffffffe - a[14];
    r[15] = 0x1ffffffe - a[15];
}

/**
 * @brief Add two field elements
 * @param[out] r Result of the addition, i.e. the sum r = a + b
 * @param[in] a Operand
 * @param[in] b Operand
 */
void fe448_add(fe448 r, const fe448 a, const fe448 b)
{
    r[ 0] = a[ 0] + b[ 0];
    r[ 1] = a[ 1] + b[ 1];
    r[ 2] = a[ 2] + b[ 2];
    r[ 3] = a[ 3] + b[ 3];
    r[ 4] = a[ 4] + b[ 4];
    r[ 5] = a[ 5] + b[ 5];
    r[ 6] = a[ 6] + b[ 6];
    r[ 7] = a[ 7] + b[ 7];
    r[ 8] = a[ 8] + b[ 8];
    r[ 9] = a[ 9] + b[ 9];
    r[10] = a[10] + b[10];
    r[11] = a[11] + b[11];
    r[12] = a[12] + b[12];
    r[13] = a[13] + b[13];
    r[14] = a[14] + b[14];
    r[15] = a[15] + b[15];
}

/**
 * @brief Subtract one field element from another
 * @param[out] r Result of the subtraction, i.e. the difference r = a - b
 * @param[in] a Minuend
 * @param[in] b Subtrahend
 */
void fe448_sub(fe448 r, const fe448 a, const fe448 b)
{
    /* Check against underflow */
    FE3C_SANITY_CHECK(b[ 0] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(b));
    FE3C_SANITY_CHECK(b[ 1] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(b));
    FE3C_SANITY_CHECK(b[ 2] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(b));
    FE3C_SANITY_CHECK(b[ 3] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(b));
    FE3C_SANITY_CHECK(b[ 4] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(b));
    FE3C_SANITY_CHECK(b[ 5] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(b));
    FE3C_SANITY_CHECK(b[ 6] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(b));
    FE3C_SANITY_CHECK(b[ 7] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(b));
    FE3C_SANITY_CHECK(b[ 8] <= 0x1ffffffc, FE448_STR, FE448_TO_STR(b));
    FE3C_SANITY_CHECK(b[ 9] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(b));
    FE3C_SANITY_CHECK(b[10] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(b));
    FE3C_SANITY_CHECK(b[11] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(b));
    FE3C_SANITY_CHECK(b[12] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(b));
    FE3C_SANITY_CHECK(b[13] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(b));
    FE3C_SANITY_CHECK(b[14] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(b));
    FE3C_SANITY_CHECK(b[15] <= 0x1ffffffe, FE448_STR, FE448_TO_STR(b));

    /* Set r to 2p-a so as to not require strong reduction of a */
    r[ 0] = a[ 0] + 0x1ffffffe - b[ 0];
    r[ 1] = a[ 1] + 0x1ffffffe - b[ 1];
    r[ 2] = a[ 2] + 0x1ffffffe - b[ 2];
    r[ 3] = a[ 3] + 0x1ffffffe - b[ 3];
    r[ 4] = a[ 4] + 0x1ffffffe - b[ 4];
    r[ 5] = a[ 5] + 0x1ffffffe - b[ 5];
    r[ 6] = a[ 6] + 0x1ffffffe - b[ 6];
    r[ 7] = a[ 7] + 0x1ffffffe - b[ 7];
    r[ 8] = a[ 8] + 0x1ffffffc - b[ 8];
    r[ 9] = a[ 9] + 0x1ffffffe - b[ 9];
    r[10] = a[10] + 0x1ffffffe - b[10];
    r[11] = a[11] + 0x1ffffffe - b[11];
    r[12] = a[12] + 0x1ffffffe - b[12];
    r[13] = a[13] + 0x1ffffffe - b[13];
    r[14] = a[14] + 0x1ffffffe - b[14];
    r[15] = a[15] + 0x1ffffffe - b[15];
    /* We could also call fe448_neg() followed by fe448_add(), but this would require
     * an intermediate fe variable to support aliasing */
}

/**
 * @brief Multiply two field elements
 * @param[out] r Result of the multiplication, i.e. the product r = a b
 * @param[in] a Operand
 * @param[in] b Operand
 */
void fe448_mul(fe448 r, const fe448 a, const fe448 b)
{
    /* Note that we are using the so-called "golden-ratio prime" which facilitates fast
     * Karatsuba multiplication. Let S = 2^224. Then:
     *
     *   (x + zS)(u + vS) = xu + (xv + zu)S + zvS^2 =
     *                    = (xu + zv) + (xv + zu + zv)S (mod p)
     *                    = (xu + zv) + ((x+z)(u+v) - xu)S (mod p)
     *
     * since p = S^2 - S - 1. Note that in our representation the terms x and z (u and v)
     * correspond to limbs 0-7 and 8-15, respectively.
     */

    u64 a0  = a[ 0];
    u64 a1  = a[ 1];
    u64 a2  = a[ 2];
    u64 a3  = a[ 3];
    u64 a4  = a[ 4];
    u64 a5  = a[ 5];
    u64 a6  = a[ 6];
    u64 a7  = a[ 7];
    u64 a8  = a[ 8];
    u64 a9  = a[ 9];
    u64 a10 = a[10];
    u64 a11 = a[11];
    u64 a12 = a[12];
    u64 a13 = a[13];
    u64 a14 = a[14];
    u64 a15 = a[15];

    u64 b0  = b[ 0];
    u64 b1  = b[ 1];
    u64 b2  = b[ 2];
    u64 b3  = b[ 3];
    u64 b4  = b[ 4];
    u64 b5  = b[ 5];
    u64 b6  = b[ 6];
    u64 b7  = b[ 7];
    u64 b8  = b[ 8];
    u64 b9  = b[ 9];
    u64 b10 = b[10];
    u64 b11 = b[11];
    u64 b12 = b[12];
    u64 b13 = b[13];
    u64 b14 = b[14];
    u64 b15 = b[15];

    /* Do a simple schoolbook multiplication of (x+z)(u+v). Let:
     *
     *   m = (x+z) = (m7, m6, m5, m4, m3, m2, m1, m0) and k = (u+v) = (k7, k6, k5, k4, k3, k2, k1, k0)
     *
     * where mi, ki are all 28-bit digits for i=0,1,2,3,4,5,6,7. We shall compute simple products and add
     * them together column by column:
     *
     *                                         m7   m6   m5   m4   m3   m2   m1   m0
     *                                         k7   k6   k5   k4   k3   k2   k1   k0
     * -------------------------------------------------------------------------------
     *                                      | m0k7 m0k6 m0k5 m0k4 m0k3 m0k2 m0k1 m0k0
     *                                 m1k7 | m1k6 m1k5 m1k4 m1k3 m1k2 m1k1 m1k0
     *                            m2k7 m2k6 | m2k5 m2k4 m2k3 m2k2 m2k1 m2k0
     *                       m3k7 m3k6 m3k5 | m3k4 m3k3 m3k2 m3k1 m3k0
     *                  m4k7 m4k6 m4k5 m4k4 | m4k3 m4k2 m4k1 m4k0
     *             m5k7 m5k6 m5k5 m5k4 m5k3 | m5k2 m5k1 m5k0
     *        m6k7 m6k6 m6k5 m6k4 m6k3 m6k2 | m6k1 m6k0
     *   m7k7 m7k6 m7k5 m7k4 m7k3 m7k2 m7k1 | m7k0
     *                                      |
     *                   2^224 cutoff_______|
     *
     * Now recall that we are interested in the expression ((x+z)(u+v) - xu)2^224
     * and so the terms to the left of the 2^224 cutoff point above will ultimately
     * exceed 2^448 and so must be brought down using the identity:
     *
     *   n mod p = (2^448 e + 2^224 f + g) mod (2^448 - 2^224 - 1)
     *           = (g + (2^448 - 2^224 - 1)e + 2^224 e + e + 2^224 f) mod (2^448 - 2^224 - 1)
     *           = (g + 2^224 (e + f) + e) mod (2^448 - 2^224 - 1)
     *
     * (cf. comments in fe448_weak_reduce()). In our case the e term corresponds to the
     * partial products mikj where i+j>7.
     */

    u64 m0 = a0 + a8;
    u64 m1 = a1 + a9;
    u64 m2 = a2 + a10;
    u64 m3 = a3 + a11;
    u64 m4 = a4 + a12;
    u64 m5 = a5 + a13;
    u64 m6 = a6 + a14;
    u64 m7 = a7 + a15;

    u64 k0 = b0 + b8;
    u64 k1 = b1 + b9;
    u64 k2 = b2 + b10;
    u64 k3 = b3 + b11;
    u64 k4 = b4 + b12;
    u64 k5 = b5 + b13;
    u64 k6 = b6 + b14;
    u64 k7 = b7 + b15;

    u64 r0  =         m1*k7 + m2*k6 + m3*k5 + m4*k4 + m5*k3 + m6*k2 + m7*k1;
    u64 r1  =                 m2*k7 + m3*k6 + m4*k5 + m5*k4 + m6*k3 + m7*k2;
    u64 r2  =                         m3*k7 + m4*k6 + m5*k5 + m6*k4 + m7*k3;
    u64 r3  =                                 m4*k7 + m5*k6 + m6*k5 + m7*k4;
    u64 r4  =                                         m5*k7 + m6*k6 + m7*k5;
    u64 r5  =                                                 m6*k7 + m7*k6;
    u64 r6  =                                                         m7*k7;
    u64 r7  = 0;
    /* Add at the 2^224 "level" the terms which exceeded 2^448 (e in the above identity),
     * which are currently held in r0-r7 */
    u64 r8  = r0 + m0*k0;
    u64 r9  = r1 + m0*k1 + m1*k0;
    u64 r10 = r2 + m0*k2 + m1*k1 + m2*k0;
    u64 r11 = r3 + m0*k3 + m1*k2 + m2*k1 + m3*k0;
    u64 r12 = r4 + m0*k4 + m1*k3 + m2*k2 + m3*k1 + m4*k0;
    u64 r13 = r5 + m0*k5 + m1*k4 + m2*k3 + m3*k2 + m4*k1 + m5*k0;
    u64 r14 = r6 + m0*k6 + m1*k5 + m2*k4 + m3*k3 + m4*k2 + m5*k1 + m6*k0;
    u64 r15 =      m0*k7 + m1*k6 + m2*k5 + m3*k4 + m4*k3 + m5*k2 + m6*k1 + m7*k0;

    /* Do a schoolbook multiplication for x*u and z*v as well. Here, however, we do not risk
     * exceeding 2^448 since all inputs are bound by 2^224. */
    u64 xu0  = a0*b0;
    u64 xu1  = a0*b1 + a1*b0;
    u64 xu2  = a0*b2 + a1*b1 + a2*b0;
    u64 xu3  = a0*b3 + a1*b2 + a2*b1 + a3*b0;
    u64 xu4  = a0*b4 + a1*b3 + a2*b2 + a3*b1 + a4*b0;
    u64 xu5  = a0*b5 + a1*b4 + a2*b3 + a3*b2 + a4*b1 + a5*b0;
    u64 xu6  = a0*b6 + a1*b5 + a2*b4 + a3*b3 + a4*b2 + a5*b1 + a6*b0;
    u64 xu7  = a0*b7 + a1*b6 + a2*b5 + a3*b4 + a4*b3 + a5*b2 + a6*b1 + a7*b0;
    u64 xu8  =         a1*b7 + a2*b6 + a3*b5 + a4*b4 + a5*b3 + a6*b2 + a7*b1;
    u64 xu9  =                 a2*b7 + a3*b6 + a4*b5 + a5*b4 + a6*b3 + a7*b2;
    u64 xu10 =                         a3*b7 + a4*b6 + a5*b5 + a6*b4 + a7*b3;
    u64 xu11 =                                 a4*b7 + a5*b6 + a6*b5 + a7*b4;
    u64 xu12 =                                         a5*b7 + a6*b6 + a7*b5;
    u64 xu13 =                                                 a6*b7 + a7*b6;
    u64 xu14 =                                                         a7*b7;

    u64 zv0  = a8*b8;
    u64 zv1  = a8*b9  + a9*b8;
    u64 zv2  = a8*b10 + a9*b9 +  a10*b8;
    u64 zv3  = a8*b11 + a9*b10 + a10*b9  + a11*b8;
    u64 zv4  = a8*b12 + a9*b11 + a10*b10 + a11*b9  + a12*b8;
    u64 zv5  = a8*b13 + a9*b12 + a10*b11 + a11*b10 + a12*b9  + a13*b8;
    u64 zv6  = a8*b14 + a9*b13 + a10*b12 + a11*b11 + a12*b10 + a13*b9  + a14*b8;
    u64 zv7  = a8*b15 + a9*b14 + a10*b13 + a11*b12 + a12*b11 + a13*b10 + a14*b9  + a15*b8;
    u64 zv8  =          a9*b15 + a10*b14 + a11*b13 + a12*b12 + a13*b11 + a14*b10 + a15*b9;
    u64 zv9  =                   a10*b15 + a11*b14 + a12*b13 + a13*b12 + a14*b11 + a15*b10;
    u64 zv10 =                             a11*b15 + a12*b14 + a13*b13 + a14*b12 + a15*b11;
    u64 zv11 =                                       a12*b15 + a13*b14 + a14*b13 + a15*b12;
    u64 zv12 =                                                 a13*b15 + a14*b14 + a15*b13;
    u64 zv13 =                                                           a14*b15 + a15*b14;
    u64 zv14 =                                                                     a15*b15;

    /* Subtract xu 2^224 */
    r0  -= xu8;
    r1  -= xu9;
    r2  -= xu10;
    r3  -= xu11;
    r4  -= xu12;
    r5  -= xu13;
    r6  -= xu14;
    r7  -= 0;
    r8  -= xu0 + xu8;
    r9  -= xu1 + xu9;
    r10 -= xu2 + xu10;
    r11 -= xu3 + xu11;
    r12 -= xu4 + xu12;
    r13 -= xu5 + xu13;
    r14 -= xu6 + xu14;
    r15 -= xu7;

    r1  += r0  >> 28;  r0  &= LOW_28_BITS_MASK;
    r2  += r1  >> 28;  r1  &= LOW_28_BITS_MASK;
    r3  += r2  >> 28;  r2  &= LOW_28_BITS_MASK;
    r4  += r3  >> 28;  r3  &= LOW_28_BITS_MASK;
    r5  += r4  >> 28;  r4  &= LOW_28_BITS_MASK;
    r6  += r5  >> 28;  r5  &= LOW_28_BITS_MASK;
    r7  += r6  >> 28;  r6  &= LOW_28_BITS_MASK;
    r8  += r7  >> 28;  r7  &= LOW_28_BITS_MASK;
    r9  += r8  >> 28;  r8  &= LOW_28_BITS_MASK;
    r10 += r9  >> 28;  r9  &= LOW_28_BITS_MASK;
    r11 += r10 >> 28;  r10 &= LOW_28_BITS_MASK;
    r12 += r11 >> 28;  r11 &= LOW_28_BITS_MASK;
    r13 += r12 >> 28;  r12 &= LOW_28_BITS_MASK;
    r14 += r13 >> 28;  r13 &= LOW_28_BITS_MASK;
    r15 += r14 >> 28;  r14 &= LOW_28_BITS_MASK;
    r0  += r15 >> 28;
    r8  += r15 >> 28;
    r15 &= LOW_28_BITS_MASK;

    /* Add (xu + zv) */
    r0  +=  xu0 + zv0;
    r1  +=  xu1 + zv1;
    r2  +=  xu2 + zv2;
    r3  +=  xu3 + zv3;
    r4  +=  xu4 + zv4;
    r5  +=  xu5 + zv5;
    r6  +=  xu6 + zv6;
    r7  +=  xu7 + zv7;
    r8  +=  xu8 + zv8;
    r9  +=  xu9 + zv9;
    r10 += xu10 + zv10;
    r11 += xu11 + zv11;
    r12 += xu12 + zv12;
    r13 += xu13 + zv13;
    r14 += xu14 + zv14;

    r1  += r0  >> 28;  r0  &= LOW_28_BITS_MASK;
    r2  += r1  >> 28;  r1  &= LOW_28_BITS_MASK;
    r3  += r2  >> 28;  r2  &= LOW_28_BITS_MASK;
    r4  += r3  >> 28;  r3  &= LOW_28_BITS_MASK;
    r5  += r4  >> 28;  r4  &= LOW_28_BITS_MASK;
    r6  += r5  >> 28;  r5  &= LOW_28_BITS_MASK;
    r7  += r6  >> 28;  r6  &= LOW_28_BITS_MASK;
    r8  += r7  >> 28;  r7  &= LOW_28_BITS_MASK;
    r9  += r8  >> 28;  r8  &= LOW_28_BITS_MASK;
    r10 += r9  >> 28;  r9  &= LOW_28_BITS_MASK;
    r11 += r10 >> 28;  r10 &= LOW_28_BITS_MASK;
    r12 += r11 >> 28;  r11 &= LOW_28_BITS_MASK;
    r13 += r12 >> 28;  r12 &= LOW_28_BITS_MASK;
    r14 += r13 >> 28;  r13 &= LOW_28_BITS_MASK;
    r15 += r14 >> 28;  r14 &= LOW_28_BITS_MASK;
    r0  += r15 >> 28;
    r8  += r15 >> 28;
    r15 &= LOW_28_BITS_MASK;

    r[ 0] = r0;
    r[ 1] = r1;
    r[ 2] = r2;
    r[ 3] = r3;
    r[ 4] = r4;
    r[ 5] = r5;
    r[ 6] = r6;
    r[ 7] = r7;
    r[ 8] = r8;
    r[ 9] = r9;
    r[10] = r10;
    r[11] = r11;
    r[12] = r12;
    r[13] = r13;
    r[14] = r14;
    r[15] = r15;
}

/**
 * @brief Multiply an element by itself
 * @param[out] r Result of the squaring, i.e. the product r = a a
 * @param[in] a Field element to square
 */
void fe448_square(fe448 r, const fe448 a)
{
#if !FE3C_FAST_SQUARING
    fe448_mul(r, a, a);
#else
    u64 a0  = a[ 0];
    u64 a1  = a[ 1];
    u64 a2  = a[ 2];
    u64 a3  = a[ 3];
    u64 a4  = a[ 4];
    u64 a5  = a[ 5];
    u64 a6  = a[ 6];
    u64 a7  = a[ 7];
    u64 a8  = a[ 8];
    u64 a9  = a[ 9];
    u64 a10 = a[10];
    u64 a11 = a[11];
    u64 a12 = a[12];
    u64 a13 = a[13];
    u64 a14 = a[14];
    u64 a15 = a[15];

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
     * correspond to limbs 0-7 and 8-15, respectively. Also note that the reason we gain
     * anything by using Karatsuba's trick in the last line of the above identity is
     * that squaring can be optimized by grouping like terms.
     */

    u64 m0 = a0 + a8;
    u64 m1 = a1 + a9;
    u64 m2 = a2 + a10;
    u64 m3 = a3 + a11;
    u64 m4 = a4 + a12;
    u64 m5 = a5 + a13;
    u64 m6 = a6 + a14;
    u64 m7 = a7 + a15;

    /* Use the simple schoolbook multiplication algorithm to square (x+z) */
    u64 r0  =         m1*m7 + m2*m6 + m3*m5 + m4*m4 + m5*m3 + m6*m2 + m7*m1;
    u64 r1  =                 m2*m7 + m3*m6 + m4*m5 + m5*m4 + m6*m3 + m7*m2;
    u64 r2  =                         m3*m7 + m4*m6 + m5*m5 + m6*m4 + m7*m3;
    u64 r3  =                                 m4*m7 + m5*m6 + m6*m5 + m7*m4;
    u64 r4  =                                         m5*m7 + m6*m6 + m7*m5;
    u64 r5  =                                                 m6*m7 + m7*m6;
    u64 r6  =                                                         m7*m7;
    u64 r7  = 0;
    /* Add at the 2^224 "level" the terms which exceeded 2^448 which are currently held in r0-r3.
     * See comments in fe448_mul() for a more detailed explanation. */
    u64 r8  = r0 + m0*m0;
    u64 r9  = r1 + m0*m1 + m1*m0;
    u64 r10 = r2 + m0*m2 + m1*m1 + m2*m0;
    u64 r11 = r3 + m0*m3 + m1*m2 + m2*m1 + m3*m0;
    u64 r12 = r4 + m0*m4 + m1*m3 + m2*m2 + m3*m1 + m4*m0;
    u64 r13 = r5 + m0*m5 + m1*m4 + m2*m3 + m3*m2 + m4*m1 + m5*m0;
    u64 r14 = r6 + m0*m6 + m1*m5 + m2*m4 + m3*m3 + m4*m2 + m5*m1 + m6*m0;
    u64 r15 =      m0*m7 + m1*m6 + m2*m5 + m3*m4 + m4*m3 + m5*m2 + m6*m1 + m7*m0;

    /* Use schoolbook multiplication to square x and z as well, this time without risk of
     * exceeding 2^448 since all inputs are bound by 2^224. */
    u64 xx0  = a0*a0;
    u64 xx1  = a0*a1 + a1*a0;
    u64 xx2  = a0*a2 + a1*a1 + a2*a0;
    u64 xx3  = a0*a3 + a1*a2 + a2*a1 + a3*a0;
    u64 xx4  = a0*a4 + a1*a3 + a2*a2 + a3*a1 + a4*a0;
    u64 xx5  = a0*a5 + a1*a4 + a2*a3 + a3*a2 + a4*a1 + a5*a0;
    u64 xx6  = a0*a6 + a1*a5 + a2*a4 + a3*a3 + a4*a2 + a5*a1 + a6*a0;
    u64 xx7  = a0*a7 + a1*a6 + a2*a5 + a3*a4 + a4*a3 + a5*a2 + a6*a1 + a7*a0;
    u64 xx8  =         a1*a7 + a2*a6 + a3*a5 + a4*a4 + a5*a3 + a6*a2 + a7*a1;
    u64 xx9  =                 a2*a7 + a3*a6 + a4*a5 + a5*a4 + a6*a3 + a7*a2;
    u64 xx10 =                         a3*a7 + a4*a6 + a5*a5 + a6*a4 + a7*a3;
    u64 xx11 =                                 a4*a7 + a5*a6 + a6*a5 + a7*a4;
    u64 xx12 =                                         a5*a7 + a6*a6 + a7*a5;
    u64 xx13 =                                                 a6*a7 + a7*a6;
    u64 xx14 =                                                         a7*a7;

    u64 zz0  = a8*a8;
    u64 zz1  = a8*a9  + a9*a8;
    u64 zz2  = a8*a10 + a9*a9 +  a10*a8;
    u64 zz3  = a8*a11 + a9*a10 + a10*a9  + a11*a8;
    u64 zz4  = a8*a12 + a9*a11 + a10*a10 + a11*a9  + a12*a8;
    u64 zz5  = a8*a13 + a9*a12 + a10*a11 + a11*a10 + a12*a9  + a13*a8;
    u64 zz6  = a8*a14 + a9*a13 + a10*a12 + a11*a11 + a12*a10 + a13*a9  + a14*a8;
    u64 zz7  = a8*a15 + a9*a14 + a10*a13 + a11*a12 + a12*a11 + a13*a10 + a14*a9  + a15*a8;
    u64 zz8  =          a9*a15 + a10*a14 + a11*a13 + a12*a12 + a13*a11 + a14*a10 + a15*a9;
    u64 zz9  =                   a10*a15 + a11*a14 + a12*a13 + a13*a12 + a14*a11 + a15*a10;
    u64 zz10 =                             a11*a15 + a12*a14 + a13*a13 + a14*a12 + a15*a11;
    u64 zz11 =                                       a12*a15 + a13*a14 + a14*a13 + a15*a12;
    u64 zz12 =                                                 a13*a15 + a14*a14 + a15*a13;
    u64 zz13 =                                                           a14*a15 + a15*a14;
    u64 zz14 =                                                                     a15*a15;

    /* Subtract x^2 2^224 */
    r0  -= xx8;
    r1  -= xx9;
    r2  -= xx10;
    r3  -= xx11;
    r4  -= xx12;
    r5  -= xx13;
    r6  -= xx14;
    r7  -= 0;
    r8  -= xx0 + xx8;
    r9  -= xx1 + xx9;
    r10 -= xx2 + xx10;
    r11 -= xx3 + xx11;
    r12 -= xx4 + xx12;
    r13 -= xx5 + xx13;
    r14 -= xx6 + xx14;
    r15 -= xx7;

    r1  += r0  >> 28;  r0  &= LOW_28_BITS_MASK;
    r2  += r1  >> 28;  r1  &= LOW_28_BITS_MASK;
    r3  += r2  >> 28;  r2  &= LOW_28_BITS_MASK;
    r4  += r3  >> 28;  r3  &= LOW_28_BITS_MASK;
    r5  += r4  >> 28;  r4  &= LOW_28_BITS_MASK;
    r6  += r5  >> 28;  r5  &= LOW_28_BITS_MASK;
    r7  += r6  >> 28;  r6  &= LOW_28_BITS_MASK;
    r8  += r7  >> 28;  r7  &= LOW_28_BITS_MASK;
    r9  += r8  >> 28;  r8  &= LOW_28_BITS_MASK;
    r10 += r9  >> 28;  r9  &= LOW_28_BITS_MASK;
    r11 += r10 >> 28;  r10 &= LOW_28_BITS_MASK;
    r12 += r11 >> 28;  r11 &= LOW_28_BITS_MASK;
    r13 += r12 >> 28;  r12 &= LOW_28_BITS_MASK;
    r14 += r13 >> 28;  r13 &= LOW_28_BITS_MASK;
    r15 += r14 >> 28;  r14 &= LOW_28_BITS_MASK;
    r0  += r15 >> 28;
    r8  += r15 >> 28;
    r15 &= LOW_28_BITS_MASK;

    /* Add (x^2 + z^2) */
    r0  +=  xx0 + zz0;
    r1  +=  xx1 + zz1;
    r2  +=  xx2 + zz2;
    r3  +=  xx3 + zz3;
    r4  +=  xx4 + zz4;
    r5  +=  xx5 + zz5;
    r6  +=  xx6 + zz6;
    r7  +=  xx7 + zz7;
    r8  +=  xx8 + zz8;
    r9  +=  xx9 + zz9;
    r10 += xx10 + zz10;
    r11 += xx11 + zz11;
    r12 += xx12 + zz12;
    r13 += xx13 + zz13;
    r14 += xx14 + zz14;

    r1  += r0  >> 28;  r0  &= LOW_28_BITS_MASK;
    r2  += r1  >> 28;  r1  &= LOW_28_BITS_MASK;
    r3  += r2  >> 28;  r2  &= LOW_28_BITS_MASK;
    r4  += r3  >> 28;  r3  &= LOW_28_BITS_MASK;
    r5  += r4  >> 28;  r4  &= LOW_28_BITS_MASK;
    r6  += r5  >> 28;  r5  &= LOW_28_BITS_MASK;
    r7  += r6  >> 28;  r6  &= LOW_28_BITS_MASK;
    r8  += r7  >> 28;  r7  &= LOW_28_BITS_MASK;
    r9  += r8  >> 28;  r8  &= LOW_28_BITS_MASK;
    r10 += r9  >> 28;  r9  &= LOW_28_BITS_MASK;
    r11 += r10 >> 28;  r10 &= LOW_28_BITS_MASK;
    r12 += r11 >> 28;  r11 &= LOW_28_BITS_MASK;
    r13 += r12 >> 28;  r12 &= LOW_28_BITS_MASK;
    r14 += r13 >> 28;  r13 &= LOW_28_BITS_MASK;
    r15 += r14 >> 28;  r14 &= LOW_28_BITS_MASK;
    r0  += r15 >> 28;
    r8  += r15 >> 28;
    r15 &= LOW_28_BITS_MASK;

    r[ 0] = r0;
    r[ 1] = r1;
    r[ 2] = r2;
    r[ 3] = r3;
    r[ 4] = r4;
    r[ 5] = r5;
    r[ 6] = r6;
    r[ 7] = r7;
    r[ 8] = r8;
    r[ 9] = r9;
    r[10] = r10;
    r[11] = r11;
    r[12] = r12;
    r[13] = r13;
    r[14] = r14;
    r[15] = r15;
#endif /* !FE3C_FAST_SQUARING */
}

/**
 * @brief Encode a field element according to RFC 8032
 * @param[out] buffer Output buffer for the encoded field element
 * @param[in] a Field element to encode
 */
void fe448_encode(u8 *buffer, fe448 a)
{
    /* Canonicalize the element first */
    fe448_strong_reduce(a, a);

    /* Store the temporary results of bit operations in separate variables (mapped to separate
     * registers) to allow for greater instruction-level parallelism */

    /* Store the lowest limb + whatever can fit (4 bits) of the second lowest limb */
    u32 t0  = ( a[ 0] >>  0 ) | ( a[ 1] << 28 );
    /* 4 bits of a[1] are in t0, store the rest (24 bits) here + whatever can fit
     * (8 bits) of a[2] */
    u32 t1  = ( a[ 1] >>  4 ) | ( a[ 2] << 24 );
    /* 8 bits of a[2] are in t1, store the rest (20 bits) here + whatever can fit
     * (12 bits) of a[3] */
    u32 t2  = ( a[ 2] >>  8 ) | ( a[ 3] << 20 );
    /* 12 bits of a[3] are in t2, store the rest (16 bits) here + whatever can fit
     * (16 bits) of a[4] */
    u32 t3  = ( a[ 3] >> 12 ) | ( a[ 4] << 16 );
    /* 16 bits of a[4] are in t3, store the rest (12 bits) here + whatever can fit
     * (20 bits) of a[5] */
    u32 t4  = ( a[ 4] >> 16 ) | ( a[ 5] << 12 );
    /* 20 bits of a[5] are in t4, store the rest (8 bits) here + whatever can fit
     * (24 bits) of a[6] */
    u32 t5  = ( a[ 5] >> 20 ) | ( a[ 6] <<  8 );
    /* 24 bits of a[6] are in t5, store the rest (4 bits) here + all of a[7] */
    u32 t6  = ( a[ 6] >> 24 ) | ( a[ 7] <<  4 );
    /* Repeat the steps from above */
    u32 t7  = ( a[ 8] >>  0 ) | ( a[ 9] << 28 );
    u32 t8  = ( a[ 9] >>  4 ) | ( a[10] << 24 );
    u32 t9  = ( a[10] >>  8 ) | ( a[11] << 20 );
    u32 t10 = ( a[11] >> 12 ) | ( a[12] << 16 );
    u32 t11 = ( a[12] >> 16 ) | ( a[13] << 12 );
    u32 t12 = ( a[13] >> 20 ) | ( a[14] <<  8 );
    u32 t13 = ( a[14] >> 24 ) | ( a[15] <<  4 );

    /* The field elements get encoded as little-endian byte strings according to RFC 8032 */
    store_32(&buffer[ 0 * 4], t0);
    store_32(&buffer[ 1 * 4], t1);
    store_32(&buffer[ 2 * 4], t2);
    store_32(&buffer[ 3 * 4], t3);
    store_32(&buffer[ 4 * 4], t4);
    store_32(&buffer[ 5 * 4], t5);
    store_32(&buffer[ 6 * 4], t6);
    store_32(&buffer[ 7 * 4], t7);
    store_32(&buffer[ 8 * 4], t8);
    store_32(&buffer[ 9 * 4], t9);
    store_32(&buffer[10 * 4], t10);
    store_32(&buffer[11 * 4], t11);
    store_32(&buffer[12 * 4], t12);
    store_32(&buffer[13 * 4], t13);
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
int fe448_decode(fe448 r, const u8 *buffer)
{
    r[ 0] = ( _load_32(&buffer[ 0]) >>  0 ) & LOW_28_BITS_MASK;
    /* Offset by 28 bits, i.e. three full bytes and a shift of four bits */
    r[ 1] = ( _load_32(&buffer[ 3]) >>  4 ) & LOW_28_BITS_MASK;
    /* Note that at this point the two shifts by four have added up to a
     * full byte which we incorporate into the offset (array index) */
    r[ 2] = ( _load_32(&buffer[ 7]) >>  0 ) & LOW_28_BITS_MASK;
    /* Rinse and repeat */
    r[ 3] = ( _load_32(&buffer[10]) >>  4 ) & LOW_28_BITS_MASK;
    r[ 4] = ( _load_32(&buffer[14]) >>  0 ) & LOW_28_BITS_MASK;
    r[ 5] = ( _load_32(&buffer[17]) >>  4 ) & LOW_28_BITS_MASK;
    r[ 6] = ( _load_32(&buffer[21]) >>  0 ) & LOW_28_BITS_MASK;
    r[ 7] = ( _load_32(&buffer[24]) >>  4 ) & LOW_28_BITS_MASK;
    r[ 8] = ( _load_32(&buffer[28]) >>  0 ) & LOW_28_BITS_MASK;
    r[ 9] = ( _load_32(&buffer[31]) >>  4 ) & LOW_28_BITS_MASK;
    r[10] = ( _load_32(&buffer[35]) >>  0 ) & LOW_28_BITS_MASK;
    r[11] = ( _load_32(&buffer[38]) >>  4 ) & LOW_28_BITS_MASK;
    r[12] = ( _load_32(&buffer[42]) >>  0 ) & LOW_28_BITS_MASK;
    r[13] = ( _load_32(&buffer[45]) >>  4 ) & LOW_28_BITS_MASK;
    r[14] = ( _load_32(&buffer[49]) >>  0 ) & LOW_28_BITS_MASK;
    r[15] = ( _load_32(&buffer[52]) >>  4 ) & LOW_28_BITS_MASK;

    /* Check that the last byte is cleared (except for possibly the highest bit)
     * and that the rest of the bytes (which we have just parsed into limbs)
     * encode a canonical integer (i.e. smaller than p) */
    return fe448_is_canonical(r) & ( (buffer[56] & 0x7F) == 0 );
}
