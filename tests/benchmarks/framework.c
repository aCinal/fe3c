#include "framework.h"
#include <fe3c/eddsa.h>
#include <assert.h>
#include <limits.h>

void run_benchmark(const bench_params * params) {

    int iterations;
    double elapsed;
    long stopwatch_start;
    eddsa_curve curve_id = params->curve_id;

    /* Produce a random secret key */
    u8 secret_key[eddsa_get_secret_key_length(curve_id)];
    params->get_random(secret_key, sizeof(secret_key));

    /* Recover the corresponding public key */
    u8 public_key[eddsa_get_public_key_length(curve_id)];
    eddsa_derive_public_key(public_key, secret_key, curve_id);

    /* Generate a random message */
    size_t message_length = params->message_length;
    u8 message[message_length];
    params->get_random(message, sizeof(message));

    /* Generate a random context */
    u8 context_length = params->context_length;
    u8 context[context_length];
    params->get_random(context, sizeof(context));

    u8 signature[eddsa_get_signature_length(curve_id)];

    eddsa_sign_request sign_req = {
        .signature = signature,
        .message = message,
        .secret_key = secret_key,
        .context = context,
        .message_length = message_length,
        .context_length = context_length,
        .curve_id = curve_id
    };
    *params->stop_flag = 0;
    stopwatch_start = params->start_stopwatch(params->expiry_time);
    iterations = 0;
    while (*params->stop_flag == 0 && iterations < INT_MAX) {

        eddsa_sign(&sign_req);
        iterations++;
    }
    elapsed = params->stop_stopwatch(stopwatch_start);
    params->handle_result(curve_id, message_length, 0, iterations, elapsed);


    eddsa_verify_request verify_req = {
        .signature = signature,
        .message = message,
        .public_key = public_key,
        .context = context,
        .message_length = message_length,
        .context_length = context_length,
        .curve_id = curve_id
    };
    *params->stop_flag = 0;
    stopwatch_start = params->start_stopwatch(params->expiry_time);
    iterations = 0;
    while (*params->stop_flag == 0 && iterations < INT_MAX) {

        int ok = eddsa_verify(&verify_req);
        assert(ok);
        iterations++;
    }
    elapsed = params->stop_stopwatch(stopwatch_start);
    params->handle_result(curve_id, message_length, 1, iterations, elapsed);
}
