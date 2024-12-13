#if !FE3C_PARALLEL_GROUP_OPS
    #error "Build system inconsistency detected! points_ed448.c in use despite FE3C_PARALLEL_GROUP_OPS not being set"
#endif /* !FE3C_PARALLEL_GROUP_OPS */

#include "parallel.h"

#include <points/points.h>
#include <field_elements/field_elements_ed448.h>
#include <utils/utils.h>
#if FE3C_ED448_COMB_METHOD
    #include <points/comb/comb_ed448.h>
#endif /* FE3C_ED448_COMB_METHOD */
#if FE3C_ED448_LATTICE_BASIS_REDUCTION
    #include <points/lattice_basis_reduction/lattices_ed448.h>
#endif /* FE3C_ED448_LATTICE_BASIS_REDUCTION */

#define ED448_STR \
    "    X = " FE448_STR "\n" \
    "    Y = " FE448_STR "\n" \
    "    Z = " FE448_STR "\n" \
    "    T = " FE448_STR
#define ED448_TO_STR(p) \
    FE448_TO_STR((p)->X), FE448_TO_STR((p)->Y), FE448_TO_STR((p)->Z), FE448_TO_STR((p)->T)

#if FE3C_ED448_COMB_METHOD
struct ed448_comb_work {
    point_ed448 *thread_result;
    const i8 *scalar_recoding;
};
#else
struct ed448_ladder_work {
    point_ed448 *thread_result;
    const u8 *scalar;
};
#endif /* FE3C_ED448_COMB_METHOD */

#if FE3C_ED448_LATTICE_BASIS_REDUCTION
struct ed448_lattices_work {
    point_ed448 *thread_result;
    const u8 * delta_response;
    const point_ed448 **lookup;
};
#endif /* FE3C_ED448_LATTICE_BASIS_REDUCTION */

/* Note that the below basepoint is the basepoint on the 4-isogenous curve: y^2 - x^2 = 1 + (d-1)x^2y^2 */
static const point_ed448 ed448_basepoint = {
    .X = ED448_BASEPOINT_X,
    .Y = ED448_BASEPOINT_Y,
    .Z = ED448_BASEPOINT_Z,
    .T = ED448_BASEPOINT_T
};
static const point_ed448 ed448_basepoint_times_2_225 = {
    .X = ED448_BASEPOINT_TIMES_2_225_X,
    .Y = ED448_BASEPOINT_TIMES_2_225_Y,
    .Z = ED448_BASEPOINT_TIMES_2_225_Z,
    .T = ED448_BASEPOINT_TIMES_2_225_T
};
#if FE3C_ED448_LATTICE_BASIS_REDUCTION
static const point_ed448 ed448_basepoint_times_2_225_plus_1 = {
    .X = ED448_BASEPOINT_TIMES_2_225_PLUS_1_X,
    .Y = ED448_BASEPOINT_TIMES_2_225_PLUS_1_Y,
    .Z = ED448_BASEPOINT_TIMES_2_225_PLUS_1_Z,
    .T = ED448_BASEPOINT_TIMES_2_225_PLUS_1_T
};
#endif /* FE3C_ED448_LATTICE_BASIS_REDUCTION */

#if FE3C_ENABLE_SANITY_CHECKS
static inline int ed448_is_on_curve(const point_ed448 *p)
{
    fe448 x, y, z;
    fe448 lhs;
    fe448 rhs;

    /* Check if the point belongs to the 4-isogenous twisted Edwards curve
     * defined by the equation:
     *
     *                   y^2 - x^2 = 1 + (d-1)x^2y^2
     *
     * where d = -39081.
     */

    /* Set x := X^2, y := Y^2, Z := Z^2 */
    fe448_square(x, p->X);
    fe448_square(y, p->Y);
    fe448_square(z, p->Z);

    /* Set lhs := (Y^2 - X^2) Z^2 (left-hand side of the homogenous curve equation) */
    fe448_sub(lhs, y, x);
    fe448_mul(lhs, lhs, z);

    /* Set rhs := d X^2 Y^2 + Z^4 (right-hand side of the homogenous curve equation) */
    fe448_mul(rhs, x, y);
    fe448_mul(rhs, rhs, ed448twist_d);
    fe448_square(z, z);
    fe448_add(rhs, rhs, z);

    /* Subtract the left-hand side from the right-hand side and check against zero */
    fe448_sub(y, rhs, lhs);
    fe448_strong_reduce(y, y);

    return fe448_equal(fe448_zero, y);
}

static inline int ed448_valid_extended_projective(const point_ed448 *p)
{
    /* Check the consistency of the extended projective coordinate */
    fe448 xy, tz;
    fe448_mul(xy, p->X, p->Y);
    fe448_mul(tz, p->T, p->Z);
    fe448_strong_reduce(xy, xy);
    fe448_strong_reduce(tz, tz);
    return fe448_equal(tz, xy);
}
#endif /* FE3C_ENABLE_SANITY_CHECKS */

static inline void ed448_identity(point_ed448 *p)
{
    fe448_copy(p->X, fe448_zero);
    fe448_copy(p->Y, fe448_one);
    fe448_copy(p->Z, fe448_one);
    fe448_copy(p->T, fe448_zero);
}

static void ed448_point_negate(point_ed448 *p)
{
    fe448_neg(p->X, p->X);
    fe448_neg(p->T, p->T);
}

static void ed448_points_add(point_ed448 *r, const point_ed448 *p, const point_ed448 *q)
{
    FE3C_SANITY_CHECK(ed448_is_on_curve(p), ED448_STR, ED448_TO_STR(p));
    FE3C_SANITY_CHECK(ed448_is_on_curve(q), ED448_STR, ED448_TO_STR(q));
    FE3C_SANITY_CHECK(ed448_valid_extended_projective(p), ED448_STR, ED448_TO_STR(p));
    FE3C_SANITY_CHECK(ed448_valid_extended_projective(q), ED448_STR, ED448_TO_STR(q));

    /* For optimal stack and cache usage we reduce the number of variables
     * allocated relative to the algorithm description in RFC 8032. For
     * clarity, the comments include the names of variables as they appear
     * in RFC 8032. */
    fe448 A, B, E;

    /* A := (Y1-X1)*(Y2-X2) */
    fe448_sub(A, p->Y, p->X);
    fe448_sub(B, q->Y, q->X);
    fe448_mul(A, A, B);

    /* B := (Y1+X1)*(Y2+X2) */
    fe448_add(E, p->Y, p->X);
    fe448_add(B, q->Y, q->X);
    fe448_mul(B, E, B);

    /* C := T1*2*d*T2 */
    fe448_mul(r->T, p->T, q->T);
    fe448_mul(r->T, r->T, ed448twist_d);
    fe448_add(r->T, r->T, r->T);
    fe448_weak_reduce(r->T, r->T);

    /* D := Z1*2*Z2 */
    fe448_mul(r->Z, p->Z, q->Z);
    fe448_add(r->Z, r->Z, r->Z);

    /* E := B-A */
    fe448_sub(E, B, A);
    /* F := D-C */
    fe448_sub(r->X, r->Z, r->T);
    /* G := D+C */
    fe448_add(r->Z, r->Z, r->T);
    /* H := B+A */
    fe448_add(r->T, B, A);

    /* Y3 := G*H */
    fe448_mul(r->Y, r->Z, r->T);
    /* T3 := E*H */
    fe448_mul(r->T, E, r->T);
    /* Z3 := F*G */
    fe448_mul(r->Z, r->X, r->Z);
    /* X3 := E*F */
    fe448_mul(r->X, E, r->X);
}


static void ed448_point_double(point_ed448 *r, const point_ed448 *p, int set_extended_coordinate)
{
    FE3C_SANITY_CHECK(ed448_is_on_curve(p), ED448_STR, ED448_TO_STR(p));

    /* For optimal stack and cache usage we reduce the number of variables
     * allocated relative to the algorithm description in RFC 8032. For
     * clarity, the comments include the names of variables as they appear
     * in RFC 8032. */
    fe448 G, H;

    /* A := X1^2 */
    fe448_square(G, p->X);
    /* B := Y1^2*/
    fe448_square(r->T, p->Y);

    /* H := A+B */
    fe448_add(H, G, r->T);
    /* G := A-B */
    fe448_sub(G, G, r->T);

    /* E := H-(X1+Y1)^2 */
    fe448_add(r->T, p->X, p->Y);
    fe448_square(r->T, r->T);
    fe448_sub(r->T, H, r->T);

    /* C := 2*Z1^2 */
    fe448_square(r->Z, p->Z);
    fe448_add(r->Z, r->Z, r->Z);
    /* F := C+G */
    fe448_add(r->Z, r->Z, G);

    /* X3 := E*F */
    fe448_mul(r->X, r->T, r->Z);
    /* Y3 := G*H */
    fe448_mul(r->Y, G, H);
    /* Z3 := F*G */
    fe448_mul(r->Z, r->Z, G);

    /* When scheduled to be followed by another doubling we can skip setting the extended coordinate T
     * which is not needed for doubling */
    if (set_extended_coordinate)
        fe448_mul(r->T, r->T, H);  /* T3 := E*H */
}

static int ed448_points_equal_modulo_cofactor(const point_ed448 *p, const point_ed448 *q)
{
    /* We have already (during decoding) cleared the cofactor via the isogeny, compare for equality immediately */
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

static void ed448_encode(u8 *buf, const point *pgen)
{
    const point_ed448 *p = (const point_ed448 *) pgen;

    fe448 x;
    fe448 y;
    fe448 z;

    /* Apply the dual isogeny to move the point back onto the untwisted curve:
     *
     *         (x, y) -> ( 2xy / (y^2 + x^2) , (y^2 - x^2) / (2 - y^2 + x^2) )
     *
     * or in standard projective coordinates:
     *
     *   (X, Y, Z)  -> ( 2XY / (Y^2 + X^2) , (Y^2 - X^2) / (2Z^2 - Y^2 + X^2) , Z' )
     *
     * To avoid modular division set Z' := (X^2 + Y^2)(2Z^2 - Y^2 + X^2). Then:
     *
     *         X' = 2XY (2Z^2 - Y^2 - X^2) and Y' = (Y^2 - X^2)(X^2 + Y^2)
     */

    fe448 A, B, C, D;
    /* A := X^2 + Y^2 */
    fe448_square(x, p->X);
    fe448_square(y, p->Y);
    fe448_add(A, x, y);
    /* B := Y^2 - X^2 */
    fe448_sub(B, y, x);
    /* C := (X + Y)^2 */
    fe448_add(C, p->X, p->Y);
    fe448_square(C, C);
    /* D := C - A = 2XY */
    fe448_weak_reduce(A, A);
    fe448_sub(D, C, A);
    /* C := 2Z^2 (reuse C since it was only used for doing the Karatsuba's trick) */
    fe448_square(C, p->Z);
    fe448_add(C, C, C);
    fe448_weak_reduce(B, B);
    /* C := C - B = 2Z^2 - Y^2 + X^2 */
    fe448_sub(C, C, B);

    /* x := C*D = 2XY (2Z^2 - Y^2 + X^2) */
    fe448_mul(x, C, D);
    /* y := B*A = (Y^2 - X^2)(X^2 + Y^2) */
    fe448_mul(y, B, A);
    /* z := A*C = (X^2 + Y^2)(2Z^2 - Y^2 + X^2) */
    fe448_mul(z, A, C);

    /* Affinize the point */
    fe448_invert(z, z);
    fe448_mul(x, x, z);
    fe448_mul(y, y, z);

    /* Encode the y-coordinate */
    fe448_encode(buf, y);
    /* Encode the "sign" of the x coordinate (parity) in the most
     * significant bit of the last byte */
    FE3C_SANITY_CHECK(buf[56] == 0, "buf[56] = 0x%x", buf[56]);
    buf[56] |= fe448_lsb(x) << 7;

    /* Zeroize intermediate results to not leak any secrets via projective coordinates
     * (particularities of the representative of an equivalence class) */
    purge_secrets(A, sizeof(A));
    purge_secrets(B, sizeof(B));
    purge_secrets(C, sizeof(C));
    purge_secrets(D, sizeof(D));
    purge_secrets(z, sizeof(z));
}

static inline int ed448_is_ok_order(const point_ed448 *p)
{
    /* Note that all low-order points on the original untwisted curve are in the
     * kernel of the 4-isogeny we have used to move to a twisted curve. Therefore
     * to check for low-order input points we must only check the resulting point
     * on the isogenous curve against the identity. */

    fe448 aux;
    /* Check X = 0 */
    fe448_strong_reduce(aux, p->X);
    int equal = fe448_equal(fe448_zero, aux);

    /* Check Y = Z */
    fe448_sub(aux, p->Y, p->Z);
    fe448_strong_reduce(aux, aux);
    equal &= fe448_equal(fe448_zero, aux);

    return 1 - equal;
}

static int ed448_decode(point *pgen, const u8 *buf)
{
    point_ed448 *p = (point_ed448 *) pgen;

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

    /* Apply the 4-isogeny to move the point onto a twisted curve with the affine equation:
     *
     *                                y^2 - x^2 = 1 + (d-1)x^2y^2
     *
     * since on the twisted curve arithmetic is faster. Note that all low-order points are in the
     * kernel of this isogeny and so get mapped to the identity element. The map is defined as:
     *
     *         (x, y) -> ( 2xy / (y^2 - x^2) , (y^2 + x^2) / (2 - y^2 - x^2) )
     *
     * or in standard projective coordinates:
     *
     *   (X, Y, Z)  -> ( 2XY / (Y^2 - X^2) , (Y^2 + X^2) / (2Z^2 - Y^2 - X^2) , Z' )
     *
     * To avoid modular division set Z' := (Y^2 - X^2)(2Z^2 - Y^2 - X^2). Then:
     *
     *                     X' = 2XY (2Z^2 - Y^2 - X^2)
     *                     Y' = (Y^2 - X^2)(X^2 + Y^2)
     *                     T' = X'Y'/Z' = 2XY (X^2 + Y^2)
     */

    fe448 A, B, C, D;
    /* A := X^2 + Y^2 */
    fe448_square(C, p->X);
    fe448_square(D, p->Y);
    fe448_add(A, C, D);
    /* B := Y^2 - X^2 */
    fe448_sub(B, D, C);
    /* C := (X + Y)^2 */
    fe448_add(C, p->X, p->Y);
    fe448_square(C, C);

    fe448_weak_reduce(A, A);
    /* D := C - A = 2XY */
    fe448_sub(D, C, A);
    /* Note that we know that Z=1 at this point, which saves us a squaring */
    fe448_add(C, fe448_one, fe448_one);
    /* C := C - A = 2Z^2 - Y^2 - X^2 */
    fe448_sub(C, C, A);

    /* X := D*C = 2XY (2Z^2 - Y^2 - X^2) */
    fe448_mul(p->X, D, C);
    /* Y := B*A = (Y^2 - X^2)(X^2 + Y^2) */
    fe448_mul(p->Y, B, A);
    /* Z := B*C = (Y^2 - X^2)(2Z^2 - Y^2 - X^2) */
    fe448_mul(p->Z, B, C);
    /* T := D*A = 2XY (X^2 + Y^2) */
    fe448_mul(p->T, D, A);

    /* Check that we have a valid point */
    success &= ed448_is_ok_order(p);
    return success;
}

static inline void ed448_conditional_move(point_ed448 *r, const point_ed448 *p, int move)
{
    fe448_conditional_move(r->X, p->X, move);
    fe448_conditional_move(r->Y, p->Y, move);
    fe448_conditional_move(r->Z, p->Z, move);
    fe448_conditional_move(r->T, p->T, move);
}

#if !FE3C_ED448_COMB_METHOD
static inline void ed448_conditional_swap(point_ed448 *r, point_ed448 *q, point_ed448 *temp, int swap)
{
    /* We could implement this more efficiently, but if someone is using
     * naive scalar multiplication then they probably value space more
     * than speed */
    ed448_conditional_move(temp, r, swap);
    ed448_conditional_move(r, q, swap);
    ed448_conditional_move(q, temp, swap);
}

static void ed448_scalar_multiply_partial(point_ed448 *r, const point_ed448 *p, const u8 *s, int start_bit, int stop_bit)
{
    point_ed448 R[3];
    ed448_identity(&R[0]);
    R[1] = *p;
    /* R[2] is an auxiliary buffer for swaps */

    int bits[2] = { 0, 0 };
    for (int i = start_bit; i >= stop_bit; i--) {

        /* Recover the ith bit of the scalar */
        bits[0] = array_bit(s, i);
        ed448_conditional_swap(&R[0], &R[1], &R[2], bits[0] ^ bits[1]);
        ed448_points_add(&R[1], &R[1], &R[0]);
        ed448_point_double(&R[0], &R[0], 1);
        bits[1] = bits[0];
    }

    ed448_conditional_swap(&R[0], &R[1], &R[2], bits[1]);
    *r = R[0];
    purge_secrets(&R, sizeof(R));
    purge_secrets(bits, sizeof(bits));
}

static void ed448_ladder_parallel_callback(void *arg)
{
    struct ed448_ladder_work *work = arg;
    ed448_scalar_multiply_partial(work->thread_result, &ed448_basepoint_times_2_225, work->scalar, 447, 225);
}

static inline void ed448_multiply_basepoint_ladder(point_ed448 *r, const u8 *s)
{
    point_ed448 upper_half;
    struct ed448_ladder_work ladder_work = {
        .thread_result = &upper_half,
        .scalar = s
    };
    struct parallel_work work = {
        .func = ed448_ladder_parallel_callback,
        .arg = &ladder_work
    };
    int parallel = schedule_parallel_work(&work);
    if (!parallel) {
        /* Failed to offload the upper half to a worker thread,
         * do everything ourselves */
        ed448_scalar_multiply_partial(r, &ed448_basepoint, s, 447, 0);
    } else {
        /* Some of the work is being done in parallel, focus on the lower half */
        ed448_scalar_multiply_partial(r, &ed448_basepoint, s, 224, 0);
        /* Wait for the upper half */
        wait_for_parallel_work();
        /* Join the halves */
        ed448_points_add(r, r, &upper_half);
        purge_secrets(&upper_half, sizeof(upper_half));
    }
}
#else
static void ed448_comb_loop(point_ed448 *result, const i8 *scalar_recoding, int odd)
{
    FE3C_SANITY_CHECK(0 == odd || 1 == odd, NULL);

    ed448_identity(result);
    ed448_precomp p;
    for (int i = odd; i < 113; i += 2) {

        /* We let the loop index run twice as fast and skip every other entry of recoding,
         * but correct for it in the j index (j = i / 2) */
        ed448_comb_read_precomp(&p, i >> 1, scalar_recoding[i]);
        ed448_comb_add_precomp(result, result, &p);
    }

    /* Clear the last accessed precomputed point */
    purge_secrets(&p, sizeof(p));
}

static void ed448_comb_parallel_callback(void *arg)
{
    struct ed448_comb_work *work = arg;
    ed448_comb_loop(work->thread_result, work->scalar_recoding, 0);
}

static inline void ed448_multiply_basepoint_comb(point_ed448 *r, const u8 *s)
{
    /* See src/points/points_ed448.c for a description of the comb method algorithm used here.
     * The core of the algorithm uses two loops iterating respectively over the odd and the even
     * indices of the signed digit recoding of the scalar. Try offloading one of the loops to a
     * separate thread to exploit parallelism. If not possible, fall back to a serial algorithm. */
    i8 recoding[113];
    ed448_comb_recode_scalar_into_width4_sd(recoding, s);

    point_ed448 even_part;
    struct ed448_comb_work comb_work = {
        .thread_result = &even_part,
        .scalar_recoding = recoding
    };
    struct parallel_work work = {
        .func = ed448_comb_parallel_callback,
        .arg = &comb_work
    };
    int parallel = schedule_parallel_work(&work);
    if (!parallel) {

        /* Failed to offload the even indices loop to a worker thread,
         * fine... we'll do it ourselves */
        ed448_comb_loop(&even_part, recoding, 0);
    }

    /* Do the odd indices loop */
    ed448_comb_loop(r, recoding, 1);

    ed448_point_double(r, r, 0);
    ed448_point_double(r, r, 0);
    ed448_point_double(r, r, 0);
    ed448_point_double(r, r, 1);

    if (parallel)
        wait_for_parallel_work();

    /* Add the odd and even results */
    ed448_points_add(r, r, &even_part);

    /* Clear the recoding of the secret scalar from the stack */
    purge_secrets(recoding, sizeof(recoding));
    /* Clear the partial result from the stack */
    purge_secrets(&even_part, sizeof(even_part));
}
#endif /* !FE3C_ED448_COMB_METHOD */

static inline void ed448_map_scalar_to_isogenous_curve(u8 r[57], const u8 s[57])
{
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

    for (int i = 0; i < 56; i++)
        r[i] = (r[i + 1] << 6) | (r[i] >> 2);

    r[56] = (u8)(chain << 6) | (r[56] >> 2);
}

static void ed448_multiply_basepoint(point *rgen, const u8 *sraw)
{
    point_ed448 *r = (point_ed448 *) rgen;
    u8 s[57];
    ed448_map_scalar_to_isogenous_curve(s, sraw);
#if !FE3C_ED448_COMB_METHOD
    ed448_multiply_basepoint_ladder(r, s);
#else
    ed448_multiply_basepoint_comb(r, s);
#endif /* !FE3C_ED448_COMB_METHOD */
}

#if !FE3C_ED448_LATTICE_BASIS_REDUCTION
static inline void ed448_double_scalar_multiply_vartime(point_ed448 *r, const u8 *s, const u8 *h, const point_ed448 *p)
{
    /* Note that since the points here are on the isogenous twisted Edwards curve, the result is not
     * really multiplication [s]B + [h]A, but (if we were to apply the dual isogeny and go back to
     * the original Ed448 curve) [4][s]B + [4][h]A */

    /* To reduce stack usage, only store pointers to the public key A
     * and the basepoint B, and allocate only one point (A+B) */
    point_ed448 sum;
    ed448_points_add(&sum, p, &ed448_basepoint);
    const point_ed448 *lookup[3];
    lookup[0] = &ed448_basepoint;  /* [0]A + [1]B */
    lookup[1] = p;                 /* [1]A + [0]B */
    lookup[2] = &sum;              /* [1]A + [1]B */

    ed448_identity(r);

    /* Note that canonical scalars have at most 446 bits, which
     * corresponds to the loop counter initialization */
    for (int i = 445; i >= 0; i--) {

        ed448_point_double(r, r, 1);
        int index = (array_bit(h, i) << 1) | array_bit(s, i);
        if (index)
            ed448_points_add(r, r, lookup[index - 1]);
    }
}
#else
static void ed448_lattices_parallel_callback(void *arg)
{
    struct ed448_lattices_work *work = arg;
    /* Handle the left-hand side of the Schnorr equation */
    ed448_identity(work->thread_result);
    for (int i = 224; i >= 0; i--) {

        ed448_point_double(work->thread_result, work->thread_result, 1);
        int lookup_index = (array_bit(work->delta_response, 225 + i) << 1) | array_bit(work->delta_response, i);
        if (lookup_index)
            ed448_points_add(work->thread_result, work->thread_result, work->lookup[lookup_index - 1]);
    }
}
#endif /* !FE3C_ED448_LATTICE_BASIS_REDUCTION */

static int ed448_check_group_equation(point *pubkey_gen, point *commit_gen, const u8 *challenge, const u8 *response)
{
    point_ed448 *public_key = (point_ed448 *) pubkey_gen;
    point_ed448 *commitment = (point_ed448 *) commit_gen;

#if !FE3C_ED448_LATTICE_BASIS_REDUCTION
    ed448_point_negate(public_key);
    point_ed448 commitment_pretender;
    /* Compute S*B - h*A */
    ed448_double_scalar_multiply_vartime(&commitment_pretender, response, challenge, public_key);
    /* Check if 2^c*(S*B - h*A) == 2^c*R */
    return ed448_points_equal_modulo_cofactor(&commitment_pretender, commitment);
#else
    /* Follow eprint:2020/454 and find a value k such that both
     * k and k*h (where h is the challenge) are short modulo the
     * group order L. Instead of verifying [c][S]B = [c]R + [c][h]A, we
     * shall check that:
     *
     *               [c]([kS]B - [k]R - [kh]A) = (0, 1)
     *
     * Both k (mod L) and kh (mod L) will be below 224 bits in
     * length, and kS we will split in half and use the precomputed
     * value of [2^{224}]B to cut the number of point doublings in half.
     */

    u8 delta[32];
    int delta_negative;
    u8 delta_challenge[32];
    int delta_challenge_negative;
    u8 delta_response[57];
    /* Construct a lattice spanned by { (L, 0), (h, 1) } and find its
     * size-reduced basis. Specifically, we are interested in a short
     * vector (v0, v1) with v0 = v1*h (mod L). Both v0 and v1 may be
     * negative and we must not normalize them to range [0, L), since
     * L-vi will not be short anymore. Instead, return absolute values
     * delta_challenge = |v0| and delta = |v1|, but keep track of the
     * signs. Based on the signs, we shall negate the group elements
     * before doing the multi-scalar multiplication below. Compute
     * also v1*S (mod L), which we will split in half as described above. */
    ed448_lattice_basis_reduction(
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
        ed448_point_negate(commitment);
    if (!delta_challenge_negative)
        ed448_point_negate(public_key);

    /* Use two small lookup tables instead of one large one. It is possible to precompute more combinations of the points
     * here and use a 15-entry lookup table saving a point addition in each loop iteration. */
    const point_ed448 *right_lookup[3];
    const point_ed448 *left_lookup[3];

    left_lookup[0] = &ed448_basepoint;
    left_lookup[1] = &ed448_basepoint_times_2_225;
    left_lookup[2] = &ed448_basepoint_times_2_225_plus_1;

    point_ed448 sum;
    ed448_points_add(&sum, commitment, public_key);
    right_lookup[0] = commitment;
    right_lookup[1] = public_key;
    right_lookup[2] = &sum;

    point_ed448 accumulator;
    point_ed448 thread_accumulator;
    struct ed448_lattices_work lattices_work = {
        .thread_result = &thread_accumulator,
        .delta_response = delta_response,
        .lookup = left_lookup
    };
    struct parallel_work work = {
        .func = ed448_lattices_parallel_callback,
        .arg = &lattices_work
    };
    int parallel = schedule_parallel_work(&work);
    ed448_identity(&accumulator);
    for (int i = 224; i >= 0; i--) {

        ed448_point_double(&accumulator, &accumulator, 1);
        int right_lookup_index = (array_bit(delta_challenge, i) << 1) | array_bit(delta, i);
        if (right_lookup_index)
            ed448_points_add(&accumulator, &accumulator, right_lookup[right_lookup_index - 1]);
        /* Only handle the left-hand side of the Schnorr equation if it is not being handled in parallel */
        if (!parallel) {
            int left_lookup_index = (array_bit(delta_response, 225 + i) << 1) | array_bit(delta_response, i);
            if (left_lookup_index)
                ed448_points_add(&accumulator, &accumulator, left_lookup[left_lookup_index - 1]);
        }
    }

    if (parallel) {
        wait_for_parallel_work();
        ed448_points_add(&accumulator, &accumulator, &thread_accumulator);
    }

    point_ed448 identity;
    ed448_identity(&identity);
    /* Clear the cofactor and check if the result is the identity */
    return ed448_points_equal_modulo_cofactor(&accumulator, &identity);
#endif /* !FE3C_ED448_LATTICE_BASIS_REDUCTION */
}

const group_ops ed448_group_ops = {
    .encode = ed448_encode,
    .decode = ed448_decode,
    .multiply_basepoint = ed448_multiply_basepoint,
    .check_group_equation = ed448_check_group_equation
};
