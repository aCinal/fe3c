
#ifndef __POINTS_COMB_COMB_ED448_H
#define __POINTS_COMB_COMB_ED448_H

#include <points/points.h>
#include <field_elements/field_elements_ed448.h>

#if !FE3C_ED448_SMALL_PRECOMPUTATION

/* Store intermediate results needed for addition in the precomputation table */
typedef struct ed448_precomp_internal {
    fe448 YplusX;
    fe448 YminusX;
    fe448 T2d;
} ed448_precomp_internal;

typedef ed448_precomp_internal ed448_precomp;

#else

/* Store the affine representation only */
typedef struct ed448_precomp_internal {
    fe448 X;
    fe448 Y;
} ed448_precomp_internal;

typedef point_ed448 ed448_precomp;

#endif /* !FE3C_ED448_SMALL_PRECOMPUTATION */

void ed448_comb_read_precomp(ed448_precomp * r, u8 j, i8 ijt);
void ed448_comb_add_precomp(point_ed448 * r, const point_ed448 * p, const ed448_precomp * q);

extern const ed448_precomp_internal ed448_comb_precomp[57][8];

static inline void ed448_comb_recode_scalar_into_width4_sd(i8 recoding[113], const u8 s[56]) {

    i8 carry = 0;
    for (int i = 0; i < 56; i++) {

        /* From each byte of the scalar extract the two 4-bit digits - note that the exponent array
         * is ordered down columns first (column index changes more slowly) which allows for simpler
         * recoding of the scalar than in Lim/Lee method (where each entry of the recoding table has
         * to be a sum over bits distant by a). */
        recoding[2 * i + 0] = ( (s[i] >> 0) & 0xF ) + carry;
        /* Check if recoding[2 * i + 0] is larger than 2^{w-1} = 8 and if so make recoding[2 * i + 0] negative
         * (subtract 2^w = 16) and propagate the carry to the next digit */
        carry = recoding[2 * i + 0] + 0x8;
        /* Note that carry can only be 0 or 1 at this point */
        carry >>= 4;
        recoding[2 * i + 0] -= carry << 4;

        recoding[2 * i + 1] = ( (s[i] >> 4) & 0xF ) + carry;
        carry = recoding[2 * i + 1] + 0x8;
        carry >>= 4;
        recoding[2 * i + 1] -= carry << 4;
    }
    recoding[112] = carry;
}

#endif /* __POINTS_COMB_COMB_ED448_H */
