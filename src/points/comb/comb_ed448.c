#if !FE3C_ED448_ISOGENY
    #error "Build system inconsistency detected! comb_ed448.c in use despite FE3C_ED448_ISOGENY not being set"
#endif /* !FE3C_ED448_ISOGENY */

#if FE3C_SMALL_PRECOMPUTATION
    #error "Build system inconsistency detected! comb_ed448.c in use despite FE3C_SMALL_PRECOMPUTATION being set"
#endif /* FE3C_SMALL_PRECOMPUTATION */

#include <points/comb/comb_ed448.h>
#include <utils/utils.h>

static inline void ed448_comb_cmov(ed448_precomp * r, const ed448_precomp_internal * p, int move) {

    fe448_conditional_move(r->YplusX, p->YplusX, move);
    fe448_conditional_move(r->YminusX, p->YminusX, move);
    fe448_conditional_move(r->T2d, p->T2d, move);
}

static inline void ed448_comb_cneg(ed448_precomp * p, int negate) {

    /* Negate the X and T coordinates conditionally, which amounts to swapping
     * YminusX with YplusX and negating T2d */
    fe448 YminusX, minusT2d;

    /* Conditionally swap YminusX with YplusX */
    fe448_copy(YminusX, p->YminusX);
    fe448_neg(minusT2d, p->T2d);
    fe448_conditional_move(p->YminusX, p->YplusX, negate);
    fe448_conditional_move(p->YplusX, YminusX, negate);

    /* Conditionally negate T2d */
    fe448_conditional_move(p->T2d, minusT2d, negate);
}

void ed448_comb_read_precomp(ed448_precomp * r, u8 j, i8 ijt) {

    FE3C_SANITY_CHECK(j < static_array_len(ed448_comb_precomp), "j=%d", j);

    /* Check the sign bit, cast ijt to u8 first to counteract sign extension */
    u8 negate = (u8) ijt >> 7;
    /* If the sign bit is set we obtain an identity mask which results in computing (ijt-2ijt) or -ijt */
    u8 ijtabs = ijt - 2 * ((-negate) & ijt);
    FE3C_SANITY_CHECK(ijtabs <= static_array_len(ed448_comb_precomp[0]), "ijt=%d", ijt);

    /* Start with the identity - if ijt is 0 then we leave the result as the identity */
    fe448_copy(r->YplusX, fe448_one);
    fe448_copy(r->YminusX, fe448_one);
    fe448_copy(r->T2d, fe448_zero);

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
}

void ed448_comb_add_precomp(point_ed448 * r, const point_ed448 * p, const ed448_precomp * q) {

    /* TODO: Reuse some old variables to reduce stack usage */
    fe448 A, B, C, D, E, F, G, H;

    /* A := (Y1-X1)*(Y2-X2) */
    fe448_sub(E, p->Y, p->X);
    fe448_mul(A, E, q->YminusX);

    /* B := (Y1+X1)*(Y2+X2) */
    fe448_add(G, p->Y, p->X);
    fe448_mul(B, G, q->YplusX);

    /* C := T1*2*d*T2 */
    fe448_mul(C, p->T, q->T2d);

    /* D := Z1*2*Z2, but we know Z2=1 for precomputed points */
    fe448_add(D, p->Z, p->Z);

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
    /* Z3 := F*G */
    fe448_mul(r->Z, F, G);
    /* T3 := E*H */
    fe448_mul(r->T, E, H);
}