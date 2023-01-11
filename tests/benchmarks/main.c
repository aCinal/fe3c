#include "framework.h"
#include <fe3c/eddsa.h>
#include <unistd.h>
#include <signal.h>
#include <sys/random.h>
#include <time.h>
#include <assert.h>
#include <stdio.h>

static void bench_linux_get_random(u8 * buf, size_t sz);
static long bench_linux_start_stopwatch(unsigned int expiry_time);
static double bench_linux_stop_stopwatch(long start);
static void bench_linux_handle_result(eddsa_curve curve_id, size_t message_length, int verify, int iterations, double elapsed);
static void sigalrm_handler(int signo);

static volatile int s_stop_test;

int main(int argc, char * argv[]) {

    bench_params params;
    params.get_random = bench_linux_get_random;
    params.start_stopwatch = bench_linux_start_stopwatch;
    params.stop_stopwatch = bench_linux_stop_stopwatch;
    params.handle_result = bench_linux_handle_result;
    params.message_length = 20;
    params.context_length = 0;
    params.expiry_time = 10;
    params.stop_flag = &s_stop_test;
    params.cache_public_key = 1;

    printf("--- Running %s with message_length=%d, context_length=%d (expiry_time=%d)\n", \
        argv[0], params.message_length, params.context_length, params.expiry_time);

    /* Install a SIGARLM handler */
    signal(SIGALRM, sigalrm_handler);

    params.curve_id = EDDSA_CURVE_ED25519;
    run_benchmark(&params);

    params.curve_id = EDDSA_CURVE_ED448;
    run_benchmark(&params);

    return 0;
}

static void bench_linux_get_random(u8 * buf, size_t sz) {

    assert(sz == getrandom(buf, sz, 0));
}

static long bench_linux_start_stopwatch(unsigned int expiry_time) {

    static struct timespec now;

    int ret = clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &now);
    assert(ret == 0);

    /* Schedule an alarm */
    alarm(expiry_time);

    return (long) &now;
}

static double bench_linux_stop_stopwatch(long start) {

    struct timespec now;
    /* Get current time */
    int ret = clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &now);
    assert(ret == 0);

    struct timespec * start_struct = (struct timespec *) start;

    /* Convert the timestamps to seconds and compute the time elapsed */
    double nows = now.tv_sec + now.tv_nsec * 1e-9;
    double starts = start_struct->tv_sec + start_struct->tv_nsec * 1e-9;
    double delta = nows - starts;

    /* Disarm any active alarms */
    alarm(0);

    return delta;
}

static void bench_linux_handle_result(eddsa_curve curve_id, size_t message_length, int verify, int iterations, double elapsed) {

    double secs_per_sig = elapsed / iterations;
    double sigs_per_sec = iterations / elapsed;

    printf(
        "%s %s (msg len: %u): %d signatures in %lf s (%lf s/signature, %lf %s)\n", \
        curve_id == EDDSA_CURVE_ED25519 ?
            "Ed25519" :
            "Ed448  ",
        verify ?
            "verification" :
            "signing     ",
        message_length,
        iterations,
        elapsed,
        secs_per_sig,
        sigs_per_sec,
        verify ?
            "verifications/s" :
            "signatures/s"
    );
}

static void sigalrm_handler(int signo) {

    /* Break out of the benchmark loop */
    s_stop_test = 1;
}
