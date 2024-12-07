#include "stack.h"
#include <fe3c/eddsa.h>
#include <esp_random.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#define STACK_MEASUREMENT_TASK_STACK_DEPTH  (8 * 1024)
#define STACK_MEASUREMENT_TASK_PRIORITY     (configMAX_PRIORITIES - 4)

static void stack_measurement_task(void *args);

typedef struct stack_measurement_context {
    SemaphoreHandle_t sem;
    eddsa_sign_request *sign_req;
    eddsa_verify_request *verify_req;
} stack_measurement_context;

void stack_dimensioning_for_curve(eddsa_curve curve_id)
{
    /* Synchronously spawn two tasks that shall sign and verify a message and compare
     * stack watermarks before and after the respective calls */

    stack_measurement_context context;

    context.sem = xSemaphoreCreateBinary();
    assert(context.sem);

    u8 message[20];
    /* Generate a random message */
    esp_fill_random(message, sizeof(message));

    u8 secret_key[eddsa_get_secret_key_length(curve_id)];
    /* Generate a random secret key */
    esp_fill_random(secret_key, sizeof(secret_key));

    u8 public_key[eddsa_get_public_key_length(curve_id)];
    /* Derive the corresponding public key */
    eddsa_derive_public_key(public_key, secret_key, curve_id);

    u8 signature[eddsa_get_signature_length(curve_id)];

    eddsa_sign_request sign_req = {
        .signature = signature,
        .message = message,
        .secret_key = secret_key,
        .curve_id = curve_id
    };
    context.sign_req = &sign_req;
    context.verify_req = NULL;

    /* FreeRTOS binary semaphores are initially locked, start the child task and
     * block on the semaphore */

    BaseType_t ret = xTaskCreate(
        stack_measurement_task,
        "signer",
        STACK_MEASUREMENT_TASK_STACK_DEPTH,
        &context,
        STACK_MEASUREMENT_TASK_PRIORITY,
        NULL
    );
    assert(pdPASS == ret);
    /* Wait for the child task to finish */
    (void) xSemaphoreTake(context.sem, portMAX_DELAY);

    eddsa_verify_request verify_req = {
        .message = message,
        .signature = signature,
        .public_key = public_key,
        .curve_id = curve_id
    };

    context.sign_req = NULL;
    context.verify_req = &verify_req;

    ret = xTaskCreate(
        stack_measurement_task,
        "verifier",
        STACK_MEASUREMENT_TASK_STACK_DEPTH,
        &context,
        STACK_MEASUREMENT_TASK_PRIORITY,
        NULL
    );
    assert(pdPASS == ret);
    /* Wait for the child task to finish */
    (void) xSemaphoreTake(context.sem, portMAX_DELAY);

    /* Destroy the semaphore */
    vSemaphoreDelete(context.sem);
}

static void stack_measurement_task(void *args)
{
    stack_measurement_context *context = (stack_measurement_context *) args;

    /* Assert only one request is set */
    assert(NULL == context->sign_req || NULL == context->verify_req);
    /* Assert the semaphore is available */
    assert(context->sem);

    BaseType_t watermark_pre = uxTaskGetStackHighWaterMark(NULL);

    if (context->sign_req) {

        eddsa_sign(context->sign_req);

    } else if (context->verify_req) {

        int ret = eddsa_verify(context->verify_req);
        assert(ret);
    }

    BaseType_t watermark_post = uxTaskGetStackHighWaterMark(NULL);

    /* Note that each watermark denotes the number of unused stack words */
    BaseType_t stack_usage = watermark_pre - watermark_post;

    printf("[+]   %s uses %d words of stack memory (%d bytes)\n", \
        context->sign_req ? "Signing" : "Verification", \
        stack_usage, 4 * stack_usage);

    /* Release the parent task */
    xSemaphoreGive(context->sem);

    /* Kill ourselves */
    vTaskDelete(NULL);
}
