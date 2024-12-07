#include <points/points.h>
#include <field_elements/field_elements_ed25519.h>
#include <utils/utils.h>
#if FE3C_ED25519_COMB_METHOD
    #include <points/comb/comb_ed25519.h>
#endif /* FE3C_ED25519_COMB_METHOD */
#if FE3C_ED25519_LATTICE_BASIS_REDUCTION
    #include <points/lattice_basis_reduction/lattices_ed25519.h>
#endif /* FE3C_ED25519_LATTICE_BASIS_REDUCTION */


#define ED25519_STR \
    "    X = " FE25519_STR "\n" \
    "    Y = " FE25519_STR "\n" \
    "    Z = " FE25519_STR "\n" \
    "    T = " FE25519_STR
#define ED25519_TO_STR(p) \
    FE25519_TO_STR((p)->X), FE25519_TO_STR((p)->Y), FE25519_TO_STR((p)->Z), FE25519_TO_STR((p)->T)

static const point_ed25519 ed25519_basepoint = {
    .X = ED25519_BASEPOINT_X,
    .Y = ED25519_BASEPOINT_Y,
    .Z = ED25519_BASEPOINT_Z,
    .T = ED25519_BASEPOINT_T
};
#if FE3C_ED25519_LATTICE_BASIS_REDUCTION
static const point_ed25519 ed25519_basepoint_times_2_128 = {
    .X = ED25519_BASEPOINT_TIMES_2_128_X,
    .Y = ED25519_BASEPOINT_TIMES_2_128_Y,
    .Z = ED25519_BASEPOINT_TIMES_2_128_Z,
    .T = ED25519_BASEPOINT_TIMES_2_128_T
};
static const point_ed25519 ed25519_basepoint_times_2_128_plus_1 = {
    .X = ED25519_BASEPOINT_TIMES_2_128_PLUS_1_X,
    .Y = ED25519_BASEPOINT_TIMES_2_128_PLUS_1_Y,
    .Z = ED25519_BASEPOINT_TIMES_2_128_PLUS_1_Z,
    .T = ED25519_BASEPOINT_TIMES_2_128_PLUS_1_T
};
#endif /* FE3C_ED25519_LATTICE_BASIS_REDUCTION */

#if FE3C_ENABLE_SANITY_CHECKS
static inline int ed25519_is_on_curve(const point_ed25519 *p)
{
    fe25519 x, y, z;
    fe25519 lhs;
    fe25519 rhs;

    /* Set x := X^2, y := Y^2, Z := Z^2 */
    fe25519_square(x, p->X);
    fe25519_square(y, p->Y);
    fe25519_square(z, p->Z);

    /* Set lhs := (Y^2 - X^2) Z^2 (left-hand side of the homogenous curve equation) */
    fe25519_sub(lhs, y, x);
    fe25519_mul(lhs, lhs, z);

    /* Set rhs := d X^2 Y^2 + Z^4 (right-hand side of the homogenous curve equation) */
    fe25519_mul(rhs, x, y);
    fe25519_mul(rhs, rhs, ed25519_d);
    fe25519_square(z, z);
    fe25519_add(rhs, rhs, z);

    /* Subtract the left-hand side from the right-hand side and check against zero */
    fe25519_sub(y, rhs, lhs);
    fe25519_strong_reduce(y, y);

    return fe25519_equal(fe25519_zero, y);
}

static inline int ed25519_valid_extended_projective(const point_ed25519 *p)
{
    /* Check the consistency of the extended projective coordinate */
    fe448 xy, tz;
    fe25519_mul(xy, p->X, p->Y);
    fe25519_mul(tz, p->T, p->Z);
    fe25519_strong_reduce(xy, xy);
    fe25519_strong_reduce(tz, tz);
    return fe25519_equal(tz, xy);
}
#endif /* FE3C_ENABLE_SANITY_CHECKS */

static inline void ed25519_identity(point_ed25519 *p)
{
    fe25519_copy(p->X, fe25519_zero);
    fe25519_copy(p->Y, fe25519_one);
    fe25519_copy(p->Z, fe25519_one);
    fe25519_copy(p->T, fe25519_zero);
}

static void ed25519_point_negate(point_ed25519 *p)
{
    fe25519_neg(p->X, p->X);
    fe25519_neg(p->T, p->T);
}

static void ed25519_points_add(point_ed25519 *r, const point_ed25519 *p, const point_ed25519 *q)
{
    FE3C_SANITY_CHECK(ed25519_is_on_curve(p), ED25519_STR, ED25519_TO_STR(p));
    FE3C_SANITY_CHECK(ed25519_is_on_curve(q), ED25519_STR, ED25519_TO_STR(q));
    FE3C_SANITY_CHECK(ed25519_valid_extended_projective(p), ED25519_STR, ED25519_TO_STR(p));
    FE3C_SANITY_CHECK(ed25519_valid_extended_projective(q), ED25519_STR, ED25519_TO_STR(q));

    /* For optimal stack and cache usage we reduce the number of variables
     * allocated relative to the algorithm description in RFC 8032. For
     * clarity, the comments include the names of variables as they appear
     * in RFC 8032. */
    fe25519 A, B, E;

    /* A := (Y1-X1)*(Y2-X2) */
    fe25519_sub(A, p->Y, p->X);
    fe25519_sub(B, q->Y, q->X);
    fe25519_mul(A, A, B);

    /* B := (Y1+X1)*(Y2+X2) */
    fe25519_add(E, p->Y, p->X);
    fe25519_add(B, q->Y, q->X);
    fe25519_mul(B, E, B);

    /* C := T1*2*d*T2 */
    fe25519_mul(r->T, p->T, q->T);
    fe25519_mul(r->T, r->T, ed25519_d);
    fe25519_add(r->T, r->T, r->T);

    /* D := Z1*2*Z2 */
    fe25519_mul(r->Z, p->Z, q->Z);
    fe25519_add(r->Z, r->Z, r->Z);

    /* E := B-A */
    fe25519_sub(E, B, A);
    /* F := D-C */
    fe25519_sub(r->X, r->Z, r->T);
    /* G := D+C */
    fe25519_add(r->Z, r->Z, r->T);
    /* H := B+A */
    fe25519_add(r->T, B, A);

    /* Y3 := G*H */
    fe25519_mul(r->Y, r->Z, r->T);
    /* T3 := E*H */
    fe25519_mul(r->T, E, r->T);
    /* Z3 := F*G */
    fe25519_mul(r->Z, r->X, r->Z);
    /* X3 := E*F */
    fe25519_mul(r->X, E, r->X);
}

static void ed25519_point_double(point_ed25519 *r, const point_ed25519 *p, int set_extended_coordinate)
{
    FE3C_SANITY_CHECK(ed25519_is_on_curve(p), ED25519_STR, ED25519_TO_STR(p));

    /* For optimal stack and cache usage we reduce the number of variables
     * allocated relative to the algorithm description in RFC 8032. For
     * clarity, the comments include the names of variables as they appear
     * in RFC 8032. */
    fe25519 G, H;

    /* A := X1^2 */
    fe25519_square(G, p->X);
    /* B := Y1^2*/
    fe25519_square(r->T, p->Y);

    /* H := A+B */
    fe25519_add(H, G, r->T);
    /* G := A-B */
    fe25519_sub(G, G, r->T);

    /* E := H-(X1+Y1)^2 */
    fe25519_add(r->T, p->X, p->Y);
    fe25519_square(r->T, r->T);
    fe25519_sub(r->T, H, r->T);

    /* C := 2*Z1^2 */
    fe25519_square(r->Z, p->Z);
    fe25519_add(r->Z, r->Z, r->Z);
    /* F := C+G */
    fe25519_add(r->Z, r->Z, G);

    /* X3 := E*F */
    fe25519_mul(r->X, r->T, r->Z);
    /* Y3 := G*H */
    fe25519_mul(r->Y, G, H);
    /* Z3 := F*G */
    fe25519_mul(r->Z, r->Z, G);

    /* When scheduled to be followed by another doubling we can skip setting the extended coordinate T
     * which is not needed for doubling */
    if (set_extended_coordinate)
        fe25519_mul(r->T, r->T, H);  /* T3 := E*H */
}

static int ed25519_points_equal_modulo_cofactor(const point_ed25519 *p, const point_ed25519 *q)
{
    point_ed25519 pmq;

    /* Compute the difference p-q */
    pmq = *q;
    ed25519_point_negate(&pmq);
    ed25519_points_add(&pmq, p, &pmq);
    /* Double the difference three times to obtain cofactor (8) times the difference */
    ed25519_point_double(&pmq, &pmq, 0);
    ed25519_point_double(&pmq, &pmq, 0);
    /* We ignore the extended coordinate, so it is safe to skip setting it */
    ed25519_point_double(&pmq, &pmq, 0);

    /* Check 8(p-q) against the identity */

    /* Check X = 0 */
    fe25519_strong_reduce(pmq.X, pmq.X);
    int equal = fe25519_equal(fe25519_zero, pmq.X);

    /* Check Y = Z */
    fe25519_sub(pmq.T, pmq.Y, pmq.Z);
    fe25519_strong_reduce(pmq.T, pmq.T);
    equal &= fe25519_equal(fe25519_zero, pmq.T);

    return equal;
}

static void ed25519_encode(u8 *buf, const point *pgen)
{
    const point_ed25519 *p = (const point_ed25519 *) pgen;
    fe25519 x;
    fe25519 y;
    fe25519 z_inv;

    /* Affinize the point */
    fe25519_invert(z_inv, p->Z);
    fe25519_mul(x, p->X, z_inv);
    fe25519_mul(y, p->Y, z_inv);

    /* Encode the y-coordinate */
    fe25519_encode(buf, y);
    /* Encode the "sign" of the x-coordinate (parity) in the most
     * significant bit of the last byte */
    FE3C_SANITY_CHECK((buf[31] >> 7) == 0, "buf[31] = 0x%x", buf[31]);
    buf[31] |= fe25519_lsb(x) << 7;

    /* Zeroize intermediate results to not leak any secrets via projective coordinates
     * (particularities of the representative of an equivalence class) */
    purge_secrets(z_inv, sizeof(z_inv));
}

static inline int ed25519_is_ok_order(const point_ed25519 *p)
{
    point_ed25519 e;
    ed25519_identity(&e);
    /* Check if p lies in the "cofactor subgroup" */
    return 1 - ed25519_points_equal_modulo_cofactor(p, &e);
}

static int ed25519_decode(point *pgen, const u8 *buf)
{
    point_ed25519 *p = (point_ed25519 *) pgen;

    int success = 1;
    /* Recover the "sign" or "parity" of the x-coordinate */
    int parity = buf[31] >> 7;
    success &= fe25519_decode(p->Y, buf);

    /* We now need to recover the x-coordinate. Note that the curve equation
     * -x^2 + y^2 = 1 + d x^2 y^2 implies that x^2 = u / v, where u = y^2 - 1
     * and v = d y^2 + 1. The denominator is always non-zero mod p = 2^255 - 19.
     * To compute the square root we first compute the candidate square root
     * x = (u/v)^{(p+3)/8}. Let a = (u/v), b = a^{(p+3)/8} and note that
     * a^{-1} b^2 = a^{-1} a^{(p+3)/4} = a^{(p-1)/4} which is the square root of
     * a^{(p-1)/2} which is necessarily equal to -1, 1, or 0 (Legendre symbol).
     * If it is equal to 1, then b^2 is equal to a and so b is the square root.
     * If it is equal to -1, then -b^2 = a and so the square root of a is i b,
     * where i is the "positive" square root of -1 equal (congruent mod p) to
     * 2^{(p-1)/4}. Otherwise no square root exists and decoding fails. */

    /* To compute (u/v)^{(p+3)/8} without inverting v we use the following
     * identity: (u/v)^{(p+3)/8} = u (u v)^{(p-5)/8}. Note that p = 5 (mod 8)
     * and so all the exponentiations are well defined. */

    fe25519 u;
    fe25519 v;
    /* Set u := y^2 */
    fe25519_square(u, p->Y);

    /* Copy y^2 to v */
    fe25519_copy(v, u);
    /* Subtract one to obtain u = y^2 - 1 */
    fe25519_sub(u, u, fe25519_one);
    fe25519_strong_reduce(u, u);
    /* Set v := d y^2 */
    fe25519_mul(v, v, ed25519_d);
    /* Set v := d y^2 + 1 */
    fe25519_add(v, v, fe25519_one);

    /* So as to not allocate any more variables on the stack reuse p->T
     * for temporary storage of intermediate results */

    /* Set p->T to u v */
    fe25519_mul(p->T, u, v);
    /* Raise p->T to (p-5)/8 */
    fe25519_exp_p_minus_5_over_8(p->T, p->T);

    /* Set p->X := u (u v)^{(p-5)/8} */
    fe25519_mul(p->X, u, p->T);

    /* Now we must consider the three cases:
     * 1. If v x^2 = u (mod p), then x is the square root
     * 2. If v x^2 = -u (mod p), then i x is the square root where i^2 = -1 and i = 2^{(p-1)/4}
     * 3. Otherwise, no square root exists and decoding fails
     */

    /* Compute v x^2 in p->T */
    fe25519_square(p->T, p->X);
    fe25519_mul(p->T, p->T, v);
    /* Normalize u and p->T before doing any comparisons */
    fe25519_strong_reduce(u, u);
    fe25519_strong_reduce(p->T, p->T);
    /* Check if we need to multiply by the square root of -1 */
    int multiply_by_i = 1 - fe25519_equal(p->T, u);
    /* Set p->T := i x */
    fe25519_mul(p->T, p->X, fe25519_i);

    /* At this point we have p->X equal to x as calculated by (u/v)^{(p+3)/8}, and v equal to i x.
     * If multiply_by_i is one, then we have excluded case 1. (v x^2 is definitely not equal to u).
     * In such a case we want to set p->X := i x: */
    fe25519_conditional_move(p->X, p->T, multiply_by_i);

    /* At this point p->X p->X v must be equal to u if we have a valid decoding (since p->X is either
     * x or i x by now). */
    fe25519_mul(v, v, p->X);
    fe25519_mul(v, v, p->X);
    fe25519_strong_reduce(v, v);
    /* Note that u has already been normalized */
    success &= fe25519_equal(v, u);

    /* We must still take into account the encoded sign of x. Set v equal to -x and conditionally
     * move it into p->X, if the parity of x is incorrect now */
    fe25519_neg(v, p->X);
    fe25519_conditional_move(p->X, v, parity ^ fe25519_lsb(p->X));
    /* RFC 8032 dictates that if x0, i.e. parity, is one and x=0, decoding fails, but since
     * the only points with x=0 (i.e. (0, 1) and (0, -1)) are of low order and so we reject
     * them anyway, we skip this check. */

    /* If decoding failed, set the result to identity to ensure we have a valid point and not
     * rely on any other code properly handling invalid ones */
    fe25519_conditional_move(p->X, fe25519_zero, 1 - success);
    fe25519_conditional_move(p->Y, fe25519_one, 1 - success);

    /* Set Z to one (normalized projective representation) */
    fe25519_copy(p->Z, fe25519_one);
    /* Set the extended coordinate T to the correct value */
    fe25519_mul(p->T, p->X, p->Y);
    /* Check that we have a valid point */
    success &= ed25519_is_ok_order(p);

    return success;
}

static inline void ed25519_conditional_move(point_ed25519 *r, const point_ed25519 *p, int move)
{
    fe25519_conditional_move(r->X, p->X, move);
    fe25519_conditional_move(r->Y, p->Y, move);
    fe25519_conditional_move(r->Z, p->Z, move);
    fe25519_conditional_move(r->T, p->T, move);
}

#if !FE3C_ED25519_COMB_METHOD
static inline void ed25519_conditional_swap(point_ed25519 *r, point_ed25519 *q, point_ed25519 *temp, int swap)
{
    /* We could implement this more efficiently, but if someone is using
     * naive scalar multiplication then they probably value space more
     * than speed */
    ed25519_conditional_move(temp, r, swap);
    ed25519_conditional_move(r, q, swap);
    ed25519_conditional_move(q, temp, swap);
}

static inline void ed25519_multiply_basepoint_ladder(point_ed25519 *r, const u8 *s)
{
    point_ed25519 R[3];
    ed25519_identity(&R[0]);
    R[1] = ed25519_basepoint;
    /* R[2] is an auxiliary buffer for swaps */

    int bits[2] = { 0, 0 };
    /* Do a simple "Montgomery ladder" (in the group). Note that the input scalar must have been pruned,
     * which results in bit 255 being cleared (and bit 254 being set), or reduced modulo the group order,
     * in which case bit 255 will also be cleared, so we can safely skip it. */
    for (int i = 254; i >= 0; i--) {

        /* Recover the ith bit of the scalar */
        bits[0] = array_bit(s, i);
        ed25519_conditional_swap(&R[0], &R[1], &R[2], bits[0] ^ bits[1]);
        ed25519_points_add(&R[1], &R[1], &R[0]);
        ed25519_point_double(&R[0], &R[0], 1);
        bits[1] = bits[0];
    }

    ed25519_conditional_swap(&R[0], &R[1], &R[2], bits[1]);
    *r = R[0];
    purge_secrets(&R, sizeof(R));
    purge_secrets(bits, sizeof(bits));
}
#else
static inline void ed25519_multiply_basepoint_comb(point_ed25519 *r, const u8 *s)
{
    /* Implement the comb method with signed digit scalar representation. Start by recoding the
     * scalar into an array of a columns S[d] each being an integer in signed digit representation
     * of length w (width-w SD), i.e. let:
     *
     *                                                  a-1
     *                                                  ___
     *                                                  \         dw
     *                    s = S[a-1]S[a-2]...S[1]S[0] = /__ S[d] 2        (sum of columns)
     *                                                  d=0
     *
     * Note that the exponent array (to borrow nomenclature from "Handbook of Applied Cryptography")
     * is organized top-to-bottom and then right-to-left as opposed to the traditional Lim/Lee
     * comb method, where it is the top row of the array that corresponds to the least significant bits
     * of the scalar. Here the rightmost column corresponds to w least significant bits of s.
     *
     * From right to left we divide the w x a blocks into w x v blocks, each of size b = ceil(a/v),
     * and obtain:
     *
     *       a-1              v-1 b-1                          b-1     v-1
     *       ___              ___ ___                          ___     ___
     *       \         dw     \   \              (jb + t)w     \    tw \              jbw
     *  sP = /__ S[d] 2   P = /__ /__ S[jb + t] 2          P = /__ 2   /__ S[jb + t] 2    P
     *       d=0              j=0 t=0                          t=0     j=0
     *
     * where S[jb + t] is width-w SD representation (note that iterating over a columns is the same
     * as iterating over v vertical subblocks and within each subblock iterating over the b columns).
     *
     * We have precomputed the values:
     *
     *                                      w-1            w-2
     *                   G[0][kd] = e[w-1] 2   P + e[w-2] 2   P + ... + e[0] P = kdP
     *
     *                                           jbw            jbw
     *                               G[j][kd] = 2   G[0][kd] = 2    kdP
     *
     * where the index kd is equal to the decimal value of (e[w-1]...e[1]e[0]). Then sP can be written
     * as:
     *
     *                                        b-1     v-1
     *                                        ___     ___
     *                                        \    tw \
     *                                   sP = /__ 2   /__ G[j][S[jb + t]]
     *                                        t=0     j=0
     *
     * Note that the element for S[jb + t] = 0 is the group identity which we do not bother storing
     * in the precomputation table and so we offset the table by one (note that for Edwards curves
     * we are protected against any side-channel attacks here since the formula for adding the identity
     * is the same as for adding any other point). Also note that G[j][i] = -G[j][-i] (since we use SD
     * encoding) and since negating elliptic curve points is almost free, we can store only the points
     * corresponding to positive indices S[jb + t].
     */

    /* Use w = 4 (width-4 SD) and v = 32. For scalars of length 256 (actually less than that, but
     * a power of two is easier to work with plus with signed digits we may get an additional 4-bit
     * digit) we get a = 64 and b = 2. */
    i8 recoding[64];
    ed25519_comb_recode_scalar_into_width4_sd(recoding, s);

    ed25519_identity(r);
    ed25519_precomp p;
    /* Since we have b=2 we only have two iterations of the outermost loop of the algorithm
     * (Algorithm 3 in "Improved Fixed-base Comb Method for Fast Scalar Multiplication") which
     * implements the double summation at the end of the above description. Also for the second
     * iteration we have t=0 so we can skip the 2^{tw} = 2^0. Note that iterating from j=0 to v-1
     * over G[j][S[jb+t]] for t=1 corresponds to only accessing the elements of G (precomp table)
     * indexed by odd positions of the recoding above (jb+t = 1, 3, 5, ...), since b=2. Similarly,
     * when t=0 (second iteration) we shall only access the elements of G indexed by even-index
     * entries of the recoding. */
    for (int i = 1; i < 64; i += 2) {

        /* We let the loop index run twice as fast and skip every other entry of recoding,
         * but correct for it in the j index (j = i / 2) */
        ed25519_comb_read_precomp(&p, i >> 1, recoding[i]);
        ed25519_comb_add_precomp(r, r, &p);
    }

    /* Compute 2^{tw} Q = 2^4 Q */
    ed25519_point_double(r, r, 0);
    ed25519_point_double(r, r, 0);
    ed25519_point_double(r, r, 0);
    ed25519_point_double(r, r, 1);

    /* Do the second iteration of the outermost loop, i.e. iterate over even indices of the recoding
     * (see explanation above). */
    for (int i = 0; i < 64; i += 2) {

        /* We let the loop index run twice as fast and skip every other entry of recoding,
         * but correct for it in the j index (j = i / 2) */
        ed25519_comb_read_precomp(&p, i >> 1, recoding[i]);
        ed25519_comb_add_precomp(r, r, &p);
    }

    /* At this point Q := 2^{tw} Q is a no-op since 2^{tw} Q = 2^0 Q */

    /* Clear the recoding of the secret scalar from the stack */
    purge_secrets(recoding, sizeof(recoding));
    /* Clear the last accessed precomputed point */
    purge_secrets(&p, sizeof(p));
}
#endif /* !FE3C_ED25519_COMB_METHOD */

static void ed25519_multiply_basepoint(point *rgen, const u8 *s)
{
    point_ed25519 *r = (point_ed25519 *) rgen;
#if !FE3C_ED25519_COMB_METHOD
    ed25519_multiply_basepoint_ladder(r, s);
#else
    ed25519_multiply_basepoint_comb(r, s);
#endif /* !FE3C_ED25519_COMB_METHOD */
}

#if !FE3C_ED25519_LATTICE_BASIS_REDUCTION
static inline void ed25519_double_scalar_multiply_vartime(point_ed25519 *r, const u8 *s, const u8 *h, const point_ed25519 *p)
{
    /* To reduce stack usage, only store pointers to the public key A
     * and the basepoint B, and allocate only one point (A+B) */
    point_ed25519 sum;
    ed25519_points_add(&sum, p, &ed25519_basepoint);
    const point_ed25519 *lookup[3];
    lookup[0] = &ed25519_basepoint;  /* [0]A + [1]B */
    lookup[1] = p;                   /* [1]A + [0]B */
    lookup[2] = &sum;                /* [1]A + [1]B */

    ed25519_identity(r);

    /* Note that canonical scalars have at most 253 bits, which
     * corresponds to the loop counter initialization */
    for (int i = 252; i >= 0; i--) {

        ed25519_point_double(r, r, 1);
        int index = (array_bit(h, i) << 1) | array_bit(s, i);
        if (index)
            ed25519_points_add(r, r, lookup[index - 1]);
    }
}
#endif /* !FE3C_ED25519_LATTICE_BASIS_REDUCTION */

static int ed25519_check_group_equation(point *pubkey_gen, point *commit_gen, const u8 *challenge, const u8 *response)
{
    point_ed25519 *public_key = (point_ed25519 *) pubkey_gen;
    point_ed25519 *commitment = (point_ed25519 *) commit_gen;

#if !FE3C_ED25519_LATTICE_BASIS_REDUCTION
    point_ed25519 commitment_pretender;
    /* Compute S*B - h*A */
    ed25519_point_negate(public_key);
    ed25519_double_scalar_multiply_vartime(&commitment_pretender, response, challenge, public_key);
    /* Check if 2^c*(S*B - h*A) == 2^c*R */
    return ed25519_points_equal_modulo_cofactor(&commitment_pretender, commitment);
#else
    /* Follow eprint:2020/454 and find a value k such that both
     * k and k*h (where h is the challenge) are short modulo the
     * group order L. Instead of verifying [c][S]B = [c]R + [c][h]A, we
     * shall check that:
     *
     *               [c]([kS]B - [k]R - [kh]A) = (0, 1)
     *
     * Both k (mod L) and kh (mod L) will be below 128 bits in
     * length, and kS we will split in half and use the precomputed
     * value of [2^{128}]B to cut the number of point doublings in half.
     */

    u8 delta[16];
    int delta_negative;
    u8 delta_challenge[16];
    int delta_challenge_negative;
    u8 delta_response[32];
    /* Construct a lattice spanned by { (L, 0), (h, 1) } and find its
     * size-reduced basis. Specifically, we are interested in a short
     * vector (v0, v1) with v0 = v1*h (mod L). Both v0 and v1 may be
     * negative and we must not normalize them to range [0, L), since
     * L-vi will not be short anymore. Instead, return absolute values
     * delta_challenge = |v0| and delta = |v1|, but keep track of the
     * signs. Based on the signs, we shall negate the group elements
     * before doing the multi-scalar multiplication below. Compute
     * also v1*S (mod L), which we will split in half as described above. */
    ed25519_lattice_basis_reduction(
        delta_challenge,
        &delta_challenge_negative,
        delta,
        &delta_negative,
        delta_response,
        challenge,
        response
    );

    /* Negate the input points only if their associated scalars remained positive */
    if (!delta_negative)
        ed25519_point_negate(commitment);
    if (!delta_challenge_negative)
        ed25519_point_negate(public_key);

    /* Use two small lookup tables instead of one large one. It is possible to precompute more combinations of the points
     * here and use a 15-entry lookup table saving a point addition in each loop iteration. */
    const point_ed25519 *right_lookup[3];
    const point_ed25519 *left_lookup[3];

    point_ed25519 sum;
    ed25519_points_add(&sum, commitment, public_key);
    right_lookup[0] = commitment;
    right_lookup[1] = public_key;
    right_lookup[2] = &sum;

    left_lookup[0] = &ed25519_basepoint;
    left_lookup[1] = &ed25519_basepoint_times_2_128;
    left_lookup[2] = &ed25519_basepoint_times_2_128_plus_1;

    point_ed25519 accumulator;
    ed25519_identity(&accumulator);
    for (int i = 127; i >= 0; i--) {

        ed25519_point_double(&accumulator, &accumulator, 1);
        int right_lookup_index = (array_bit(delta_challenge, i) << 1) | array_bit(delta, i);
        if (right_lookup_index)
            ed25519_points_add(&accumulator, &accumulator, right_lookup[right_lookup_index - 1]);
        int left_lookup_index = (array_bit(delta_response, 128 + i) << 1) | array_bit(delta_response, i);
        if (left_lookup_index)
            ed25519_points_add(&accumulator, &accumulator, left_lookup[left_lookup_index - 1]);
    }

    point_ed25519 identity;
    ed25519_identity(&identity);
    /* Clear the cofactor and check if the result is the identity */
    return ed25519_points_equal_modulo_cofactor(&accumulator, &identity);
#endif /* !FE3C_ED25519_LATTICE_BASIS_REDUCTION */
}

const group_ops ed25519_group_ops = {
    .encode = ed25519_encode,
    .decode = ed25519_decode,
    .multiply_basepoint = ed25519_multiply_basepoint,
    .check_group_equation = ed25519_check_group_equation
};
