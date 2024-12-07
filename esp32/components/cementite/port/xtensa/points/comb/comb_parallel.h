

#ifndef __FE3C_PORT_XTENSA_POINTS_COMB_COMB_PARALLEL_H
#define __FE3C_PORT_XTENSA_POINTS_COMB_COMB_PARALLEL_H

#include <fe3c/eddsa.h>
#include <points/points.h>

typedef struct comb_thread_work {
    point *thread_result;
    const i8 *scalar_recoding;
    eddsa_curve curve_id;
} comb_thread_work;

int comb_dispatch_loop_to_thread(comb_thread_work *work);
void comb_join_worker_thread(void);

#endif /* __FE3C_PORT_XTENSA_POINTS_COMB_COMB_PARALLEL_H */
