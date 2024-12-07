#include <points/lattice_basis_reduction/lattices_ed448.h>
#include <utils/utils.h>

#if !FE3C_ED448_LATTICE_BASIS_REDUCTION
    #error "Build system inconsistency detected! lattices_ed448.c in use despite FE3C_ED448_LATTICE_BASIS_REDUCTION not being set"
#endif /* !FE3C_ED448_LATTICE_BASIS_REDUCTION */

/* We keep the group order one byte short in this file, since that byte is always zero
 * (TODO: one day we could look into using this shorter representation everywhere) */
static const u8 group_order[] = {
    0xf3, 0x44, 0x58, 0xab, 0x92, 0xc2, 0x78, 0x23,
    0x55, 0x8f, 0xc5, 0x8d, 0x72, 0xc2, 0x6c, 0x21,
    0x90, 0x36, 0xd6, 0xae, 0x49, 0xdb, 0x4e, 0xc4,
    0xe9, 0x23, 0xca, 0x7c, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f
};

/* The c part of the group order L = 2^446 - c */
static const u8 group_order_crandall_tail[] = {
    0x0d, 0xbb, 0xa7, 0x54, 0x6d, 0x3d, 0x87, 0xdc,
    0xaa, 0x70, 0x3a, 0x72, 0x8d, 0x3d, 0x93, 0xde,
    0x6f, 0xc9, 0x29, 0x51, 0xb6, 0x24, 0xb1, 0x3b,
    0x16, 0xdc, 0x35, 0x83
};

static const u32 group_order_squared[] = {
    0x1ba1fea9, 0xc1adfbb8, 0x49e0a8b2, 0xb91bf537,
    0xe764d815, 0x4525492b, 0xa2b8716d, 0x4ae17cf6,
    0xba3c47c4, 0xf1a9cc14, 0x7e4d070a, 0x92052bcb,
    0x9f823b72, 0xc3402a93, 0x55ac2279, 0x91bc6149,
    0x46e2c7aa, 0x10b66139, 0xd76b1b48, 0xe2276da4,
    0xbe6511f4, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0x0fffffff
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
 * @param number A 112-byte little-endian number
 * @return Length of number as defined by Pornin in eprint:2020/454
 * @note If x >= 0, then bitlength(x) is the smallest integer m >= 0 such that x < 2^m. If x < 0, then bitlength(x) = bitlength(-x-1)
 */
static inline int bitlength(const u32 *number)
{
    /* If the number is negative, we are interested in bitlength(-x-1), i.e., bitlength of the one's complement of x. XOR all bytes
     * of the number with 0xFF in this case to negate them. */
    u32 mask = -(number[27] >> 31);

    int i;
    /* Find the top-most non-zero limb */
    for (i = 27; i >= 0; i--)
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
 * @param[out] out A little-endian signed two's complement 112-byte result
 * @param[in] left The first 56-byte operand
 * @param[in] right The second operand
 * @param[in] right_length Length of the second operand in bytes (can be 32 or 56)
 */
static inline void multiply(u8 *out, const u8 *left, const u8 *right, size_t right_length)
{
    /* TODO: Try sign-extending left and right (probably suffer either an increase in memory
     * for the full sign-extended operands or more branches/loops for handling the sign extension algorithmically).
     * For now, just multiply the absolute values and note the signs. */
    u8 labs[56];
    u8 rabs[56];

    FE3C_SANITY_CHECK(right_length == 32 || right_length == 56, "Bad right_length: %d", right_length);
    int lneg = absolute_value(labs, left, 56);
    int rneg = absolute_value(rabs, right, right_length);
    /* Zero-extend (or sign-extend, it is positive anyway) the absolute value of the right operand if necessary */
    (void) memset(rabs + right_length, 0, 56 - right_length);

    /* Multiply absolute values */
    u32 carry = 0;
    for (int k = 0; k < 56; k++) {

        for (int i = 0; i <= k; i++)
            carry += labs[i] * rabs[k - i];
        out[k] = (u8) carry;
        carry >>= 8;
    }

    for (int k = 56; k < 112; k++) {

        for (int i = k + 1 - 56; i < 56; i++)
            carry += labs[i] * rabs[k - i];
        out[k] = (u8) carry;
        carry >>= 8;
    }

    /* Negate the result if necessary */
    if (lneg ^ rneg)
        negate(out, out, 112);
}

/**
 * @brief Determine if one unsigned little-endian 112-byte number is less than the other
 * @param[in] left The first 112-byte number
 * @param[in] right The second 112-byte number
 * @return 1 if left is less than right as an unsigned number, 0 otherwise
 */
static inline int unsigned_less_than(const u32 *left, const u32 *right)
{
    /* Try subtracting right from left and see if we get non-zero borrow */
    u64 borrow = 0;
    for (int i = 0; i < 28; i++)
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
 * @brief Weakly reduce an unsigned 114-byte little-endian number modulo the group order L, i.e., bring it to range [0, 2L)
 * @param[out] out The output buffer
 * @param[in] in The unreduced input number
 */
static inline void weak_unsigned_reduce_mod_group_order(u8 *out, const u8 *in)
{
    /* Let L be the group order and c = 2^446 - L. Use the identity
     *
     *                      2^448 = 4*c (mod L)
     *
     * This is a copy-paste from esp32/components/cementite/port/xtensa/scalars/scalars_ed448_32.c.
     * Look there for the rationale.
     */

    u8 t[87];
    u32 carry = 0;
    for (int k = 0; k < 28; k++) {

        carry += in[k];
        for (int i = 0; i <= k; i++)
            carry += in[56 + i] * ( (u32) group_order_crandall_tail[k - i] << 2 );
        t[k] = (u8) carry;
        carry >>= 8;
    }
    for (int k = 28; k < 56; k++) {

        carry += in[k];
        for (int i = k + 1 - 28; i <= k; i++)
            carry += in[56 + i] * ( (u32) group_order_crandall_tail[k - i] << 2 );
        t[k] = (u8) carry;
        carry >>= 8;
    }
    for (int k = 56; k < 58; k++) {

        for (int i = k + 1 - 28; i <= k; i++)
            carry += in[56 + i] * ( (u32) group_order_crandall_tail[k - i] << 2 );
        t[k] = (u8) carry;
        carry >>= 8;
    }
    for (int k = 58; k < 86; k++) {

        for (int i = k + 1 - 28; i < 58; i++)
            carry += in[56 + i] * ( (u32) group_order_crandall_tail[k - i] << 2 );
        t[k] = (u8) carry;
        carry >>= 8;
    }
    FE3C_SANITY_CHECK(carry <= 2, "Unexpected carry after first-pass scalar reduction: %" PRId32, carry);
    t[86] = (u8) carry;
    carry = 0;

    /* We have reduced s down to (at most) 87 bytes. Do the same trick again and
     * bring down the 31 bytes above the 56-byte boundary. This way we shall get down to
     * 31 + 29 - 1 = 59 bytes (after multiplying the 31 bytes by the 29-byte
     * 4 * group_order_crandall_tail). */
    for (int k = 0; k < 28; k++) {

        carry += t[k];
        for (int i = 0; i <= k; i++)
            carry += t[56 + i] * ( (u32) group_order_crandall_tail[k - i] << 2 );
        t[k] = (u8) carry;
        carry >>= 8;
    }
    for (int k = 28; k < 31; k++) {

        carry += t[k];
        for (int i = k + 1 - 28; i <= k; i++)
            carry += t[56 + i] * ( (u32) group_order_crandall_tail[k - i] << 2 );
        t[k] = (u8) carry;
        carry >>= 8;
    }
    for (int k = 31; k < 56; k++) {

        carry += t[k];
        for (int i = k + 1 - 28; i < 31; i++)
            carry += t[56 + i] * ( (u32) group_order_crandall_tail[k - i] << 2 );
        t[k] = (u8) carry;
        carry >>= 8;
    }
    for (int k = 56; k < 59; k++) {

        for (int i = k + 1 - 28; i < 31; i++) {
            carry += t[56 + i] * ( (u32) group_order_crandall_tail[k - i] << 2 );
        }
        t[k] = (u8) carry;
        carry >>= 8;
    }
    FE3C_SANITY_CHECK(carry == 0, "Unexpected carry after second-pass scalar reduction: %" PRId32, carry);

    /* Do one last rough reduction. At this point we have only three bytes above
     * the 56-byte boundary. */
    for (int k = 0; k < 3; k++) {

        carry += t[k];
        for (int i = 0; i <= k; i++)
            carry += t[56 + i] * ( (u32) group_order_crandall_tail[k - i] << 2 );
        t[k] = (u8) carry;
        carry >>= 8;
    }
    for (int k = 3; k < 28; k++) {

        carry += t[k];
        for (int i = 0; i < 3; i++)
            carry += t[56 + i] * ( (u32) group_order_crandall_tail[k - i] << 2 );
        t[k] = (u8) carry;
        carry >>= 8;
    }
    for (int k = 28; k < 31; k++) {

        carry += t[k];
        for (int i = k + 1 - 28; i < 3; i++)
            carry += t[56 + i] * ( (u32) group_order_crandall_tail[k - i] << 2 );
        t[k] = (u8) carry;
        carry >>= 8;
    }
    /* Propagate the carry */
    for (int k = 31; k < 56; k++) {

        carry += t[k];
        t[k] = (u8) carry;
        carry >>= 8;
    }
    FE3C_SANITY_CHECK(carry == 0, "Unexpected carry after third-pass scalar reduction: %" PRId32, carry);

    /* Do a careful (albeit still weak) reduction and combine it with a writeback to the output buffer */
    u32 overflow = (t[55] >> 6);
    t[55] &= 0x3f;
    for (int k = 0; k < 28; k++) {

        carry += t[k];
        carry += overflow * group_order_crandall_tail[k];
        out[k] = (u8) carry;
        carry >>= 8;
    }
    /* Propagate the carry */
    for (int k = 28; k < 56; k++) {

        carry += t[k];
        out[k] = (u8) carry;
        carry >>= 8;
    }
    FE3C_SANITY_CHECK(carry == 0, "Unexpected carry after fourth-pass scalar reduction: %" PRId32, carry);

    /* At this point we have successfully done a weak reduction and
     * the scalar is now smaller than 2L */
}

/**
 * @brief Reduce a signed little-endian 112-byte number modulo the order of the Ed448 group
 * @param[out] out A 32-byte output buffer
 * @param[in] in The 64-byte input number
 */
static inline void reduce_mod_group_order(u8 *out, const u8 *in)
{
    u8 inabs[114];
    /* The input can be negative in two's complement. Let us cheaply take the absolute value
     * and reduce that using the identity 2^448 = 4*c (mod L), where L is the group order,
     * and c = L - 2^446. Afterwards, we can account for the original sign of the input. */
    int negative_input = absolute_value(inabs, in, 112);
    /* The input buffer is 112-byte long, but the reduction subroutine currently expects 114-byte inputs,
     * pad the input with zeroes. */
    inabs[112] = 0;
    inabs[113] = 0;
    /* Reduce the absolute value weakly to get a value in range [0, 2L) */
    weak_unsigned_reduce_mod_group_order(out, inabs);

    /* Reuse the inabs buffer to complete the reduction */
    u16 borrow = 0;
    /* Try subtracting the group order L and check if borrow is set afterwards */
    for (int i = 0; i < 56; i++) {
        borrow = out[i] - group_order[i] - borrow;
        inabs[i] = (u8) borrow;
        /* Extract the sign bit to identify the borrow */
        borrow >>= 15;
    }

    /* If borrow is non-zero at this point, we have a canonical value already */
    if (0 == borrow)
        (void) memcpy(out, inabs, 56);

    if (negative_input) {
        borrow = 0;
        /* Subtract the scalar from the group order to negate it in the field */
        for (int i = 0; i < 56; i++) {
            borrow = group_order[i] - out[i] - borrow;
            out[i] = (u8) borrow;
            borrow >>= 15;
        }
    }
}

void ed448_lattice_basis_reduction(u8 *delta_challenge, int *dc_sign, u8 *delta, int *d_sign, u8 *delta_response, const u8 *challenge, const u8 *response)
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

    /* Note that we work with (u0, u1) and (v0, v1) stored on 225 bits only. We can "forget" about the higher
     * bits as we know they will not matter in the long run. */

    u32 u0_limbs[8];
    u32 u1_limbs[8];
    u32 v0_limbs[8];
    u32 v1_limbs[8];
    u32 nu_limbs[28];
    u32 nv_limbs[28];
    u32 uv[28];

    /* Some of the computations are performed directly with byte buffers, but for the performance-critical code
     * in the loop we switch to using 32-bit packed-radix limbs. Note that the challenge and the response, if
     * canonical, are represented on 56 bytes only, not 57 as in other parts of the code. */

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
    multiply((u8 *) nv_limbs, challenge, challenge, 56);
    load_limbs(nv_limbs, (u8 *) nv_limbs, sizeof(nv_limbs));
    nv_limbs[0] += 1;
    /* Propagate the carry */
    u32 *iter = nv_limbs;
    while (*iter == 0)
        *(++iter) += 1;

    /* Compute the inner product <u,v> = h*L (multiply assumes two's complement, but both challenge and group_order are positive) */
    multiply((u8 *) uv, challenge, group_order, 56);
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
        if (s <= 450)
            break;

        /* Compute max{0, len(<u, v>) - len(N(v))} */
        s = bitlength(uv) - s;
        if (s < 0)
            s = 0;

        if (0 == (uv[27] >> 31)) {

            sub_leftshifted(u0, v0,   s, 8);
            sub_leftshifted(u1, v1,   s, 8);
            add_leftshifted(nu, nv, 2*s, 28);
            sub_leftshifted(nu, uv, s+1, 28);
            sub_leftshifted(uv, nv,   s, 28);

        } else {

            add_leftshifted(u0, v0,   s, 8);
            add_leftshifted(u1, v1,   s, 8);
            add_leftshifted(nu, nv, 2*s, 28);
            add_leftshifted(nu, uv, s+1, 28);
            add_leftshifted(uv, nv,   s, 28);
        }
    }

    /* (v0, v1) is our short vector of interest at this point.
     * v0 corresponds to delta_challenge and v1 corresponds to delta.
     * Either may be negative, and we will want to pass only absolute
     * values to our caller (and set the negative flags accordingly).
     * Before taking the absolute values, however, we must multiply
     * delta times the response scalar and reduce it modulo the group
     * order. */

    store_limbs(delta_challenge, v0, 32);
    store_limbs(delta, v1, 32);
    multiply((u8 *) nu, response, delta, 32);
    reduce_mod_group_order(delta_response, (u8 *) nu);
    /* Callers pass in 57-byte buffers for delta_response */
    delta_response[56] = 0;

    /* Compute absolute values of |v0| and |v1| and let the caller know what
     * their original signs were. */
    *dc_sign = absolute_value(delta_challenge, delta_challenge, 32);
    *d_sign = absolute_value(delta, delta, 32);
}
