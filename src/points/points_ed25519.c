#include <points/points.h>
#include <field_elements/field_elements_ed25519.h>
#include <utils/utils.h>
#if FE3C_OPTIMIZATION_COMB_METHOD
    #include <points/points_ed25519_comb_method.h>
#endif /* FE3C_OPTIMIZATION_COMB_METHOD */

#define ED25519_STR \
    "    X = " FE25519_STR "\n" \
    "    Y = " FE25519_STR "\n" \
    "    Z = " FE25519_STR "\n" \
    "    T = " FE25519_STR
#define ED25519_TO_STR(p) \
    FE25519_TO_STR((p)->X), FE25519_TO_STR((p)->Y), FE25519_TO_STR((p)->Z), FE25519_TO_STR((p)->T)

static const point_ed25519 ed25519_basepoint = {
#if FE3C_64BIT
    .X = { 0x62d608f25d51a, 0x412a4b4f6592a, 0x75b7171a4b31d, 0x1ff60527118fe, 0x216936d3cd6e5 },
    .Y = { 0x6666666666658, 0x4cccccccccccc, 0x1999999999999, 0x3333333333333, 0x6666666666666 },
    .Z = { 1, 0, 0, 0, 0 },
    .T = { 0x68ab3a5b7dda3, 0xeea2a5eadbb, 0x2af8df483c27e, 0x332b375274732, 0x67875f0fd78b7 }
#else
    .X = { 0x325d51a, 0x18b5823, 0xf6592a, 0x104a92d, 0x1a4b31d, 0x1d6dc5c, 0x27118fe, 0x7fd814, 0x13cd6e5, 0x85a4db },
    .Y = { 0x2666658, 0x1999999, 0xcccccc, 0x1333333, 0x1999999, 0x666666, 0x3333333, 0xcccccc, 0x2666666, 0x1999999 },
    .Z = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    .T = { 0x1b7dda3, 0x1a2ace9, 0x25eadbb, 0x3ba8a, 0x83c27e, 0xabe37d, 0x1274732, 0xccacdd, 0xfd78b7, 0x19e1d7c }
#endif /* FE3C_64BIT */
};

#if FE3C_ENABLE_SANITY_CHECKS
static inline int ed25519_is_on_curve(const point_ed25519 * p) {

    fe25519 x, y, z;
    fe25519 lhs;
    fe25519 rhs;

    /* Set x := X^2, y := Y^2, Z := Z^2 */
    fe_square(x, p->X);
    fe_square(y, p->Y);
    fe_square(z, p->Z);

    /* Set lhs := (Y^2 - X^2) Z^2 (left-hand side of the homogenous curve equation) */
    fe_sub(lhs, y, x);
    fe_mul(lhs, lhs, z);

    /* Set rhs := d X^2 Y^2 + Z^4 (right-hand side of the homogenous curve equation) */
    fe_mul(rhs, x, y);
    fe_mul(rhs, rhs, ed25519_d);
    fe_square(z, z);
    fe_add(rhs, rhs, z);

    /* Subtract the left-hand side from the right-hand side and check against zero */
    fe_sub(y, rhs, lhs);
    fe_strong_reduce(y, y);

    return fe_equal(fe_zero, y);
}

static inline int ed25519_valid_extended_projective(const point_ed25519 * p) {

    /* Check the consistency of the extended projective coordinate */
    fe448 xy, tz;
    fe_mul(xy, p->X, p->Y);
    fe_mul(tz, p->T, p->Z);
    fe_strong_reduce(xy, xy);
    fe_strong_reduce(tz, tz);
    return fe_equal(tz, xy);
}
#endif /* FE3C_ENABLE_SANITY_CHECKS */

static inline void ed25519_identity(point_ed25519 * p) {

    fe_copy(p->X, fe_zero);
    fe_copy(p->Y, fe_one);
    fe_copy(p->Z, fe_one);
    fe_copy(p->T, fe_zero);
}

static int ed25519_points_equal(const point * pgen, const point * qgen) {

    const point_ed25519 * p = (const point_ed25519 *) pgen;
    const point_ed25519 * q = (const point_ed25519 *) qgen;

    fe25519 lhs;
    fe25519 rhs;

    /* Check X1 Z2 = X2 Z1 */
    fe_mul(lhs, p->X, q->Z);
    fe_mul(rhs, q->X, p->Z);
    fe_strong_reduce(lhs, lhs);
    fe_strong_reduce(rhs, rhs);
    int equal = fe_equal(lhs, rhs);

    /* Check Y1 Z2 = Y2 Z1 */
    fe_mul(lhs, p->Y, q->Z);
    fe_mul(rhs, q->Y, p->Z);
    fe_strong_reduce(lhs, lhs);
    fe_strong_reduce(rhs, rhs);
    equal &= fe_equal(lhs, rhs);

    return equal;
}

static void ed25519_encode(u8 * buf, const point * pgen) {

    const point_ed25519 * p = (const point_ed25519 *) pgen;
    fe25519 x;
    fe25519 y;
    fe25519 z_inv;

    /* Affinize the point */
    fe_invert(z_inv, p->Z);
    fe_mul(x, p->X, z_inv);
    fe_mul(y, p->Y, z_inv);

    /* Encode the y-coordinate */
    fe_encode(buf, y);
    /* Encode the "sign" of the x-coordinate (parity) in the most
     * significant bit of the last byte */
    FE3C_SANITY_CHECK((buf[31] >> 7) == 0, "buf[31] = 0x%x", buf[31]);
    buf[31] |= fe_lsb(x) << 7;

    /* Zeroize intermediate results to not leak any secrets via projective coordinates
     * (particularities of the representative of an equivalence class) */
    purge_secrets(z_inv, sizeof(z_inv));
}

static void ed25519_point_double(point_ed25519 * r, const point_ed25519 * p, int set_extended_coordinate) {

    FE3C_SANITY_CHECK(ed25519_is_on_curve(p), ED25519_STR, ED25519_TO_STR(p));

    /* TODO: Reuse some old variables to reduce stack usage */
    fe25519 A, B, C, E, F, G, H;

    /* A := X1^2 */
    fe_square(A, p->X);
    /* B := Y1^2*/
    fe_square(B, p->Y);

    /* C := 2*Z1^2 */
    fe_square(C, p->Z);
    fe_add(C, C, C);

    /* H := A+B */
    fe_add(H, A, B);

    /* E := H-(X1+Y1)^2 */
    fe_add(E, p->X, p->Y);
    fe_square(E, E);
    fe_sub(E, H, E);

    /* G := A-B */
    fe_sub(G, A, B);

    /* F := C+G */
    fe_add(F, C, G);

    /* X3 := E*F */
    fe_mul(r->X, E, F);
    /* Y3 := G*H */
    fe_mul(r->Y, G, H);
    /* Z3 := F*G */
    fe_mul(r->Z, F, G);

    /* When scheduled to be followed by another doubling we can skip setting the extended coordinate T
     * which is not needed for doubling */
    if (set_extended_coordinate) {

        /* T3 := E*H */
        fe_mul(r->T, E, H);
    }
}

static inline int ed25519_is_ok_order(const point_ed25519 * p) {

    /* TODO: Check against predefined table of low-order points (or reject even mixed-order points
     * by checking that L times the point gives the group identity) */
    point_ed25519 q;
    point_ed25519 e;
    ed25519_identity(&e);

    /* Double the point three times to obtain cofactor (8) times the point */
    ed25519_point_double(&q, p, 0);
    ed25519_point_double(&q, &q, 0);
    /* Do not assume that the ed25519_points_equal implementation ignores the extended coordinate
     * and set it in the doubling procedure */
    ed25519_point_double(&q, &q, 1);

    /* Check if equal to the identity */
    return 1 - ed25519_points_equal((const point *) &q, (const point *) &e);
}

static int ed25519_decode(point * pgen, const u8 * buf) {

    point_ed25519 * p = (point_ed25519 *) pgen;

    int success = 1;
    /* Recover the "sign" or "parity" of the x-coordinate */
    int parity = buf[31] >> 7;
    success &= fe_decode(p->Y, buf);

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
    fe_square(u, p->Y);

    /* Copy y^2 to v */
    fe_copy(v, u);
    /* Subtract one to obtain u = y^2 - 1 */
    fe_sub(u, u, fe_one);
    fe_strong_reduce(u, u);
    /* Set v := d y^2 */
    fe_mul(v, v, ed25519_d);
    /* Set v := d y^2 + 1 */
    fe_add(v, v, fe_one);

    /* So as to not allocate any more variables on the stack reuse p->T
     * for temporary storage of intermediate results */

    /* Set p->T to u v */
    fe_mul(p->T, u, v);
    /* Raise p->T to (p-5)/8 */
    fe_exp_p_minus_5_over_8(p->T, p->T);

    /* Set p->X := u (u v)^{(p-5)/8} */
    fe_mul(p->X, u, p->T);

    /* Now we must consider the three cases:
     * 1. If v x^2 = u (mod p), then x is the square root
     * 2. If v x^2 = -u (mod p), then i x is the square root where i^2 = -1 and i = 2^{(p-1)/4}
     * 3. Otherwise, no square root exists and decoding fails
     */

    /* Compute v x^2 in p->T */
    fe_square(p->T, p->X);
    fe_mul(p->T, p->T, v);
    /* Normalize u and p->T before doing any comparisons */
    fe_strong_reduce(u, u);
    fe_strong_reduce(p->T, p->T);
    /* Check if we need to multiply by the square root of -1 */
    int multiply_by_i = 1 - fe_equal(p->T, u);
    /* Set p->T := i x */
    fe_mul(p->T, p->X, fe_i);

    /* At this point we have p->X equal to x as calculated by (u/v)^{(p+3)/8}, and v equal to i x.
     * If multiply_by_i is one, then we have excluded case 1. (v x^2 is definitely not equal to u).
     * In such a case we want to set p->X := i x: */
    fe_conditional_move(p->X, p->T, multiply_by_i);

    /* At this point p->X p->X v must be equal to u if we have a valid decoding (since p->X is either
     * x or i x by now). */
    fe_mul(v, v, p->X);
    fe_mul(v, v, p->X);
    fe_strong_reduce(v, v);
    /* Note that u has already been normalized */
    success &= fe_equal(v, u);

    /* We must still take into account the encoded sign of x. Set v equal to -x and conditionally
     * move it into p->X, if the parity of x is incorrect now */
    fe_neg(v, p->X);
    fe_conditional_move(p->X, v, parity ^ fe_lsb(p->X));
    /* RFC 8032 dictates that if x0, i.e. parity, is one and x=0, decoding fails, but since
     * the only points with x=0 (i.e. (0, 1) and (0, -1)) are of low order and so we reject
     * them anyway, we skip this check. */

    /* If decoding failed, set the result to identity to ensure we have a valid point and not
     * rely on any other code properly handling invalid ones */
    fe_conditional_move(p->X, fe_zero, 1 - success);
    fe_conditional_move(p->Y, fe_one, 1 - success);

    /* Set Z to one (normalized projective representation) */
    fe_copy(p->Z, fe_one);
    /* Set the extended coordinate T to the correct value */
    fe_mul(p->T, p->X, p->Y);
    /* Check that we have a valid point */
    success &= ed25519_is_ok_order(p);

    return success;
}

static void ed25519_points_add(point_ed25519 * r, const point_ed25519 * p, const point_ed25519 * q) {

    FE3C_SANITY_CHECK(ed25519_is_on_curve(p), ED25519_STR, ED25519_TO_STR(p));
    FE3C_SANITY_CHECK(ed25519_is_on_curve(q), ED25519_STR, ED25519_TO_STR(q));
    FE3C_SANITY_CHECK(ed25519_valid_extended_projective(p), ED25519_STR, ED25519_TO_STR(p));
    FE3C_SANITY_CHECK(ed25519_valid_extended_projective(q), ED25519_STR, ED25519_TO_STR(q));

    /* TODO: Reuse some old variables to reduce stack usage */
    fe25519 A, B, C, D, E, F, G, H;

    /* A := (Y1-X1)*(Y2-X2) */
    fe_sub(E, p->Y, p->X);
    fe_sub(F, q->Y, q->X);
    fe_mul(A, E, F);

    /* B := (Y1+X1)*(Y2+X2) */
    fe_add(G, p->Y, p->X);
    fe_add(H, q->Y, q->X);
    fe_mul(B, G, H);

    /* C := T1*2*d*T2 */
    fe_mul(C, p->T, q->T);
    fe_mul(C, C, ed25519_d);
    fe_add(C, C, C);

    /* D := Z1*2*Z2 */
    fe_mul(D, p->Z, q->Z);
    fe_add(D, D, D);

    /* E := B-A */
    fe_sub(E, B, A);
    /* F := D-C */
    fe_sub(F, D, C);
    /* G := D+C */
    fe_add(G, D, C);
    /* H := B+A */
    fe_add(H, B, A);

    /* X3 := E*F */
    fe_mul(r->X, E, F);
    /* Y3 := G*H */
    fe_mul(r->Y, G, H);
    /* T3 := E*H */
    fe_mul(r->T, E, H);
    /* Z3 := F*G */
    fe_mul(r->Z, F, G);
}

#if !FE3C_OPTIMIZATION_COMB_METHOD
static inline void ed25519_scalar_multiply(point_ed25519 * r, const point_ed25519 * p, const u8 * s) {

    FE3C_SANITY_CHECK(ed25519_is_on_curve(p), ED25519_STR, ED25519_TO_STR(p));

    point_ed25519 R[2];
    ed25519_identity(&R[0]);
    R[1] = *p;

    /* Do a simple Montgomery ladder */
    for (int i = 255; i >= 0; i--) {

        /* Recover the ith bit of the scalar */
        int bit = ( s[i >> 3] >> (i & 0x7) ) & 1;
        ed25519_points_add(&R[1 - bit], &R[1 - bit], &R[bit]);
        ed25519_point_double(&R[bit], &R[bit], 1);
    }

    *r = R[0];
    purge_secrets(&R[1], sizeof(R[1]));
}
#endif /* !FE3C_OPTIMIZATION_COMB_METHOD */

static void ed25519_multiply_basepoint(point * rgen, const u8 * s) {

    point_ed25519 * r = (point_ed25519 *) rgen;
#if !FE3C_OPTIMIZATION_COMB_METHOD
    ed25519_scalar_multiply(r, &ed25519_basepoint, s);
#else
    /* Implement the improved comb method from "Improved Fixed-base Comb Method for Fast
     * Scalar Multiplication" by Mohamed et. al. Start by recoding the scalar into an array
     * of a columns S[d] each being an integer represented in non-adjacent form (NAF) of length w
     * (width-w NAF), i.e. let:
     *
     *                                                  a-1
     *                                                  ___
     *                                                  \         dw
     *                    s = S[a-1]S[a-2]...S[1]S[0] = /__ S[d] 2        (sum of columns)
     *                                                  d=0
     *
     * Note that the exponent array (to borrow nomenclature from "Handbook of Applied Cryptography")
     * is organized top-to-bottom and then right-to-left as opposed to the traditional Lim/Lee
     * comb method, where it is the top row of the array that corresponds to a least significant bits
     * of the scalar. Here the rightmost column corresponds to w least significant bits of s.
     *
     * From right to left we divide the w x a blocks into w x v blocks, each of size b = ceil(a/v),
     * and obtain:
     *
     *       a-1              v-1 b-1                          v-1     b-1
     *       ___              ___ ___                          ___     ___
     *       \         dw     \   \              (jb + t)w     \    tw \              jbw
     *  sP = /__ S[d] 2   P = /__ /__ S[jb + t] 2          P = /__ 2   /__ S[jb + t] 2    P
     *       d=0              j=0 t=0                          j=0     t=0
     *
     * where S[jb + t] is width-w NAF representation (note that iterating over a columns is the same
     * as iterating over v vertical subblocks and within each subblock iterating over the b columns).
     *
     * We have precomputed the values:
     *
     *                                      w-1            w-2
     *                   G[0][kd] = e[w-1] 2   P + e[w-2] 2   P + ... + e[0] P = kdP
     *
     *                                           jwb            jwb
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
     * is the same as for adding any other point). Also note that G[j][i] = -G[j][-i] (since we use NAF
     * encoding) and since negating elliptic curve points is almost free, we can store only the points
     * corresponding to positive indices S[jb + t].
     */

    /* Use w = 4 (width-4 NAF) and v = 32. For scalars of length 256 (actually less than that, but
     * a power of two is easier to work with plus with NAF we may get an additional 4-bit digit) we
     * get a = 64 and b = 2. */
    i8 naf[64];
    ed25519_comb_recode_scalar_into_4naf(naf, s);

    ed25519_identity(r);
    point_precomp p;
    /* Since we have b=2 we only have two iterations of the outermost loop of the algorithm
     * (Algorithm 3 in "Improved Fixed-base Comb Method for Fast Scalar Multiplication") which
     * implements the double summation at the end of the above description. Also for the second
     * iteration we have t=0 so we can skip the 2^{tw} = 2^0. Note that iterating from j=0 to v-1
     * over G[j][S[jb+t]] for t=1 corresponds to only accessing the elements of G (precomp table)
     * indexed by odd positions of the recoding above (jb+t = 1, 3, 5, ...), since b=2. Similarly,
     * when t=0 (second iteration) we shall only access the elements of G indexed by even-index
     * entries of the recoding. */
    for (int i = 1; i < 64; i += 2) {

        /* We let the loop index run twice as fast and skip every other entry of naf,
         * but correct for it in the j index (j = i / 2) */
        ed25519_comb_read_precomp(&p, i >> 1, naf[i]);
        /* For the last iteration skip setting the extended coordinate */
        ed25519_points_add_precomp(r, r, &p, i < 62);
    }

    /* Compute 2^{tw} Q = 2^4 Q */
    ed25519_point_double(r, r, 0);
    ed25519_point_double(r, r, 0);
    ed25519_point_double(r, r, 0);
    ed25519_point_double(r, r, 1);

    /* Do the second iteration of the outermost loop, i.e. iterate over even indices of the recoding
     * (see explanation above). */
    for (int i = 0; i < 64; i += 2) {

        /* We let the loop index run twice as fast and skip every other entry of naf,
         * but correct for it in the j index (j = i / 2) */
        ed25519_comb_read_precomp(&p, i >> 1, naf[i]);
        /* For the last iteration skip setting the extended coordinate */
        ed25519_points_add_precomp(r, r, &p, i < 62);
    }

    /* At this point Q := 2^{tw} Q is a no-op since 2^{tw} Q = 2^0 Q */

    /* Clear the recoding of the secret scalar from the stack */
    purge_secrets(naf, sizeof(naf));
    /* Clear the last accessed precomputed point */
    purge_secrets(&p, sizeof(p));
#endif /* !FE3C_OPTIMIZATION_COMB_METHOD */
}

static void ed25519_point_negate(point * pgen) {

    point_ed25519 * p = (point_ed25519 *) pgen;
    fe_neg(p->X, p->X);
    fe_neg(p->T, p->T);
}

static inline void ed25519_conditional_move(point_ed25519 * r, const point_ed25519 * p, int move) {

    fe_conditional_move(r->X, p->X, move);
    fe_conditional_move(r->Y, p->Y, move);
    fe_conditional_move(r->Z, p->Z, move);
    fe_conditional_move(r->T, p->T, move);
}

static void ed25519_double_scalar_multiply(point * rgen, const u8 * s, const u8 * h, const point * pgen) {

    point_ed25519 * r = (point_ed25519 *) rgen;
    const point_ed25519 * p = (const point_ed25519 *) pgen;

    point_ed25519 G[15];
    G[0] = ed25519_basepoint;                    /* [0]A + [1]B */
    ed25519_point_double(&G[ 1], &G[ 0], 1);     /* [0]A + [2]B */
    ed25519_points_add(&G[ 2], &G[ 1], &G[ 0]);  /* [0]A + [3]B */
    G[3] = *p;                                   /* [1]A + [0]B */
    ed25519_points_add(&G[ 4], &G[ 3], &G[ 0]);  /* [1]A + [1]B */
    ed25519_points_add(&G[ 5], &G[ 3], &G[ 1]);  /* [1]A + [2]B */
    ed25519_points_add(&G[ 6], &G[ 3], &G[ 2]);  /* [1]A + [3]B */
    ed25519_point_double(&G[ 7], &G[ 3], 1);     /* [2]A + [0]B */
    ed25519_points_add(&G[ 8], &G[ 7], &G[ 0]);  /* [2]A + [1]B */
    ed25519_point_double(&G[ 9], &G[ 4], 1);     /* [2]A + [2]B */
    ed25519_points_add(&G[10], &G[ 7], &G[ 2]);  /* [2]A + [3]B */
    ed25519_points_add(&G[11], &G[ 7], &G[ 3]);  /* [3]A + [0]B */
    ed25519_points_add(&G[12], &G[11], &G[ 0]);  /* [3]A + [1]B */
    ed25519_points_add(&G[13], &G[11], &G[ 1]);  /* [3]A + [2]B */
    ed25519_points_add(&G[14], &G[11], &G[ 2]);  /* [3]A + [3]B */

    /* Represent the two scalars as arrays of base-4 digits:
     *
     *                 s = (s127, s126, ..., s2, s1, s0)
     *                 h = (h127, h126, ..., h2, h1, h0)
     *
     * where si, hi are in { 0, 1, 2, 3 }. At each iteration i add to the
     * accumulator the point [si]B + [hi]A. Use the two base-4 digits to
     * find the point in the precomputation table.
     */
    ed25519_identity(r);

    for (int i = 127; i >= 0; i--) {

        ed25519_point_double(r, r, 0);
        ed25519_point_double(r, r, 1);

        /* Recode the scalars on the fly into base-4 representation */
        u8 ss = (array_bit(s, 2*i + 1) << 1) | array_bit(s, 2*i);
        u8 hh = (array_bit(h, 2*i + 1) << 1) | array_bit(h, 2*i);
        u8 index = (hh << 2) | ss;
        /* TODO: Study how we can use NAF here to reduce the amount
         * of precomputation. The problem with NAF seems to be that
         * we must additionally precompute the sums [n]A + [k]B where
         * n and k have different signs. */

        /* Access all elements of the array to prevent cache-based timing
         * attacks. Note that for most applications constant-time signature
         * verification is not needed. It is for the few that care that we
         * do this. */
        point_ed25519 t;
        ed25519_identity(&t);
        ed25519_conditional_move(&t, &G[ 0], byte_equal(index,  1));
        ed25519_conditional_move(&t, &G[ 1], byte_equal(index,  2));
        ed25519_conditional_move(&t, &G[ 2], byte_equal(index,  3));
        ed25519_conditional_move(&t, &G[ 3], byte_equal(index,  4));
        ed25519_conditional_move(&t, &G[ 4], byte_equal(index,  5));
        ed25519_conditional_move(&t, &G[ 5], byte_equal(index,  6));
        ed25519_conditional_move(&t, &G[ 6], byte_equal(index,  7));
        ed25519_conditional_move(&t, &G[ 7], byte_equal(index,  8));
        ed25519_conditional_move(&t, &G[ 8], byte_equal(index,  9));
        ed25519_conditional_move(&t, &G[ 9], byte_equal(index, 10));
        ed25519_conditional_move(&t, &G[10], byte_equal(index, 11));
        ed25519_conditional_move(&t, &G[11], byte_equal(index, 12));
        ed25519_conditional_move(&t, &G[12], byte_equal(index, 13));
        ed25519_conditional_move(&t, &G[13], byte_equal(index, 14));
        ed25519_conditional_move(&t, &G[14], byte_equal(index, 15));

        ed25519_points_add(r, r, &t);
    }
}

group_ops ed25519_group_ops = {
    .points_equal = ed25519_points_equal,
    .encode = ed25519_encode,
    .decode = ed25519_decode,
    .multiply_basepoint = ed25519_multiply_basepoint,
    .point_negate = ed25519_point_negate,
    .double_scalar_multiply = ed25519_double_scalar_multiply
};
