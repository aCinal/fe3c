#include <points/points.h>
#include <scalars/scalars.h>
#include <field_elements/field_elements_ed448.h>
#if FE3C_OPTIMIZATION_COMB_METHOD
    #include <points/points_ed448_comb_method.h>
#endif /* FE3C_OPTIMIZATION_COMB_METHOD */

static const point ed448_identity = {
    .X = fe_zero,
    .Y = fe_one,
    .Z = fe_one
};

#if FE3C_ENABLE_SANITY_CHECKS
static inline int ed448_is_on_curve(const point * p) {

    fe x, y, z;
    fe lhs;
    fe rhs;

    /* Set x ::= X^2, y ::= Y^2, Z ::= Z^2 */
    fe_square(&x, &p->X);
    fe_square(&y, &p->Y);
    fe_square(&z, &p->Z);

    /* Set lhs ::= (X^2 + Y^2) Z^2 (left-hand side of the homogenous curve equation) */
    fe_add(&lhs, &x, &y);
    fe_mul(&lhs, &lhs, &z);

    /* Set rhs ::= d X^2 Y^2 + Z^4 (right-hand side of the homogenous curve equation) */
    fe_mul(&rhs, &x, &y);
    fe_mul(&rhs, &rhs, &ed448_d);
    fe_square(&z, &z);
    fe_add(&rhs, &rhs, &z);

    /* Subtract the left-hand side from the right-hand side and check against zero */
    fe_sub(&y, &rhs, &lhs);
    fe_strong_reduce(&y, &y);

    return fe_equal(&fe_zero, &y);
}
#endif /* FE3C_ENABLE_SANITY_CHECKS */

static int ed448_points_equal(const point * p, const point * q) {

    fe lhs;
    fe rhs;

    /* Check X1 Z2 = X2 Z1 */
    fe_mul(&lhs, &p->X, &q->Z);
    fe_mul(&rhs, &q->X, &p->Z);
    fe_strong_reduce(&lhs, &lhs);
    fe_strong_reduce(&rhs, &rhs);
    int equal = fe_equal(&lhs, &rhs);

    /* Check Y1 Z2 = Y2 Z1 */
    fe_mul(&lhs, &p->Y, &q->Z);
    fe_mul(&rhs, &q->Y, &p->Z);
    fe_strong_reduce(&lhs, &lhs);
    fe_strong_reduce(&rhs, &rhs);
    equal &= fe_equal(&lhs, &rhs);

    return equal;
}

static void ed448_encode(u8 * buf, const point * p) {

    fe x;
    fe y;
    fe z_inv;

    /* Affinize the point */
    fe_invert(&z_inv, &p->Z);
    fe_mul(&x, &p->X, &z_inv);
    fe_mul(&y, &p->Y, &z_inv);

    /* Encode the y-coordinate */
    fe_encode(buf, &y);
    /* Encode the "sign" of the x coordinate (parity) in the most
     * significant bit of the last byte */
    FE3C_SANITY_CHECK(buf[56] == 0);
    buf[56] |= fe_lsb(&x) << 7;
}

static void ed448_point_double(point * r, const point * p) {

    /* TODO: Reuse some old variables to reduce stack usage */
    fe B, C, D, E, H, J;

    /* B ::= (X1+Y1)^2 */
    fe_add(&B, &p->X, &p->Y);
    fe_square(&B, &B);
    /* C ::= X1^2 */
    fe_square(&C, &p->X);
    /* D ::= Y1^2 */
    fe_square(&D, &p->Y);
    /* E ::= C+D */
    fe_add(&E, &C, &D);
    /* H ::= Z1^2 */
    fe_square(&H, &p->Z);
    /* J ::= E-2*H */
    fe_double(&J, &H);
    fe_sub(&J, &E, &J);

    /* X3 ::= (B-E)*J */
    fe_sub(&r->X, &B, &E);
    fe_mul(&r->X, &r->X, &J);

    /* Y3 ::= E*(C-D) */
    fe_sub(&r->Y, &C, &D);
    fe_mul(&r->Y, &E, &r->Y);

    /* Z3 ::= E*J */
    fe_mul(&r->Z, &E, &J);
}

static inline int ed448_is_ok_order(const point * p) {

    /* TODO: Check against predefined table of low-order points */
    point q;

    /* Double the point twice to obtain cofactor (4) times the point */
    ed448_point_double(&q, p);
    ed448_point_double(&q, &q);

    /* Check if equal to the identity */
    return 1 - ed448_points_equal(&q, &ed448_identity);
}

static int ed448_decode(point * p, const u8 * buf) {

    int success = 1;
    /* Recover the "sign" or "parity" of the x-coordinate */
    int parity = buf[56] >> 7;
    success &= fe_decode(&p->Y, buf);

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

    fe u;
    fe v;
    /* Set u ::= y^2 */
    fe_square(&u, &p->Y);

    /* Copy y^2 to v */
    v = u;
    /* Subtract one to obtain u = y^2 - 1 */
    fe_sub(&u, &u, &fe_one);
    fe_strong_reduce(&u, &u);
    /* Set v ::= d y^2 */
    fe_mul(&v, &v, &ed448_d);
    /* Set v ::= d y^2 - 1 */
    fe_sub(&v, &v, &fe_one);

    /* Set p->X to u v */
    fe_mul(&p->X, &u, &v);
    /* Raise p->X to (p-3)/4 */
    fe_exp_p_minus_3_over_4(&p->X, &p->X);
    /* Set p->X to u (u v)^{(p-3)/4} */
    fe_mul(&p->X, &u, &p->X);

    /* So as to not allocate any more variables on the stack reuse p->Z
     * for temporary storage of intermediate results */
    fe_square(&p->Z, &p->X);
    fe_mul(&p->Z, &p->Z, &v);

    /* Check that v * x^2 = u */
    success &= fe_equal(&p->Z, &u);

    /* We must still take into account the encoded sign of x. Set v equal to -x and conditionally
     * move it into p->X, if the parity of x is incorrect now */
    fe_neg(&v, &p->X);
    fe_conditional_move(&p->X, &v, parity ^ fe_lsb(&p->X));
    /* TODO: RFC 8032 dictates that if x0, i.e. parity, is one and x=0, decoding fails.
     * This is required to have unique codability/decodability of e.g. the identity element. */

    /* Set Z to one (normalized projective representation) */
    p->Z = fe_one;
    /* Check that we have a valid point */
    success &= ed448_is_ok_order(p);

    return success;
}

static void ed448_points_add(point * r, const point * p, const point * q) {

    FE3C_SANITY_CHECK(ed448_is_on_curve(p));
    FE3C_SANITY_CHECK(ed448_is_on_curve(q));

    /* TODO: Reuse some old variables to reduce stack usage */
    fe A, B, C, D, E, F, G, H;

    /* A ::= Z1*Z2 */
    fe_mul(&A, &p->Z, &q->Z);
    /* B ::= A^2 */
    fe_square(&B, &A);
    /* C ::= X1*X2 */
    fe_mul(&C, &p->X, &q->X);
    /* D ::= Y1*Y2 */
    fe_mul(&D, &p->Y, &q->Y);
    /* E ::= d*C*D */
    fe_mul(&E, &ed448_d, &C);
    fe_mul(&E, &E, &D);
    /* F ::= B-E */
    fe_sub(&F, &B, &E);
    /* G ::= B+E */
    fe_add(&G, &B, &E);

    /* H ::= (X1+Y1)*(X2+Y2) */
    /* At this point we can safely use r->Z for temporary storage
     * even if r is an alias for one of the inputs */
    fe_add(&r->Z, &p->X, &p->Y);
    fe_add(&H, &q->X, &q->Y);
    fe_mul(&H, &r->Z, &H);

    /* X3 ::= A*F*(H-C-D) */
    /* Note that we use Karatsuba's trick to obtain
     * X1*Y2+X2*Y2 as H-C-D. */
    fe_sub(&r->X, &H, &C);
    fe_sub(&r->X, &r->X, &D);
    fe_mul(&r->X, &r->X, &A);
    fe_mul(&r->X, &r->X, &F);

    /* Y3 ::= A*G*(D-C) */
    fe_sub(&r->Y, &D, &C);
    fe_mul(&r->Y, &r->Y, &A);
    fe_mul(&r->Y, &r->Y, &G);

    /* Z3 ::= F*G */
    fe_mul(&r->Z, &F, &G);
}

static void ed448_scalar_multiply(point * r, const point * p, const u8 * s) {

    FE3C_SANITY_CHECK(ed448_is_on_curve(p));

    point R[2] = {
        ed448_identity,
        *p,
    };

    /* Do a simple Montgomery ladder for now */
    for (int i = 455; i >= 0; i--) {

        /* Recover the ith bit of the scalar */
        int bit = ( s[i >> 3] >> (i & 0x7) ) & 1;
        ed448_points_add(&R[1 - bit], &R[1 - bit], &R[bit]);
        ed448_point_double(&R[bit], &R[bit]);
    }

    *r = R[0];
    /* TODO: Study different implementations */
}

static void ed448_multiply_basepoint(point * r, const u8 * s) {

#if !FE3C_OPTIMIZATION_COMB_METHOD
    #if FE3C_64BIT
    static const point basepoint = {
        .X = { .ed448 = { 0x26a82bc70cc05e, 0x80e18b00938e26, 0xf72ab66511433b, 0xa3d3a46412ae1a, 0xf1767ea6de324, 0x36da9e14657047, 0xed221d15a622bf, 0x4f1970c66bed0d } },
        .Y = { .ed448 = { 0x8795bf230fa14, 0x132c4ed7c8ad98, 0x1ce67c39c4fdbd, 0x5a0c2d73ad3ff, 0xa3984087789c1e, 0xc7624bea73736c, 0x248876203756c9, 0x693f46716eb6bc } },
        .Z = fe_one
    };
    #else
    static const point basepoint = {
        .X = { .ed448 = {
            0x70cc05e, 0x26a82bc, 0x938e26, 0x80e18b0, 0x511433b, 0xf72ab66, 0x412ae1a, 0xa3d3a46,
            0xa6de324, 0xf1767e, 0x4657047, 0x36da9e1, 0x5a622bf, 0xed221d1, 0x66bed0d, 0x4f1970c } },
        .Y = { .ed448 = {
            0x230fa14, 0x8795bf, 0x7c8ad98, 0x132c4ed, 0x9c4fdbd, 0x1ce67c3, 0x73ad3ff, 0x5a0c2d,
            0x7789c1e, 0xa398408, 0xa73736c, 0xc7624be, 0x3756c9, 0x2488762, 0x16eb6bc, 0x693f467 } },
        .Z = fe_one
    };
    #endif /* FE3C_64BIT */
    ed448_scalar_multiply(r, &basepoint, s);
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
     * 448 is nicely divisible by four) we get a = 112 and b = 2. */
    i8 naf[112];
    i8 carry;

    carry = 0;
    for (int i = 0; i < 56; i++) {

        /* From each byte of the scalar extract the two 4-bit digits - note that the exponent array
         * is ordered down columns first (column index changes more slowly) which allows for simpler
         * recoding of the scalar than in Lim/Lee method (where each entry of the recoding table has
         * to be a sum over bits distant by a). */
        naf[2 * i + 0] = ( (s[i] >> 0) & 0xF ) + carry;
        /* Check if naf[2 * i + 0] is larger than 2^{w-1} = 8 and if so make naf[2 * i + 0] negative
         * (subtract 2^w = 16) and propagate the carry to the next digit */
        carry = naf[2 * i + 0] + 0x8;
        /* Note that carry can only be 0 or 1 at this point */
        carry >>= 4;
        naf[2 * i + 0] -= carry << 4;

        naf[2 * i + 1] = ( (s[i] >> 4) & 0xF ) + carry;
        carry = naf[2 * i + 1] + 0x8;
        carry >>= 4;
        naf[2 * i + 1] -= carry << 4;
    }
    FE3C_SANITY_CHECK(carry == 0);

    *r = ed448_identity;
    point p;
    /* Since we have b=2 we only have two iterations of the outermost loop of the algorithm
     * (Algorithm 3 in "Improved Fixed-base Comb Method for Fast Scalar Multiplication") which
     * implements the double summation at the end of the above description. Also for the second
     * iteration we have t=0 so we can skip the 2^{tw} = 2^0. Note that iterating from j=0 to v-1
     * over G[j][S[jb+t]] for t=1 corresponds to only accessing the elements of G (precomp table)
     * indexed by odd positions of the recoding above (jb+t = 1, 3, 5, ...), since b=2. Similarly,
     * when t=0 (second iteration) we shall only access the elements of G indexed by even-index
     * entries of the recoding. */
    for (int i = 1; i < 112; i += 2) {

        /* We let the loop index run twice as fast and skip every other entry of naf,
         * but correct for it in the j index (j = i / 2) */
        ed448_comb_read_precomp(&p, i >> 1, naf[i]);
        ed448_points_add(r, r, &p);
    }

    /* Compute 2^{tw} Q = 2^4 Q */
    ed448_point_double(r, r);
    ed448_point_double(r, r);
    ed448_point_double(r, r);
    ed448_point_double(r, r);

    /* Do the second iteration of the outermost loop, i.e. iterate over even indices of the recoding
     * (see explanation above). */
    for (int i = 0; i < 112; i += 2) {

        /* We let the loop index run twice as fast and skip every other entry of naf,
         * but correct for it in the j index (j = i / 2) */
        ed448_comb_read_precomp(&p, i >> 1, naf[i]);
        ed448_points_add(r, r, &p);
    }

    /* At this point Q := 2^{tw} Q is a no-op since 2^{tw} Q = 2^0 Q */

    /* Clear the recoding of the secret scalar from the stack */
    purge_secrets(naf, sizeof(naf));
#endif /* !FE3C_OPTIMIZATION_COMB_METHOD */
}

group_ops ed448_group_ops = {
    .points_equal = ed448_points_equal,
    .encode = ed448_encode,
    .decode = ed448_decode,
    .points_add = ed448_points_add,
    .scalar_multiply = ed448_scalar_multiply,
    .multiply_basepoint = ed448_multiply_basepoint
};
