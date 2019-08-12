#include "passfail.h"
#include <pthread.h>
#include <signal.h>
#include <common/time.h>
#include <qurt.h>
#include <qurt_timer.h>

#define TF_YIELD()
#define TF_ASSERT(x)    fail_if(!(x))
#define TF_MSG              printf
#define TF_SLEEP(x)     qurt_timer_sleep(1000 * (x))

#define MAX_WAIT_TIME_MS    100

typedef struct timer_thread_handles_t   timer_thread_handles_t;
struct timer_thread_handles_t
{
    pthread_t one_shot_sigwait_thread;
    pthread_t one_shot_callback_thread;
    pthread_t periodic_thread;
    pthread_t periodic_thread2;
    pthread_t multi_periodic_thread;
    pthread_t periodic_callback_thread;
    pthread_t multi_one_shot_sigwait_thread;
    pthread_t multi_one_shot_callback_thread;
};

static timer_thread_handles_t g_thread_handles;

void *
posix_timer_thread_periodic_timer(void * unused)
{
    int exit_status = 0;
    pthread_exit((void *) exit_status);
    return NULL;
}

void *
posix_timer_thread_one_shot_test(void * unused)
{
    int               exit_status = 201;
    clockid_t         clockid     = 0;
    sigevent          evp;
    timer_t           timerid;
    struct itimerspec ivalue;
    sigset_t          set;
    int               sig = 0;
    int               status;
    pthread_attr_t    attr;
    int               i;

    sigemptyset(&set);
    sigaddset(&set, SIGRTMIN);

    evp.sigev_notify          = SIGEV_SIGNAL;
    evp.sigev_signo           = SIGRTMIN;           /* Signal number.           */
    evp.sigev_value.sival_int = SIGRTMIN;           /* Signal value.            */
    evp.sigev_notify_function = 0;                  /* Notification function.   */

    attr.priority = PTHREAD_DEFAULT_PRIORITY + 1;
    attr.stacksize = PTHREAD_MIN_STACKSIZE;
    evp.sigev_notify_attributes = &attr;

    timer_create(clockid, &evp, &timerid);

    ivalue.it_value.tv_sec     = 0;
    ivalue.it_value.tv_nsec    = MAX_WAIT_TIME_MS * 1000;
    ivalue.it_interval.tv_sec  = 0;
    ivalue.it_interval.tv_nsec = 0;

    timer_settime(timerid, 0, &ivalue, NULL);

    /* Block till timer expires */
    status = sigwait(&set, &sig);
    TF_ASSERT(status == 0);
    TF_ASSERT(SIGRTMIN == sig);

    /* clean up*/
    timer_delete(timerid);

    /* test creating and deleting timers */
    for (i=0; i<3000; i++)
    {
        timer_create(clockid, &evp, &timerid);
        timer_delete(timerid);    
    }
    pthread_exit((void *) exit_status);
    return NULL;
}

void *
posix_timer_thread_periodic_test(void * unused)
{
    int               exit_status = 203;
    clockid_t         clockid     = 0;
    sigevent          evp;
    timer_t           timerid;
    struct itimerspec ivalue;
    sigset_t          set;
    int               sig = 0;
    int               status;
    int               i;

    sigemptyset(&set);
    sigaddset(&set, SIGRTMIN);

    evp.sigev_notify          = SIGEV_SIGNAL;
    evp.sigev_signo           = SIGRTMIN;           /* Signal number.           */
    evp.sigev_value.sival_int = SIGRTMIN;           /* Signal value.            */
    evp.sigev_notify_function = 0;                  /* Notification function.   */
    evp.sigev_notify_attributes = 0;

    timer_create(clockid, &evp, &timerid);
    ivalue.it_value.tv_sec     = 0;
    ivalue.it_value.tv_nsec    = MAX_WAIT_TIME_MS * 1000;
    ivalue.it_interval.tv_sec  = 0;
    ivalue.it_interval.tv_nsec = MAX_WAIT_TIME_MS * 1500; /* periodic timer */

    timer_settime(timerid, 0, &ivalue, NULL);

    for (i = 0; i < 5; i++) /* wait for 5 times of expiration */
    {
        /* Block till timer expires */
        status = sigwait(&set, &sig);
        TF_ASSERT(status == 0);
        TF_ASSERT(SIGRTMIN == sig);
    }

    /* clean up*/
    timer_delete(timerid);
    pthread_exit((void *) exit_status);
    return NULL;
}

/* test NULL evp for timer_create */
void *
posix_timer_thread_periodic_test2(void * unused)
{
    int               exit_status = 203;
    clockid_t         clockid     = 0;
    sigevent          evp;
    timer_t           timerid;
    struct itimerspec ivalue;
    sigset_t          set;
    int               sig = 0;
    int               status;
    int               i;

    sigemptyset(&set);
    sigaddset(&set, SIGALRM);
    timer_create(clockid, NULL, &timerid);

    ivalue.it_value.tv_sec     = 0;
    ivalue.it_value.tv_nsec    = MAX_WAIT_TIME_MS * 1000;
    ivalue.it_interval.tv_sec  = 0;
    ivalue.it_interval.tv_nsec = MAX_WAIT_TIME_MS * 1500; /* periodic timer */

    timer_settime(timerid, 0, &ivalue, NULL);

    for (i = 0; i < 5; i++) /* wait for 5 times of expiration */
    {
        /* Block till timer expires */
        status = sigwait(&set, &sig);
        TF_ASSERT(status == 0);
        TF_ASSERT(SIGALRM == sig);
    }

    /* clean up*/
    timer_delete(timerid);
    pthread_exit((void *) exit_status);
    return NULL;
}

void *
posix_timer_thread_multi_periodic_test(void * unused)
{
    int               exit_status = 203;
    clockid_t         clockid     = 0;
    sigevent          evp1, evp2;
    timer_t           timerid1, timerid2;
    struct itimerspec ivalue1, ivalue2;
    sigset_t          set;
    int               sig = 0;
    int               status;
    int               i;
    int               quit = 0;

    sigemptyset(&set);
    sigaddset(&set, SIGRTMIN);
    sigaddset(&set, SIGRTMIN+1);

    /* create timer 1 */
    evp1.sigev_notify          = SIGEV_SIGNAL;
    evp1.sigev_signo           = SIGRTMIN;           /* Signal number.           */
    evp1.sigev_value.sival_int = SIGRTMIN;           /* Signal value.            */
    evp1.sigev_notify_function = 0;                  /* Notification function.   */
    evp1.sigev_notify_attributes = 0;

    timer_create(clockid, &evp1, &timerid1);
    ivalue1.it_value.tv_sec     = 0;
    ivalue1.it_value.tv_nsec    = MAX_WAIT_TIME_MS * 1000;
    ivalue1.it_interval.tv_sec  = 0;
    ivalue1.it_interval.tv_nsec = MAX_WAIT_TIME_MS * 1000; /* periodic timer */

    /* create timer 2 */
    evp2.sigev_notify          = SIGEV_SIGNAL;
    evp2.sigev_signo           = SIGRTMIN+1;         /* Signal number.           */
    evp2.sigev_value.sival_int = SIGRTMIN+1;         /* Signal value.            */
    evp2.sigev_notify_function = 0;                  /* Notification function.   */
    evp2.sigev_notify_attributes = 0;

    timer_create(clockid, &evp2, &timerid2);
    ivalue2.it_value.tv_sec     = 0;
    ivalue2.it_value.tv_nsec    = MAX_WAIT_TIME_MS * 10000;
    ivalue2.it_interval.tv_sec  = 0;
    ivalue2.it_interval.tv_nsec = 0;                 /* one shot timer */

    timer_settime(timerid2, 0, &ivalue2, NULL);
    timer_settime(timerid1, 0, &ivalue1, NULL);

    while (!quit)
    {
        /* Block till timer expires */
        status = sigwait(&set, &sig);
        TF_ASSERT(status == 0);
        switch (sig)
        {
        case SIGRTMIN:
            printf ("timer1 expires %d times\n", i++);
            break;
        case SIGRTMIN+1:
            printf ("timer2 expires\n");
            timer_delete(timerid1);
            timer_delete(timerid2);
            quit = 1;
            break;      
        default:
            printf ("posix_timer_thread_multi_periodic_test error\n");
            TF_ASSERT(0);        
        }        
    }

    /* clean up*/
    pthread_exit((void *) exit_status);
    return NULL;
}

#define TEST_TIMER_SIG    SIGRTMIN

void timer_callback(union sigval val)
{
    pthread_kill(g_thread_handles.one_shot_callback_thread, val.sival_int);
}

#define TIMER_NUM 10 /* 1000 will cause problem */
void *
posix_timer_thread_one_shot_cb_test(void * unused)
{
    int               exit_status = 202;
    clockid_t         clockid     = 0;
    sigevent          evp;
    timer_t           timerid[TIMER_NUM];
    struct itimerspec ivalue;
    sigset_t          set;
    int               sig = 0;
    int               status;
    int               i;

    sigemptyset(&set);
    sigaddset(&set, TEST_TIMER_SIG);

    for (i=0; i<TIMER_NUM; i++)
    {
        evp.sigev_notify          = SIGEV_THREAD;
        evp.sigev_notify_function = timer_callback;  /* Notification function.   */
        evp.sigev_value.sival_int = TEST_TIMER_SIG;
        evp.sigev_notify_attributes = 0;
    
        timer_create(clockid, &evp, &timerid[i]);
    
        ivalue.it_value.tv_sec     = 0;
        ivalue.it_value.tv_nsec    = MAX_WAIT_TIME_MS * 1000;
        ivalue.it_interval.tv_sec  = 0;
        ivalue.it_interval.tv_nsec = 0;
    
        timer_settime(timerid[i], 0, &ivalue, NULL);
        TF_SLEEP(2);
        printf("%d\n", i);
    }
    
    /* Block till timer expires */
    status = sigwait(&set, &sig);
    TF_ASSERT(status == 0);
    TF_ASSERT(TEST_TIMER_SIG == sig);

    /* clean up*/
    for (i=0; i<TIMER_NUM; i++)
    {
        timer_delete(timerid[i]);
    }
    pthread_exit((void *) exit_status);
    return NULL;
}

void timer_periodic_callback(union sigval val)
{
    pthread_kill(g_thread_handles.periodic_callback_thread, val.sival_int);
}

void *
posix_timer_thread_periodic_cb_test(void * unused)
{
    int               exit_status = 204;
    clockid_t         clockid     = 0;
    sigevent          evp;
    timer_t           timerid;
    struct itimerspec ivalue;
    sigset_t          set;
    int               sig = 0;
    int               status;
    int               i;

    sigemptyset(&set);
    sigaddset(&set, TEST_TIMER_SIG);

    evp.sigev_notify          = SIGEV_THREAD;
    evp.sigev_notify_function = timer_periodic_callback;  /* Notification function.   */
    evp.sigev_value.sival_int = TEST_TIMER_SIG;
    evp.sigev_notify_attributes = 0;

    timer_create(clockid, &evp, &timerid);

    ivalue.it_value.tv_sec     = 0;
    ivalue.it_value.tv_nsec    = MAX_WAIT_TIME_MS * 1000;
    ivalue.it_interval.tv_sec  = 0;
    ivalue.it_interval.tv_nsec = MAX_WAIT_TIME_MS * 1000; /* periodic timer */

    timer_settime(timerid, 0, &ivalue, NULL);

    for (i = 0; i < 5; i++) /* wait for 5 times of expiration */
    {
        /* Block till timer expires */
        status = sigwait(&set, &sig);
        TF_ASSERT(status == 0);
        TF_ASSERT(TEST_TIMER_SIG == sig);
    }

    /* clean up*/
    timer_delete(timerid);
    pthread_exit((void *) exit_status);
    return NULL;
}

#define TEST_MULTI_TIMER_NUM    6
void *
posix_timer_thread_multi_one_shot_test(void * unused)
{
    int               exit_status = 205;
    clockid_t         clockid     = 0;
    sigevent          evp[TEST_MULTI_TIMER_NUM];
    timer_t           timerid[TEST_MULTI_TIMER_NUM];
    struct itimerspec ivalue[TEST_MULTI_TIMER_NUM];
    sigset_t          set, empty_set;
    int               sig = 0;
    int               status, i;

    sigemptyset(&set);
    sigemptyset(&empty_set);

    for (i = 0; i < TEST_MULTI_TIMER_NUM; i++)
    {
        sigaddset(&set, SIGRTMIN + i);
        evp[i].sigev_notify            = SIGEV_SIGNAL;
        evp[i].sigev_signo             = SIGRTMIN + i;      /* Signal number */
        evp[i].sigev_value.sival_int   = SIGRTMIN + i;      /* Signal value */
        evp[i].sigev_notify_function   = 0;                 /* Notification function.   */
        evp[i].sigev_notify_attributes = 0;

        timer_create(clockid, &(evp[i]), &(timerid[i]));

        ivalue[i].it_value.tv_sec     = 0;
        ivalue[i].it_value.tv_nsec    = (MAX_WAIT_TIME_MS + (i + 1) * 30) * 1000;
        ivalue[i].it_interval.tv_sec  = 0;
        ivalue[i].it_interval.tv_nsec = 0;

        timer_settime(timerid[i], 0, &ivalue[i], NULL);
    }

    while (set != empty_set)
    {
        /* Block till timer expires */
        status = sigwait(&set, &sig);
        TF_ASSERT(status == 0);
        TF_ASSERT(sigismember(&set, sig));
        sigdelset(&set, sig);
        TF_MSG("recved signo=%d\n", sig);
        TF_MSG("set %X and empty_set %X\n", set, empty_set);
    }

    /* clean up*/
    for (i = 0; i < TEST_MULTI_TIMER_NUM; i++)
        timer_delete(timerid[i]);
    pthread_exit((void *) exit_status);
    return NULL;
}

void test_posix_timer()
{
    int tmp           = 0;
    int * exit_status = &tmp;
    int status;

    status = pthread_create(&(g_thread_handles.one_shot_sigwait_thread), NULL,
                            posix_timer_thread_one_shot_test, NULL);
    TF_ASSERT(status == 0);

    status = pthread_create(&(g_thread_handles.one_shot_callback_thread), NULL,
                            posix_timer_thread_one_shot_cb_test, NULL);
    TF_ASSERT(status == 0);

    status = pthread_create(&(g_thread_handles.periodic_thread), NULL,
                            posix_timer_thread_periodic_test, NULL);
    TF_ASSERT(status == 0);

    status = pthread_create(&(g_thread_handles.periodic_thread2), NULL,
                            posix_timer_thread_periodic_test2, NULL);
    TF_ASSERT(status == 0);

    status = pthread_create(&(g_thread_handles.multi_periodic_thread), NULL,
                            posix_timer_thread_multi_periodic_test, NULL);
    TF_ASSERT(status == 0);

    status = pthread_create(&(g_thread_handles.periodic_callback_thread), NULL,
                            posix_timer_thread_periodic_cb_test, NULL);
    TF_ASSERT(status == 0);

    status = pthread_create(&(g_thread_handles.multi_one_shot_sigwait_thread), NULL,
                            posix_timer_thread_multi_one_shot_test, NULL);
    TF_ASSERT(status == 0);

    /* Teardown */
    status = pthread_join((g_thread_handles.one_shot_sigwait_thread), (void**) &exit_status);
    TF_ASSERT(status == 0);
    TF_ASSERT(exit_status == (void*) 201);

    status = pthread_join((g_thread_handles.one_shot_callback_thread), (void**) &exit_status);
    TF_ASSERT(status == 0);
    TF_ASSERT(exit_status == (void*) 202);

    status = pthread_join((g_thread_handles.periodic_thread), (void**) &exit_status);
    TF_ASSERT(status == 0);
    TF_ASSERT(exit_status == (void*) 203);

    status = pthread_join((g_thread_handles.periodic_thread2), (void**) &exit_status);
    TF_ASSERT(status == 0);
    TF_ASSERT(exit_status == (void*) 203);

    status = pthread_join((g_thread_handles.multi_periodic_thread), (void**) &exit_status);
    TF_ASSERT(status == 0);
    TF_ASSERT(exit_status == (void*) 203);

    status = pthread_join((g_thread_handles.periodic_callback_thread), (void**) &exit_status);
    TF_ASSERT(status == 0);
    TF_ASSERT(exit_status == (void*) 204);

    status = pthread_join((g_thread_handles.multi_one_shot_sigwait_thread), (void**) &exit_status);
    TF_ASSERT(status == 0);
    TF_ASSERT(exit_status == (void*) 205);
}
