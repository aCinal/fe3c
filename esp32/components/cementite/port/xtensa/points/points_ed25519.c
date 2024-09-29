#if !FE3C_COMB_METHOD
    #error "Build system inconsistency detected! points_ed25519.c in use despite FE3C_COMB_METHOD not being set"
#endif /* !FE3C_COMB_METHOD */

#include <points/points.h>
#include <field_elements/field_elements_ed25519.h>
#include <utils/utils.h>
#include <points/comb/comb_ed25519.h>
#include "comb/comb_parallel.h"
#include "comb/comb_parallel_ed25519.h"

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

#if FE3C_ENABLE_SANITY_CHECKS
static inline int ed25519_is_on_curve(const point_ed25519 * p) {

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

static inline int ed25519_valid_extended_projective(const point_ed25519 * p) {

    /* Check the consistency of the extended projective coordinate */
    fe448 xy, tz;
    fe25519_mul(xy, p->X, p->Y);
    fe25519_mul(tz, p->T, p->Z);
    fe25519_strong_reduce(xy, xy);
    fe25519_strong_reduce(tz, tz);
    return fe25519_equal(tz, xy);
}
#endif /* FE3C_ENABLE_SANITY_CHECKS */

static inline void ed25519_identity(point_ed25519 * p) {

    fe25519_copy(p->X, fe25519_zero);
    fe25519_copy(p->Y, fe25519_one);
    fe25519_copy(p->Z, fe25519_one);
    fe25519_copy(p->T, fe25519_zero);
}

static void ed25519_point_negate(point * pgen) {

    point_ed25519 * p = (point_ed25519 *) pgen;
    fe25519_neg(p->X, p->X);
    fe25519_neg(p->T, p->T);
}

static void ed25519_points_add(point_ed25519 * r, const point_ed25519 * p, const point_ed25519 * q) {

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

static void ed25519_point_double(point_ed25519 * r, const point_ed25519 * p, int set_extended_coordinate) {

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
    if (set_extended_coordinate) {

        /* T3 := E*H */
        fe25519_mul(r->T, r->T, H);
    }
}

static int ed25519_points_equal_modulo_cofactor(const point * pgen, const point * qgen) {

    const point_ed25519 * p = (const point_ed25519 *) pgen;
    const point_ed25519 * q = (const point_ed25519 *) qgen;

    point_ed25519 pmq;

    /* Compute the difference p-q */
    pmq = *q;
    ed25519_point_negate((point *) &pmq);
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

static void ed25519_encode(u8 * buf, const point * pgen) {

    const point_ed25519 * p = (const point_ed25519 *) pgen;
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

static inline int ed25519_is_ok_order(const point_ed25519 * p) {

    point_ed25519 e;
    ed25519_identity(&e);
    /* Check if p lies in the "cofactor subgroup" */
    return 1 - ed25519_points_equal_modulo_cofactor((const point *) p, (const point *) &e);
}

static int ed25519_decode(point * pgen, const u8 * buf) {

    point_ed25519 * p = (point_ed25519 *) pgen;

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

static void ed25519_multiply_basepoint(point * rgen, const u8 * s) {

    point_ed25519 * r = (point_ed25519 *) rgen;

    /* See src/points/points_ed25519.c for a description of the comb method algorithm used here.
     * The core of the algorithm uses two loops iterating respectively over the odd and the even
     * indices of the signed digit recoding of the scalar. Try offloading one of the loops to a
     * separate thread to exploit parallelism. If not possible, fall back to a serial algorithm. */
    i8 recoding[64];
    ed25519_comb_recode_scalar_into_width4_sd(recoding, s);

    point_ed25519 even_part;
    comb_thread_work thread_work = {
        .thread_result = (point *) &even_part,
        .scalar_recoding = recoding,
        .curve_id = EDDSA_CURVE_ED25519
    };

    int parallel = comb_dispatch_loop_to_thread(&thread_work);
    if (!parallel) {

        /* Failed to offload the even indices loop to a worker thread,
         * fine... we'll do it ourselves */
        ed25519_comb_loop(&even_part, recoding, 0);
    }

    /* Do the odd indices loop */
    ed25519_comb_loop(r, recoding, 1);

    ed25519_point_double(r, r, 0);
    ed25519_point_double(r, r, 0);
    ed25519_point_double(r, r, 0);
    ed25519_point_double(r, r, 1);

    if (parallel) {

        /* Wait for the worker to finish */
        comb_join_worker_thread();
    }

    /* Add the odd and even results */
    ed25519_points_add(r, r, &even_part);

    /* Clear the recoding of the secret scalar from the stack */
    purge_secrets(recoding, sizeof(recoding));
    /* Clear the partial result from the stack */
    purge_secrets(&even_part, sizeof(even_part));
}

static inline void ed25519_conditional_move(point_ed25519 * r, const point_ed25519 * p, int move) {

    fe25519_conditional_move(r->X, p->X, move);
    fe25519_conditional_move(r->Y, p->Y, move);
    fe25519_conditional_move(r->Z, p->Z, move);
    fe25519_conditional_move(r->T, p->T, move);
}

static void ed25519_double_scalar_multiply(point * rgen, const u8 * s, const u8 * h, const point * pgen) {

    point_ed25519 * r = (point_ed25519 *) rgen;
    const point_ed25519 * p = (const point_ed25519 *) pgen;

    /* To reduce stack usage, only store pointers to the public key A
     * and the basepoint B, and allocate only one point (A+B) */
    point_ed25519 sum;
    ed25519_points_add(&sum, p, &ed25519_basepoint);
    const point_ed25519 * G[3];
    G[0] = &ed25519_basepoint;  /* [0]A + [1]B */
    G[1] = p;                   /* [1]A + [0]B */
    G[2] = &sum;                /* [1]A + [1]B */

    ed25519_identity(r);

    /* Note that canonical scalars have at most 253 bits, which
     * corresponds to the loop counter initialization */
    for (int i = 252; i >= 0; i--) {

        ed25519_point_double(r, r, 1);

        u8 index = (array_bit(h, i) << 1) | array_bit(s, i);

        /* Access all elements of the array to prevent cache-based timing
         * attacks. Note that for most applications constant-time signature
         * verification is not needed. It is for the few that care that we
         * do this. */
        point_ed25519 t;
        ed25519_identity(&t);
        ed25519_conditional_move(&t, G[0], byte_equal(index, 1));
        ed25519_conditional_move(&t, G[1], byte_equal(index, 2));
        ed25519_conditional_move(&t, G[2], byte_equal(index, 3));

        ed25519_points_add(r, r, &t);
    }
}

const group_ops ed25519_group_ops = {
    .points_equal_modulo_cofactor = ed25519_points_equal_modulo_cofactor,
    .encode = ed25519_encode,
    .decode = ed25519_decode,
    .multiply_basepoint = ed25519_multiply_basepoint,
    .point_negate = ed25519_point_negate,
    .double_scalar_multiply = ed25519_double_scalar_multiply
};
