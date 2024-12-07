#if !FE3C_ED448_SMALL_PRECOMPUTATION
    #error "Build system inconsistency detected! comb_ed448_small.c in use despite FE3C_ED448_SMALL_PRECOMPUTATION not being set"
#endif /* !FE3C_ED448_SMALL_PRECOMPUTATION */

#include <points/comb/comb_ed448.h>
#include <utils/utils.h>

static inline void ed448_comb_cmov(ed448_precomp *r, const ed448_precomp_internal *p, int move)
{
    fe448_conditional_move(r->X, p->X, move);
    fe448_conditional_move(r->Y, p->Y, move);
    /* Note that for performance purposes we do not set the Z and T coordinates correctly at this point. This
     * is done only once in ed448_comb_read_precomp. */
}

static inline void ed448_comb_cneg(ed448_precomp *p, int negate)
{
    /* Negate the X coordinate conditionally */
    fe448 mX;
    fe448_neg(mX, p->X);
    fe448_conditional_move(p->X, mX, negate);
}

void ed448_comb_read_precomp(ed448_precomp *r, u8 j, i8 ijt)
{
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

    /* Set the extended T coordinate to the correct value */
    fe448_mul(r->T, r->X, r->Y);
}

void ed448_comb_add_precomp(point_ed448 *r, const point_ed448 *p, const ed448_precomp *q)
{
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
