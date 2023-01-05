
#ifndef __FE3C_POINTS_POINTS_ED448_COMB_METHOD_H
#define __FE3C_POINTS_POINTS_ED448_COMB_METHOD_H

#include <field_elements/field_elements_ed448.h>
#include <utils/utils.h>

/* Store only affine representations in the precomputation table */
typedef struct point_precomp {
    fe448 X;
    fe448 Y;
} point_precomp;

/* Define the precomputation table, for readability it is kept in a separate file */
static const point_precomp ed448_comb_precomp[57][8] = {
#if FE3C_32BIT
    #include <points/precomputation/precomputation_ed448_32.h>
#else
    #include <points/precomputation/precomputation_ed448_64.h>
#endif /* FE3C_32BIT */
};

static inline void ed448_point_precomp_conditional_move(point_ed448 *r, const point_precomp *p, int move) {

    fe_conditional_move(r->X, p->X, move);
    fe_conditional_move(r->Y, p->Y, move);
}

static inline void ed448_point_conditional_neg_in_place(point_ed448 * p, int negate) {

    /* Negate the X coordinate conditionally */
    fe448 mX;
    fe_neg(mX, p->X);
    fe_conditional_move(p->X, mX, negate);
}

static inline void ed448_comb_read_precomp(point_ed448 * r, u8 j, i8 ijt) {

    FE3C_SANITY_CHECK(j < sizeof(ed448_comb_precomp), NULL);

    /* Check the sign bit, cast ijt to u8 first to counteract sign extension */
    u8 negate = (u8) ijt >> 7;
    /* If the sign bit is set we obtain an identity mask which results in computing (ijt-2ijt) or -ijt */
    u8 ijtabs = ijt - 2 * ((-negate) & ijt);
    FE3C_SANITY_CHECK(ijtabs <= sizeof(ed448_comb_precomp[0]), "ijt=%d", ijt);

    /* Start with the identity - if ijt is 0 then we leave the result as the identity */
    fe_copy(r->X, fe_zero);
    fe_copy(r->Y, fe_one);
    /* Set the Z coordinate to one for good */
    fe_copy(r->Z, fe_one);

    /* Choose one entry of the precomputation table in a branchless manner
     * - an added advantage is that we access all elements in a given row
     * (for a given subblock j) thus preventing cache-based timing attacks. */
    ed448_point_precomp_conditional_move(r, &ed448_comb_precomp[j][0], byte_equal(ijtabs, 1));
    ed448_point_precomp_conditional_move(r, &ed448_comb_precomp[j][1], byte_equal(ijtabs, 2));
    ed448_point_precomp_conditional_move(r, &ed448_comb_precomp[j][2], byte_equal(ijtabs, 3));
    ed448_point_precomp_conditional_move(r, &ed448_comb_precomp[j][3], byte_equal(ijtabs, 4));
    ed448_point_precomp_conditional_move(r, &ed448_comb_precomp[j][4], byte_equal(ijtabs, 5));
    ed448_point_precomp_conditional_move(r, &ed448_comb_precomp[j][5], byte_equal(ijtabs, 6));
    ed448_point_precomp_conditional_move(r, &ed448_comb_precomp[j][6], byte_equal(ijtabs, 7));
    ed448_point_precomp_conditional_move(r, &ed448_comb_precomp[j][7], byte_equal(ijtabs, 8));

    /* Negate the point if necessary */
    ed448_point_conditional_neg_in_place(r, negate);
}

static inline void ed448_comb_recode_scalar_into_4naf(i8 naf[113], const u8 s[56]) {

    i8 carry = 0;
    for (int i = 0; i < 56; i++) {

        /* From each byte of the scalar extract the two 4-bit digits - note that the exponent array
         * is ordered down columns first (column index changes more slowly) which allows for simpler
         * recoding of the scalar than in Lim/Lee method (where each entry of the recoding table has
         * to be a sum over bits distant by a). */
        naf[2 * i + 0] = ( (s[i] >> 0) & 0xF ) + carry;
        /* Check if naf[2 * i + 0] is larger than 2^{w-1} = 8 and if so make naf[2 * i + 0] negative
         * (subtract 2^w = 16) and propagate the carry to the next digit */
        carry = naf[2 * i + 0] + 0x8;
        /* Note that carry can only be 0 or 1 at this point */
        carry >>= 4;
        naf[2 * i + 0] -= carry << 4;

        naf[2 * i + 1] = ( (s[i] >> 4) & 0xF ) + carry;
        carry = naf[2 * i + 1] + 0x8;
        carry >>= 4;
        naf[2 * i + 1] -= carry << 4;
    }
    naf[112] = carry;
}

#endif /* __FE3C_POINTS_POINTS_ED448_COMB_METHOD_H */
