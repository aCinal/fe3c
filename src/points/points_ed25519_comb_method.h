
#ifndef __FE3C_POINTS_POINTS_ED25519_COMB_METHOD_H
#define __FE3C_POINTS_POINTS_ED25519_COMB_METHOD_H

#include <field_elements/field_elements_ed25519.h>

/* Store affine coordinates X,Y and the extended coordinate T=XY in the precomputation table */
typedef struct point_precomp {
    fe25519 X;
    fe25519 Y;
    /* TODO: Consider storing only X and Y and recomputing T each time (performance-storage tradeoff) */
    fe25519 T;
} point_precomp;

/* Define the precomputation table, for readability it is kept in a separate file */
static const point_precomp ed25519_comb_precomp[32][8] = {
#if FE3C_32BIT
    #include <points/precomputation/precomputation_ed25519_32.h>
#else
    #include <points/precomputation/precomputation_ed25519_64.h>
#endif /* FE3C_32BIT */
};

#define equal(x, y)  ({ \
    u8 __aux = (x ^ y); \
    __aux |= (__aux >> 4); \
    __aux |= (__aux >> 2); \
    __aux |= (__aux >> 1); \
    1 & (__aux ^ 1); \
})

#define point_conditional_move(p, ijt, k) ({ \
    u8 __move = equal((ijt), k); \
    fe_conditional_move(&p->X, (fe *) &ed25519_comb_precomp[j][k - 1].X, __move); \
    fe_conditional_move(&p->Y, (fe *) &ed25519_comb_precomp[j][k - 1].Y, __move); \
    fe_conditional_move(&p->T, (fe *) &ed25519_comb_precomp[j][k - 1].T, __move); \
})

static inline void ed25519_point_conditional_neg_in_place(point * p, int negate) {

    /* Negate the X and T coordinates conditionally */
    fe mX, mT;
    fe_neg(&mX, &p->X);
    fe_neg(&mT, &p->T);
    fe_conditional_move(&p->X, &mX, negate);
    fe_conditional_move(&p->T, &mT, negate);
}

static inline void ed25519_comb_read_precomp(point * r, u8 j, i8 ijt) {

    FE3C_SANITY_CHECK( j < sizeof(ed25519_comb_precomp) );

    /* Check the sign bit, cast ijt to u8 first to counteract sign extension */
    u8 negate = (u8) ijt >> 7;
    /* If the sign bit is set we obtain an identity mask which results in computing (ijt-2ijt) or -ijt */
    u8 ijtabs = ijt - 2 * ((-negate) & ijt);
    FE3C_SANITY_CHECK( ijtabs <= sizeof(ed25519_comb_precomp[0]) );

    /* Start with the identity - if ijt is 0 then we leave the result as the identity */
    r->X = fe_zero;
    r->Y = fe_one;
    r->T = fe_zero;
    /* Set the Z coordinate to one for good */
    r->Z = fe_one;

    /* Choose one entry of the precomputation table in a branchless manner
     * - an added advantage is that we access all elements in a given row
     * (for a given subblock j) thus preventing cache-based timing attacks. */
    point_conditional_move(r, ijtabs, 1);
    point_conditional_move(r, ijtabs, 2);
    point_conditional_move(r, ijtabs, 3);
    point_conditional_move(r, ijtabs, 4);
    point_conditional_move(r, ijtabs, 5);
    point_conditional_move(r, ijtabs, 6);
    point_conditional_move(r, ijtabs, 7);
    point_conditional_move(r, ijtabs, 8);

    /* Negate the point if necessary */
    ed25519_point_conditional_neg_in_place(r, negate);
}

#endif /* __FE3C_POINTS_POINTS_ED25519_COMB_METHOD_H */
