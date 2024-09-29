#include <framework.h>
#include <fe3c/eddsa.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_random.h>
#include <esp_timer.h>
#include "stack.h"

#define BENCHMARK_TASK_STACK_DEPTH  (8 * 1024)
#define BENCHMARK_TASK_PRIORITY     (configMAX_PRIORITIES - 4)

static void benchmark_task(void * arg);
static void bench_esp32_get_random(u8 * buf, size_t sz);
static long bench_esp32_start_stopwatch(unsigned int expiry_time);
static double bench_esp32_stop_stopwatch(long start);
static void bench_esp32_handle_result(eddsa_curve curve_id, size_t message_length, int verify, int iterations, double elapsed);
static void timer_callback(void * arg);

static volatile int s_stop_test;

typedef struct esp32_bench_stopwatch {
    uint64_t start_tick;
    esp_timer_handle_t timer_handle;
} esp32_bench_stopwatch;

void app_main(void) {

    /* Spawn a new task to have strict control over the stack size */
    BaseType_t ret = xTaskCreate(
        benchmark_task,
        "benchmark",
        BENCHMARK_TASK_STACK_DEPTH,
        NULL,
        BENCHMARK_TASK_PRIORITY,
        NULL
    );
    assert(pdPASS == ret);
}

static void benchmark_task(void * arg) {

    (void) arg;

    /* Measure stack usage */
    if (eddsa_is_curve_supported(EDDSA_CURVE_ED25519)) {

        printf("[+] Running stack dimensioning for curve Ed25519...\n");
        stack_dimensioning_for_curve(EDDSA_CURVE_ED25519);

    } else {

        printf("[-] Curve Ed25519 is not supported in the current build. Skipping stack dimensioning...\n");
    }

    if (eddsa_is_curve_supported(EDDSA_CURVE_ED448)) {

        printf("[+] Running stack dimensioning for curve Ed448...\n");
        stack_dimensioning_for_curve(EDDSA_CURVE_ED448);

    } else {

        printf("[-] Curve Ed448 is not supported in the current build. Skipping stack dimensioning...\n");
    }

    bench_params params;
    params.get_random = bench_esp32_get_random;
    params.start_stopwatch = bench_esp32_start_stopwatch;
    params.stop_stopwatch = bench_esp32_stop_stopwatch;
    params.handle_result = bench_esp32_handle_result;
    params.message_length = 20;
    params.context_length = 0;
    params.expiry_time = 10;
    params.stop_flag = &s_stop_test;
    params.cache_public_key = 1;

    if (eddsa_is_curve_supported(EDDSA_CURVE_ED25519)) {

        printf("[+] Running Ed25519 benchmark with message_length=%d, context_length=%d (expiry_time=%d)\n", \
            params.message_length, params.context_length, params.expiry_time);

        params.curve_id = EDDSA_CURVE_ED25519;
        run_benchmark(&params);

    } else {

        printf("[-] Curve Ed25519 is not supported in the current build. Skipping benchmarking...\n");
    }

    if (eddsa_is_curve_supported(EDDSA_CURVE_ED448)) {

        printf("[+] Running Ed448 benchmark with message_length=%d, context_length=%d (expiry_time=%d)\n", \
            params.message_length, params.context_length, params.expiry_time);

        params.curve_id = EDDSA_CURVE_ED448;
        run_benchmark(&params);

    } else {

        printf("[-] Curve Ed448 is not supported in the current build. Skipping benchmarking...\n");
    }

    printf("[+] Benchmarking complete\n");

    /* Our job here is done, kill ourselves */
    vTaskDelete(NULL);
}

static void bench_esp32_get_random(u8 * buf, size_t sz) {

    esp_fill_random(buf, sz);
}

static long bench_esp32_start_stopwatch(unsigned int expiry_time) {

    static esp32_bench_stopwatch stopwatch;

    /* Create a timer */
    esp_timer_create_args_t timer_args;
    timer_args.callback = timer_callback;
    timer_args.dispatch_method = ESP_TIMER_TASK;
    timer_args.arg = NULL;
    timer_args.name = "bench_timer";
    timer_args.skip_unhandled_events = true;
    esp_err_t err = esp_timer_create(&timer_args, &stopwatch.timer_handle);
    assert(ESP_OK == err);

    /* Get current time */
    stopwatch.start_tick = (uint64_t) esp_timer_get_time();

    /* Arm the timer */
    err = esp_timer_start_once(stopwatch.timer_handle, expiry_time * 1000 * 1000);
    assert(ESP_OK == err);

    return (long) &stopwatch;
}

static double bench_esp32_stop_stopwatch(long start) {

    uint64_t end_tick = (uint64_t) esp_timer_get_time();
    esp32_bench_stopwatch * stopwatch = (esp32_bench_stopwatch *) start;
    uint64_t start_tick = stopwatch->start_tick;

    /* Disarm any active timers */
    (void) esp_timer_stop(stopwatch->timer_handle);
    esp_err_t err = esp_timer_delete(stopwatch->timer_handle);
    assert(ESP_OK == err);

    /* Convert from microseconds to seconds */
    return ((double) end_tick - start_tick) * 1e-6;
}

static void bench_esp32_handle_result(eddsa_curve curve_id, size_t message_length, int verify, int iterations, double elapsed) {

    double secs_per_sig = elapsed / iterations;
    double sigs_per_sec = iterations / elapsed;

    /* Print the results via UART */
    printf(
        "[+]   %s %s (msg len: %u): %d signatures in %lf s (%lf s/signature, %lf %s)\n", \
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

static void timer_callback(void * arg) {

    (void) arg;
    /* Break out of the benchmark loop */
    s_stop_test = 1;
}
