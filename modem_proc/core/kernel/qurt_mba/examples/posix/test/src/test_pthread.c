#include <assert.h>
#include "passfail.h"
#include <qurt.h>
#include <pthread.h>
#include <semaphore.h>
#include <qurt_timer.h>

#define TF_YIELD()      pthread_yield()
#define TF_ASSERT(x)    fail_if(!(x))
#define TF_MSG    printf
#define TF_SLEEP(x)     qurt_timer_sleep(1000 * (x))
#define TF_USLEEP(x)    qurt_timer_sleep(x)

extern void qurt_timer_init(void);
extern int test_posix_semaphore();
extern void posix_test_mq_open();
extern void posix_test_mq_send();
extern void posix_test_mq_00();
extern void posix_test_mq_01();
extern void posix_test_mq_02();
extern void posix_test_mq_03();
extern void posix_test_mq_realtime();
extern void posix_test_mq_audio();

extern int test_posix_mutex();
extern void posix_test_signal();
extern void test_posix_timer();

int key_data[32]  = {0};
int key[32];

void tls_destructor (void *data)
{
   int key_index;

   key_index = *(int *)data - 1;
   qurt_tls_set_specific (key[key_index], NULL);
}

void* tls_test_thread(void *arg)
{
    int i;
    int exit_status = 100;
    int status;
    int value;

    TF_MSG ("tls_test_thread is called.\n");

    /* Leave 1 for QUBE and 1 for C library */
    for (i = 0; i < 30; i++) {
        status = qurt_tls_create_key (&key[i], tls_destructor);
        TF_ASSERT(status == EOK);
        key_data[i] = i + 1;
    }

    for (i = 0; i < 30; i++) {
        status = qurt_tls_set_specific (key[i], (&key_data[i]));
        TF_ASSERT(status == EOK);
    }

    for (i = 0; i < 30; i++) {
        value = *(int *)qurt_tls_get_specific (key[i]);
        TF_ASSERT(value == (i + 1));
    }

 #if 0
    for (i = 0; i < 30; i++) {
       status = qurt_tls_key_delete (key[i]);
       if (status != EOK)
          failnow ("Key delete fail for ite %d ret val %d", i, status);
    }
 #endif /* 0 */

    TF_MSG ("tls_test_thread exit.\n");

    pthread_exit((void*) exit_status);
}

int posix_test_tls(void)
{
    int            i;
    int            status;
    int            tmp           = 0;
    int            * exit_status = &tmp;
    pthread_t      t1;
    pthread_attr_t thread_attr;

    TF_MSG("POSIX TLS Test Started \n");


    /* First Thread */
    status = pthread_attr_init(&thread_attr);
    TF_ASSERT(status == 0);
    pthread_attr_setthreadname(&thread_attr, "tls_test");

    status = pthread_create(&t1, &thread_attr, tls_test_thread, 0);
    TF_ASSERT(status == 0);

    TF_MSG("POSIX TLS Test wait for join.\n");

    status = pthread_join(t1, (void**) &exit_status);
    TF_ASSERT(status == 0);
    TF_ASSERT(exit_status == (void*) 100);

    TF_MSG("test_tls passed\n");
}

int test_posix_mutex(void)
{
    int                 status;
    pthread_mutexattr_t mutex_attr;
    pthread_mutex_t     test_mutex;

    /* test default (recursive) mutex */
    TF_ASSERT(pthread_mutex_init(&test_mutex, NULL) == EOK);

    // fail, unlock with no lock  (failed, unlock not holder)
    //TF_ASSERT( pthread_mutex_unlock( &test_mutex ) != EOK );

    // success, lock mutex
    status = pthread_mutex_lock(&test_mutex);
    TF_ASSERT(status == EOK);
    // success, trylock mutex
    status = pthread_mutex_trylock(&test_mutex);
    TF_ASSERT(status == EOK);                     // it's the same thread, it locks again
    // success, unlock mutex
    status = pthread_mutex_unlock(&test_mutex);   // unlock - trylock
    TF_ASSERT(status == EOK);
    // success, unlock mutex
    status = pthread_mutex_unlock(&test_mutex);   // unlock - lock
    TF_ASSERT(status == EOK);
    // success, trylock mutex
    status = pthread_mutex_trylock(&test_mutex);
    TF_ASSERT(status == EOK);
    // unlock mutex
    status = pthread_mutex_unlock(&test_mutex);
    TF_ASSERT(status == EOK);

    TF_ASSERT(pthread_mutex_destroy(&test_mutex) == EOK);

    // fail lock, mutex invalid
    TF_ASSERT(pthread_mutex_lock(NULL) != EOK);

    /* test non-recursive mutex */
    TF_ASSERT(pthread_mutexattr_init(&mutex_attr) == EOK);
    TF_ASSERT(pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_NORMAL) == EOK);
    TF_ASSERT(pthread_mutex_init(&test_mutex, &mutex_attr) == EOK);

    // fail, unlock with no lock  (failed, unlock not holder)
    //TF_ASSERT( pthread_mutex_unlock( &test_mutex ) != EOK );

    // success, lock mutex
    status = pthread_mutex_lock(&test_mutex);
    TF_ASSERT(status == EOK);
    // success, trylock mutex
    status = pthread_mutex_trylock(&test_mutex);
    TF_ASSERT(status != EOK);    // since it's not recursive, it returns error

    // success, unlock mutex
    status = pthread_mutex_unlock(&test_mutex);   // unlock - lock
    TF_ASSERT(status == EOK);
    // success, trylock mutex
    status = pthread_mutex_trylock(&test_mutex);
    TF_ASSERT(status == EOK);
    // unlock mutex
    status = pthread_mutex_unlock(&test_mutex);
    TF_ASSERT(status == EOK);

    TF_ASSERT(pthread_mutex_destroy(&test_mutex) == EOK);

    return 0;
}

void test_pthread_prio()
{
    struct sched_param param;
    int                policy;
    int                status;
    int                orig_pri;

    orig_pri = PTHREAD_DEFAULT_PRIORITY;

    status = pthread_getschedparam(pthread_self(), &policy, &param);
    TF_ASSERT(status == 0);
    TF_ASSERT(param.sched_priority == orig_pri);

    param.sched_priority = 100;
    status               = pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
    TF_ASSERT(status == 0);

    status = pthread_getschedparam(pthread_self(), &policy, &param);
    TF_ASSERT(status == 0);
    TF_ASSERT(param.sched_priority == 100);

    status = pthread_setschedprio(pthread_self(), 102);
    TF_ASSERT(status == 0);

    status = pthread_getschedparam(pthread_self(), &policy, &param);
    TF_ASSERT(status == 0);
    TF_ASSERT(param.sched_priority == 102);
}

void* ThreadMain1(void *arg)
{
    int i;
    int exit_status = 100;

    TF_MSG("\n testing test_pthread_prio\n");
    test_pthread_prio();

    for (i = 0; i < 10; i++)
    {
        TF_MSG(" Starting Thread 1, id=%d \n", pthread_self());
    }

    TF_MSG("\n testing posix_test_mq_audio\n");
    posix_test_mq_audio();
    TF_YIELD();
    TF_MSG("\n testing posix_test_mq_open\n");
    posix_test_mq_open();
    TF_YIELD();
    TF_MSG("\n testing posix_test_mq_send\n");
    posix_test_mq_send();
    TF_YIELD();
    TF_MSG("\n testing posix_test_mq_00\n");
    posix_test_mq_00();
    TF_MSG("\n testing posix_test_mq_01\n");
    posix_test_mq_01();
    TF_MSG("\n testing posix_test_mq_02\n");
    posix_test_mq_02();
    TF_MSG("\n testing posix_test_mq_03\n");
    posix_test_mq_03();
    TF_MSG("\n testing posix_test_mq_realtime\n");
    posix_test_mq_realtime();

    //pthread_exit((void*) exit_status);
    return (void*) exit_status;
}

void* ThreadMain2(void *arg)
{
    int exit_status = 101;

    TF_MSG(" Starting Thread 2, id=%d \n", pthread_self());
    pthread_exit((void*) exit_status);
}

volatile int dummy;
void * test_thread (void* arg)
{
    int j;
    int exit_status = (int)arg;

    for(j=0; j<1000; j++)
        dummy = j;

    TF_MSG("test thread %d exit\n", (int)arg);
    pthread_exit((void*) exit_status);
}

pthread_t pthread_handle;
void test_posix_thread_create_start(int index)
{
    pthread_attr_t      thread_attr; /* Thread attribute */
    struct sched_param  param;       /* Thread parameters */
    int status;

    status = pthread_attr_init(&thread_attr);
    TF_ASSERT( status == EOK );
    status = pthread_attr_setthreadname(&thread_attr, "c2kAppCfg");
    TF_ASSERT( status == EOK );
    param.sched_priority = 1;
    status = pthread_attr_setschedparam(&thread_attr, &param);
    TF_ASSERT( status == EOK );
    status = pthread_create(&pthread_handle, &thread_attr, test_thread , (void*)index);
    TF_ASSERT( status == EOK );
    TF_MSG("test thread %d was created\n", index);
}

void test_posix_thread_create_stop(int index)
{
    void * status;
    int ret;
    ret = pthread_join(pthread_handle, &status);
    TF_ASSERT( ret == EOK );
    TF_ASSERT( index == (int)status);
}

int test_posix_thread_create(void)
{
    int i;
    for(i=0; i<3000; i++)
    {
        TF_MSG("cycle %d\n", i);
        test_posix_thread_create_start(i);
        test_posix_thread_create_stop(i);
    }
}

int test_posix_pthread(void)
{
    int            i;
    int            status;
    int            tmp           = 0;
    int            * exit_status = &tmp;
    pthread_t      t1, t2;
    pthread_attr_t thread_attr;
    cpu_set_t      cpuset = CONFIG_NR_CPUS;

    TF_MSG("POSIX pthread Test Started \n");


    /* First Thread */
    status = pthread_attr_init(&thread_attr);
    TF_ASSERT(status == 0);
    pthread_attr_setthreadname(&thread_attr, "thrd_test1");
    pthread_attr_settimetestid(&thread_attr, 0x1234);
    pthread_attr_setaffinity_np(&thread_attr, sizeof(cpu_set_t), &cpuset);

    status = pthread_create(&t1, &thread_attr, ThreadMain1, 0);
    TF_ASSERT(status == 0);

    /* Second Thread */
    pthread_attr_setthreadname(&thread_attr, "thrd_test2");
    pthread_attr_settimetestid(&thread_attr, 0x1235);
    pthread_attr_setaffinity_np(&thread_attr, sizeof(cpu_set_t), &cpuset);
    status = pthread_create(&t2, &thread_attr, ThreadMain2, 0);
    TF_ASSERT(status == 0);

    status = pthread_join(t1, (void**) &exit_status);
    TF_ASSERT(status == 0);
    TF_ASSERT(exit_status == (void*) 100);

    TF_SLEEP(1);

    status = pthread_join(t2, (void**) &exit_status);
    TF_ASSERT(status == 0);
    TF_ASSERT(exit_status == (void*) 101);

    //pthread_kill( t1, SIGKILL );
    //status = pthread_kill( t2, SIGKILL );
    //TF_MSG ("%d\n", status);
    //TF_ASSERT(status == 0);

    test_posix_thread_create();

    TF_MSG("test_pthread passed\n");
}

#define NUM_THREADS              5
#define TCOUNT                   15
#define COUNT_LIMIT_SIGNAL       12
#define COUNT_LIMIT_BROADCAST    28

int             count                   = 0;
int             thread_ids[NUM_THREADS] = { 0, 1, 2, 3, 4 };
pthread_mutex_t count_mutex        = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  count_threshold_cv = NULL; //PTHREAD_COND_INITIALIZER;

void *inc_count(void *t)
{
    int  i;
    long my_id = (long) t;
    int  ret;

    for (i = 0; i < TCOUNT; i++)
    {
        pthread_mutex_lock(&count_mutex);
        count++;

        /*
           Check the value of count and signal waiting thread when condition is
           reached.  Note that this occurs while mutex is locked.
         */
        if (count == COUNT_LIMIT_SIGNAL)
        {
            TF_MSG("inc_count(): thread %ld, count = %d  Signal Threshold reached.\n",
                   my_id, count);
            ret = pthread_cond_signal(&count_threshold_cv);
            TF_ASSERT(ret == 0);
        }
        if (count == COUNT_LIMIT_BROADCAST)
        {
            TF_MSG("inc_count(): thread %ld, count = %d  Broadcast Threshold reached.\n",
                   my_id, count);
            ret = pthread_cond_broadcast(&count_threshold_cv);
            TF_ASSERT(ret == 0);
        }

        TF_MSG("inc_count(): thread %ld, count = %d\n", my_id, count);
        pthread_mutex_unlock(&count_mutex);

        /* sleep so threads can alternate on mutex lock */
        TF_SLEEP(1);
    }
    TF_MSG("thread %ld exit\n", my_id);
    pthread_exit(NULL);
}

void *watch_count(void *t)
{
    long my_id = (long) t;
    int  ret;

    TF_MSG("Starting watch_count(): thread %ld\n", my_id);

    /*
       Lock mutex and wait for signal.  Note that the pthread_cond_wait
       routine will automatically and atomically unlock mutex while it waits.
       Also, note that if COUNT_LIMIT_SIGNAL is reached before this routine is run by
       the waiting thread, the loop will be skipped to prevent pthread_cond_wait
       from never returning.
     */
    pthread_mutex_lock(&count_mutex);
    if (count < COUNT_LIMIT_SIGNAL)
    {
        ret = pthread_cond_wait(&count_threshold_cv, &count_mutex);
        TF_ASSERT(ret == 0);
        TF_MSG("watch_count(): thread %ld Condition signal received.\n", my_id);
    }
    pthread_mutex_unlock(&count_mutex);
    TF_MSG("thread %ld exit\n", my_id);
    pthread_exit(NULL);
}

void test_posix_condition_variable(void)
{
    int                 i, rc;
    pthread_t           threads[NUM_THREADS];
    pthread_attr_t      attr;
    struct sched_param  param;
    int                 ret;
    pthread_mutexattr_t mutex_attr;

    pthread_mutexattr_init(&mutex_attr);

    /* mutex used with Condition Variable has to be normal mutex (non-recursive) */
    pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_NORMAL);

    /* Initialize mutex and condition variable objects */
    pthread_mutex_init(&count_mutex, &mutex_attr);
    pthread_cond_init(&count_threshold_cv, NULL);

    /* Setup the thread attributes */
    ret = pthread_attr_init(&attr);
    TF_ASSERT(ret == 0);

    param.sched_priority = 100;
    pthread_attr_setschedparam(&attr, &param);
    pthread_attr_setthreadname(&attr, "cond_wait1");
    pthread_create(&threads[0], &attr, watch_count, (void *) thread_ids[0]);

    param.sched_priority = 101;
    pthread_attr_setschedparam(&attr, &param);
    pthread_attr_setthreadname(&attr, "cond_wait2");
    pthread_create(&threads[1], &attr, watch_count, (void *) thread_ids[1]);

    param.sched_priority = 102;
    pthread_attr_setschedparam(&attr, &param);
    pthread_attr_setthreadname(&attr, "cond_wait3");
    pthread_create(&threads[2], &attr, watch_count, (void *) thread_ids[2]);

    param.sched_priority = 103;
    pthread_attr_setschedparam(&attr, &param);
    pthread_attr_setthreadname(&attr, "cond_send1");
    pthread_create(&threads[3], &attr, inc_count, (void *) thread_ids[3]);

    param.sched_priority = 104;
    pthread_attr_setschedparam(&attr, &param);
    pthread_attr_setthreadname(&attr, "cond_send2");
    pthread_create(&threads[4], &attr, inc_count, (void *) thread_ids[4]);

    /* Wait for all threads to complete */
    for (i = 0; i < NUM_THREADS; i++)
    {
        ret = pthread_join(threads[i], NULL);
        TF_ASSERT(ret == 0);
    }
    TF_MSG("Main(): Waited on %d  threads. Done.\n", NUM_THREADS);

    /* Clean up and exit */
    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&count_mutex);
    pthread_cond_destroy(&count_threshold_cv);
}

#define THREADS_IN_TEST       60
#define THREADS_IN_BARRIER    10

pthread_attr_t         attr[THREADS_IN_TEST];
pthread_t              t[THREADS_IN_TEST];

static qurt_barrier_t test_barrier;

void* barrier_thread_main(void * name)
{
    int exit_status = 0;
    int id = (int) name;
    TF_MSG("thread %d started\n", id);

    if (id < 20 + THREADS_IN_BARRIER && id >= 20)
    {
        TF_MSG("id %d reach barrier 1st time\n", id);
        qurt_barrier_wait(&test_barrier);
        TF_SLEEP(10);
        TF_MSG("id %d reach barrier 2nd time\n", id);
        qurt_barrier_wait(&test_barrier);
    }

    return (void*) exit_status;
}

void test_posix_barrier(void)
{
    int                i;
    char               name[32];
    struct sched_param param;

    qurt_barrier_init(&test_barrier, THREADS_IN_BARRIER);

    for (i = 0; i < THREADS_IN_TEST; i++)
    {
        pthread_attr_init(&attr[i]);
        snprintf(name, sizeof(name), "t%d", i);
        pthread_attr_setthreadname(&attr[i], name);
        param.sched_priority = 100 + i;
        pthread_attr_setschedparam(&attr[i], &param);
        pthread_create(&t[i], &attr[i], barrier_thread_main, (void *) i);
    }

    for (i = 0; i < THREADS_IN_TEST; i++)
    {
        pthread_join(t[i], NULL);
    }
}

int main(void)
{
    qurt_init(); //have to do this once
    qurt_timer_init();

    TF_MSG("\n testing posix_test_tls\n");
    posix_test_tls();
    TF_MSG("\n testing posix_test_signal\n");
    posix_test_signal();
    TF_MSG("\n testing posix_test_mutex\n");
    test_posix_mutex();
    TF_MSG("\n testing test_posix_condition_variable\n");
    test_posix_condition_variable();
    TF_MSG("\n testing test_posix_barrier\n");
    test_posix_barrier();
    TF_MSG("\n testing test_posix_semaphore\n");
    test_posix_semaphore();
    TF_MSG("\n testing test_posix_pthread\n");
    test_posix_pthread();
    TF_MSG("\n testing test_posix_timer\n");
    test_posix_timer();

    passnow("All tests passed\n");
}

