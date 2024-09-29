#include "comb_parallel_ed25519.h"
#include <points/comb/comb_ed25519.h>
#include <field_elements/field_elements_ed25519.h>
#include <utils/utils.h>

static inline void ed25519_identity(point_ed25519 * p) {

    fe25519_copy(p->X, fe25519_zero);
    fe25519_copy(p->Y, fe25519_one);
    fe25519_copy(p->Z, fe25519_one);
    fe25519_copy(p->T, fe25519_zero);
}

void ed25519_comb_loop(point_ed25519 * result, const i8 * scalar_recoding, int odd) {

    FE3C_SANITY_CHECK(0 == odd || 1 == odd, NULL);

    ed25519_identity(result);
    ed25519_precomp p;
    for (int i = odd; i < 64; i += 2) {

        /* We let the loop index run twice as fast and skip every other entry of recoding,
         * but correct for it in the j index (j = i / 2) */
        ed25519_comb_read_precomp(&p, i >> 1, scalar_recoding[i]);
        ed25519_comb_add_precomp(result, result, &p);
    }

    /* Clear the last accessed precomputed point */
    purge_secrets(&p, sizeof(p));
}
