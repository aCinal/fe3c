
#ifndef __BENCHMARKS_FRAMEWORK_H
#define __BENCHMARKS_FRAMEWORK_H

#include <fe3c/eddsa.h>

typedef struct bench_params {
    void (*get_random)(u8 *buf, size_t sz);
    long (*start_stopwatch)(unsigned int expiry_time);
    double (*stop_stopwatch)(long start);
    void (*handle_result)(eddsa_curve curve_id, size_t message_length, int verify, int iterations, double elapsed);
    volatile int *stop_flag;
    unsigned int expiry_time;
    int cache_public_key;
    size_t message_length;
    u8 context_length;
    eddsa_curve curve_id;
} bench_params;

void run_benchmark(const bench_params *params);

#endif /* __BENCHMARKS_FRAMEWORK_H */
