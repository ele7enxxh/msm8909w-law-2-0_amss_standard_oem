/*===========================================================================

                           INCLUDE FILES

   ===========================================================================*/
#include "passfail.h"
#include <qurt.h>
#include <signal.h>
#include <pthread.h>
#include <common/time.h>
#include <qurt_timer.h>

#define TF_YIELD()
#define TF_ASSERT(x)    fail_if(!(x))
#define TF_MSG              printf
#define TF_SLEEP(x)     qurt_timer_sleep(1000 * (x))

#define MAX_WAIT_TIME_MS    100

#define TEST_SIGNAL_SIG1    SIGRTMIN
#define TEST_SIGNAL_SIG2    (SIGRTMIN + 1)

typedef struct signal_thread_handles_t   signal_thread_handles_t;
struct signal_thread_handles_t
{
    pthread_t thread_one;
    pthread_t thread_two;
};
static signal_thread_handles_t g_thread_handles;

void *
posix_signal_thread_one(void *arg)
{
    int      exit_status = 120;
    sigset_t set;
    sigset_t rcvd_set;
    int      sig = 0;

    TF_SLEEP(1);

    sigemptyset(&set);
    sigaddset(&set, TEST_SIGNAL_SIG1);
    sigaddset(&set, TEST_SIGNAL_SIG2);

    sigemptyset(&rcvd_set);

    // cannot compare set != rcvd_set on UNIX because they are structs
    while (!sigismember(&rcvd_set, TEST_SIGNAL_SIG1) || !sigismember(&rcvd_set, TEST_SIGNAL_SIG2))
    {
        sigwait(&set, &sig);
        sigaddset(&rcvd_set, sig);
    }
    pthread_exit((void *) exit_status);
    return NULL;
}

static sigset_t received_set;
void sa_handler_sig_two(int sig)
{
    if (sig != TEST_SIGNAL_SIG1)
    {
        //error
    }
    else
    {
        sigaddset(&received_set, sig);
    }
}
void sa_sigaction_sig_four(int sig, siginfo_t * siginfo, void *ctxt)
{
    if (sig != TEST_SIGNAL_SIG2)
    {
        //error
    }
    else
    {
        sigaddset(&received_set, sig);
    }
}
void *
posix_signal_thread_two(void *arg)
{
    int              exit_status = 121;
    sigset_t         set;
    int              sig = 0;
    struct sigaction sa_sig_2;
    struct sigaction sa_sig_4;


    TF_SLEEP(1);
    sa_sig_2.sa_handler = sa_handler_sig_two;
    sigemptyset(&sa_sig_2.sa_mask);
    sa_sig_2.sa_flags     = 0;
    sa_sig_2.sa_sigaction = NULL;

    sa_sig_4.sa_handler = NULL;
    sigemptyset(&sa_sig_4.sa_mask);
    sa_sig_4.sa_flags     = SA_SIGINFO;
    sa_sig_4.sa_sigaction = sa_sigaction_sig_four;

    sigemptyset(&set);
    sigaddset(&set, TEST_SIGNAL_SIG1);
    sigaddset(&set, TEST_SIGNAL_SIG2);

    sigemptyset(&received_set);

    //register sigaction.
    sigaction(TEST_SIGNAL_SIG1, &sa_sig_2, NULL);
    sigaction(TEST_SIGNAL_SIG2, &sa_sig_4, NULL);

    // cannot compare set != received_set on UNIX because they are structs
    while (!sigismember(&received_set, TEST_SIGNAL_SIG1) || !sigismember(&received_set, TEST_SIGNAL_SIG2))
    {
        sigsuspend(&set);
    }
    pthread_exit((void *) exit_status);
    return NULL;
}

void posix_test_signal()
{
    int            tmp           = 0;
    int            * exit_status = &tmp;
    int            status;
    pthread_attr_t thread_attr;
    int            ret;

    status = pthread_attr_init(&thread_attr);
    TF_ASSERT(status == 0);

    pthread_attr_setthreadname(&thread_attr, "sig_test1");
    ret = pthread_create(&(g_thread_handles.thread_one), &thread_attr,
                   posix_signal_thread_one, NULL);
    TF_ASSERT(ret == 0);                   

    pthread_attr_setthreadname(&thread_attr, "sig_test2");
    ret = pthread_create(&(g_thread_handles.thread_two), &thread_attr,
                   posix_signal_thread_two, NULL);
    TF_ASSERT(ret == 0);

    status = pthread_kill(g_thread_handles.thread_one, TEST_SIGNAL_SIG1);
    TF_ASSERT(status == 0);

    status = pthread_kill(g_thread_handles.thread_one, TEST_SIGNAL_SIG2);
    TF_ASSERT(status == 0);

    status = pthread_kill(g_thread_handles.thread_two, TEST_SIGNAL_SIG1);
    TF_ASSERT(status == 0);

    status = pthread_kill(g_thread_handles.thread_two, TEST_SIGNAL_SIG2);
    TF_ASSERT(status == 0);

    status = pthread_join((g_thread_handles.thread_one), (void**) &exit_status);
    TF_ASSERT(status == 0);
    TF_ASSERT(exit_status == (void*) 120);

    status = pthread_join((g_thread_handles.thread_two), (void**) &exit_status);
    TF_ASSERT(status == 0);
    TF_ASSERT(exit_status == (void*) 121);
}
