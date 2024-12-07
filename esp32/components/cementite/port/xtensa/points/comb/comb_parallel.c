#include "comb_parallel.h"
#include <utils/utils.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <freertos/queue.h>

#if FE3C_SUPPORT_CURVE_ED25519
    #include "comb_parallel_ed25519.h"
#endif /* FE3C_SUPPORT_CURVE_ED25519 */

#if FE3C_SUPPORT_CURVE_ED448
    #include "comb_parallel_ed448.h"
#endif /* FE3C_SUPPORT_CURVE_ED448 */

#define COMB_THREAD_STACK_SIZE  ( 2 * 1024 )

static StaticSemaphore_t worker_mutex_buffer;
static StaticQueue_t work_queue_buffer;
static comb_thread_work *work_queue_storage[1];
static StaticSemaphore_t completion_semaphore_buffer;
static StackType_t worker_stack[COMB_THREAD_STACK_SIZE];
static StaticTask_t worker_tcb;

static SemaphoreHandle_t worker_mutex;
static QueueHandle_t work_queue;
static SemaphoreHandle_t completion_semaphore;
static TaskHandle_t worker_thread;

/* Note that we do lazy initialization of the structures above and so
 * we need a statically-initialized synchronization primitive to prevent
 * double initialization */
static portMUX_TYPE init_spinlock = portMUX_INITIALIZER_UNLOCKED;
static int initialized = 0;

static void comb_lazy_init(void);
static void comb_worker_body(void *arg);

int comb_dispatch_loop_to_thread(comb_thread_work *work)
{
    if (unlikely(0 == initialized))
        comb_lazy_init();

    /* Try acquiring the worker lock */
    if (pdFALSE == xSemaphoreTake(worker_mutex, 0)) {

        /* Worker busy, fall back to the serial algorithm */
        return 0;
    }

    /* Set worker priority equal to ours */
    vTaskPrioritySet(worker_thread, uxTaskPriorityGet(NULL));

    /* Push a work descriptor to the queue */
    (void) xQueueSend(work_queue, &work, 0);

    return 1;
}

void comb_join_worker_thread(void)
{
    /* Block on the completion semaphore until the worker thread completes */
    (void) xSemaphoreTake(completion_semaphore, portMAX_DELAY);
    /* Release the worker mutex */
    (void) xSemaphoreGive(worker_mutex);
}

static void comb_lazy_init(void)
{
    portENTER_CRITICAL(&init_spinlock);
    /* Check again if no one initialized the structures while
     * we were spinning */
    if (0 == initialized) {
        /* Create a mutex for serializing access to the worker thread */
        worker_mutex = xSemaphoreCreateMutexStatic(&worker_mutex_buffer);
        /* Create a work queue for the thread */
        work_queue = xQueueCreateStatic(1, sizeof(comb_thread_work *), (uint8_t *) work_queue_storage, &work_queue_buffer);
        /* Create the semaphore for signalling end of work */
        completion_semaphore = xSemaphoreCreateBinaryStatic(&completion_semaphore_buffer);
        /* Spawn the worker thread */
        worker_thread = xTaskCreateStatic(
            comb_worker_body,
            "fe3c_worker",
            COMB_THREAD_STACK_SIZE,
            NULL,
            uxTaskPriorityGet(NULL),
            worker_stack,
            &worker_tcb
        );
        /* Set the flag to indicate successful initialization */
        initialized = 1;
    }
    portEXIT_CRITICAL(&init_spinlock);
}

static void comb_worker_body(void *arg)
{
    (void) arg;

    FE3C_SANITY_CHECK(work_queue, NULL);
    FE3C_SANITY_CHECK(completion_semaphore, NULL);

    comb_thread_work *work;

    for (;;) {

        /* Block indefinitely waiting for work to do */
        (void) xQueueReceive(work_queue, &work, portMAX_DELAY);

#if FE3C_SUPPORT_CURVE_ED25519
        if (EDDSA_CURVE_ED25519 == work->curve_id) {

            /* Iterate over the even indices in the recoding and produce a partial result */
            ed25519_comb_loop((point_ed25519 *) work->thread_result, work->scalar_recoding, 0);
        }
#endif /* FE3C_SUPPORT_CURVE_ED25519 */

#if FE3C_SUPPORT_CURVE_ED448
        if (EDDSA_CURVE_ED448 == work->curve_id) {

            /* Iterate over the even indices in the recoding and produce a partial result */
            ed448_comb_loop((point_ed448 *) work->thread_result, work->scalar_recoding, 0);
        }
#endif /* FE3C_SUPPORT_CURVE_ED448 */

        /* Signal completion of the work */
        (void) xSemaphoreGive(completion_semaphore);
    }
}
