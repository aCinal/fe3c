#include <port/points/comb/comb_parallel_ed448.h>
#include <points/comb/comb_ed448.h>
#include <field_elements/field_elements_ed448.h>
#include <utils/utils.h>

static inline void ed448_identity(point_ed448 * p) {

    fe448_copy(p->X, fe448_zero);
    fe448_copy(p->Y, fe448_one);
    fe448_copy(p->Z, fe448_one);
    fe448_copy(p->T, fe448_zero);
}

void ed448_comb_loop(point_ed448 * result, const i8 * scalar_recoding, int odd) {

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
