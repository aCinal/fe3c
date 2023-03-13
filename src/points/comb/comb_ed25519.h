
#ifndef __POINTS_COMB_COMB_ED25519_H
#define __POINTS_COMB_COMB_ED25519_H

#include <points/points.h>
#include <field_elements/field_elements_ed25519.h>

#if !FE3C_SMALL_PRECOMPUTATION

/* Store intermediate results needed for addition in the precomputation table */
typedef struct ed25519_precomp_internal {
    fe25519 YplusX;
    fe25519 YminusX;
    fe25519 T2d;
} ed25519_precomp_internal;

typedef ed25519_precomp_internal ed25519_precomp;

void ed25519_comb_read_precomp(ed25519_precomp * r, u8 j, i8 ijt);
void ed25519_comb_add_precomp(point_ed25519 * r, const point_ed25519 * p, const ed25519_precomp * q);

#else

/* When not using the isogenous curve, store the affine representation only */
typedef struct ed25519_precomp_internal {
    fe25519 X;
    fe25519 Y;
} ed25519_precomp_internal;

typedef point_ed25519 ed25519_precomp;

void ed25519_comb_read_precomp(ed25519_precomp * r, u8 j, i8 ijt);
#define ed25519_comb_add_precomp ed25519_points_add

#endif /* !FE3C_SMALL_PRECOMPUTATION */

extern const ed25519_precomp_internal ed25519_comb_precomp[32][8];

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

#endif /* __POINTS_COMB_COMB_ED25519_H */
