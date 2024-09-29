
#ifndef __FE3C_PORT_XTENSA_POINTS_COMB_COMB_PARALLEL_ED25519_H
#define __FE3C_PORT_XTENSA_POINTS_COMB_COMB_PARALLEL_ED25519_H

#include <points/points.h>

void ed25519_comb_loop(point_ed25519 * result, const i8 * scalar_recoding, int odd);

#endif /* __FE3C_PORT_XTENSA_POINTS_COMB_COMB_PARALLEL_ED25519_H */
