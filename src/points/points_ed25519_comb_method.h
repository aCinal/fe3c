
#ifndef __FE3C_POINTS_POINTS_ED25519_COMB_METHOD_H
#define __FE3C_POINTS_POINTS_ED25519_COMB_METHOD_H

#include <field_elements/field_elements_ed25519.h>
#include <utils/utils.h>

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
    ed25519_point_precomp_conditional_move(r, &ed25519_comb_precomp[j][0], byte_equal(ijtabs, 1));
    ed25519_point_precomp_conditional_move(r, &ed25519_comb_precomp[j][1], byte_equal(ijtabs, 2));
    ed25519_point_precomp_conditional_move(r, &ed25519_comb_precomp[j][2], byte_equal(ijtabs, 3));
    ed25519_point_precomp_conditional_move(r, &ed25519_comb_precomp[j][3], byte_equal(ijtabs, 4));
    ed25519_point_precomp_conditional_move(r, &ed25519_comb_precomp[j][4], byte_equal(ijtabs, 5));
    ed25519_point_precomp_conditional_move(r, &ed25519_comb_precomp[j][5], byte_equal(ijtabs, 6));
    ed25519_point_precomp_conditional_move(r, &ed25519_comb_precomp[j][6], byte_equal(ijtabs, 7));
    ed25519_point_precomp_conditional_move(r, &ed25519_comb_precomp[j][7], byte_equal(ijtabs, 8));

    /* Negate the point if necessary */
    ed25519_point_precomp_conditional_neg_in_place(r, negate);
}

static inline void ed25519_points_add_precomp(point_ed25519 * r, const point_ed25519 * p, const point_precomp * q, int set_extended_coordinate) {

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
    /* Z3 ::= F*G */
    fe_mul(r->Z, F, G);

    /* When scheduled to be followed by another doubling we can skip setting the extended coordinate T
     * which is not needed for doubling */
    if (set_extended_coordinate) {

        /* T3 ::= E*H */
        fe_mul(r->T, E, H);
    }
}

static inline void ed25519_comb_recode_scalar_into_4naf(i8 naf[64], const u8 s[32]) {

    i8 carry;

    /* Split the scalar into base-16 unsigned representation */
    for (int i = 0; i < 32; i++) {

        /* From each byte of the scalar extract the two 4-bit digits - note that the exponent array
         * is ordered down columns first (column index changes more slowly) which allows for simpler
         * recoding of the scalar than in Lim/Lee method (where each entry of the recoding table has
         * to be a sum over bits distant by a). */
        naf[2 * i + 0] = ( (s[i] >> 0) & 0xF );
        naf[2 * i + 1] = ( (s[i] >> 4) & 0xF );
    }

    /* Recode the scalar into NAF representation */
    carry = 0;
    for (int i = 0; i < 63; i++) {

        naf[i] += carry;
        /* Check if naf[i] is larger than 2^{w-1} = 8 and if so make naf[i] negative
         * (subtract 2^w = 16) and propagate the carry to the next digit */
        carry = naf[i] + 0x8;
        /* Note that carry can only be 0 or 1 at this point */
        carry >>= 4;
        naf[i] -= carry << 4;
    }
    /* Propagate the carry to the overflow digit (top 4 bits) */
    naf[63] += carry;
}

#endif /* __FE3C_POINTS_POINTS_ED25519_COMB_METHOD_H */
