
#ifndef __FE3C_POINTS_POINTS_ED25519_COMB_METHOD_H
#define __FE3C_POINTS_POINTS_ED25519_COMB_METHOD_H

#include <field_elements/field_elements_ed25519.h>

/* Store intermediate results needed for addition in the precomputation table */
typedef struct point_precomp {
    fe25519 YplusX;
    fe25519 YminusX;
    fe25519 T2d;
} point_precomp;

/* Define the precomputation table, for readability it is kept in a separate file */
static const point_precomp ed25519_comb_precomp[32][8] = {
#if FE3C_32BIT
    #include <points/precomputation/precomputation_ed25519_32.h>
#else
    #include <points/precomputation/precomputation_ed25519_64.h>
#endif /* FE3C_32BIT */
};

static inline void ed25519_point_precomp_conditional_move(point_precomp *r, const point_precomp *p, int move) {

    fe_conditional_move(r->YplusX, p->YplusX, move);
    fe_conditional_move(r->YminusX, p->YminusX, move);
    fe_conditional_move(r->T2d, p->T2d, move);
}

static inline void ed25519_point_precomp_conditional_neg_in_place(point_precomp * p, int negate) {

    /* Negate the X and T coordinates conditionally, which amounts to swapping
     * YminusX with YplusX and negating T2d */
    fe25519 YminusX, minusT2d;

    /* Conditionally swap YminusX with YplusX */
    fe_copy(YminusX, p->YminusX);
    fe_neg(minusT2d, p->T2d);
    fe_conditional_move(p->YminusX, p->YplusX, negate);
    fe_conditional_move(p->YplusX, YminusX, negate);

    /* Conditionally negate T2d */
    fe_conditional_move(p->T2d, minusT2d, negate);
}

#define equal(x, y)  ({ \
    u8 __aux = (x ^ y); \
    __aux |= (__aux >> 4); \
    __aux |= (__aux >> 2); \
    __aux |= (__aux >> 1); \
    1 & (__aux ^ 1); \
})

static inline void ed25519_comb_read_precomp(point_precomp * r, u8 j, i8 ijt) {

    FE3C_SANITY_CHECK(j < sizeof(ed25519_comb_precomp), NULL);

    /* Check the sign bit, cast ijt to u8 first to counteract sign extension */
    u8 negate = (u8) ijt >> 7;
    /* If the sign bit is set we obtain an identity mask which results in computing (ijt-2ijt) or -ijt */
    u8 ijtabs = ijt - 2 * ((-negate) & ijt);
    FE3C_SANITY_CHECK(ijtabs <= sizeof(ed25519_comb_precomp[0]), "ijt=%d", ijt);

    /* Start with the identity - if ijt is 0 then we leave the result as the identity */
    fe_copy(r->YplusX, fe_one);
    fe_copy(r->YminusX, fe_one);
    fe_copy(r->T2d, fe_zero);

    /* Choose one entry of the precomputation table in a branchless manner
     * - an added advantage is that we access all elements in a given row
     * (for a given subblock j) thus preventing cache-based timing attacks. */
    ed25519_point_precomp_conditional_move(r, &ed25519_comb_precomp[j][0], equal(ijtabs, 1));
    ed25519_point_precomp_conditional_move(r, &ed25519_comb_precomp[j][1], equal(ijtabs, 2));
    ed25519_point_precomp_conditional_move(r, &ed25519_comb_precomp[j][2], equal(ijtabs, 3));
    ed25519_point_precomp_conditional_move(r, &ed25519_comb_precomp[j][3], equal(ijtabs, 4));
    ed25519_point_precomp_conditional_move(r, &ed25519_comb_precomp[j][4], equal(ijtabs, 5));
    ed25519_point_precomp_conditional_move(r, &ed25519_comb_precomp[j][5], equal(ijtabs, 6));
    ed25519_point_precomp_conditional_move(r, &ed25519_comb_precomp[j][6], equal(ijtabs, 7));
    ed25519_point_precomp_conditional_move(r, &ed25519_comb_precomp[j][7], equal(ijtabs, 8));

    /* Negate the point if necessary */
    ed25519_point_precomp_conditional_neg_in_place(r, negate);
}

static inline void ed25519_points_add_precomp(point_ed25519 * r, const point_ed25519 * p, const point_precomp * q) {

    /* TODO: Reuse some old variables to reduce stack usage */
    fe25519 A, B, C, D, E, F, G, H;

    /* A ::= (Y1-X1)*(Y2-X2) */
    fe_sub(E, p->Y, p->X);
    fe_mul(A, E, q->YminusX);

    /* B ::= (Y1+X1)*(Y2+X2) */
    fe_add(G, p->Y, p->X);
    fe_mul(B, G, q->YplusX);

    /* C ::= T1*2*d*T2 */
    fe_mul(C, p->T, q->T2d);

    /* D ::= Z1*2*Z2, but we know Z2=1 for precomputed points */
    fe_add(D, p->Z, p->Z);

    /* E ::= B-A */
    fe_sub(E, B, A);
    /* F ::= D-C */
    fe_sub(F, D, C);
    /* G ::= D+C */
    fe_add(G, D, C);
    /* H ::= B+A */
    fe_add(H, B, A);

    /* X3 ::= E*F */
    fe_mul(r->X, E, F);
    /* Y3 ::= G*H */
    fe_mul(r->Y, G, H);
    /* T3 ::= E*H */
    fe_mul(r->T, E, H);
    /* Z3 ::= F*G */
    fe_mul(r->Z, F, G);
}

#endif /* __FE3C_POINTS_POINTS_ED25519_COMB_METHOD_H */
