#include <points/points.h>
#include <curves/curves.h>
#include <field_elements/field_elements.h>
#if FE3C_32BIT
#include <field_elements/field_elements_ed25519_32.h>
#else
#include <field_elements/field_elements_ed25519_64.h>
#endif

static point ed25519_identity = {
    .X = { .ed25519 = { 0 } },
    .Y = { .ed25519 = { 1 } },
    .Z = { .ed25519 = { 1 } },
    .T = { .ed25519 = { 0 } },
};

#if FE3C_ENABLE_SANITY_CHECKS
static inline int ed25519_is_on_curve(const point * p) {

    fe x, y, z;
    fe lhs;
    fe rhs;
    fe xy, tz;

    /* Set x ::= X^2, y ::= Y^2, Z ::= Z^2 */
    fe_square(&x, &p->X);
    fe_square(&y, &p->Y);
    fe_square(&z, &p->Z);

    /* Set lhs ::= (Y^2 - X^2) Z^2 (left-hand side of the homogenous curve equation) */
    fe_sub(&lhs, &y, &x);
    fe_mul(&lhs, &lhs, &z);

    /* Set rhs ::= d X^2 Y^2 + Z^4 (right-hand side of the homogenous curve equation) */
    fe_mul(&rhs, &x, &y);
    fe_mul(&rhs, &rhs, &ed25519_d);
    fe_square(&z, &z);
    fe_add(&rhs, &rhs, &z);

    /* Subtract the left-hand side from the right-hand side and check against zero */
    fe_sub(&y, &rhs, &lhs);
    fe_strong_reduce(&y, &y);

    /* The function is used only for sanity checking - we may as well check that T
     * is consistent */
    fe_mul(&xy, &p->X, &p->Y);
    fe_mul(&tz, &p->T, &p->Z);
    fe_strong_reduce(&xy, &xy);
    fe_strong_reduce(&tz, &tz);

    return fe_equal(&fe_zero, &y) & fe_equal(&tz, &xy);
}
#endif /* FE3C_ENABLE_SANITY_CHECKS */

static int ed25519_points_equal(point * p, point * q) {

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

static void ed25519_encode(u8 * buf, const point * p) {

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
    FE3C_SANITY_CHECK( (buf[31] >> 7) == 0);
    buf[31] |= fe_lsb(&x) << 7;
}

static void ed25519_point_double(point * r, const point * p) {

    /* TODO: Reuse some old variables to reduce stack usage */
    fe A, B, C, E, F, G, H;

    /* A ::= X1^2 */
    fe_square(&A, &p->X);
    /* B ::= Y1^2*/
    fe_square(&B, &p->Y);

    /* C ::= 2*Z1^2 */
    fe_square(&C, &p->Z);
    fe_double(&C, &C);

    /* H ::= A+B */
    fe_add(&H, &A, &B);

    /* E ::= H-(X1+Y1)^2 */
    fe_add(&E, &p->X, &p->Y);
    fe_square(&E, &E);
    fe_sub(&E, &H, &E);

    /* G ::= A-B */
    fe_sub(&G, &A, &B);

    /* F ::= C+G */
    fe_add(&F, &C, &G);

    /* X3 ::= E*F */
    fe_mul(&r->X, &E, &F);
    /* Y3 ::= G*H */
    fe_mul(&r->Y, &G, &H);
    /* T3 ::= E*H */
    fe_mul(&r->T, &E, &H);
    /* Z3 ::= F*G */
    fe_mul(&r->Z, &F, &G);
}

static inline int ed25519_is_ok_order(const point * p) {

    /* TODO: Check against predefined table of low-order points */
    point q;

    /* Double the point three times to obtain cofactor times the point */
    ed25519_point_double(&q, p);
    ed25519_point_double(&q, &q);
    ed25519_point_double(&q, &q);

    /* Check if equal to the identity */
    return 1 - ed25519_points_equal(&q, &ed25519_identity);
}

static int ed25519_decode(point * p, const u8 * buf) {

    int success = 1;
    /* Recover the "sign" or "parity" of the x-coordinate */
    int parity = buf[31] >> 7;
    fe_decode(&p->Y, buf);
    /* Fail the decoding procedure if the decoded point is not canonical */
    success &= fe_is_canonical(&p->Y);

    /* We now need to recover the x-coordinate. Note that the curve equation
     * -x^2 + y^2 = 1 + d x^2 y^2 implies that x^2 = u / v, where u = y^2 - 1
     * and v = d y^2 + 1. The denominator is always non-zero mod p = 2^255 - 19.
     * To compute the square root we first compute the candidate square root
     * x = (u/v)^{(p+3)/8}. Let a = (u/v), b = a^{(p+3)/8} and note that
     * a^{-1} b^2 = a a^{(p+3)/4} = a^{(p-1)/4} which is the square root of
     * a^{(p-1)/2} which is necessarily equal to -1, 1, or 0 (Legendre symbol).
     * If it is equal to 1, then b^2 is equal to a and so b is the square root.
     * If it is equal to -1, then -b^2 = a and so the square root of a is i b,
     * where i is the "positive" square root of -1 equal (congruent mod p) to
     * 2^{(p-1)/4}. Otherwise no square root exists and decoding fails. */

    /* To compute (u/v)^{(p+3)/8} without inverting v we use the following
     * identity: (u/v)^{(p+3)/8} = u (u v)^{(p-5)/8}. Note that p = 5 (mod 8)
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
    fe_mul(&v, &v, &ed25519_d);
    /* Set v ::= d y^2 + 1 */
    fe_add(&v, &v, &fe_one);

    /* So as to not allocate any more variables on the stack reuse p->T
     * for temporary storage of intermediate results */

    /* Set p->T to u v */
    fe_mul(&p->T, &u, &v);
    /* Raise p->T to (p-5)/8 */
    fe_exp_p_minus_5_over_8(&p->T, &p->T);

    /* Set p->X ::= u (u v)^{(p-5)/8} */
    fe_mul(&p->X, &u, &p->T);

    /* Now we must consider the three cases:
     * 1. If v x^2 = u (mod p), then x is the square root
     * 2. If v x^2 = -u (mod p), then i x is the square root where i^2 = -1 and i = 2^{(p-1)/4}
     * 3. Otherwise, no square root exists and decoding fails
     */

    /* Compute v x^2 in p->T */
    fe_square(&p->T, &p->X);
    fe_mul(&p->T, &p->T, &v);
    /* Normalize u and p->T before doing any comparisons */
    fe_strong_reduce(&u, &u);
    fe_strong_reduce(&p->T, &p->T);
    /* Check if we need to multiply by the square root of -1 */
    int multiply_by_i = 1 - fe_equal(&p->T, &u);
    /* Set p->T ::= i x */
    fe_mul(&p->T, &p->X, &fe_i);

    /* At this point we have p->X equal to x as calculated by (u/v)^{(p+3)/8}, and v equal to i x.
     * If multiply_by_i is one, then we have excluded case 1. (v x^2 is definitely not equal to u).
     * In such a case we want to set p->X ::= i x: */
    fe_conditional_move(&p->X, &p->T, multiply_by_i);

    /* At this point p->X p->X v must be equal to u if we have a valid decoding (since p->X is either
     * x or i x by now). */
    fe_mul(&v, &v, &p->X);
    fe_mul(&v, &v, &p->X);
    fe_strong_reduce(&v, &v);
    /* Note that u has already been normalized */
    success &= fe_equal(&v, &u);

    /* We must still take into account the encoded sign of x. Set v equal to -x and conditionally
     * move it into p->X, if the parity of x is incorrect now */
    fe_neg(&v, &p->X);
    fe_conditional_move(&p->X, &v, parity ^ fe_lsb(&p->X));
    /* TODO: RFC 8032 dictates that if x0, i.e. parity, is one and x=0, decoding fails.
     * This is required to have unique codability/decodability of e.g. the identity element. */

    /* Set Z to one (normalized projective representation) */
    p->Z = fe_one;
    /* Check that we have a valid point, note that the extended coordinate T has not yet been set
     * but it is not needed for the following validation */
    /* NOTE: the above claim is only true if we use a specialized routine for doubling or check
     * the points against a predefined list in ed25519_is_ok_order*/
    success &= ed25519_is_ok_order(p);

    /* If decoding failed we must still set the point to something, set it to the identity (0, 1) */
    /* TODO: Check if we need to do this or can this be skipped (do we run a risk of setting Z=0
     * if we leave the contents of (X, Y) inconsistent, i.e. not corresponding to a point on the
     * curve - if not, we should be ok to leave p as is) */
    fe_conditional_move(&p->X, &fe_zero, 1 - success);
    fe_conditional_move(&p->Y, &fe_one, 1 - success);

    /* Set the extended coordinate T to the correct value */
    fe_mul(&p->T, &p->X, &p->Y);

    return success;
}

static void ed25519_points_add(point * r, const point * p, const point * q) {

    FE3C_SANITY_CHECK(ed25519_is_on_curve(p));
    FE3C_SANITY_CHECK(ed25519_is_on_curve(q));

    /* TODO: Reuse some old variables to reduce stack usage */
    fe A, B, C, D, E, F, G, H;

    /* A ::= (Y1-X1)*(Y2-X2) */
    fe_sub(&E, &p->Y, &p->X);
    fe_sub(&F, &q->Y, &q->X);
    fe_mul(&A, &E, &F);

    /* B ::= (Y1+X1)*(Y2+X2) */
    fe_add(&G, &p->Y, &p->X);
    fe_add(&H, &q->Y, &q->X);
    fe_mul(&B, &G, &H);

    /* C ::= T1*2*d*T2 */
    fe_mul(&C, &p->T, &q->T);
    fe_mul(&C, &C, &ed25519_d);
    fe_double(&C, &C);

    /* D ::= Z1*2*Z2 */
    fe_mul(&D, &p->Z, &q->Z);
    fe_double(&D, &D);

    /* E ::= B-A */
    fe_sub(&E, &B, &A);
    /* F ::= D-C */
    fe_sub(&F, &D, &C);
    /* G ::= D+C */
    fe_add(&G, &D, &C);
    /* H ::= B+A */
    fe_add(&H, &B, &A);

    /* X3 ::= E*F */
    fe_mul(&r->X, &E, &F);
    /* Y3 ::= G*H */
    fe_mul(&r->Y, &G, &H);
    /* T3 ::= E*H */
    fe_mul(&r->T, &E, &H);
    /* Z3 ::= F*G */
    fe_mul(&r->Z, &F, &G);
}

static void ed25519_scalar_multiply(point * r, const point * p, const u8 * s) {

    FE3C_SANITY_CHECK(ed25519_is_on_curve(p));

    point R[2] = {
        ed25519_identity,
        *p,
    };

    /* Do a simple Montgomery ladder for now */
    for (int i = 255; i >= 0; i--) {

        /* Recover the ith bit of the scalar */
        int bit = ( s[i >> 3] >> (i & 0x7) ) & 1;
        ed25519_points_add(&R[1 - bit], &R[1 - bit], &R[bit]);
        ed25519_point_double(&R[bit], &R[bit]);
    }

    *r = R[0];

    /* TODO: Study different implementations */
}

static void ed25519_multiply_basepoint(point * r, const u8 * s) {

    /* TODO: Use precomputation (comb method) */
#if FE3C_64BIT
    static const point basepoint = {
        .X = { .ed25519 = { 0x62d608f25d51a, 0x412a4b4f6592a, 0x75b7171a4b31d, 0x1ff60527118fe, 0x216936d3cd6e5 } },
        .Y = { .ed25519 = { 0x6666666666658, 0x4cccccccccccc, 0x1999999999999, 0x3333333333333, 0x6666666666666 } },
        .Z = fe_one,
        .T = { .ed25519 = { 0x68ab3a5b7dda3, 0xeea2a5eadbb, 0x2af8df483c27e, 0x332b375274732, 0x67875f0fd78b7 } }
    };
#else
    static const point basepoint = {
        .X = { .ed25519 = { 0x325d51a, 0x18b5823, 0xf6592a, 0x104a92d, 0x1a4b31d, 0x1d6dc5c, 0x27118fe, 0x7fd814, 0x13cd6e5, 0x85a4db } },
        .Y = { .ed25519 = { 0x2666658, 0x1999999, 0xcccccc, 0x1333333, 0x1999999, 0x666666, 0x3333333, 0xcccccc, 0x2666666, 0x1999999 } },
        .Z = fe_one,
        .T = { .ed25519 = { 0x1b7dda3, 0x1a2ace9, 0x25eadbb, 0x3ba8a, 0x83c27e, 0xabe37d, 0x1274732, 0xccacdd, 0xfd78b7, 0x19e1d7c } }
    };
#endif

    ed25519_scalar_multiply(r, &basepoint, s);
}

group_ops ed25519_group_ops = {
    .points_equal = ed25519_points_equal,
    .encode = ed25519_encode,
    .decode = ed25519_decode,
    .points_add = ed25519_points_add,
    .scalar_multiply = ed25519_scalar_multiply,
    .multiply_basepoint = ed25519_multiply_basepoint
};
