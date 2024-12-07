#include <points/lattice_basis_reduction/lattices_ed25519.h>
#include <utils/utils.h>

#if !FE3C_ED25519_LATTICE_BASIS_REDUCTION
    #error "Build system inconsistency detected! lattices_ed25519.c in use despite FE3C_ED25519_LATTICE_BASIS_REDUCTION not being set"
#endif /* !FE3C_ED25519_LATTICE_BASIS_REDUCTION */

static const u8 group_order[] = {
    0xed, 0xd3, 0xf5, 0x5c, 0x1a, 0x63, 0x12, 0x58,
    0xd6, 0x9c, 0xf7, 0xa2, 0xde, 0xf9, 0xde, 0x14,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10
};

static const u32 group_order_squared[] = {
    0xab128969, 0xe2edf685, 0x2298a31d, 0x68039276,
    0xd217f5be, 0x3dceec73, 0x1b7c309a, 0xa1b39941,
    0x4b9eba7d, 0xcb024c63, 0xd45ef39a, 0x029bdf3b,
    0x00000000, 0x00000000, 0x00000000, 0x01000000,
};

/**
 * @brief Encode a byte buffer into limbs
 * @param[out] out An array of limbs
 * @param[in] in A raw byte-buffer input
 * @param[in] length Length of the input buffer in bytes (must be a multiple of limb size)
 */
static inline void load_limbs(u32 *out, const u8 *in, int length)
{
    FE3C_SANITY_CHECK((length & 0b11) == 0, "Length %d if not a multiple of word size", length);
#if FE3C_LILENDIAN_TARGET
    if ((void *) in != (void *) out)
        (void) memcpy(out, in, length);
#else
    /* Big-endian target or endianness unknown (take the safe route) */
    int wordcount = length / 4;
    for (int i = 0; i < wordcount; i++) {
        u32 word  = (u32) in[4*i + 0] << 0;
            word |= (u32) in[4*i + 1] << 8;
            word |= (u32) in[4*i + 2] << 16;
            word |= (u32) in[4*i + 3] << 24;
        out[i] = word;
    }
#endif /* FE3C_LILENDIAN_TARGET */
}

/**
 * @brief Decode a limbs buffer into bytes
 * @param[out] out An array of bytes
 * @param[in] in A limb-buffer input
 * @param[in] length Length of the output buffer in bytes (must be a multiple of limb size)
 */
static inline void store_limbs(u8 *out, const u32 *in, int length)
{
    FE3C_SANITY_CHECK((length & 0b11) == 0, "Length %d if not a multiple of word size", length);
#if FE3C_LILENDIAN_TARGET
    if ((void *) in != (void *) out)
        (void) memcpy(out, in, length);
#else
    /* Big-endian target or endianness unknown (take the safe route) */
    int wordcount = length / 4;
    for (int i = 0; i < wordcount; i++) {
        u32 word = in[i];
        out[4*i + 0] = (u8) (word >> 0);
        out[4*i + 1] = (u8) (word >> 8);
        out[4*i + 2] = (u8) (word >> 16);
        out[4*i + 3] = (u8) (word >> 24);
    }
#endif /* FE3C_LILENDIAN_TARGET */
}

/**
 * @brief Get the length of a number defined as the minimum size of the binary representation of x using two's complement and exluding the sign bit
 * @param number A 64-byte little-endian number
 * @return Length of number as defined by Pornin in eprint:2020/454
 * @note If x >= 0, then bitlength(x) is the smallest integer m >= 0 such that x < 2^m. If x < 0, then bitlength(x) = bitlength(-x-1)
 */
static inline int bitlength(const u32 *number)
{
    /* If the number is negative, we are interested in bitlength(-x-1), i.e., bitlength of the one's complement of x. XOR all bytes
     * of the number with 0xFF in this case to negate them. */
    u32 mask = -(number[15] >> 31);

    int i;
    /* Find the top-most non-zero limb */
    for (i = 15; i >= 0; i--)
        if (mask ^ number[i])
            break;

    /* Handle zero */
    if (i == -1)
        return 0;

    /* Return the number of bits unchecked + however many significant
     * bits there are in the top-most non-zero limb. */
    return (32 * i) + (32 - u32_nlz(mask ^ number[i]));
}

/**
 * @brief Negate a little-endian signed two's complement integer
 * @param[out] out The buffer to write the negated number to (can be equal to in)
 * @param[in] in The input number
 * @param[in] length Length of the buffers in bytes
 */
static inline void negate(u8 *out, const u8 *in, int length)
{
    u16 carry = 1;
    for (int i = 0; i < length; i++) {
        /* IMPORTANT: C promotes u8 to an int when used as an operand with ~, cast back to u8 */
        carry += (u8) ~in[i];
        out[i] = (u8) carry;
        carry >>= 8;
    }
}

/**
 * @brief Take the absolute value of a little-endian signed two's complement integer
 * @param[out] out The buffer to write the absolute value to (can be equal to in)
 * @param[in] in The input number
 * @param[in] length Length of the buffers in bytes
 * @return The original sign bit of the input
 */
static inline int absolute_value(u8 *out, const u8 *in, size_t length)
{
    /* Read the sign bit */
    int negative = in[length - 1] >> 7;
    if (negative)
        negate(out, in, length);
    else if (in != out)
        (void) memcpy(out, in, length);

    return negative;
}

/**
 * @brief Multiply two little-endian signed two's complement numbers
 * @param[out] out A little-endian signed two's complement 64-byte result
 * @param[in] left The first 32-byte operand
 * @param[in] right The second operand
 * @param[in] right_length Length of the second operand in bytes (can be 16 or 32)
 */
static inline void multiply(u8 *out, const u8 *left, const u8 *right, size_t right_length)
{
    /* TODO: Try sign-extending left and right (probably suffer either an increase in memory
     * for the full sign-extended operands or more branches/loops for handling the sign extension algorithmically).
     * For now, just multiply the absolute values and note the signs. */
    u8 labs[32];
    u8 rabs[32];

    FE3C_SANITY_CHECK(right_length == 16 || right_length == 32, "Bad right_length: %d", right_length);
    int lneg = absolute_value(labs, left, 32);
    int rneg = absolute_value(rabs, right, right_length);
    /* Zero-extend (or sign-extend, it is positive anyway) the absolute value of the right operand if necessary */
    (void) memset(rabs + right_length, 0, 32 - right_length);

    /* Multiply absolute values */
    u32 carry = 0;
    for (int k = 0; k < 32; k++) {

        for (int i = 0; i <= k; i++)
            carry += labs[i] * rabs[k - i];
        out[k] = (u8) carry;
        carry >>= 8;
    }

    for (int k = 32; k < 64; k++) {

        for (int i = k + 1 - 32; i < 32; i++)
            carry += labs[i] * rabs[k - i];
        out[k] = (u8) carry;
        carry >>= 8;
    }

    /* Negate the result if necessary */
    if (lneg ^ rneg)
        negate(out, out, 64);
}

/**
 * @brief Determine if one unsigned little-endian 64-byte number is less than the other
 * @param[in] left The first 64-byte number
 * @param[in] right The second 64-byte number
 * @return 1 if left is less than right as an unsigned number, 0 otherwise
 */
static inline int unsigned_less_than(const u32 *left, const u32 *right)
{
    /* Try subtracting right from left and see if we get non-zero borrow */
    u64 borrow = 0;
    for (int i = 0; i < 16; i++)
        borrow = ((u64) left[i] - (u64) right[i] - borrow) >> 63;
    return (int) borrow;
}

/**
 * @brief Compute in place an fused multiply-and-add of the form left + 2^shift * right
 * @param[in,out] left Both the first operand and the output buffer
 * @param[in] right The second operand
 * @param[in] shift Leftshift to be applied to the right operand
 * @param[in] length Length of the operands (and a bound on the length of the result) in limbs
 */
static inline void add_leftshifted(u32 *left, const u32 *right, int shift, int length)
{
    int limbshift = shift >> 5;
    int bitshift = shift & 0x1f;
    u64 carry = 0;
    for (int i = 0; i < length - limbshift; i++) {
        carry += (u64) left[i + limbshift] + ((u64) right[i] << bitshift);
        left[i + limbshift] = (u32) carry;
        carry >>= 32;
    }
}

/**
 * @brief Compute in place an fused multiply-and-subtract of the form left - 2^shift * right
 * @param[in,out] left Both the first operand and the output buffer
 * @param[in] right The second operand
 * @param[in] shift Leftshift to be applied to the right operand
 * @param[in] length Length of the operands (and a bound on the length of the result) in limbs
 */
static inline void sub_leftshifted(u32 *left, const u32 *right, int shift, int length)
{
    int limbshift = shift >> 5;
    int bitshift = shift & 0x1f;
    u64 borrow = 0;
    for (int i = 0; i < length - limbshift; i++) {
        borrow = (u64) left[i + limbshift] - ((u64) right[i] << bitshift) - borrow;
        left[i + limbshift] = (u32) borrow;
        /* Count how many 0x100000000's were borrowed. Negate the borrow to get the total value borrowed,
         * round that up to 0x100000000 by adding 0xFFFFFFFF and then dividing by 0x100000000. */
        borrow = ((u64) 0xFFFFFFFF - (u64) borrow) >> 32;
    }
}

/**
 * @brief Weakly reduce an unsigned 64-byte little-endian number modulo the group order L, i.e., bring it to range [-L, L)
 * @param[out] out The output buffer
 * @param[in] in The unreduced input number
 * @return -1 if the result is in range [-L, 0) and requires the addition of L, 0 if the result is 0, and 1 if the result is canonical and positive
 */
static inline int weak_unsigned_reduce_mod_group_order(u8 *out, const u8 *in)
{
    /* Let L be the group order and c = L - 2^252. Use the identity
     *
     *                    2^256 = -16*c (mod L)
     *
     * This is a copy-paste from esp32/components/cementite/port/xtensa/scalars/scalars_ed25519_32.c.
     * Look there for the rationale.
     */

    i16 t[49];
    i32 carry = 0;
    /* Go down from 64 (unsigned) bytes to 49 (signed) as a first pass */
    for (int k = 0; k < 16; k++) {

        carry += in[k];
        for (int i = 0; i <= k; i++)
            carry -= in[32 + i] * ( (i32) group_order[k - i] << 4 );
        t[k] = (u8) carry;
        /* Note that we right-shift a signed integer which is probably undefined behaviour in C.
         * If this ever turns out to be a problem, we can sign extend manually, this code is not
         * performance-critical. */
        carry >>= 8;
    }

    for (int k = 16; k < 32; k++) {

        carry += in[k];
        for (int i = k + 1 - 16; i <= k; i++)
            carry -= in[32 + i] * ( (i32) group_order[k - i] << 4 );
        t[k] = (u8) carry;
        carry >>= 8;
    }

    for (int k = 32; k < 48; k++) {

        for (int i = k + 1 - 16; i < 32; i++)
            carry -= in[32 + i] * ( (i32) group_order[k - i] << 4 );
        t[k] = (u8) carry;
        carry >>= 8;
    }
    FE3C_SANITY_CHECK(carry <= 0 && carry >= -2, "Unexpected carry after first-pass scalar reduction: %" PRId32, carry);
    t[48] = carry;
    carry = 0;

    /* Do another round, going down to 33 (signed) bytes */
    for (int k = 0; k < 16; k++) {

        carry += t[k];
        for (int i = 0; i <= k; i++)
            carry -= t[32 + i] * ( (i32) group_order[k - i] << 4 );
        t[k] = (u8) carry;
        carry >>= 8;
    }

    for (int k = 16; k < 17; k++) {

        carry += t[k];
        for (int i = k + 1 - 16; i <= k; i++)
            carry -= t[32 + i] * ( (i32) group_order[k - i] << 4 );
        t[k] = (u8) carry;
        carry >>= 8;
    }

    for (int k = 17; k < 32; k++) {

        carry += t[k];
        for (int i = k + 1 - 16; i < 17; i++)
            carry -= t[32 + i] * ( (i32) group_order[k - i] << 4 );
        t[k] = (u8) carry;
        carry >>= 8;
    }
    FE3C_SANITY_CHECK(carry <= 3 && carry >= -2, "Unexpected carry after second-pass scalar reduction: %" PRId32, carry);
    t[32] = carry;
    carry = 0;

    /* Do a more careful reduction, by employing the identity:
     *
     *                 2^252 = -c (mod L)
     *
     * and combine it with a writeback to the out buffer.
     * (Do a zero comparison as we go.)
     */
    u8 nonzero = 0;
    i32 overflow = (t[32] << 4) | (t[31] >> 4);
    t[31] &= 0x0f;
    for (int k = 0; k < 16; k++) {
        carry += t[k];
        carry -= overflow * group_order[k];
        out[k] = (u8) carry;
        nonzero |= (u8) carry;
        carry >>= 8;
    }
    /* Propagate the carry */
    for (int k = 16; k < 32; k++) {
        carry += t[k];
        out[k] = (u8) carry;
        nonzero |= (u8) carry;
        carry >>= 8;
    }
    FE3C_SANITY_CHECK(carry <= 0 && carry >= -1, "Unexpected carry after third-pass scalar reduction: %" PRId32, carry);
    /* Collapse nonzero to a flag */
    nonzero |= (nonzero >> 4);
    nonzero |= (nonzero >> 2);
    nonzero |= (nonzero >> 1);
    nonzero &= 1;

    /* Return -1 if carry == -1, 0 if result is 0, 1 if result is positive and canonical */
    return (2 * carry + 1) * nonzero;
}

/**
 * @brief Reduce a signed little-endian 64-byte number modulo the order of the Ed25519 group
 * @param[out] out A 32-byte output buffer
 * @param[in] in The 64-byte input number
 */
static inline void reduce_mod_group_order(u8 *out, const u8 *in)
{
    u8 inabs[64];
    /* The input can be negative in two's complement. Let us cheaply take the absolute value
     * and reduce that using the identity 2^256 = -16*c (mod L), where L is the group order,
     * and c = L - 2^252. Afterwards, we can account for the original sign of the input. */
    int negative_input = absolute_value(inabs, in, 64);
    /* Reduce the absolute value weakly to get a value in range [-L, L) and an indicator
     * whether we got something negative or not. */
    int weak_sign = weak_unsigned_reduce_mod_group_order(out, inabs);
    /* TODO: There seems to be potential for optimization here if we take negative_input into
     * account immediately before handling weak_sign */
    u16 carry;
    switch (weak_sign) {
    case 0:
        /* Got zero, we are done */
        return;

    case -1:
        carry = 0;
        /* Result in range [-L, 0), add group order */
        for (int i = 0; i < 32; i++) {
            carry += (u16) out[i] + (u16) group_order[i];
            out[i] = (u8) carry;
            carry >>= 8;
        }
        break;

    case 1:
    default:
        break;
    }

    if (negative_input) {
        u16 borrow = 0;
        /* Result should be negated, subtract from group order */
        for (int i = 0; i < 32; i++) {
            borrow = (u16) group_order[i] - (u16) out[i] - borrow;
            out[i] = (u8) borrow;
            borrow >>= 15;
        }
    }
}

void ed25519_lattice_basis_reduction(u8 *delta_challenge, int *dc_sign, u8 *delta, int *d_sign, u8 *delta_response, const u8 *challenge, const u8 *response)
{
    /* Given challenge h and response S, consider the lattice spanned by (L, 0) and (h, 1) and find its size-reduced basis.
     * Specifically, find a short vector (v0, v1) in the lattice. It being in the lattice implies that v0 = v1*h (mod L).
     * Return delta_challenge = |v0|, dc_sign = sign(v0), delta = |v1|, d_sign = sign(v1), delta_response = v1*S (mod L).
     *
     * Use Pornin's optimization of Lagrange's algorithm for finding a shortest vector in a lattice of dimension two (eprint:2020/454).
     */

    /*
     * Vanilla Lagrange's algorithm resembles the Gram-Schmidt process. Compute the inner product of u=(L,0) and v=(h,1),
     * denote it p=<u,v>, then subtract qv from u, where q = p/N(v), i.e., subtract the projection of u onto v. Then,
     * u - qv is shorter and more orthogonal to v (it would be orthogonal if we were working in a vector space R^2,
     * but we are in a lattice setting). Instead of recomputing the inner product p, and the norms N(u), N(v) from scratch
     * each time, we keep track of them in variables and update them, e.g., we update N(u) <- N(u) + q^2N(v) - 2qp.
     *
     * In Pornin's algorithm we force q to be a power of 2, thereby making multiplications by q or q^2 super efficient.
     * As a consequence, after a loop iteration, it is no longer always the case that u is shorter than v, so any swapping
     * of the shorter and the longer vector must be made conditional.
     */

    /* Note that we work with (u0, u1) and (v0, v1) stored on 128 bits only. We can "forget" about the higher
     * bits as we know they will not matter in the long run. */
    u32 u0_limbs[4];
    u32 u1_limbs[4];
    u32 v0_limbs[4];
    u32 v1_limbs[4];
    u32 nu_limbs[16];
    u32 nv_limbs[16];
    u32 uv[16];

    /* Some of the computations are performed directly with byte buffers, but for the performance-critical code
     * in the loop we switch to using 32-bit packed-radix limbs */

    /* Set u = (L, 0) */
    load_limbs(u0_limbs, group_order, sizeof(u0_limbs));
    (void) memset(u1_limbs, 0, sizeof(u1_limbs));

    /* Set v = (h, 1) */
    load_limbs(v0_limbs, challenge, sizeof(v0_limbs));
    (void) memset(v1_limbs, 0, sizeof(v1_limbs));
    v1_limbs[0] = 1;

    /* Compute the initial N(u), i.e., the group order squared */
    (void) memcpy(nu_limbs, group_order_squared, sizeof(nu_limbs));

    /* Compute the initial N(v), i.e., h^2 + 1 (multiply assumes two's complement, but challenge is definitely positive) */
    multiply((u8 *) nv_limbs, challenge, challenge, 32);
    load_limbs(nv_limbs, (u8 *) nv_limbs, sizeof(nv_limbs));
    nv_limbs[0] += 1;
    /* Propagate the carry */
    u32 *iter = nv_limbs;
    while (*iter == 0)
        *(++iter) += 1;

    /* Compute the inner product <u,v> = h*L (multiply assumes two's complement, but both challenge and group_order are positive) */
    multiply((u8 *) uv, challenge, group_order, 32);
    load_limbs(uv, (u8 *) uv, sizeof(uv));

    /* Use pointers to do swaps efficiently by repointing the pointers */
    u32 *u0 = u0_limbs;
    u32 *u1 = u1_limbs;
    u32 *v0 = v0_limbs;
    u32 *v1 = v1_limbs;
    u32 *nu = nu_limbs;
    u32 *nv = nv_limbs;

    for ( ; /* as long as needed */ ; ) {

        if (unsigned_less_than(nu, nv)) {
            /* Swap u with v */
            swap_pointers(u0, v0);
            swap_pointers(u1, v1);
            swap_pointers(nu, nv);
        }

        int s = bitlength(nv);
        /* Check if we are good to go */
        if (s <= 253)
            break;

        /* Compute max{0, len(<u, v>) - len(N(v))} */
        s = bitlength(uv) - s;
        if (s < 0)
            s = 0;

        if (0 == (uv[15] >> 31)) {

            sub_leftshifted(u0, v0,   s, 4);
            sub_leftshifted(u1, v1,   s, 4);
            add_leftshifted(nu, nv, 2*s, 16);
            sub_leftshifted(nu, uv, s+1, 16);
            sub_leftshifted(uv, nv,   s, 16);

        } else {

            add_leftshifted(u0, v0,   s, 4);
            add_leftshifted(u1, v1,   s, 4);
            add_leftshifted(nu, nv, 2*s, 16);
            add_leftshifted(nu, uv, s+1, 16);
            add_leftshifted(uv, nv,   s, 16);
        }
    }

    /* (v0, v1) is our short vector of interest at this point.
     * v0 corresponds to delta_challenge and v1 corresponds to delta.
     * Either may be negative, and we will want to pass only absolute
     * values to our caller (and set the negative flags accordingly).
     * Before taking the absolute values, however, we must multiply
     * delta times the response scalar and reduce it modulo the group
     * order. */

    store_limbs(delta_challenge, v0, 16);
    store_limbs(delta, v1, 16);
    multiply((u8 *) nu, response, delta, 16);
    reduce_mod_group_order(delta_response, (u8 *) nu);

    /* Compute absolute values of |v0| and |v1| and let the caller know what
     * their original signs were. */
    *dc_sign = absolute_value(delta_challenge, delta_challenge, 16);
    *d_sign = absolute_value(delta, delta, 16);
}
