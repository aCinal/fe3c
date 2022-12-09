
#ifndef __FE3C_POINTS_POINTS_ED448_COMB_METHOD_H
#define __FE3C_POINTS_POINTS_ED448_COMB_METHOD_H

#include <field_elements/field_elements_ed448.h>

/* Define the number of blocks (nomenclature from "More Flexible Exponentiation with Precomputation"
 * by Lim & Lee) - this corresponds to the h in the algorithm description in points_ed448.c */
#define ED448_COMB_H  4
/* Define the size of each horizontal layer, i.e. the a in the algorithm description */
#define ED448_COMB_A  112
/* Define the number of subblocks in a horizontal layer, i.e. the v in the algorithm description */
#define ED448_COMB_V  56
/* Define the width of each subblock, i.e. the b in the algorithm description */
#define ED448_COMB_B  2

/* Store only affine representations in the precomputation table */
typedef struct point_precomp {
    fe448 X;
    fe448 Y;
} point_precomp;

/* Define the precomputation table, for readability it is kept in a separate file */
static const point_precomp ed448_comb_precomp[][(1 << ED448_COMB_H) - 1] = {
#if FE3C_32BIT
    #include <points/precomputation/precomputation_ed448_32.h>
#else
    #include <points/precomputation/precomputation_ed448_64.h>
#endif /* FE3C_32BIT */
};
/* Let the compiler allocate the array according to the initializer and then assert that the size is correct */
FE3C_COMPILE_TIME_SANITY_CHECK( sizeof(ed448_comb_precomp) == ED448_COMB_V * ( (1 << ED448_COMB_H) - 1 ) * sizeof(point_precomp) );

#define DECLARE_SCALAR_RECODING(name) \
    u8 name[ED448_COMB_V][ED448_COMB_B]

#define read_bit(scalar, bit) ({ \
    int __bytes = (bit) >> 3; \
    int __bits = (bit) & 0x7; \
    1 & ((scalar)[__bytes] >> __bits); \
})

static inline void ed448_comb_recode_scalar(DECLARE_SCALAR_RECODING(recoding), const u8 * scalar) {

    /* Iterate over the columns (along the rows) of the exponent array (nomenclature from "Handbook of Applied Cryptography") */
    for (int j = 0; j < ED448_COMB_V; j++) {

        /* We unroll the loops to reduce indentation and maintain code readability, but
         * for that we need an assertion that the comb parameters are correct */
        FE3C_COMPILE_TIME_SANITY_CHECK(ED448_COMB_B == 2);
        FE3C_COMPILE_TIME_SANITY_CHECK(ED448_COMB_H == 4);

        /* Iterate over the bits in each vertical subblock and for each 0 <= k < ED448_COMB_B,
         * iterate over the layers of the exponent array, i.e. down a column (0 <= i < ED448_COMB_H) */
        recoding[j][0]  = read_bit(scalar, 0 * ED448_COMB_A + j * ED448_COMB_B + 0) << 0;
        recoding[j][0] |= read_bit(scalar, 1 * ED448_COMB_A + j * ED448_COMB_B + 0) << 1;
        recoding[j][0] |= read_bit(scalar, 2 * ED448_COMB_A + j * ED448_COMB_B + 0) << 2;
        recoding[j][0] |= read_bit(scalar, 3 * ED448_COMB_A + j * ED448_COMB_B + 0) << 3;

        /* Iterate over the layers of the exponent array (down a column) */
        recoding[j][1]  = read_bit(scalar, 0 * ED448_COMB_A + j * ED448_COMB_B + 1) << 0;
        recoding[j][1] |= read_bit(scalar, 1 * ED448_COMB_A + j * ED448_COMB_B + 1) << 1;
        recoding[j][1] |= read_bit(scalar, 2 * ED448_COMB_A + j * ED448_COMB_B + 1) << 2;
        recoding[j][1] |= read_bit(scalar, 3 * ED448_COMB_A + j * ED448_COMB_B + 1) << 3;
    }
}

#define equal(x, y)  ({ \
    u8 __aux = (x ^ y); \
    __aux |= (__aux >> 4); \
    __aux |= (__aux >> 2); \
    __aux |= (__aux >> 1); \
    1 & (__aux ^ 1); \
})

#define point_conditional_move(p, ijk, k) ({ \
    u8 __move = equal((ijk), k); \
    fe_conditional_move(&p->X, (fe *) &ed448_comb_precomp[j][k - 1].X, __move); \
    fe_conditional_move(&p->Y, (fe *) &ed448_comb_precomp[j][k - 1].Y, __move); \
})

static inline void ed448_comb_read_precomp(point * r, u8 j, u8 ijk) {

    FE3C_SANITY_CHECK( j < ED448_COMB_V );
    FE3C_SANITY_CHECK( ijk < (1 << ED448_COMB_H) );

    /* Start with the identity - if ijk is 0 then we leave the result as the identity */
    r->X = fe_zero;
    r->Y = fe_one;
    /* Set the Z coordinate to one for good */
    r->Z = fe_one;

    /* Choose one entry of the precomputation table in a branchless manner
     * - an added advantage is that we access all elements in a given row
     * (for a given subblock j) thus preventing cache-based timing attacks. */
    point_conditional_move(r, ijk, 1);
    point_conditional_move(r, ijk, 2);
    point_conditional_move(r, ijk, 3);
    point_conditional_move(r, ijk, 4);
    point_conditional_move(r, ijk, 5);
    point_conditional_move(r, ijk, 6);
    point_conditional_move(r, ijk, 7);
    point_conditional_move(r, ijk, 8);
    point_conditional_move(r, ijk, 9);
    point_conditional_move(r, ijk, 10);
    point_conditional_move(r, ijk, 11);
    point_conditional_move(r, ijk, 12);
    point_conditional_move(r, ijk, 13);
    point_conditional_move(r, ijk, 14);
    point_conditional_move(r, ijk, 15);
}

#endif /* __FE3C_POINTS_POINTS_ED448_COMB_METHOD_H */
