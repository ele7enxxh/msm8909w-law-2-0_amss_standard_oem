#include <assert.h>
#include "passfail.h"
#include <qurt.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>
#include <qurt_timer.h>

#define NUM_PEEKS            5
#define EXIT_STATUS_ERROR    -1

#define TF_YIELD()
#define TF_ASSERT(x)    fail_if(!(x))
#define TF_MSG    printf
#define TF_SLEEP(x)     qurt_timer_sleep(1000 * (x))
#define TF_USLEEP(x)    qurt_timer_sleep(x)

// Global variable declarations
static pthread_attr_t thread_attr[1];
static pthread_t      thread[1];
static unsigned int   shared_resource = 0;
static sem_t          test_sem;
static sem_t          test_sem2;

// QTF testcases' helper function definitions
static void *thread_entry_fn(void *unused)
{
    int status;

    unused = unused;
    // Loop forever
    while (1)
    {
        // Gotta have a sleep so our thread can be deleted
        TF_YIELD();

        // Wait on the Semaphore
        status = sem_wait(&test_sem2);
        TF_ASSERT(status == EOK);
        TF_ASSERT(errno == 0);
        // Access shared resource
        shared_resource++;
        // Post on the Semaphore
        status = sem_post(&test_sem);
        TF_ASSERT(status == EOK);
    }

    pthread_exit((void*) EXIT_STATUS_ERROR);
    return NULL;
}

int test_posix_semaphore()
{
    int status;
    int prev_val;
    int i;

    // Setup the Semaphore;
    status = sem_init(&test_sem, 0, 1);
    TF_ASSERT(status == 0);

    status = sem_init(&test_sem2, 0, 0);
    TF_ASSERT(status == 0);

    status = sem_post(&test_sem);
    TF_ASSERT(status == 0);
    status = sem_post(&test_sem);
    TF_ASSERT(status == 0);
    status = sem_post(&test_sem);
    TF_ASSERT(status == 0);

    status = sem_getvalue(&test_sem, &prev_val);
    TF_ASSERT(status == 0);
    TF_ASSERT(prev_val == 4);    

    status = sem_wait(&test_sem);
    TF_ASSERT(status == 0);

    status = sem_getvalue(&test_sem, &prev_val);
    TF_ASSERT(status == 0);
    TF_ASSERT(prev_val == 3);    

    status = sem_wait(&test_sem);
    TF_ASSERT(status == 0);

    status = sem_getvalue(&test_sem, &prev_val);
    TF_ASSERT(status == 0);
    TF_ASSERT(prev_val == 2);    

    status = sem_wait(&test_sem);
    TF_ASSERT(status == 0);

    status = sem_getvalue(&test_sem, &prev_val);
    TF_ASSERT(status == 0);
    TF_ASSERT(prev_val == 1);    

    // Setup the thread attributes
    status = pthread_attr_init(&thread_attr[0]);
    TF_ASSERT(status == EOK);

    // Create the thread
    pthread_attr_setthreadname(&thread_attr[0], "sem_test");
    status = pthread_create(&thread[0], &thread_attr[0], thread_entry_fn, NULL);
    TF_ASSERT(status == EOK);

    // TEST

    // Verify that the thread is incrementing the shared_resource
    prev_val = 0;
    TF_MSG("Checking for incrementing shared_resource");
    for (i = 0; i < NUM_PEEKS; i++)
    {
        TF_YIELD(); TF_YIELD(); TF_YIELD();

        TF_MSG("Wait on semaphore");
        status = sem_wait(&test_sem);
        TF_ASSERT(status == EOK);

        TF_MSG("  prev_val(%d), shared_resource(%d)", prev_val, shared_resource);
//    TF_ASSERT( prev_val != shared_resource );
//    prev_val = shared_resource;

        TF_SLEEP(1);

        TF_MSG("Post on semaphore");
        status = sem_post(&test_sem2);
        TF_ASSERT(status == EOK);
    }

#if 0
    // Grab the mutex and make sure that the shared_resource stops moving
    TF_MSG("Checking for shared_resource stop");
    TF_MSG("Wait on semaphore");
    status = sem_wait(&test_sem);
    TF_ASSERT(status == EOK);
    prev_val = shared_resource;
    for (i = 0; i < NUM_PEEKS; i++)
    {
        TF_YIELD();
        TF_MSG("  prev_val(%d), shared_resource(%d)", prev_val, shared_resource);
        TF_ASSERT(prev_val == shared_resource);
    }
    TF_MSG("Post on semaphore");
    status = sem_post(&test_sem);
    TF_ASSERT(status == EOK);

    // Now verify that it's moving again
    prev_val = 0;
    for (i = 0; i < NUM_PEEKS; i++)
    {
        TF_YIELD(); TF_YIELD(); TF_YIELD();

        TF_MSG("Wait on semaphore");
        status = sem_wait(&test_sem);
        TF_ASSERT(status == EOK);

        TF_MSG("  prev_val(%d), shared_resource(%d)", prev_val, shared_resource);
        TF_ASSERT(prev_val != shared_resource);
        prev_val = shared_resource;

        TF_MSG("Post on semaphore");
        status = sem_post(&test_sem);
        TF_ASSERT(status == EOK);
    }
#endif

    //teardown
    TF_SLEEP(1);

    // Delete the Semaphore
    status = sem_destroy(&test_sem);
    TF_ASSERT(status == 0);

    status = sem_destroy(&test_sem2);
    TF_ASSERT(status == 0);

    // Kill the thread
    status = pthread_kill(thread[0], SIGKILL);
    TF_ASSERT(status == EOK);
}

