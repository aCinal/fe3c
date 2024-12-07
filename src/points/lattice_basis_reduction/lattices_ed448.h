
#ifndef __POINTS_LATTICE_BASIS_REDUCTION_LATTICES_ED448_H
#define __POINTS_LATTICE_BASIS_REDUCTION_LATTICES_ED448_H

#include <global_defs.h>

void ed448_lattice_basis_reduction(u8 *delta_challenge, int *dc_sign, u8 *delta, int *d_sign, u8 *delta_response, const u8 *challenge, const u8 *response);

#endif /* __POINTS_LATTICE_BASIS_REDUCTION_LATTICES_ED448_H */
