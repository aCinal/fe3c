
#ifndef __FE3C_PORT_XTENSA_POINTS_PARALLEL_H
#define __FE3C_PORT_XTENSA_POINTS_PARALLEL_H

#include <fe3c/eddsa.h>
#include <points/points.h>

typedef struct parallel_work {
    void (*func)(void *);
    void *arg;
} parallel_work;

int schedule_parallel_work(parallel_work *work);
void wait_for_parallel_work(void);

#endif /* __FE3C_PORT_XTENSA_POINTS_PARALLEL_H */
