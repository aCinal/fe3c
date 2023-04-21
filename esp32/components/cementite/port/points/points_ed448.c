#if FE3C_ED448_ISOGENY
    #error "Build system inconsistency detected! port/points/points_ed448.c in use despite FE3C_ED448_ISOGENY being set"
#endif /* FE3C_ED448_ISOGENY */

#if !FE3C_COMB_METHOD
    #error "Build system inconsistency detected! port/points/points_ed448.c in use despite FE3C_COMB_METHOD not being set"
#endif /* !FE3C_COMB_METHOD */

#include <points/points.h>
#include <field_elements/field_elements_ed448.h>
#include <utils/utils.h>
#include <points/comb/comb_ed448.h>
#include <port/points/comb/comb_parallel.h>
#include <port/points/comb/comb_parallel_ed448.h>

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
    fe448 A, C, G;

    /* A := Z1*Z2 */
    fe448_mul(A, p->Z, q->Z);
    /* C := X1*X2 */
    fe448_mul(C, p->X, q->X);
    /* H := (X1+Y1)*(X2+Y2) */
    fe448_add(r->Z, p->X, p->Y);
    fe448_add(r->X, q->X, q->Y);
    fe448_mul(r->X, r->Z, r->X);
    /* D := Y1*Y2 */
    fe448_mul(r->Y, p->Y, q->Y);

    /* We need to compute:
     *
     *           E := d*C*D,   G := B+E,   F := B-E
     *
     * where B=A^2, and then finally:
     *
     *   X3 := A*F*(H-C-D), Y3 := A*G*(D-C), and Z3 := F*G
     *
     * Start by computing A*(H-C-D), A*(D-C) and E=d*C*D.
     */

    /* Recall that r->X stores H and r->Y stores D,
     * and so we compute A*(H-C-D) here */
    fe448_sub(r->X, r->X, C);
    fe448_sub(r->X, r->X, r->Y);
    fe448_mul(r->X, r->X, A);

    /* Compute D-C */
    fe448_sub(G, r->Y, C);
    /* E := d*C*D */
    fe448_mul(C, ed448_d, C);
    fe448_mul(C, C, r->Y);
    /* Set r->Y to A*(D-C) */
    fe448_mul(r->Y, A, G);

    /* B := A^2 */
    fe448_square(A, A);
    /* F := B-E */
    fe448_sub(r->Z, A, C);
    /* G := B+E */
    fe448_add(C, A, C);

    /* Do the missing multiplications of X3 and Y3 by F and G, respectively */
    fe448_mul(r->X, r->X, r->Z);
    fe448_mul(r->Y, r->Y, C);
    /* Compute Z3 as F*G */
    fe448_mul(r->Z, r->Z, C);
}

static void ed448_multiply_basepoint(point * rgen, const u8 * s) {

    point_ed448 * r = (point_ed448 *) rgen;

    /* See src/points/points_ed448.c for a description of the comb method algorithm used here.
     * The core of the algorithm uses two loops iterating respectively over the odd and the even
     * indices of the signed digit recoding of the scalar. Try offloading one of the loops to a
     * separate thread to exploit parallelism. If not possible, fall back to a serial algorithm. */
    i8 recoding[113];
    ed448_comb_recode_scalar_into_width4_sd(recoding, s);

    point_ed448 even_part;
    comb_thread_work thread_work = {
        .thread_result = (point *) &even_part,
        .scalar_recoding = recoding,
        .curve_id = EDDSA_CURVE_ED448
    };

    int parallel = comb_dispatch_loop_to_thread(&thread_work);
    if (!parallel) {

        /* Failed to offload the even indices loop to a worker thread,
         * fine... we'll do it ourselves */
        ed448_comb_loop(&even_part, recoding, 0);
    }

    /* Do the odd indices loop */
    ed448_comb_loop(r, recoding, 1);

    ed448_point_double(r, r);
    ed448_point_double(r, r);
    ed448_point_double(r, r);
    ed448_point_double(r, r);

    if (parallel) {

        /* Wait for the worker to finish */
        comb_join_worker_thread();
    }

    /* Add the odd and even results */
    ed448_points_add(r, r, &even_part);

    /* Clear the recoding of the secret scalar from the stack */
    purge_secrets(recoding, sizeof(recoding));
    /* Clear the partial result from the stack */
    purge_secrets(&even_part, sizeof(even_part));
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
