#include "parallel.h"
#include <utils/utils.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <freertos/queue.h>

#define WORKER_STACK_SIZE  ( 2 * 1024 )

static StaticSemaphore_t worker_mutex_buffer;
static StaticQueue_t work_queue_buffer;
static parallel_work *work_queue_storage[1];
static StaticSemaphore_t completion_semaphore_buffer;
static StackType_t worker_stack[WORKER_STACK_SIZE];
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

static void worker_body(void *arg)
{
    (void) arg;

    FE3C_SANITY_CHECK(work_queue, NULL);
    FE3C_SANITY_CHECK(completion_semaphore, NULL);

    parallel_work *work;
    for (;;) {

        /* Block indefinitely waiting for work to do */
        (void) xQueueReceive(work_queue, &work, portMAX_DELAY);
        /* Do the work */
        work->func(work->arg);
        /* Signal completion */
        (void) xSemaphoreGive(completion_semaphore);
    }
}

static void worker_lazy_init(void)
{
    portENTER_CRITICAL(&init_spinlock);
    /* Check again if no one initialized the worker while
     * we were spinning */
    if (0 == initialized) {
        /* Create a mutex for serializing access to the worker thread */
        worker_mutex = xSemaphoreCreateMutexStatic(&worker_mutex_buffer);
        /* Create a work queue for the thread */
        work_queue = xQueueCreateStatic(1, sizeof(parallel_work *), (uint8_t *) work_queue_storage, &work_queue_buffer);
        /* Create the semaphore for signalling end of work */
        completion_semaphore = xSemaphoreCreateBinaryStatic(&completion_semaphore_buffer);
        /* Spawn the worker thread */
        worker_thread = xTaskCreateStatic(
            worker_body,
            "fe3c_worker",
            WORKER_STACK_SIZE,
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

int schedule_parallel_work(parallel_work *work)
{
    if (unlikely(0 == initialized))
        worker_lazy_init();

    /* Try acquiring the worker lock. If the worker is busy,
     * fall back to the serial algorithm. */
    if (pdFALSE == xSemaphoreTake(worker_mutex, 0))
        return 0;

    /* Set worker priority equal to ours */
    vTaskPrioritySet(worker_thread, uxTaskPriorityGet(NULL));

    /* Push a work descriptor to the queue */
    (void) xQueueSend(work_queue, &work, 0);

    return 1;
}

void wait_for_parallel_work(void)
{
    /* Block on the completion semaphore until the worker thread completes */
    (void) xSemaphoreTake(completion_semaphore, portMAX_DELAY);
    /* Release the worker mutex */
    (void) xSemaphoreGive(worker_mutex);
}
