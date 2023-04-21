
#ifndef __FE3C_PORT_POINTS_COMB_COMB_PARALLEL_ED448_H
#define __FE3C_PORT_POINTS_COMB_COMB_PARALLEL_ED448_H

#include <points/points.h>

void ed448_comb_loop(point_ed448 * result, const i8 * scalar_recoding, int odd);

#endif /* __FE3C_PORT_POINTS_COMB_COMB_PARALLEL_ED448_H */
