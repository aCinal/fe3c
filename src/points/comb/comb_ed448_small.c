#if FE3C_OPTIMIZATION_ED448_ISOGENY && !FE3C_SMALL_PRECOMPUTATION
    #error "Build system inconsistency detected! comb_ed448_small.c in use despite FE3C_OPTIMIZATION_ED448_ISOGENY being set and FE3C_SMALL_PRECOMPUTATION not being set"
#endif /* FE3C_OPTIMIZATION_ED448_ISOGENY && !FE3C_SMALL_PRECOMPUTATION */

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

#if FE3C_OPTIMIZATION_ED448_ISOGENY
    /* Set the extended T coordinate to the correct value */
    fe448_mul(r->T, r->X, r->Y);
#endif /* FE3C_OPTIMIZATION_ED448_ISOGENY */
}
