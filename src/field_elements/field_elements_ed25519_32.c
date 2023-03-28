#include <field_elements/field_elements_ed25519.h>
#include <utils/utils.h>

#if !FE3C_32BIT
    #error "Build system inconsistency detected! field_elements_ed25519_32.c in use despite FE3C_32BIT not being set"
#endif /* !FE3C_32BIT */

/* Use a mixed-radix representation, the first (0th) limb is 26-bits long, the next is 25-bits long, the one after
 * that - 26-bits long - and so on. */
#define LOW_25_BITS_MASK  0x1ffffffU
#define LOW_26_BITS_MASK  0x3ffffffU

/* Elliptic curve constant d = -121665/121666 */
const fe25519 ed25519_d = {
    0x35978a3, 0xd37284, 0x3156ebd, 0x6a0a0e, 0x1c029, 0x179e898, 0x3a03cbb, 0x1ce7198, 0x2e2b6ff, 0x1480db3
};
/* Additive identity in the field */
const fe25519 fe25519_zero = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
/* Multiplicative identity in the field */
const fe25519 fe25519_one = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
/* "Positive" (even) square root of -1 in the field */
const fe25519 fe25519_i = {
    0x20ea0b0, 0x186c9d2, 0x8f189d, 0x35697f, 0xbd0c60, 0x1fbd7a7, 0x2804c9e, 0x1e16569, 0x4fc1d, 0xae0c92
};

static inline u32 _load_32(const u8 src[4]) {

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

static inline void _store_32(u8 dst[4], u32 src) {

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
void fe25519_copy(fe25519 r, const fe25519 a) {

    r[0] = a[0];
    r[1] = a[1];
    r[2] = a[2];
    r[3] = a[3];
    r[4] = a[4];
    r[5] = a[5];
    r[6] = a[6];
    r[7] = a[7];
    r[8] = a[8];
    r[9] = a[9];
}

/**
 * @brief Check if an elements is in its canonical form, i.e. less than the modulus
 * @param a Field element to check
 * @return 1 if a is in canonical form, 0 otherwise
 */
static inline int fe25519_is_canonical(const fe25519 a) {

    int canonical = 1;
    canonical &= (a[0] <  0x3ffffed);
    canonical &= (a[1] <= 0x1ffffff);
    canonical &= (a[2] <= 0x3ffffff);
    canonical &= (a[3] <= 0x1ffffff);
    canonical &= (a[4] <= 0x3ffffff);
    canonical &= (a[5] <= 0x1ffffff);
    canonical &= (a[6] <= 0x3ffffff);
    canonical &= (a[7] <= 0x1ffffff);
    canonical &= (a[8] <= 0x3ffffff);
    canonical &= (a[9] <= 0x1ffffff);
    return canonical;
}

/**
 * @brief Do a constant-time equality check
 * @param[in] a The first element to check
 * @param[in] b The second element to check
 * @return 1 if a = b, 0 otherwise
 * @note The elements should be reduced by the caller first
 */
int fe25519_equal(const fe25519 a, const fe25519 b) {

    fe_limb_type sum = 0;

    /* Do an XOR between the two elements, if they are equal this should amount
     * to zero */
    sum |= a[0] ^ b[0];
    sum |= a[1] ^ b[1];
    sum |= a[2] ^ b[2];
    sum |= a[3] ^ b[3];
    sum |= a[4] ^ b[4];
    sum |= a[5] ^ b[5];
    sum |= a[6] ^ b[6];
    sum |= a[7] ^ b[7];
    sum |= a[8] ^ b[8];
    sum |= a[9] ^ b[9];

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
void fe25519_conditional_move(fe25519 r, const fe25519 a, int move) {

    /* Set the mask to 0x00000000 if move is 0 or to 0xFFFFFFFF if it is 1 */
    const fe_limb_type mask = (fe_limb_type)( -(i32) move );

    /* The conditional move implementation relies on the identity (x ^ y) ^ y = x */

    /* Copy the current contents of r into the t array */
    fe_limb_type t0 = r[0];
    fe_limb_type t1 = r[1];
    fe_limb_type t2 = r[2];
    fe_limb_type t3 = r[3];
    fe_limb_type t4 = r[4];
    fe_limb_type t5 = r[5];
    fe_limb_type t6 = r[6];
    fe_limb_type t7 = r[7];
    fe_limb_type t8 = r[8];
    fe_limb_type t9 = r[9];

    /* XOR the t's with the limbs of a */
    fe_limb_type x0 = t0 ^ a[0];
    fe_limb_type x1 = t1 ^ a[1];
    fe_limb_type x2 = t2 ^ a[2];
    fe_limb_type x3 = t3 ^ a[3];
    fe_limb_type x4 = t4 ^ a[4];
    fe_limb_type x5 = t5 ^ a[5];
    fe_limb_type x6 = t6 ^ a[6];
    fe_limb_type x7 = t7 ^ a[7];
    fe_limb_type x8 = t8 ^ a[8];
    fe_limb_type x9 = t9 ^ a[9];

    /* Set the x array to all zeroes if move is zero or leave it be if move is one */
    x0 &= mask;
    x1 &= mask;
    x2 &= mask;
    x3 &= mask;
    x4 &= mask;
    x5 &= mask;
    x6 &= mask;
    x7 &= mask;
    x8 &= mask;
    x9 &= mask;

    /* If move=0 then x0-x9 are all zero and so we write back the limbs of r into r.
     * If move=1 then x0-x9 contain r XOR a and so by XORing back with the limbs of r we
     * write the limbs of a into r */
    r[0] = t0 ^ x0;
    r[1] = t1 ^ x1;
    r[2] = t2 ^ x2;
    r[3] = t3 ^ x3;
    r[4] = t4 ^ x4;
    r[5] = t5 ^ x5;
    r[6] = t6 ^ x6;
    r[7] = t7 ^ x7;
    r[8] = t8 ^ x8;
    r[9] = t9 ^ x9;
}

/**
 * @brief Do a weak modular reduction modulo the underlying field's characteristic p
 * @param[out] r Result of the reduction
 * @param[in] a Field element to be reduced
 * @note Note that the result need to be in canonical form, i.e. between 0 and p-1, it need only be less than 2p
 */
void fe25519_weak_reduce(fe25519 r, const fe25519 a) {

    /* Do a "relaxed" reduction (to borrow terminology form Michael Scott's "Slothful reduction" paper)
     * - this ensures the result is less than 2p (where p = 2^255 - 19) */

    /* Use 64-bit-wide auxiliary variables */
    u64 t0 = a[0];
    u64 t1 = a[1];
    u64 t2 = a[2];
    u64 t3 = a[3];
    u64 t4 = a[4];
    u64 t5 = a[5];
    u64 t6 = a[6];
    u64 t7 = a[7];
    u64 t8 = a[8];
    u64 t9 = a[9];

    /* The modulus can be expressed as p = s - c, where s = 2^255 and c = 19.
     * Express (split) the value a as a = x + sy. Then
     *
     *     a mod p = (x + sy) mod (s - c)
     *             = (x + (s - c)y + cy) mod (s - c)
     *             = (x + cy) mod m
     *
     * To split a into (x, y) we simply let the carries bubble up to the top
     * limb. Whatever has overflown the top limb is equal to y and in the limbs
     * we have the value x.
     */

    /* Add whatever has overflown in t0 (a[0]) to t1 (a[1])
     * and mask out the overflow in t0 */
    t1 += t0 >> 26;  t0 &= LOW_26_BITS_MASK;
    /* Rinse and repeat for the other limbs */
    t2 += t1 >> 25;  t1 &= LOW_25_BITS_MASK;
    t3 += t2 >> 26;  t2 &= LOW_26_BITS_MASK;
    t4 += t3 >> 25;  t3 &= LOW_25_BITS_MASK;
    t5 += t4 >> 26;  t4 &= LOW_26_BITS_MASK;
    t6 += t5 >> 25;  t5 &= LOW_25_BITS_MASK;
    t7 += t6 >> 26;  t6 &= LOW_26_BITS_MASK;
    t8 += t7 >> 25;  t7 &= LOW_25_BITS_MASK;
    t9 += t8 >> 26;  t8 &= LOW_26_BITS_MASK;
    /* Add c*y to x */
    t0 += 19 * (t9 >> 25);
    t9 &= LOW_25_BITS_MASK;

    /* Do this once more to ensure we correctly normalize
     * all integers possible to be represented in our system,
     * i.e. we don't get surprised by 19 * (t9 >> 25) leading
     * to overflow in t0 */
    t1 += t0 >> 26;  t0 &= LOW_26_BITS_MASK;
    t2 += t1 >> 25;  t1 &= LOW_25_BITS_MASK;
    t3 += t2 >> 26;  t2 &= LOW_26_BITS_MASK;
    t4 += t3 >> 25;  t3 &= LOW_25_BITS_MASK;
    t5 += t4 >> 26;  t4 &= LOW_26_BITS_MASK;
    t6 += t5 >> 25;  t5 &= LOW_25_BITS_MASK;
    t7 += t6 >> 26;  t6 &= LOW_26_BITS_MASK;
    t8 += t7 >> 25;  t7 &= LOW_25_BITS_MASK;
    t9 += t8 >> 26;  t8 &= LOW_26_BITS_MASK;
    t0 += 19 * (t9 >> 25);
    t9 &= LOW_25_BITS_MASK;

    r[0] = t0;
    r[1] = t1;
    r[2] = t2;
    r[3] = t3;
    r[4] = t4;
    r[5] = t5;
    r[6] = t6;
    r[7] = t7;
    r[8] = t8;
    r[9] = t9;
}

/**
 * @brief Do a strong modular reduction modulo the underlying field's characteristic p
 * @param[out] r Result of the reduction
 * @param[in] a Field element to be reduced
 * @note The result is guaranteed to be in canonical form, i.e. between 0 and p-1
 */
void fe25519_strong_reduce(fe25519 r, const fe25519 a) {

    fe25519_weak_reduce(r, a);
    /* After the weak reduction r is congruent to a and less than 2p */

    /* Compute r-p and conditionally use it as a result if r is larger than p */
    fe25519 t;
    /* Store r + 19 in t (we will subtract 2^255 from it resulting in t = r - p) */
    t[0] = r[0] + 19;
    t[1] = r[1];
    t[2] = r[2];
    t[3] = r[3];
    t[4] = r[4];
    t[5] = r[5];
    t[6] = r[6];
    t[7] = r[7];
    t[8] = r[8];
    t[9] = r[9];

    t[1] += t[0] >> 26;  t[0] &= LOW_26_BITS_MASK;
    t[2] += t[1] >> 25;  t[1] &= LOW_25_BITS_MASK;
    t[3] += t[2] >> 26;  t[2] &= LOW_26_BITS_MASK;
    t[4] += t[3] >> 25;  t[3] &= LOW_25_BITS_MASK;
    t[5] += t[4] >> 26;  t[4] &= LOW_26_BITS_MASK;
    t[6] += t[5] >> 25;  t[5] &= LOW_25_BITS_MASK;
    t[7] += t[6] >> 26;  t[6] &= LOW_26_BITS_MASK;
    t[8] += t[7] >> 25;  t[7] &= LOW_25_BITS_MASK;
    t[9] += t[8] >> 26;  t[8] &= LOW_26_BITS_MASK;

    /* At this point t[9] contains the highest limb of r + 19. Try subtracting
     * 2^255 - if we get an underflow this means that r < 2^255 - 19 and so r is the
     * final result. Otherwise we must return r - p (note that this includes the case
     * where r = p, as no underflow will occur then and t[9] will be equal to
     * zero). */
    t[9] -= (1ULL << 25);

    /* Check the highest bit of t[4] for underflow. If the highest bit is set then
     * underflow occurred and so we return r, otherwise we set r ::= t and return that */
    fe25519_conditional_move(r, t, (t[9] >> 31) ^ 1);
}

/**
 * @brief Negate an element in the field
 * @param[out] r The result of negation
 * @param[in] a Element to be negated
 */
void fe25519_neg(fe25519 r, const fe25519 a) {

    /* Check against underflow */
    FE3C_SANITY_CHECK(a[0] <= 0x7ffffda, FE25519_STR, FE25519_TO_STR(a));
    FE3C_SANITY_CHECK(a[1] <= 0x3fffffe, FE25519_STR, FE25519_TO_STR(a));
    FE3C_SANITY_CHECK(a[2] <= 0x7fffffe, FE25519_STR, FE25519_TO_STR(a));
    FE3C_SANITY_CHECK(a[3] <= 0x3fffffe, FE25519_STR, FE25519_TO_STR(a));
    FE3C_SANITY_CHECK(a[4] <= 0x7fffffe, FE25519_STR, FE25519_TO_STR(a));
    FE3C_SANITY_CHECK(a[5] <= 0x3fffffe, FE25519_STR, FE25519_TO_STR(a));
    FE3C_SANITY_CHECK(a[6] <= 0x7fffffe, FE25519_STR, FE25519_TO_STR(a));
    FE3C_SANITY_CHECK(a[7] <= 0x3fffffe, FE25519_STR, FE25519_TO_STR(a));
    FE3C_SANITY_CHECK(a[8] <= 0x7fffffe, FE25519_STR, FE25519_TO_STR(a));
    FE3C_SANITY_CHECK(a[9] <= 0x3fffffe, FE25519_STR, FE25519_TO_STR(a));

    /* Set r to 2p-a so as to not require strong reduction of a */
    r[0] = 0x7ffffda - a[0];
    r[1] = 0x3fffffe - a[1];
    r[2] = 0x7fffffe - a[2];
    r[3] = 0x3fffffe - a[3];
    r[4] = 0x7fffffe - a[4];
    r[5] = 0x3fffffe - a[5];
    r[6] = 0x7fffffe - a[6];
    r[7] = 0x3fffffe - a[7];
    r[8] = 0x7fffffe - a[8];
    r[9] = 0x3fffffe - a[9];
}

/**
 * @brief Add two field elements
 * @param[out] r Result of the addition, i.e. the sum r = a + b
 * @param[in] a Operand
 * @param[in] b Operand
 */
void fe25519_add(fe25519 r, const fe25519 a, const fe25519 b) {

    r[0] = a[0] + b[0];
    r[1] = a[1] + b[1];
    r[2] = a[2] + b[2];
    r[3] = a[3] + b[3];
    r[4] = a[4] + b[4];
    r[5] = a[5] + b[5];
    r[6] = a[6] + b[6];
    r[7] = a[7] + b[7];
    r[8] = a[8] + b[8];
    r[9] = a[9] + b[9];
}

/**
 * @brief Subtract one field element from another
 * @param[out] r Result of the subtraction, i.e. the difference r = a - b
 * @param[in] a Minuend
 * @param[in] b Subtrahend
 */
void fe25519_sub(fe25519 r, const fe25519 a, const fe25519 b) {

    /* Check against underflow */
    FE3C_SANITY_CHECK(b[0] <= 0x7ffffda, FE25519_STR, FE25519_TO_STR(b));
    FE3C_SANITY_CHECK(b[1] <= 0x3fffffe, FE25519_STR, FE25519_TO_STR(b));
    FE3C_SANITY_CHECK(b[2] <= 0x7fffffe, FE25519_STR, FE25519_TO_STR(b));
    FE3C_SANITY_CHECK(b[3] <= 0x3fffffe, FE25519_STR, FE25519_TO_STR(b));
    FE3C_SANITY_CHECK(b[4] <= 0x7fffffe, FE25519_STR, FE25519_TO_STR(b));
    FE3C_SANITY_CHECK(b[5] <= 0x3fffffe, FE25519_STR, FE25519_TO_STR(b));
    FE3C_SANITY_CHECK(b[6] <= 0x7fffffe, FE25519_STR, FE25519_TO_STR(b));
    FE3C_SANITY_CHECK(b[7] <= 0x3fffffe, FE25519_STR, FE25519_TO_STR(b));
    FE3C_SANITY_CHECK(b[8] <= 0x7fffffe, FE25519_STR, FE25519_TO_STR(b));
    FE3C_SANITY_CHECK(b[9] <= 0x3fffffe, FE25519_STR, FE25519_TO_STR(b));

    /* Compute a + 2p - b so as to not risk underflow */
    r[0] = a[0] + 0x7ffffda - b[0];
    r[1] = a[1] + 0x3fffffe - b[1];
    r[2] = a[2] + 0x7fffffe - b[2];
    r[3] = a[3] + 0x3fffffe - b[3];
    r[4] = a[4] + 0x7fffffe - b[4];
    r[5] = a[5] + 0x3fffffe - b[5];
    r[6] = a[6] + 0x7fffffe - b[6];
    r[7] = a[7] + 0x3fffffe - b[7];
    r[8] = a[8] + 0x7fffffe - b[8];
    r[9] = a[9] + 0x3fffffe - b[9];
    /* We could also call fe25519_neg() followed by fe25519_add(), but this would require
     * an intermediate fe variable to support aliasing */
}

/**
 * @brief Multiply two field elements
 * @param[out] r Result of the multiplication, i.e. the product r = a b
 * @param[in] a Operand
 * @param[in] b Operand
 */
void fe25519_mul(fe25519 r, const fe25519 a, const fe25519 b) {

    u64 r0, r1, r2, r3, r4, r5, r6, r7, r8, r9;

    u64 a0 = a[0];
    u64 a1 = a[1];
    u64 a2 = a[2];
    u64 a3 = a[3];
    u64 a4 = a[4];
    u64 a5 = a[5];
    u64 a6 = a[6];
    u64 a7 = a[7];
    u64 a8 = a[8];
    u64 a9 = a[9];

    u64 b0 = b[0];
    u64 b1 = b[1];
    u64 b2 = b[2];
    u64 b3 = b[3];
    u64 b4 = b[4];
    u64 b5 = b[5];
    u64 b6 = b[6];
    u64 b7 = b[7];
    u64 b8 = b[8];
    u64 b9 = b[9];

    /* Fuse the multiplication with (weak) reduction using the identity:
     *
     *                       a + b 2^255 = a + (2^255 - 19) b + 19 b = a + 19 b (mod p)
     *
     * We use the schoolbook multiplication algorithm (for now) which multiplies
     *
     *                              a = (a9, a8, a7, a6, a5, a4, a3, a2, a1, a0)
     *                              b = (b9, b8, b7, b6, b5, b4, b3, b2, b1, b0)
     *
     * (where ai, bi are 26-bit digits for i even and 25-bit digits for i odd) by computing the simple products
     * and adding them together in columns:
     *
     *                                                    (25) (26) (25) (26) (25) (26) (25) (26) (25) (26)
     *                                                     a9   a8   a7   a6   a5   a4   a3   a2   a1   a0
     *                                                     b9   b8   b7   b6   b5   b4   b3   b2   b1   b0
     * ------------------------------------------------------------------------------------------------------
     *                                                  | a0b9 a0b8 a0b7 a0b6 a0b5 a0b4 a0b3 a0b2 a0b1 a0b0
     *                                             a1b9 | a1b8 a1b7 a1b6 a1b5 a1b4 a1b3 a1b2 a1b1 a1b0
     *                                        a2b9 a2b8 | a2b7 a2b6 a2b5 a2b4 a2b3 a2b2 a2b1 a2b0
     *                                   a3b9 a3b8 a3b7 | a3b6 a3b5 a3b4 a3b3 a3b2 a3b1 a3b0
     *                              a4b9 a4b8 a4b7 a4b6 | a4b5 a4b4 a4b3 a4b2 a4b1 a4b0
     *                         a5b9 a5b8 a5b7 a5b6 a5b5 | a5b4 a5b3 a5b2 a5b1 a5b0
     *                    a6b9 a6b8 a6b7 a6b6 a6b5 a6b4 | a6b3 a6b2 a6b1 a6b0
     *               a7b9 a7b8 a7b7 a7b6 a7b5 a7b4 a7b3 | a7b2 a7b1 a7b0
     *          a8b9 a8b8 a8b7 a8b6 a8b5 a8b4 a8b3 a8b2 | a8b1 a8b0
     *     a9b9 a9b8 a9b7 a9b6 a9b5 a9b4 a9b3 a9b2 a9b1 | a9b0
     *                                                  |
     *                              2^255 cutoff________|
     *
     * All the partial products to the left of the 2^255 cutoff correspond to the "b" term in the identity above
     * and so are shifted down by 2^255 after multiplying by 19. Also keep in mind that we use a mixed-radix
     * representation of integers, and so some digits are 25-bits wide and some are 26-bits wide. This results
     * in some digits being multiplied by two to account for the difference in exponents (26-25). Consider
     * multiplying (x, y, z) * (a, b, c). Naive multiplication would result in:
     *
     *     zc*2^0 + (zb + yc)*2^26 + (za + yb + xc)*2^{26+25} + (ya + xb)^{26+25+26} + xa*2{26+25+26+25}
     *
     * but actually the partial product yb has a weight of 2^{26+26}. To account for this we multiply it by two
     * and put it in the limb of weight 2^{26+25}.
     */

    /* Start with the lowest-weight terms */
    r0 = a0*b0;
    /* Bring down the overflow from above the 2^255 cutoff point. Note that terms which are products of two odd-numbered limbs
     * are multiplied by two. This is because odd-numbered limbs have weights of the form two to the power of n*(26 + 25) + 26,
     * i.e. there is one more factor of 2^26 in their weighted value. This means that if we multiply two such limbs of weights,
     * say, exp{i*(26 + 25) + 26} and exp{j*(26 + 25) + 26} (where exp denotes exponentation with base 2), then the product has
     * weight exp{(i+j)(26 + 25) + 2*26}. We would like to put this result in an even-numbered limb, which has weight of the
     * form exp{k*(26+25)}. To allow this we multiply the value by two, thus reducing the weight of the product to the desired
     * exp{(i+j)(26 + 25) + 26 + 25} = exp{(i + j + 1)(26 + 25)}. */
    r0 += 19 * ( 2*a1*b9 + a2*b8 + 2*a3*b7 + a4*b6 + 2*a5*b5 + a6*b4 + 2*a7*b3 + a8*b2 + 2*a9*b1 );
    /* Any overflow in r0 put in r1 and clear it in r0 */
    r1 = r0 >> 26;  r0 &= LOW_26_BITS_MASK;

    r1 += a0*b1 + a1*b0;
    /* Bring down the overflow from above the 2^255 cutoff point */
    r1 += 19 * ( a2*b9 + a3*b8 + a4*b7 + a5*b6 + a6*b5 + a7*b4 + a8*b3 + a9*b2 );
    /* Rinse and repeat */
    r2 = r1 >> 25;  r1 &= LOW_25_BITS_MASK;

    r2 += a0*b2 + 2*a1*b1 + a2*b0;
    r2 += 19 * (2*a3*b9 + a4*b8 + 2*a5*b7 + a6*b6 + 2*a7*b5 + a8*b4 + 2*a9*b3);
    r3 = r2 >> 26;  r2 &= LOW_26_BITS_MASK;

    r3 += a0*b3 + a1*b2 + a2*b1 + a3*b0;
    r3 += 19 * ( a4*b9 + a5*b8 + a6*b7 + a7*b6 + a8*b5 + a9*b4 );
    r4 = r3 >> 25;  r3 &= LOW_25_BITS_MASK;

    r4 += a0*b4 + 2*a1*b3 + a2*b2 + 2*a3*b1 + a4*b0;
    r4 += 19 * ( 2*a5*b9 + a6*b8 + 2*a7*b7 + a8*b6 + 2*a9*b5 );
    r5 = r4 >> 26;  r4 &= LOW_26_BITS_MASK;

    r5 += a0*b5 + a1*b4 + a2*b3 + a3*b2 + a4*b1 + a5*b0;
    r5 += 19 * ( a6*b9 + a7*b8 + a8*b7 + a9*b6 );
    r6 = r5 >> 25;  r5 &= LOW_25_BITS_MASK;

    r6 += a0*b6 + 2*a1*b5 + a2*b4 + 2*a3*b3 + a4*b2 + 2*a5*b1 + a6*b0;
    r6 += 19 * ( 2*a7*b9 + a8*b8 + 2*a9*b7 );
    r7 = r6 >> 26;  r6 &= LOW_26_BITS_MASK;

    r7 += a0*b7 + a1*b6 + a2*b5 + a3*b4 + a4*b3 + a5*b2 + a6*b1 + a7*b0;
    r7 += 19 * ( a8*b9 + a9*b8);
    r8 = r7 >> 25;  r7 &= LOW_25_BITS_MASK;

    r8 += a0*b8 + 2*a1*b7 + a2*b6 + 2*a3*b5 + a4*b4 + 2*a5*b3 + a6*b2 + 2*a7*b1 + a8*b0;
    r8 += 19 * ( 2*a9*b9 );
    r9 = r8 >> 26;  r8 &= LOW_26_BITS_MASK;

    r9 += a0*b9 + a1*b8 + a2*b7 + a3*b6 + a4*b5 + a5*b4 + a6*b3 + a7*b2 + a8*b1 + a9*b0;
    /* The result may have exceeded 2^255, do another round just as in
     * fe25519_weak_reduce() */
    r0 += 19 * (r9 >> 25);
    r9 &= LOW_25_BITS_MASK;
    r1 += r0 >> 26;  r0 &= LOW_26_BITS_MASK;
    r2 += r1 >> 25;  r1 &= LOW_25_BITS_MASK;
    r3 += r2 >> 26;  r2 &= LOW_26_BITS_MASK;
    r4 += r3 >> 25;  r3 &= LOW_25_BITS_MASK;
    r5 += r4 >> 26;  r4 &= LOW_26_BITS_MASK;
    r6 += r5 >> 25;  r5 &= LOW_25_BITS_MASK;
    r7 += r6 >> 26;  r6 &= LOW_26_BITS_MASK;
    r8 += r7 >> 25;  r7 &= LOW_25_BITS_MASK;
    r9 += r8 >> 26;  r8 &= LOW_26_BITS_MASK;
    r0 += 19 * (r9 >> 25);
    r9 &= LOW_25_BITS_MASK;

    r[0] = r0;
    r[1] = r1;
    r[2] = r2;
    r[3] = r3;
    r[4] = r4;
    r[5] = r5;
    r[6] = r6;
    r[7] = r7;
    r[8] = r8;
    r[9] = r9;
}

/**
 * @brief Multiply an element by itself
 * @param[out] r Result of the squaring, i.e. the product r = a a
 * @param[in] a Field element to square
 */
void fe25519_square(fe25519 r, const fe25519 a) {

#if !FE3C_FAST_SQUARING
    fe25519_mul(r, a, a);
#else
    u64 r0, r1, r2, r3, r4, r5, r6, r7, r8, r9;

    u64 a0 = a[0];
    u64 a1 = a[1];
    u64 a2 = a[2];
    u64 a3 = a[3];
    u64 a4 = a[4];
    u64 a5 = a[5];
    u64 a6 = a[6];
    u64 a7 = a[7];
    u64 a8 = a[8];
    u64 a9 = a[9];
    /* Do the naive schoolbook multiplication but allow the compiler to group together
     * like terms. Partial single-precision products in the schoolbook multiplication
     * are of the form aibj for different i and j. When squaring the term aibj is
     * equivalent to ajbi which we can levarage to halve the number of single-precision
     * multiplications. */

    /* Start with the lowest-weight terms */
    r0 = a0*a0;
    /* Bring down the overflow from above the 2^255 cutoff point. Note that terms which are products of two odd-numbered limbs
     * are multiplied by two - see comments in fe25519_mul() for a detailed explanation of this. */
    r0 += 19 * ( 2*a1*a9 + a2*a8 + 2*a3*a7 + a4*a6 + 2*a5*a5 + a6*a4 + 2*a7*a3 + a8*a2 + 2*a9*a1 );
    /* Any overflow in r0 put in r1 and clear it in r0 */
    r1 = r0 >> 26;  r0 &= LOW_26_BITS_MASK;

    r1 += a0*a1 + a1*a0;
    /* Bring down the overflow from above the 2^255 cutoff point */
    r1 += 19 * ( a2*a9 + a3*a8 + a4*a7 + a5*a6 + a6*a5 + a7*a4 + a8*a3 + a9*a2 );
    /* Rinse and repeat */
    r2 = r1 >> 25;  r1 &= LOW_25_BITS_MASK;

    r2 += a0*a2 + 2*a1*a1 + a2*a0;
    r2 += 19 * (2*a3*a9 + a4*a8 + 2*a5*a7 + a6*a6 + 2*a7*a5 + a8*a4 + 2*a9*a3);
    r3 = r2 >> 26;  r2 &= LOW_26_BITS_MASK;

    r3 += a0*a3 + a1*a2 + a2*a1 + a3*a0;
    r3 += 19 * ( a4*a9 + a5*a8 + a6*a7 + a7*a6 + a8*a5 + a9*a4 );
    r4 = r3 >> 25;  r3 &= LOW_25_BITS_MASK;

    r4 += a0*a4 + 2*a1*a3 + a2*a2 + 2*a3*a1 + a4*a0;
    r4 += 19 * ( 2*a5*a9 + a6*a8 + 2*a7*a7 + a8*a6 + 2*a9*a5 );
    r5 = r4 >> 26;  r4 &= LOW_26_BITS_MASK;

    r5 += a0*a5 + a1*a4 + a2*a3 + a3*a2 + a4*a1 + a5*a0;
    r5 += 19 * ( a6*a9 + a7*a8 + a8*a7 + a9*a6 );
    r6 = r5 >> 25;  r5 &= LOW_25_BITS_MASK;

    r6 += a0*a6 + 2*a1*a5 + a2*a4 + 2*a3*a3 + a4*a2 + 2*a5*a1 + a6*a0;
    r6 += 19 * ( 2*a7*a9 + a8*a8 + 2*a9*a7 );
    r7 = r6 >> 26;  r6 &= LOW_26_BITS_MASK;

    r7 += a0*a7 + a1*a6 + a2*a5 + a3*a4 + a4*a3 + a5*a2 + a6*a1 + a7*a0;
    r7 += 19 * ( a8*a9 + a9*a8);
    r8 = r7 >> 25;  r7 &= LOW_25_BITS_MASK;

    r8 += a0*a8 + 2*a1*a7 + a2*a6 + 2*a3*a5 + a4*a4 + 2*a5*a3 + a6*a2 + 2*a7*a1 + a8*a0;
    r8 += 19 * ( 2*a9*a9 );
    r9 = r8 >> 26;  r8 &= LOW_26_BITS_MASK;

    r9 += a0*a9 + a1*a8 + a2*a7 + a3*a6 + a4*a5 + a5*a4 + a6*a3 + a7*a2 + a8*a1 + a9*a0;
    /* The result may have exceeded 2^255, do another round just as in
     * fe25519_weak_reduce() */
    r0 += 19 * (r9 >> 25);
    r9 &= LOW_25_BITS_MASK;
    r1 += r0 >> 26;  r0 &= LOW_26_BITS_MASK;
    r2 += r1 >> 25;  r1 &= LOW_25_BITS_MASK;
    r3 += r2 >> 26;  r2 &= LOW_26_BITS_MASK;
    r4 += r3 >> 25;  r3 &= LOW_25_BITS_MASK;
    r5 += r4 >> 26;  r4 &= LOW_26_BITS_MASK;
    r6 += r5 >> 25;  r5 &= LOW_25_BITS_MASK;
    r7 += r6 >> 26;  r6 &= LOW_26_BITS_MASK;
    r8 += r7 >> 25;  r7 &= LOW_25_BITS_MASK;
    r9 += r8 >> 26;  r8 &= LOW_26_BITS_MASK;
    r0 += 19 * (r9 >> 25);
    r9 &= LOW_25_BITS_MASK;

    r[0] = r0;
    r[1] = r1;
    r[2] = r2;
    r[3] = r3;
    r[4] = r4;
    r[5] = r5;
    r[6] = r6;
    r[7] = r7;
    r[8] = r8;
    r[9] = r9;
#endif /* !FE3C_FAST_SQUARING */
}

/**
 * @brief Encode a field element according to RFC 8032
 * @param[out] buffer Output buffer for the encoded field element
 * @param[in] a Field element to encode
 */
void fe25519_encode(u8 * buffer, fe25519 a) {

    /* Canonicalize the element first */
    fe25519_strong_reduce(a, a);

    /* Store the temporary results of bit operations in separate variables (mapped to separate
     * registers) to allow for greater instruction-level parallelism. Let A[i] denote the limb
     * a[i]. Recall that each limb has a different length according to the following
     * table:
     *
     *                              A[0]  ---  26 bits
     *                              A[1]  ---  25 bits
     *                              A[2]  ---  26 bits
     *                              A[3]  ---  25 bits
     *                              A[4]  ---  26 bits
     *                              A[5]  ---  25 bits
     *                              A[6]  ---  26 bits
     *                              A[7]  ---  25 bits
     *                              A[8]  ---  26 bits
     *                              A[9]  ---  25 bits
     */

    /* Store the lowest (26-bits wide) limb + whatever can fit (6 bits) of the second lowest limb */
    u32 t0 = a[0] | (a[1] << 26);
    /* 6 bits of a[1] are in t0, store the rest (19 bits) here + whatever can fit
     * (13 bits) of a[2] */
    u32 t1 = ( a[1] >> 6 ) | ( a[2] << 19 );
    /* 13 bits of a[2] are in t1, store the rest (13 bits) here + whatever can fit
     * (19 bits) of a[3] */
    u32 t2 = ( a[2] >> 13) | ( a[3] << 13 );
    /* 19 bits of a[3] are in t2, store the rest (6 bits) here + all of a[4]
     * (all 26 bits) */
    u32 t3 = ( a[3] >> 19 ) | ( a[4] << 6 );
    /* Store a[5] + whatever can fit (7 bits) of a[6] in t4 */
    u32 t4 = a[5] | (a[6] << 25);
    /* 7 bits of a[6] are in t0, store the rest (19 bits) here + whatever can fit
     * (13 bits) of a[7] */
    u32 t5 = ( a[6] >> 7 ) | ( a[7] << 19 );
    /* 13 bits of a[7] are in t1, store the rest (12 bits) here + whatever can fit
     * (20 bits) of a[8] */
    u32 t6 = ( a[7] >> 13) | ( a[8] << 12 );
    /* 20 bits of a[8] are in t2, store the rest (6 bits) here + all of a[9]
     * (all 25 bits) */
    u32 t7 = ( a[8] >> 20 ) | ( a[9] << 6 );

    /* The field elements get encoded as little-endian byte strings according to RFC 8032 */
    _store_32(&buffer[0 * 4], t0);
    _store_32(&buffer[1 * 4], t1);
    _store_32(&buffer[2 * 4], t2);
    _store_32(&buffer[3 * 4], t3);
    _store_32(&buffer[4 * 4], t4);
    _store_32(&buffer[5 * 4], t5);
    _store_32(&buffer[6 * 4], t6);
    _store_32(&buffer[7 * 4], t7);
}

/**
 * @brief Decode a field element according to RFC 8032
 * @param[out] r Decoded field element
 * @param[out] buffer Encoding of the field element
 * @return 1 if decoding succeeded, 0 otherwise
 */
__attribute__((warn_unused_result))
int fe25519_decode(fe25519 r, const u8 * buffer) {

    r[0] = ( _load_32(&buffer[ 0]) >> 0 ) & LOW_26_BITS_MASK;
    /* Do not offset by 8 now (another 32 bits) since we have dropped 6 bits from buffer[3]. Offset
     * by 26 bits, i.e. 3 bytes of offset + 2 bits of shift*/
    r[1] = ( _load_32(&buffer[ 3]) >> 2 ) & LOW_25_BITS_MASK;
    /* Offset by 25 bits this time (the odd-numbered limbs are 25-bits wide) */
    r[2] = ( _load_32(&buffer[ 6]) >> 3 ) & LOW_26_BITS_MASK;
    r[3] = ( _load_32(&buffer[ 9]) >> 5 ) & LOW_25_BITS_MASK;
    r[4] = ( _load_32(&buffer[12]) >> 6 ) & LOW_26_BITS_MASK;
    /* At this point the shift bits have added up to a full byte */
    r[5] = ( _load_32(&buffer[16]) >> 0 ) & LOW_25_BITS_MASK;
    r[6] = ( _load_32(&buffer[19]) >> 1 ) & LOW_26_BITS_MASK;
    r[7] = ( _load_32(&buffer[22]) >> 3 ) & LOW_25_BITS_MASK;
    r[8] = ( _load_32(&buffer[25]) >> 4 ) & LOW_26_BITS_MASK;
    /* Note that the last bit gets naturally ignored as required by RFC 8032 */
    r[9] = ( _load_32(&buffer[28]) >> 6 ) & LOW_25_BITS_MASK;

    return fe25519_is_canonical(r);
}
