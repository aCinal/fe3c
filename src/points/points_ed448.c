#include <points/points.h>
#include <field_elements/field_elements_ed448.h>
#include <utils/utils.h>
#if FE3C_OPTIMIZATION_COMB_METHOD
    #include <points/points_ed448_comb_method.h>
#endif /* FE3C_OPTIMIZATION_COMB_METHOD */

#define ED448_STR \
    "    X = " FE448_STR "\n" \
    "    Y = " FE448_STR "\n" \
    "    Z = " FE448_STR "\n" \
    "    T = " FE448_STR
#define ED448_TO_STR(p) \
    FE448_TO_STR((p)->X), FE448_TO_STR((p)->Y), FE448_TO_STR((p)->Z), FE448_TO_STR((p)->T)

/* Note that the below basepoint is the basepoint on the 4-isogenous curve: y^2 - x^2 = 1 + (d-1)x^2y^2 */
static const point_ed448 ed448_basepoint = {
#if FE3C_64BIT
    .X = { 0x0, 0x0, 0x0, 0x80000000000000, 0xfffffffffffffe, 0xffffffffffffff, 0xffffffffffffff, 0x7fffffffffffff },
    .Y = { 0x6079b4dfdd4a64, 0xc1e3ab470a1c8, 0x44d73f48e5199b, 0x50452714141818, 0x4c74c393d5242c, 0x24080526437050, 0xd48d06c13078ca, 0x8508de14f04286 },
    .Z = { 1, 0, 0, 0, 0, 0, 0, 0 },
    .T = { 0xe3c816dc198105, 0x62071833f4e093, 0x4dde98e3421403, 0xa319b57519c985, 0x794be956382384, 0xe1ddc2b86da60f, 0x50e23d5682a9ff, 0x6d3669e173c6a4 }
#else
    .X = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x8000000, 0xffffffe, 0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff, 0xfffffff, 0x7ffffff },
    .Y = { 0xfdd4a64, 0x6079b4d, 0x470a1c8, 0xc1e3ab, 0x8e5199b, 0x44d73f4, 0x4141818, 0x5045271, 0x3d5242c, 0x4c74c39, 0x6437050, 0x2408052, 0x13078ca, 0xd48d06c, 0x4f04286, 0x8508de1 },
    .Z = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    .T = { 0xc198105, 0xe3c816d, 0x3f4e093, 0x6207183, 0x3421403, 0x4dde98e, 0x519c985, 0xa319b57, 0x6382384, 0x794be95, 0x86da60f, 0xe1ddc2b, 0x682a9ff, 0x50e23d5, 0x173c6a4, 0x6d3669e }
#endif /* FE3C_64BIT */
};

#if FE3C_ENABLE_SANITY_CHECKS
static inline int ed448_is_on_curve(const point_ed448 * p) {

    /* Note that we are not checking if the point is on the Ed448 curve
     * defined by the equation:
     *
     *                     x^2 + y^2 = 1 + dx^2y^2
     *
     * where d = -39081. Instead we are checking if the point belongs to
     * the isogenous twisted Edwards curve defined by the equation:
     *
     *                   y^2 - x^2 = 1 + (d-1)x^2y^2
     */

    fe448 x, y, z;
    fe448 lhs;
    fe448 rhs;

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

static inline int ed448_valid_extended_projective(const point_ed448 * p) {

    /* Check the consistency of the extended projective coordinate */
    fe448 xy, tz;
    fe448_mul(xy, p->X, p->Y);
    fe448_mul(tz, p->T, p->Z);
    fe448_strong_reduce(xy, xy);
    fe448_strong_reduce(tz, tz);
    return fe448_equal(tz, xy);
}
#endif /* FE3C_ENABLE_SANITY_CHECKS */

static inline void ed448_identity(point_ed448 * p) {

    fe448_copy(p->X, fe448_zero);
    fe448_copy(p->Y, fe448_one);
    fe448_copy(p->Z, fe448_one);
    fe448_copy(p->T, fe448_zero);
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

static void ed448_point_double(point_ed448 * r, const point_ed448 * p, int set_extended_coordinate) {

    FE3C_SANITY_CHECK(ed448_is_on_curve(p), ED448_STR, ED448_TO_STR(p));

    /* TODO: Reuse some old variables to reduce stack usage */
    fe448 A, B, C, E, F, G, H;

    /* A := X1^2 */
    fe448_square(A, p->X);
    /* B := Y1^2*/
    fe448_square(B, p->Y);

    /* C := 2*Z1^2 */
    fe448_square(C, p->Z);
    fe448_add(C, C, C);

    /* H := A+B */
    fe448_add(H, A, B);

    /* E := H-(X1+Y1)^2 */
    fe448_add(E, p->X, p->Y);
    fe448_square(E, E);
    fe448_sub(E, H, E);

    /* G := A-B */
    fe448_sub(G, A, B);

    /* F := C+G */
    fe448_add(F, C, G);

#if FE3C_32BIT
    /* Note that F is a result of addition of two variables
     * which themselves were produced by additions. This amounts
     * to four carries, which is the most we can handle in our 28-bit
     * representation. Do a weak reduction before proceeding */
    fe448_weak_reduce(F, F);
#endif /* FE3C_32BIT */

    /* X3 := E*F */
    fe448_mul(r->X, E, F);
    /* Y3 := G*H */
    fe448_mul(r->Y, G, H);
    /* Z3 := F*G */
    fe448_mul(r->Z, F, G);

    /* When scheduled to be followed by another doubling we can skip setting the extended coordinate T
     * which is not needed for doubling */
    if (set_extended_coordinate) {

        /* T3 := E*H */
        fe448_mul(r->T, E, H);
    }
}

static inline int ed448_is_ok_order(const point_ed448 * p) {

    /* Note that all low-order points on the original untwisted curve are in the
     * kernel of the 4-isogeny we have used to move to a twisted curve. Therefore
     * to check for low-order input points we must only check the resulting point
     * on the isogenous curve against the identity. */

    /* TODO: Reject mixed-order points both for Ed448 and Ed25519. Note that the
     * check for this would need to be done before applying the 4-isogeny which
     * clears the cofactor. */
    point_ed448 e;
    ed448_identity(&e);

    /* Check if equal to the identity */
    return 1 - ed448_points_equal((const point *) p, (const point *) &e);
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

static void ed448_points_add(point_ed448 * r, const point_ed448 * p, const point_ed448 * q) {

    FE3C_SANITY_CHECK(ed448_is_on_curve(p), ED448_STR, ED448_TO_STR(p));
    FE3C_SANITY_CHECK(ed448_is_on_curve(q), ED448_STR, ED448_TO_STR(q));
    FE3C_SANITY_CHECK(ed448_valid_extended_projective(p), ED448_STR, ED448_TO_STR(p));
    FE3C_SANITY_CHECK(ed448_valid_extended_projective(q), ED448_STR, ED448_TO_STR(q));

    /* TODO: Reuse some old variables to reduce stack usage */
    fe448 A, B, C, D, E, F, G, H;

    /* A := (Y1-X1)*(Y2-X2) */
    fe448_sub(E, p->Y, p->X);
    fe448_sub(F, q->Y, q->X);
    fe448_mul(A, E, F);

    /* B := (Y1+X1)*(Y2+X2) */
    fe448_add(G, p->Y, p->X);
    fe448_add(H, q->Y, q->X);
    fe448_mul(B, G, H);

    /* C := T1*2*d'*T2 */
    fe448_mul(C, p->T, q->T);
    fe448_mul(C, C, ed448twist_d);
    fe448_add(C, C, C);
    fe448_weak_reduce(C, C);

    /* D := Z1*2*Z2 */
    fe448_mul(D, p->Z, q->Z);
    fe448_add(D, D, D);

    /* E := B-A */
    fe448_sub(E, B, A);
    /* F := D-C */
    fe448_sub(F, D, C);
    /* G := D+C */
    fe448_add(G, D, C);
    /* H := B+A */
    fe448_add(H, B, A);

    /* X3 := E*F */
    fe448_mul(r->X, E, F);
    /* Y3 := G*H */
    fe448_mul(r->Y, G, H);
    /* T3 := E*H */
    fe448_mul(r->T, E, H);
    /* Z3 := F*G */
    fe448_mul(r->Z, F, G);
}

#if !FE3C_OPTIMIZATION_COMB_METHOD
static inline void ed448_scalar_multiply(point_ed448 * r, const point_ed448 * p, const u8 * s) {

    FE3C_SANITY_CHECK(ed448_is_on_curve(p), ED448_STR, ED448_TO_STR(p));

    point_ed448 R[2];
    ed448_identity(&R[0]);
    R[1] = *p;

    /* Do a simple Montgomery ladder */
    for (int i = 447; i >= 0; i--) {

        /* Recover the ith bit of the scalar */
        int bit = ( s[i >> 3] >> (i & 0x7) ) & 1;
        ed448_points_add(&R[1 - bit], &R[1 - bit], &R[bit]);
        ed448_point_double(&R[bit], &R[bit], 1);
    }

    *r = R[0];

    purge_secrets(&R[1], sizeof(R[1]));
}
#endif /* !FE3C_OPTIMIZATION_COMB_METHOD */

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

static void ed448_multiply_basepoint(point * rgen, const u8 * sraw) {

    point_ed448 * r = (point_ed448 *) rgen;
    u8 s[57];
    /* TODO: Combine division by four with NAF-recoding for the comb method variant */
    ed448_map_scalar_to_isogenous_curve(s, sraw);
#if !FE3C_OPTIMIZATION_COMB_METHOD
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
    point_precomp p;
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
        /* For the last iteration skip setting the extended coordinate */
        ed448_points_add_precomp(r, r, &p, i < 111);
    }

    /* Compute 2^{tw} Q = 2^4 Q */
    ed448_point_double(r, r, 0);
    ed448_point_double(r, r, 0);
    ed448_point_double(r, r, 0);
    ed448_point_double(r, r, 1);

    /* Do the second iteration of the outermost loop, i.e. iterate over even indices of the recoding
     * (see explanation above). */
    for (int i = 0; i < 113; i += 2) {

        /* We let the loop index run twice as fast and skip every other entry of naf,
         * but correct for it in the j index (j = i / 2) */
        ed448_comb_read_precomp(&p, i >> 1, naf[i]);
        /* For the last iteration skip setting the extended coordinate */
        ed448_points_add_precomp(r, r, &p, i < 111);
    }

    /* At this point Q := 2^{tw} Q is a no-op since 2^{tw} Q = 2^0 Q */

    /* Clear the recoding of the secret scalar from the stack */
    purge_secrets(naf, sizeof(naf));
    /* Clear the last accessed precomputed point */
    purge_secrets(&p, sizeof(p));
#endif /* !FE3C_OPTIMIZATION_COMB_METHOD */
}

static void ed448_point_negate(point * pgen) {

    point_ed448 * p = (point_ed448 *) pgen;
    fe448_neg(p->X, p->X);
    fe448_neg(p->T, p->T);
}

static inline void ed448_conditional_move(point_ed448 * r, const point_ed448 * p, int move) {

    fe448_conditional_move(r->X, p->X, move);
    fe448_conditional_move(r->Y, p->Y, move);
    fe448_conditional_move(r->Z, p->Z, move);
    fe448_conditional_move(r->T, p->T, move);
}

static void ed448_double_scalar_multiply(point * rgen, const u8 * s, const u8 * h, const point * pgen) {

    /* Note that since the points here are on the isogenous twisted Edwards curve, the result is not
     * really multiplication [s]B + [h]A, but (if we were to apply the dual isogeny and go back to
     * the original Ed448 curve) [4][s]B + [4][h]A */

    point_ed448 * r = (point_ed448 *) rgen;
    const point_ed448 * p = (const point_ed448 *) pgen;

#if FE3C_OPTIMIZATION_BASE_FOUR_SHAMIR_TRICK
    point_ed448 G[15];
    G[0] = ed448_basepoint;                    /* [0]A + [1]B */
    ed448_point_double(&G[ 1], &G[ 0], 1);     /* [0]A + [2]B */
    ed448_points_add(&G[ 2], &G[ 1], &G[ 0]);  /* [0]A + [3]B */
    G[3] = *p;                                 /* [1]A + [0]B */
    ed448_points_add(&G[ 4], &G[ 3], &G[ 0]);  /* [1]A + [1]B */
    ed448_points_add(&G[ 5], &G[ 3], &G[ 1]);  /* [1]A + [2]B */
    ed448_points_add(&G[ 6], &G[ 3], &G[ 2]);  /* [1]A + [3]B */
    ed448_point_double(&G[ 7], &G[ 3], 1);     /* [2]A + [0]B */
    ed448_points_add(&G[ 8], &G[ 7], &G[ 0]);  /* [2]A + [1]B */
    ed448_point_double(&G[ 9], &G[ 4], 1);     /* [2]A + [2]B */
    ed448_points_add(&G[10], &G[ 7], &G[ 2]);  /* [2]A + [3]B */
    ed448_points_add(&G[11], &G[ 7], &G[ 3]);  /* [3]A + [0]B */
    ed448_points_add(&G[12], &G[11], &G[ 0]);  /* [3]A + [1]B */
    ed448_points_add(&G[13], &G[11], &G[ 1]);  /* [3]A + [2]B */
    ed448_points_add(&G[14], &G[11], &G[ 2]);  /* [3]A + [3]B */

    /* Represent the two scalars as arrays of base-4 digits:
     *
     *                 s = (s223, s222, ..., s2, s1, s0)
     *                 h = (h223, h222, ..., h2, h1, h0)
     *
     * where si, hi are in { 0, 1, 2, 3 }. At each iteration i add to the
     * accumulator the point [si]B + [hi]A. Use the two base-4 digits to
     * find the point in the precomputation table.
     */
    ed448_identity(r);

    for (int i = 223; i >= 0; i--) {

        ed448_point_double(r, r, 0);
        ed448_point_double(r, r, 1);

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
        point_ed448 t;
        ed448_identity(&t);
        ed448_conditional_move(&t, &G[ 0], byte_equal(index,  1));
        ed448_conditional_move(&t, &G[ 1], byte_equal(index,  2));
        ed448_conditional_move(&t, &G[ 2], byte_equal(index,  3));
        ed448_conditional_move(&t, &G[ 3], byte_equal(index,  4));
        ed448_conditional_move(&t, &G[ 4], byte_equal(index,  5));
        ed448_conditional_move(&t, &G[ 5], byte_equal(index,  6));
        ed448_conditional_move(&t, &G[ 6], byte_equal(index,  7));
        ed448_conditional_move(&t, &G[ 7], byte_equal(index,  8));
        ed448_conditional_move(&t, &G[ 8], byte_equal(index,  9));
        ed448_conditional_move(&t, &G[ 9], byte_equal(index, 10));
        ed448_conditional_move(&t, &G[10], byte_equal(index, 11));
        ed448_conditional_move(&t, &G[11], byte_equal(index, 12));
        ed448_conditional_move(&t, &G[12], byte_equal(index, 13));
        ed448_conditional_move(&t, &G[13], byte_equal(index, 14));
        ed448_conditional_move(&t, &G[14], byte_equal(index, 15));

        ed448_points_add(r, r, &t);
    }
#else
    /* To reduce stack usage, only store pointers to the public key A
     * and the basepoint B, and allocate only one point (A+B) */
    point_ed448 sum;
    ed448_points_add(&sum, p, &ed448_basepoint);
    const point_ed448 * G[3];
    G[0] = &ed448_basepoint;  /* [0]A + [1]B */
    G[1] = p;                 /* [1]A + [0]B */
    G[2] = &sum;              /* [1]A + [1]B */

    ed448_identity(r);

    for (int i = 447; i >= 0; i--) {

        ed448_point_double(r, r, 1);

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
#endif /* !FE3C_OPTIMIZATION_BASE_FOUR_SHAMIR_TRICK */
}

const group_ops ed448_group_ops = {
    .points_equal = ed448_points_equal,
    .encode = ed448_encode,
    .decode = ed448_decode,
    .multiply_basepoint = ed448_multiply_basepoint,
    .point_negate = ed448_point_negate,
    .double_scalar_multiply = ed448_double_scalar_multiply
};
