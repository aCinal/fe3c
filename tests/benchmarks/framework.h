
#ifndef __BENCHMARKS_FRAMEWORK_H
#define __BENCHMARKS_FRAMEWORK_H

#include <fe3c/eddsa.h>

typedef void   (* callback_get_random)       (u8 * buf, size_t sz);
typedef long   (* callback_start_stopwatch)  (unsigned int expiry_time);
typedef double (* callback_stop_stopwatch)   (long start);
typedef void   (* callback_handle_result)    (eddsa_curve curve_id, size_t message_length, int verify, int iterations, double elapsed);

typedef struct bench_params {
    eddsa_curve curve_id;
    size_t message_length;
    u8 context_length;
    unsigned int expiry_time;
    volatile int * stop_flag;
    callback_get_random get_random;
    callback_start_stopwatch start_stopwatch;
    callback_stop_stopwatch stop_stopwatch;
    callback_handle_result handle_result;
} bench_params;

void run_benchmark(const bench_params * params);

#endif /* __BENCHMARKS_FRAMEWORK_H */
