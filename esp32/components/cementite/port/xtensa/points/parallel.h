
#ifndef __FE3C_PORT_XTENSA_POINTS_PARALLEL_H
#define __FE3C_PORT_XTENSA_POINTS_PARALLEL_H

#include <fe3c/eddsa.h>
#include <points/points.h>

struct parallel_work {
    void (*func)(void *);
    void *arg;
};

int schedule_parallel_work(struct parallel_work *work);
void wait_for_parallel_work(void);

#endif /* __FE3C_PORT_XTENSA_POINTS_PARALLEL_H */
