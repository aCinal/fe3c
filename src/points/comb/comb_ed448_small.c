#if FE3C_ED448_ISOGENY && !FE3C_SMALL_PRECOMPUTATION
    #error "Build system inconsistency detected! comb_ed448_small.c in use despite FE3C_ED448_ISOGENY being set and FE3C_SMALL_PRECOMPUTATION not being set"
#endif /* FE3C_ED448_ISOGENY && !FE3C_SMALL_PRECOMPUTATION */

#include <points/comb/comb_ed448.h>
#include <utils/utils.h>

static inline void ed448_comb_cmov(ed448_precomp * r, const ed448_precomp_internal * p, int move) {

    fe448_conditional_move(r->X, p->X, move);
    fe448_conditional_move(r->Y, p->Y, move);
    /* Note that for performance purposes we do not set the Z coordinate correctly at this point. This
     * is done only once in ed448_comb_read_precomp. Similarly we do not redundantly set the extended
     * T coordinate if the isogeny is in use. */
}

static inline void ed448_comb_cneg(ed448_precomp * p, int negate) {

    /* Negate the X coordinate conditionally */
    fe448 mX;
    fe448_neg(mX, p->X);
    fe448_conditional_move(p->X, mX, negate);
}

void ed448_comb_read_precomp(ed448_precomp * r, u8 j, i8 ijt) {

    FE3C_SANITY_CHECK(j < static_array_len(ed448_comb_precomp), "j=%d", j);

    /* Check the sign bit, cast ijt to u8 first to counteract sign extension */
    u8 negate = (u8) ijt >> 7;
    /* If the sign bit is set we obtain an identity mask which results in computing (ijt-2ijt) or -ijt */
    u8 ijtabs = ijt - 2 * ((-negate) & ijt);
    FE3C_SANITY_CHECK(ijtabs <= static_array_len(ed448_comb_precomp[0]), "ijt=%d", ijt);

    /* Start with the identity - if ijt is 0 then we leave the result as the identity */
    fe448_copy(r->X, fe448_zero);
    fe448_copy(r->Y, fe448_one);
    /* Set the Z coordinate to one for good */
    fe448_copy(r->Z, fe448_one);

    /* Choose one entry of the precomputation table in a branchless manner
     * - an added advantage is that we access all elements in a given row
     * (for a given subblock j) thus preventing cache-based timing attacks. */
    ed448_comb_cmov(r, &ed448_comb_precomp[j][0], byte_equal(ijtabs, 1));
    ed448_comb_cmov(r, &ed448_comb_precomp[j][1], byte_equal(ijtabs, 2));
    ed448_comb_cmov(r, &ed448_comb_precomp[j][2], byte_equal(ijtabs, 3));
    ed448_comb_cmov(r, &ed448_comb_precomp[j][3], byte_equal(ijtabs, 4));
    ed448_comb_cmov(r, &ed448_comb_precomp[j][4], byte_equal(ijtabs, 5));
    ed448_comb_cmov(r, &ed448_comb_precomp[j][5], byte_equal(ijtabs, 6));
    ed448_comb_cmov(r, &ed448_comb_precomp[j][6], byte_equal(ijtabs, 7));
    ed448_comb_cmov(r, &ed448_comb_precomp[j][7], byte_equal(ijtabs, 8));

    /* Negate the point if necessary */
    ed448_comb_cneg(r, negate);

#if FE3C_ED448_ISOGENY
    /* Set the extended T coordinate to the correct value */
    fe448_mul(r->T, r->X, r->Y);
#endif /* FE3C_ED448_ISOGENY */
}

#if FE3C_ED448_ISOGENY
void ed448_comb_add_precomp(point_ed448 * r, const point_ed448 * p, const ed448_precomp * q) {

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

    /* D := Z1*2*Z2, but we know that Z2=1 for precomputed points */
    fe448_add(r->Z, p->Z, p->Z);

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

#else

void ed448_comb_add_precomp(point_ed448 * r, const point_ed448 * p, const ed448_precomp * q) {

    /* For optimal stack and cache usage we reduce the number of variables
     * allocated relative to the algorithm description in RFC 8032. For
     * clarity, the comments include the names of variables as they appear
     * in RFC 8032. */
    fe448 B, C;

    /* A := Z1*Z2, but we know Z2=1 for precomputed points */
    /* C := X1*X2 */
    fe448_mul(C, p->X, q->X);
    /* H := (X1+Y1)*(X2+Y2) */
    fe448_add(B, p->X, p->Y);
    fe448_add(r->X, q->X, q->Y);
    fe448_mul(r->X, B, r->X);
    /* D := Y1*Y2 */
    fe448_mul(r->Y, p->Y, q->Y);

    /* We need to compute:
     *
     *           E := d*C*D,   G := B+E,   F := B-E
     *
     * where B=A^2=(Z1)^2, and then finally:
     *
     *   X3 := A*F*(H-C-D), Y3 := A*G*(D-C), and Z3 := F*G
     *
     * Start by computing A*(H-C-D), A*(D-C) and E=d*C*D, this way we free up
     * the C buffer which we immediately reuse for E. Also after doing this we
     * can use r->Z for temporary storage while still supporting aliasing
     * as we shall no longer access p->Z past this point.
     */

    /* Recall that r->X stores H and r->Y stores D,
     * and so we compute A*(H-C-D) here */
    fe448_sub(r->X, r->X, C);
    fe448_sub(r->X, r->X, r->Y);
    fe448_mul(r->X, r->X, p->Z);

    /* Compute D-C */
    fe448_sub(B, r->Y, C);
    /* E := d*C*D */
    fe448_mul(C, ed448_d, C);
    fe448_mul(C, C, r->Y);
    /* Set r->Y to A*(D-C) */
    fe448_mul(r->Y, B, p->Z);
    /* B := A^2 */
    fe448_square(B, p->Z);
    /* r->Z is free real estate at this point,
     * reuse it for F (note that C stores the
     * value of E at this point - we shall reuse
     * it for G in a moment) */

    /* F := B-E */
    fe448_sub(r->Z, B, C);
    /* G := B+E */
    fe448_add(C, B, C);

    /* Do the missing multiplications of X3 and Y3 by F and G, respectively */
    fe448_mul(r->X, r->X, r->Z);
    fe448_mul(r->Y, r->Y, C);
    /* Compute Z3 as F*G */
    fe448_mul(r->Z, r->Z, C);
}

#endif
