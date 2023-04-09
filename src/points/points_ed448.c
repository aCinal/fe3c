#if FE3C_ED448_ISOGENY
    #error "Build system inconsistency detected! points_ed448.c in use despite FE3C_ED448_ISOGENY being set"
#endif /* FE3C_ED448_ISOGENY */

#include <points/points.h>
#include <field_elements/field_elements_ed448.h>
#include <utils/utils.h>
#if FE3C_COMB_METHOD
    #include <points/comb/comb_ed448.h>
#endif /* FE3C_COMB_METHOD */

#define ED448_STR \
    "    X = " FE448_STR "\n" \
    "    Y = " FE448_STR "\n" \
    "    Z = " FE448_STR
#define ED448_TO_STR(p) \
    FE448_TO_STR((p)->X), FE448_TO_STR((p)->Y), FE448_TO_STR((p)->Z)

static const point_ed448 ed448_basepoint = {
    .X = ED448_BASEPOINT_X,
    .Y = ED448_BASEPOINT_Y,
    .Z = ED448_BASEPOINT_Z
};

#if FE3C_ENABLE_SANITY_CHECKS
static inline int ed448_is_on_curve(const point_ed448 * p) {

    fe448 x, y, z;
    fe448 lhs;
    fe448 rhs;

    /* Check if the point belongs to the untwisted Edwards curve defined
     * by the equation:
     *
     *                     x^2 + y^2 = 1 + dx^2y^2
     *
     * where d = -39081.
     */

    /* Set x := X^2, y := Y^2, Z := Z^2 */
    fe448_square(x, p->X);
    fe448_square(y, p->Y);
    fe448_square(z, p->Z);

    /* Set lhs := (Y^2 + X^2) Z^2 (left-hand side of the homogenous curve equation) */
    fe448_add(lhs, y, x);
    fe448_mul(lhs, lhs, z);

    /* Set rhs := d X^2 Y^2 + Z^4 (right-hand side of the homogenous curve equation) */
    fe448_mul(rhs, x, y);
    fe448_mul(rhs, rhs, ed448_d);
    fe448_square(z, z);
    fe448_add(rhs, rhs, z);

    /* Subtract the left-hand side from the right-hand side and check against zero */
    fe448_sub(y, rhs, lhs);
    fe448_strong_reduce(y, y);

    return fe448_equal(fe448_zero, y);
}
#endif /* FE3C_ENABLE_SANITY_CHECKS */

static inline void ed448_identity(point_ed448 * p) {

    fe448_copy(p->X, fe448_zero);
    fe448_copy(p->Y, fe448_one);
    fe448_copy(p->Z, fe448_one);
}

static int ed448_points_equal(const point * pgen, const point * qgen) {

    const point_ed448 * p = (const point_ed448 *) pgen;
    const point_ed448 * q = (const point_ed448 *) qgen;

    fe448 lhs;
    fe448 rhs;

    /* Check X1 Z2 = X2 Z1 */
    fe448_mul(lhs, p->X, q->Z);
    fe448_mul(rhs, q->X, p->Z);
    fe448_strong_reduce(lhs, lhs);
    fe448_strong_reduce(rhs, rhs);
    int equal = fe448_equal(lhs, rhs);

    /* Check Y1 Z2 = Y2 Z1 */
    fe448_mul(lhs, p->Y, q->Z);
    fe448_mul(rhs, q->Y, p->Z);
    fe448_strong_reduce(lhs, lhs);
    fe448_strong_reduce(rhs, rhs);
    equal &= fe448_equal(lhs, rhs);

    return equal;
}

static void ed448_encode(u8 * buf, const point * pgen) {

    const point_ed448 * p = (const point_ed448 *) pgen;

    fe448 x;
    fe448 y;
    fe448 z;

    fe448_invert(z, p->Z);
    fe448_mul(x, p->X, z);
    fe448_mul(y, p->Y, z);

    /* Encode the y-coordinate */
    fe448_encode(buf, y);
    /* Encode the "sign" of the x coordinate (parity) in the most
     * significant bit of the last byte */
    FE3C_SANITY_CHECK(buf[56] == 0, "buf[56] = 0x%x", buf[56]);
    buf[56] |= fe448_lsb(x) << 7;

    /* Zeroize intermediate results to not leak any secrets via projective coordinates
     * (particularities of the representative of an equivalence class) */
    purge_secrets(z, sizeof(z));
}

static void ed448_point_double(point_ed448 * r, const point_ed448 * p) {

    FE3C_SANITY_CHECK(ed448_is_on_curve(p), ED448_STR, ED448_TO_STR(p));

    /* For optimal stack and cache usage we reduce the number of variables
     * allocated relative to the algorithm description in RFC 8032. For
     * clarity, the comments include the names of variables as they appear
     * in RFC 8032. */
    fe448 C, J;

    /* C := X1^2 */
    fe448_square(C, p->X);
    /* B := (X1+Y1)^2 */
    fe448_add(r->X, p->X, p->Y);
    fe448_square(r->X, r->X);
    /* D := Y1^2 */
    fe448_square(r->Y, p->Y);
    /* H := Z1^2 */
    fe448_square(J, p->Z);
    /* E := C+D */
    fe448_add(r->Z, C, r->Y);
    fe448_weak_reduce(r->Z, r->Z);
    /* J := E-2*H */
    fe448_add(J, J, J);
    fe448_weak_reduce(J, J);
    fe448_sub(J, r->Z, J);

    /* X3 := (B-E)*J */
    fe448_sub(r->X, r->X, r->Z);
    fe448_mul(r->X, r->X, J);

    /* Y3 := E*(C-D) */
    fe448_sub(r->Y, C, r->Y);
    fe448_mul(r->Y, r->Z, r->Y);

    /* Z3 := E*J */
    fe448_mul(r->Z, r->Z, J);
}

static inline int ed448_is_ok_order(const point_ed448 * p) {

    point_ed448 q;
    point_ed448 e;
    ed448_identity(&e);

    /* Double the point twice to map all small-order points to the identity */
    ed448_point_double(&q, p);
    ed448_point_double(&q, &q);

    return 1 - ed448_points_equal((const point *) &q, (const point *) &e);
}

static int ed448_decode(point * pgen, const u8 * buf) {

    point_ed448 * p = (point_ed448 *) pgen;

    int success = 1;
    /* Recover the "sign" or "parity" of the x-coordinate */
    int parity = buf[56] >> 7;
    success &= fe448_decode(p->Y, buf);

    /* We now need to recover the x-coordinate. Note that the curve equation
     * y^2 + x^2 = 1 + d x^2 y^2 implies that x^2 = u / v, where u = y^2 - 1
     * and v = d y^2 - 1. The denominator is always non-zero mod p, where
     * p = 2^448 - 2^224 - 1. To compute the square root we first compute the
     * candidate square root x = (u/v)^{(p+1)/4}. Let a = (u/v), b = a^{(p+1)/4}
     * and note that a^{-1} b^2 = a^{-1} a^{(p+1)/2} = a^{(p-1)/2} which is the
     * Legendre symbol of a = (u/v). For a square root to exist the Legendre
     * symbol must be equal to 1. If this is the case, then we have shown that
     * a^{-1} b^2 = 1 and so b is the square root of a. Otherwise no square root
     * of a exists in the field and decoding fails. */

    /* To compute (u/v)^{(p+1)/4} without inverting v we use the following
     * identity: (u/v)^{(p+1)/4} = u (u v)^{(p-3)/4}. Note that p = 3 (mod 4)
     * and so all the exponentiations are well defined. */

    fe448 u;
    fe448 v;
    /* Set u := y^2 */
    fe448_square(u, p->Y);

    /* Copy y^2 to v */
    fe448_copy(v, u);
    /* Subtract one to obtain u = y^2 - 1 */
    fe448_sub(u, u, fe448_one);
    fe448_strong_reduce(u, u);
    /* Set v := d y^2 */
    fe448_mul(v, v, ed448_d);
    /* Set v := d y^2 - 1 */
    fe448_sub(v, v, fe448_one);

    /* Set p->X to u v */
    fe448_mul(p->X, u, v);
    /* Raise p->X to (p-3)/4 */
    fe448_exp_p_minus_3_over_4(p->X, p->X);
    /* Set p->X to u (u v)^{(p-3)/4} */
    fe448_mul(p->X, u, p->X);

    /* So as to not allocate any more variables on the stack reuse p->Z
     * for temporary storage of intermediate results */
    fe448_square(p->Z, p->X);
    fe448_mul(p->Z, p->Z, v);

    /* Check that v * x^2 = u */
    success &= fe448_equal(p->Z, u);

    /* We must still take into account the encoded sign of x. Set v equal to -x and conditionally
     * move it into p->X, if the parity of x is incorrect now */
    fe448_neg(v, p->X);
    fe448_conditional_move(p->X, v, parity ^ fe448_lsb(p->X));
    /* RFC 8032 dictates that if x0, i.e. parity, is one and x=0, decoding fails, but since
     * the only points with x=0 (i.e. (0, 1) and (0, -1)) are of low order and so we reject
     * them anyway, we skip this check. */

    /* If decoding failed, set the result to identity to ensure we have a valid point and not
     * rely on any other code properly handling invalid ones */
    fe448_conditional_move(p->X, fe448_zero, 1 - success);
    fe448_conditional_move(p->Y, fe448_one, 1 - success);

    /* Set the Z coordinate to one */
    fe448_copy(p->Z, fe448_one);

    /* Check that we have a valid point */
    success &= ed448_is_ok_order(p);
    return success;
}

static void ed448_points_add(point_ed448 * r, const point_ed448 * p, const point_ed448 * q) {

    FE3C_SANITY_CHECK(ed448_is_on_curve(p), ED448_STR, ED448_TO_STR(p));
    FE3C_SANITY_CHECK(ed448_is_on_curve(q), ED448_STR, ED448_TO_STR(q));

    /* For optimal stack and cache usage we reduce the number of variables
     * allocated relative to the algorithm description in RFC 8032. For
     * clarity, the comments include the names of variables as they appear
     * in RFC 8032. */
    fe448 A, B, C, D, G;

    /* A := Z1*Z2 */
    fe448_mul(A, p->Z, q->Z);
    /* B := A^2 */
    fe448_square(B, A);
    /* C := X1*X2 */
    fe448_mul(C, p->X, q->X);
    /* D := Y1*Y2 */
    fe448_mul(D, p->Y, q->Y);
    /* H := (X1+Y1)*(X2+Y2) */
    fe448_add(r->Z, p->X, p->Y);
    fe448_add(r->X, q->X, q->Y);
    fe448_mul(r->X, r->Z, r->X);

    /* E := d*C*D */
    fe448_mul(r->Z, ed448_d, C);
    fe448_mul(r->Z, r->Z, D);
    /* G := B+E */
    fe448_add(G, B, r->Z);
    /* F := B-E */
    fe448_sub(r->Z, B, r->Z);

    /* X3 := A*F*(H-C-D) */
    /* Note that we use Karatsuba's trick to obtain
     * X1*Y1+X2*Y2 as H-C-D. */
    fe448_sub(r->X, r->X, C);
    fe448_sub(r->X, r->X, D);
    fe448_mul(r->X, r->X, A);
    fe448_mul(r->X, r->X, r->Z);

    /* Y3 := A*G*(D-C) */
    fe448_sub(r->Y, D, C);
    fe448_mul(r->Y, r->Y, A);
    fe448_mul(r->Y, r->Y, G);

    /* Z3 := F*G */
    fe448_mul(r->Z, r->Z, G);
}

#if !FE3C_COMB_METHOD
static inline void ed448_scalar_multiply(point_ed448 * r, const point_ed448 * p, const u8 * s) {

    FE3C_SANITY_CHECK(ed448_is_on_curve(p), ED448_STR, ED448_TO_STR(p));

    point_ed448 R[2];
    ed448_identity(&R[0]);
    R[1] = *p;

    /* Do a simple Montgomery ladder. Note that the input scalar must have been pruned
     * or reduced modulo the group order, so we can safely skip the top 8 bits of the
     * buffer and start at bit 447. */
    for (int i = 447; i >= 0; i--) {

        /* Recover the ith bit of the scalar */
        int bit = array_bit(s, i);
        ed448_points_add(&R[1 - bit], &R[1 - bit], &R[bit]);
        ed448_point_double(&R[bit], &R[bit]);
    }

    *r = R[0];

    purge_secrets(&R[1], sizeof(R[1]));
}
#endif /* !FE3C_COMB_METHOD */

static inline void ed448_map_scalar_to_isogenous_curve(u8 r[57], const u8 s[57]) {

    /* Divide the scalar by four, i.e. the order of the isogeny, since applying the
     * isogeny and its dual is equivalent to multiplying by four (the order) */

    const u8 group_order[] = {
        0xf3, 0x44, 0x58, 0xab, 0x92, 0xc2, 0x78, 0x23,
        0x55, 0x8f, 0xc5, 0x8d, 0x72, 0xc2, 0x6c, 0x21,
        0x90, 0x36, 0xd6, 0xae, 0x49, 0xdb, 0x4e, 0xc4,
        0xe9, 0x23, 0xca, 0x7c, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f,
        0x00
    };

    /* Let L denote the group order. Since L = 3 (mod 4) we have
     * that if s = k (mod 4) then s + k*L = 0 (mod 4). Start by
     * adding { 0, 1, 2, 3 } times the group order to the scalar
     * to make it divisible by four */
    u8 smod4 = s[0] & 0x3;
    u16 chain = 0;
    for (int i = 0; i < 57; i++) {

        chain += s[i];
        chain += (u16) group_order[i] * smod4;
        r[i] = (u8) chain;
        chain >>= 8;
    }

    for (int i = 0; i < 56; i++) {

        r[i] = (r[i + 1] << 6) | (r[i] >> 2);
    }
    r[56] = (u8)(chain << 6) | (r[56] >> 2);
}

static void ed448_multiply_basepoint(point * rgen, const u8 * s) {

    point_ed448 * r = (point_ed448 *) rgen;

#if !FE3C_COMB_METHOD
    ed448_scalar_multiply(r, &ed448_basepoint, s);
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

    /* Use w = 4 (width-4 NAF) and v = 56. For scalars of length 448 (actually less than that, but
     * 448 is nicely divisible by four) we get a = 112 and b = 2. Note that we must allocate one
     * extra byte for possible overflow (NAF representation may require an additional digit) */
    i8 naf[113];
    ed448_comb_recode_scalar_into_4naf(naf, s);

    ed448_identity(r);

    ed448_precomp p;
    /* Since we have b=2 we only have two iterations of the outermost loop of the algorithm
     * (Algorithm 3 in "Improved Fixed-base Comb Method for Fast Scalar Multiplication") which
     * implements the double summation at the end of the above description. Also for the second
     * iteration we have t=0 so we can skip the 2^{tw} = 2^0. Note that iterating from j=0 to v-1
     * over G[j][S[jb+t]] for t=1 corresponds to only accessing the elements of G (precomp table)
     * indexed by odd positions of the recoding above (jb+t = 1, 3, 5, ...), since b=2. Similarly,
     * when t=0 (second iteration) we shall only access the elements of G indexed by even-index
     * entries of the recoding. */
    for (int i = 1; i < 113; i += 2) {

        /* We let the loop index run twice as fast and skip every other entry of naf,
         * but correct for it in the j index (j = i / 2) */
        ed448_comb_read_precomp(&p, i >> 1, naf[i]);
        ed448_comb_add_precomp(r, r, &p);
    }

    /* Compute 2^{tw} Q = 2^4 Q */
    ed448_point_double(r, r);
    ed448_point_double(r, r);
    ed448_point_double(r, r);
    ed448_point_double(r, r);

    /* Do the second iteration of the outermost loop, i.e. iterate over even indices of the recoding
     * (see explanation above). */
    for (int i = 0; i < 113; i += 2) {

        /* We let the loop index run twice as fast and skip every other entry of naf,
         * but correct for it in the j index (j = i / 2) */
        ed448_comb_read_precomp(&p, i >> 1, naf[i]);
        ed448_comb_add_precomp(r, r, &p);
    }

    /* At this point Q := 2^{tw} Q is a no-op since 2^{tw} Q = 2^0 Q */

    /* Clear the recoding of the secret scalar from the stack */
    purge_secrets(naf, sizeof(naf));
    /* Clear the last accessed precomputed point */
    purge_secrets(&p, sizeof(p));
#endif /* !FE3C_COMB_METHOD */
}

static void ed448_point_negate(point * pgen) {

    point_ed448 * p = (point_ed448 *) pgen;
    fe448_neg(p->X, p->X);
}

static inline void ed448_conditional_move(point_ed448 * r, const point_ed448 * p, int move) {

    fe448_conditional_move(r->X, p->X, move);
    fe448_conditional_move(r->Y, p->Y, move);
    fe448_conditional_move(r->Z, p->Z, move);
}

static void ed448_double_scalar_multiply(point * rgen, const u8 * s, const u8 * h, const point * pgen) {

    /* Note that since the points here are on the isogenous twisted Edwards curve, the result is not
     * really multiplication [s]B + [h]A, but (if we were to apply the dual isogeny and go back to
     * the original Ed448 curve) [4][s]B + [4][h]A */

    point_ed448 * r = (point_ed448 *) rgen;
    const point_ed448 * p = (const point_ed448 *) pgen;

    /* To reduce stack usage, only store pointers to the public key A
     * and the basepoint B, and allocate only one point (A+B) */
    point_ed448 sum;
    ed448_points_add(&sum, p, &ed448_basepoint);
    const point_ed448 * G[3];
    G[0] = &ed448_basepoint;  /* [0]A + [1]B */
    G[1] = p;                 /* [1]A + [0]B */
    G[2] = &sum;              /* [1]A + [1]B */

    ed448_identity(r);

    /* Note that canonical scalars have at most 446 bits, which
     * corresponds to the loop counter initialization */
    for (int i = 445; i >= 0; i--) {

        ed448_point_double(r, r);

        u8 index = (array_bit(h, i) << 1) | array_bit(s, i);

        /* Access all elements of the array to prevent cache-based timing
         * attacks. Note that for most applications constant-time signature
         * verification is not needed. It is for the few that care that we
         * do this. */
        point_ed448 t;
        ed448_identity(&t);
        ed448_conditional_move(&t, G[0], byte_equal(index, 1));
        ed448_conditional_move(&t, G[1], byte_equal(index, 2));
        ed448_conditional_move(&t, G[2], byte_equal(index, 3));

        ed448_points_add(r, r, &t);
    }
}

const group_ops ed448_group_ops = {
    .points_equal = ed448_points_equal,
    .encode = ed448_encode,
    .decode = ed448_decode,
    .multiply_basepoint = ed448_multiply_basepoint,
    .point_negate = ed448_point_negate,
    .double_scalar_multiply = ed448_double_scalar_multiply
};
