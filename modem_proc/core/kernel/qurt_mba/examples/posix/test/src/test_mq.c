/*===========================================================================

                           INCLUDE FILES

   ===========================================================================*/
#include "passfail.h"
#include <atomic_ops.h>
#include <qube.h>
#include <pthread.h>
#include <signal.h>
#include <common/time.h>
#include <mqueue.h>
#include <fcntl.h>
#include <qurt_timer.h>

#define TF_YIELD()
#define TF_ASSERT(x)    fail_if(!(x))
#define TF_MSG    printf
#define TF_SLEEP(x)     qurt_timer_sleep(1000 * (x))
#define TF_USLEEP(x)    qurt_timer_sleep(x)

#define TESTOPTION1

// Variable type definition
typedef struct
{
    uint16_t payload; //use uint16 to test alignment
} message_t;

// From mq_internal.h
#define MSG_SIZE      100
#define MAX_NO_MSG    10

/* max number of messages in each queue */
/* 24 is QMSGQ_MSG_OVERHEAD */
/* 16 is QMSGQ_MSG_HDR_SIZE */
#define MQ_MAXMSG         4

#define MY_MQ_PRIORITY    0
#define MY_MQ_NAME        "my_mq"
#define MY_MQ_NAME2       "my_other_mq"
#ifdef T_UNIX
  #define MY_MQ_PERMS     (S_IRWXU | S_IRWXG | S_IRWXO)
#else
  #define MY_MQ_PERMS     0777
#endif

//TF_DEFINE_TEST_CASE(COMPONENT, posix_test_mq_open);
//TF_DEFINE_TEST_CASE(COMPONENT, posix_test_mq_send);
//TF_DEFINE_TEST_CASE(COMPONENT, posix_test_mq_00);
//TF_DEFINE_TEST_CASE(COMPONENT, posix_test_mq_01);
//TF_DEFINE_TEST_CASE(COMPONENT, posix_test_mq_02);
//TF_DEFINE_TEST_CASE(COMPONENT, posix_test_mq_03);


void posix_test_mq_open()
{
    mqd_t mq_desc1 = 0;
    mqd_t mq_desc2 = 0;
    mqd_t mq_desc3 = 0;
    mqd_t mq_desc4 = 0;
    int   ret      = -1;

    /* This creates a new queue named "/example_queue_1" and opens it for
     * sending messages only. If the queue with the same name already exists
     * function should fail (POSIX_O_EXCL). The queue file permissions are set rw for owner
     * and nothing for group/others. Queue limits set to values provided above. */
    mq_desc1 = mq_open("/mq_test_queue_1", POSIX_O_CREAT | POSIX_O_WRONLY | POSIX_O_EXCL, 0600, NULL);
    TF_ASSERT(mq_desc1 == 4);

    mq_desc2 = mq_open("/mq_test_queue_2", POSIX_O_CREAT | POSIX_O_WRONLY | POSIX_O_EXCL, 0600, NULL);
    TF_ASSERT(mq_desc2 == 5);

    ret = mq_close(mq_desc1);
    TF_ASSERT(ret == 0);

    ret = mq_unlink("/mq_test_queue_1");
    TF_ASSERT(ret == 0);

    mq_desc3 = mq_open("/mq_test_queue_3", POSIX_O_CREAT | POSIX_O_WRONLY | POSIX_O_EXCL, 0600, NULL);
    TF_ASSERT(mq_desc3 == 4);

    mq_desc4 = mq_open("/mq_test_queue_3", POSIX_O_CREAT | POSIX_O_WRONLY | POSIX_O_EXCL, 0600, NULL);
    TF_ASSERT(mq_desc4 == -1);   /* expect failure */

    mq_desc4 = mq_open("/mq_test_queue_3", POSIX_O_WRONLY | POSIX_O_EXCL);
    TF_ASSERT(mq_desc4 == 4);

    ret = mq_close(mq_desc2);
    TF_ASSERT(ret == 0);

    ret = mq_unlink("/mq_test_queue_2");
    TF_ASSERT(ret == 0);

    ret = mq_close(mq_desc3);
    TF_ASSERT(ret == 0);

    ret = mq_close(mq_desc4);
    TF_ASSERT(ret == 0);

    ret = mq_unlink("/mq_test_queue_3");
    TF_ASSERT(ret == 0);
}

pthread_t pthread_sender;
pthread_t pthread_receiver;

//static char send_buff[100];

void *
thread_sender(void *unused)
{
    int   exit_status = 8;
    mqd_t mq_desc1    = 0;
    int   ret         = -1;
    char  buff[100];

    buff[0] = 100; /* signature */
    buff[1] = 101;

    mq_desc1 = mq_open("/test_queue", POSIX_O_WRONLY | POSIX_O_EXCL);
    TF_ASSERT(mq_desc1 == 4);

    ret = mq_send(mq_desc1, buff, 100, 0);
    TF_ASSERT(ret == 0);

    ret = mq_close(mq_desc1);
    TF_ASSERT(ret == 0);

    pthread_exit((void *) exit_status);
    return NULL;
}

static void SendMessages(mqd_t mq)
{
    mqd_t              status;
    uint32_t           i;
    message_t          msg_to_send;
    unsigned long long start_peg, end_peg;
    int                tcycles;

    // Send MQ_MAX_MSGS number of messages
    for (i = 0; i < MQ_MAXMSG; i++)
    {
        // prepare the message payload
        msg_to_send.payload = (i + 1) * 3;

        TF_MSG("Sending message #%lu - Payload = %lu", i, msg_to_send.payload);
        start_peg = qurt_get_core_pcycles();
        status    = mq_send(mq, (char*) &msg_to_send, sizeof(msg_to_send),
                            MY_MQ_PRIORITY);
        end_peg = qurt_get_core_pcycles();
        tcycles = (end_peg - start_peg) / 6;
        TF_MSG("mq send -> status = %d, TCycles=%d\n", status, tcycles);
        TF_ASSERT(status == (mqd_t) 0);
    }

    // Trying to send one more message - this send should fail
    // in POSIX land using timedsend to avoid blocking on the queue longer than needed
    msg_to_send.payload = (i + 1) * 3;

    struct timespec timeout;
    timeout.tv_sec  = 0;
    timeout.tv_nsec = 0;

    TF_MSG("Sending message #%lu (should fail) - Payload = %lu", i, msg_to_send.payload);
    status = mq_timedsend(mq, (char*) &msg_to_send, sizeof(msg_to_send), MY_MQ_PRIORITY, &timeout);
    TF_MSG("mq timedsend -> status = %d, errno = %d", status, errno);
    TF_ASSERT(status == (mqd_t) -1);
    TF_ASSERT(errno == EAGAIN);

    return;
}

static void RecvMessages(mqd_t mq)
{
    ssize_t   bytes_recvd;
    message_t msg_recvd;
    //char       buffer[MSG_BUF_SIZE];
    uint32_t           priority, i;
    unsigned long long start_peg, end_peg;
    int                tcycles;

    // Receive all MQ_MAX_MSGS number of messages
    for (i = 0; i < MQ_MAXMSG; i++)
    {
        TF_MSG("Receiving message #%lu", i);
        start_peg   = qurt_get_core_pcycles();
        bytes_recvd = mq_receive(mq, (char*) &msg_recvd, sizeof(msg_recvd), (unsigned int *) &priority);
        end_peg     = qurt_get_core_pcycles();
        tcycles     = (end_peg - start_peg) / 6;
        TF_ASSERT(bytes_recvd == sizeof(message_t));
        TF_ASSERT(priority == MY_MQ_PRIORITY);

        // acquire message payload and verify contents
        //msg_recvd = (message_t*)buffer;
        TF_ASSERT(msg_recvd.payload == (i + 1) * 3);
        TF_MSG("mq receive -> bytes recvd = %d, payload = %lu, TCycles=%d\n", bytes_recvd, msg_recvd.payload, tcycles);
    }

#if 0
    // Trying to receive one more message - the queue should be empty!!!
    // in POSIX land using timedreceive to avoid blocking on the queue longer than needed
    struct timespec timeout;
    timeout.tv_sec  = 0;
    timeout.tv_nsec = 0;

    TF_MSG("Receiving message #%lu (should fail)", i);
    bytes_recvd = mq_timedreceive(mq, (char*) &msg_recvd, sizeof(msg_recvd), (unsigned int *) &priority, &timeout);
    TF_MSG("mq timedreceive -> bytes_recvd = %d, errno = %d", bytes_recvd, errno);
    TF_ASSERT(bytes_recvd == (mqd_t) -1);
    TF_ASSERT(errno == ETIMEDOUT);
#endif

    return;
}

void *
thread_receiver(void *unused)
{
    int     exit_status = 0;
    mqd_t   mq_desc1    = 0;
    ssize_t received_size;
    char    buff[100];
    int     ret;

    mq_desc1 = mq_open("/test_queue", POSIX_O_WRONLY | POSIX_O_EXCL);
    TF_ASSERT(mq_desc1 == 4);

    received_size = mq_receive(mq_desc1, buff, 100, NULL);
    TF_ASSERT(received_size != 0);

    TF_ASSERT(buff[0] == 100);
    TF_ASSERT(buff[1] == 101);

    /* do extra test here */
    /* testing sending to full queue */

    // Try sending messages
    TF_MSG("\nSending Messages:");
    SendMessages(mq_desc1);

    // Try receiving those messages
    TF_MSG("\nReceiving Messages:");
    RecvMessages(mq_desc1);

    ret = mq_close(mq_desc1);
    TF_ASSERT(ret == 0);

    //pthread_exit((void *) exit_status);
    return (void*) 18;
}

void posix_test_mq_send()
{
    pthread_attr_t thread_attr;
    mqd_t          mq_desc1      = 0;
    int            ret           = -1;
    int            tmp           = 0;
    int            * exit_status = &tmp;
    struct mq_attr attr;

    //defining mq attributes
    memset(&attr, 0, sizeof(attr));
    attr.mq_maxmsg  = MQ_MAXMSG;
    attr.mq_msgsize = sizeof(message_t);

    /* create queue */
    mq_desc1 = mq_open("/test_queue", POSIX_O_CREAT | POSIX_O_WRONLY | POSIX_O_EXCL, 0600, &attr);

    // Setup the thread attributes
    ret = pthread_attr_init(&thread_attr);
    TF_ASSERT(ret == 0);

#ifndef T_UNIX
    pthread_attr_setthreadname(&thread_attr, "receiver");
#endif // !T_UNIX
    pthread_create(&pthread_receiver, &thread_attr, thread_receiver, NULL);

#ifndef T_UNIX
    pthread_attr_setthreadname(&thread_attr, "sender");
#endif // !T_UNIX
    pthread_create(&pthread_sender, &thread_attr, thread_sender, NULL);

    ret = mq_close(mq_desc1);
    TF_ASSERT(ret == 0);

    ret = pthread_join(pthread_receiver, (void**) &exit_status);
    TF_ASSERT(ret == 0);
    TF_ASSERT(exit_status == (void*) 18);

    ret = pthread_join(pthread_sender, (void**) &exit_status);
    TF_ASSERT(ret == 0);
    TF_ASSERT(exit_status == (void*) 8);

    ret = mq_unlink("/test_queue");
    TF_ASSERT(ret == 0);
}

void posix_test_mq_00()
{
    // tests the functionality of mq_open(), mq_close() and mq_unlink() fns

    // declaring message queue variables for this test
    mqd_t     mq1, mq2;
    mqd_t     status;
    message_t msg_to_send;
    int       i;

    // defining MQ attribute and initializing it
    struct mq_attr attr;
    memset(&attr, 0, sizeof(attr));
    attr.mq_maxmsg  = MAX_NO_MSG;
    attr.mq_msgsize = sizeof(message_t);

    // Any "unlinked" mq lying on the file system with same name
    // needs to be removed
    //Check4MQ( MY_MQ_NAME );

    // Test Scenerios

    // 1. Work on a NON-EXISTENT (a.k.a. NE) message queue (a.k.a mq)
    // -------------------------------------------------------------------- //
    // Open a NE mq
    //   expected behavior - FAIL with errno = ENOENT
    mq1 = mq_open(MY_MQ_NAME, POSIX_O_RDWR);
    TF_MSG("\nmq open NE (POSIX_O_RDWR) -> mqd_t = %d, errno = %d", mq1, errno);
    TF_ASSERT(mq1 == (mqd_t) -1);
    TF_ASSERT(errno == ENOENT);

    // Close a NE mq
    //   expected behavior - FAIL with errno = EBADF
    status = mq_close(mq1);
    TF_MSG("mq close NE -> status = %d, errno = %d", status, errno);
    TF_ASSERT(status == -1);
    TF_ASSERT(errno == EBADF);

    // Unlink a NE mq
    //   expected behavior - FAIL with errno = ENOENT
    status = mq_unlink(MY_MQ_NAME);
    TF_MSG("mq unlink NE -> status = %d, errno = %d", status, errno);
    TF_ASSERT(status == -1);
    TF_ASSERT(errno == ENOENT);
    // ------------------------------------------------------------------- //

    // 2. Try to re-create/open a mq using POSIX_O_EXCL
    // ------------------------------------------------------------------- //
    // Create/Open a new mq
    //    expected behavior - PASS
    mq1 = mq_open(MY_MQ_NAME, POSIX_O_RDWR | POSIX_O_CREAT, MY_MQ_PERMS, &attr);
    TF_MSG("\nmq open(POSIX_O_RDWR|POSIX_O_CREAT) -> mqd_t = %d", mq1);
    TF_ASSERT(mq1 != (mqd_t) -1);

    // Re-Create/Open the mq with POSIX_O_EXCL
    //    expected behavior - FAIL with errno = EEXIST
    mq2 = mq_open(MY_MQ_NAME, POSIX_O_RDWR | POSIX_O_CREAT | POSIX_O_EXCL, MY_MQ_PERMS, &attr);
    TF_MSG("mq open(POSIX_O_RDWR|POSIX_O_CREAT|POSIX_O_EXCL) -> mqd_t = %d, errno = %d", mq2, errno);
    TF_ASSERT(mq2 == (mqd_t) -1);
    TF_ASSERT(errno == EEXIST);

    // /Open the mq as POSIX_O_RDWR
    //    expected behavior - PASS, returns existing mq
    mq2 = mq_open(MY_MQ_NAME, POSIX_O_RDWR, MY_MQ_PERMS, &attr);
    TF_MSG("mq open(POSIX_O_RDWR) -> mqd_t = %d", mq2);
    TF_ASSERT(mq2 != (mqd_t) -1);
    TF_ASSERT(mq_close(mq2) == 0);

    TF_ASSERT(mq_close(mq1) == 0);
    TF_ASSERT(mq_unlink(MY_MQ_NAME) == 0);
    // ------------------------------------------------------------------ //

    // 3. Try to re-create/open a mq which was opened using POSIX_O_EXCL
    // ------------------------------------------------------------------- //
    // Create/Open a new mq
    //    expected behavior - PASS
    mq1 = mq_open(MY_MQ_NAME, POSIX_O_RDWR | POSIX_O_CREAT | POSIX_O_EXCL, MY_MQ_PERMS, &attr);
    TF_MSG("\nmq open(POSIX_O_RDWR|POSIX_O_CREAT|POSIX_O_EXCL) -> mqd_t = %d", mq1);
    TF_ASSERT(mq1 != (mqd_t) -1);

    // Re-Create/Open the mq
    //    expected behavior - PASS, returns existing mq
    mq2 = mq_open(MY_MQ_NAME, POSIX_O_RDWR | POSIX_O_CREAT, MY_MQ_PERMS, &attr);
    TF_MSG("mq open(POSIX_O_RDWR|POSIX_O_CREAT) -> mqd_t = %d", mq2);
    TF_ASSERT(mq2 != (mqd_t) -1);
    TF_ASSERT(mq_close(mq2) == 0);

    // /Open the mq as POSIX_O_RDWR
    //    expected behavior - PASS, returns existing mq
    mq2 = mq_open(MY_MQ_NAME, POSIX_O_RDWR, MY_MQ_PERMS, &attr);
    TF_MSG("mq open(POSIX_O_RDWR) -> mqd_t = %d", mq2);
    TF_ASSERT(mq2 != (mqd_t) -1);
    TF_ASSERT(mq_close(mq2) == 0);

    TF_ASSERT(mq_close(mq1) == 0);
    TF_ASSERT(mq_unlink(MY_MQ_NAME) == 0);
    // ------------------------------------------------------------------ //

    // 4. Try to re-open an already unlinked mq
    // ------------------------------------------------------------------ //
    // Create/Open a new mq
    //    expected behavior - PASS
    mq1 = mq_open(MY_MQ_NAME, POSIX_O_RDWR | POSIX_O_CREAT, MY_MQ_PERMS, &attr);
    TF_MSG("\nmq open(POSIX_O_RDWR|POSIX_O_CREAT) -> mqd_t = %d", mq1);
    TF_ASSERT(mq1 != (mqd_t) -1);

    TF_ASSERT(mq_close(mq1) == 0);
    TF_ASSERT(mq_unlink(MY_MQ_NAME) == 0);

    /* test creating and delete mq */
    for (i=0; i<3000; i++)
    {
        mq1 = mq_open(MY_MQ_NAME, POSIX_O_RDWR | POSIX_O_CREAT | POSIX_O_EXCL, MY_MQ_PERMS, &attr);
        TF_ASSERT(mq1 != (mqd_t) -1);
    
        TF_ASSERT(mq_close(mq1) == 0);
        TF_ASSERT(mq_unlink(MY_MQ_NAME) == 0);
    }

    // Re-Open the mq
    //    expected behavior - FAIL with errno = ENOENT
    mq1 = mq_open(MY_MQ_NAME, POSIX_O_RDWR, MY_MQ_PERMS, &attr);
    TF_MSG("mq open(POSIX_O_RDWR) -> mqd_t = %d, errno = %d", mq1, errno);
    TF_ASSERT(mq1 == (mqd_t) -1);
    TF_ASSERT(errno == ENOENT);

    // ------------------------------------------------------------------ //

    // 5. Try to send msg to a full queue
    // ------------------------------------------------------------------ //
    // Create/Open a new mq
    //    expected behavior - FAIL
    attr.mq_maxmsg = 10;

    mq1 = mq_open(MY_MQ_NAME, POSIX_O_RDWR | POSIX_O_CREAT, MY_MQ_PERMS, &attr);
    TF_MSG("\nmq open(POSIX_O_RDWR|POSIX_O_CREAT) -> mqd_t = %d", mq1);
    TF_ASSERT(mq1 != (mqd_t) -1);

    for (i = 0; i < 10; i++)
    {
        status = mq_send(mq1, (char*) &msg_to_send, sizeof(msg_to_send), 0);
        TF_ASSERT(status == 0);
    }
    status = mq_send(mq1, (char*) &msg_to_send, sizeof(msg_to_send), 0);
    TF_ASSERT(status == -1);
    TF_ASSERT(errno == EAGAIN);

    TF_ASSERT(mq_close(mq1) == 0);
    TF_ASSERT(mq_unlink(MY_MQ_NAME) == 0);

    return;
}

void posix_test_mq_01()
{
    // tests the functionality of mq_setattr() and mq_getattr() fns

    // declaring message queue variable for this test
    mqd_t mq1 = 0;
    mqd_t status;

    // ------------------------------------------------------------------ //
    struct mq_attr new_mq_attr, old_mq_attr;

    // Try to acquire attributes on a NE mq
    status = mq_getattr(mq1, &new_mq_attr);
    TF_MSG("mq getattr  -> status = %d, errno = %d", status, errno);
    TF_ASSERT(status == (mqd_t) -1);
    TF_ASSERT(errno == EBADF);

    //defining mq attributes
    memset(&new_mq_attr, 0, sizeof(new_mq_attr));
    new_mq_attr.mq_maxmsg  = MAX_NO_MSG;
    new_mq_attr.mq_msgsize = sizeof(message_t);

    // Create/Open a message queue
    mq1 = mq_open(MY_MQ_NAME, POSIX_O_RDWR | POSIX_O_CREAT, MY_MQ_PERMS, &new_mq_attr);
    TF_MSG("mq open(POSIX_O_RDWR|POSIX_O_CREAT) -> mqd_t = %d, errno = %d", mq1, errno);
    TF_ASSERT(mq1 != (mqd_t) -1);

    // Acquire current attributes of the mq
    status = mq_getattr(mq1, &old_mq_attr);
    TF_MSG("mq getattr  -> status = %d", status);
    TF_ASSERT(status == (mqd_t) 0);
    TF_ASSERT((old_mq_attr.mq_flags & POSIX_O_NONBLOCK) == 0);

    // Try setting attribute mq_flags to something other than POSIX_O_NONBLOCK
    new_mq_attr.mq_flags = POSIX_O_RDWR;
    status               = mq_setattr(mq1, &new_mq_attr, &old_mq_attr);
    TF_MSG("mq setattr  -> status = %d, errno = %d", status, errno);
    TF_ASSERT(status == (mqd_t) -1);
    TF_ASSERT(errno == EINVAL);

    // Try setting attribute mq_flags to POSIX_O_NONBLOCK
    new_mq_attr.mq_flags = POSIX_O_NONBLOCK;
    status               = mq_setattr(mq1, &new_mq_attr, &old_mq_attr);
    TF_MSG("mq setattr  -> status = %d", status);
    TF_ASSERT(status == (mqd_t) 0);

    // Acquire current attributes of the mq
    status = mq_getattr(mq1, &old_mq_attr);
    TF_MSG("mq getattr  -> status = %d", status);
    TF_ASSERT(status == (mqd_t) 0);
    TF_ASSERT((old_mq_attr.mq_flags == (long) POSIX_O_NONBLOCK));

    // Closing and Unlinking the message queue
    TF_ASSERT(mq_close(mq1) == 0);
    TF_ASSERT(mq_unlink(MY_MQ_NAME) == 0);
    // ------------------------------------------------------------------ //
    return;
}

/* two threads for testing signal, mq and pselect */
#define USER_SIG_01      SIGRTMIN
#define USER_SIG_02      SIGRTMIN + 1
#define USER_SIG_03      SIGRTMIN + 2
#define USER_SIG_04      SIGRTMIN + 3
#define USER_SIG_EXIT    SIGRTMIN + 4


void *
thread_select_sender(void * unused)
{
    int        exit_status = 107;
    mqd_t      mq_desc1    = 0;
    int        ret         = -1;
    message_t  msg_to_send;
    sigset_t   sigset;
    int        sig   = 0;
    int        quit  = 0;
    static int count = 0;

    mq_desc1 = mq_open("/my_mq", POSIX_O_WRONLY | POSIX_O_EXCL);
    TF_ASSERT(mq_desc1 == 4);

    sigemptyset(&sigset);
    sigaddset(&sigset, USER_SIG_01);
    sigaddset(&sigset, USER_SIG_02);
    sigaddset(&sigset, USER_SIG_03);
    sigaddset(&sigset, USER_SIG_04);
    sigaddset(&sigset, USER_SIG_EXIT);

    msg_to_send.payload = 100;

    ret = mq_send(mq_desc1, (char*) &msg_to_send, sizeof(msg_to_send), 0);
    TF_ASSERT(ret == 0);

    while (quit == 0)
    {
        TF_MSG("sender: before sigwait %d times\n", count++);

        /*Block till signal comes in */
        sigwait(&sigset, &sig);

        TF_MSG("sender: returned from sigwait %d times\n", count);

        if (sig == USER_SIG_01)
        {
            pthread_kill(pthread_receiver, USER_SIG_02);
            TF_MSG("sender: pthread_kill USER_SIG_02.\n");
        }
        else if (sig == USER_SIG_02)
        {
            pthread_kill(pthread_receiver, USER_SIG_03);
            TF_MSG("sender: pthread_kill USER_SIG_03.\n");
        }
        else if (sig == USER_SIG_03)
        {
            pthread_kill(pthread_receiver, USER_SIG_04);
            TF_MSG("sender: pthread_kill USER_SIG_04.\n");
        }
        else if (sig == USER_SIG_04)
        {
            pthread_kill(pthread_receiver, USER_SIG_EXIT);
            TF_MSG("sender: pthread_kill USER_SIG_EXIT.\n");
        }
        else if (sig == USER_SIG_EXIT)
        {
            quit = 1;
        }
    }

    ret = mq_close(mq_desc1);
    TF_MSG("sender: mq_close\n");
    
    TF_ASSERT(ret == 0);

    pthread_exit((void *) exit_status);
    return NULL;
}

void sig_handler_01(int unused)
{
    //pthread_kill(pthread_sender, USER_SIG_01);
}
void sig_handler_02(int unused)
{
    pthread_kill(pthread_sender, USER_SIG_02);
}
void sig_handler_03(int unused)
{
    pthread_kill(pthread_sender, USER_SIG_03);
}
void sig_handler_04(int unused)
{
    pthread_kill(pthread_sender, USER_SIG_04);
}
static int receiver_quit = 0;
void sig_handler_exit(int unused)
{
    receiver_quit = 1;
    pthread_kill(pthread_sender, USER_SIG_EXIT);
}

void *
thread_select_receiver(void * unused)
{
    int              exit_status = 108;
    mqd_t            mq1         = 0;
    int              status, max_fd;
    message_t        msg_recvd;
    int              ret;
    fd_set           mq_set;
    sigset_t         sigset;
    uint32_t         priority;
    static int       count            = 0;
    unsigned long long start_peg, end_peg;
    
    struct sigaction sa_user_sig_01   = { sig_handler_01, 0x0, 0x0, NULL };
    struct sigaction sa_user_sig_02   = { sig_handler_02, 0x0, 0x0, NULL };
    struct sigaction sa_user_sig_03   = { sig_handler_03, 0x0, 0x0, NULL };
    struct sigaction sa_user_sig_04   = { sig_handler_04, 0x0, 0x0, NULL };
    struct sigaction sa_user_sig_exit = { sig_handler_exit, 0x0, 0x0, NULL };

    mq1 = mq_open("/my_mq", POSIX_O_WRONLY | POSIX_O_EXCL);
    TF_ASSERT(mq1 == 4);

    max_fd = mq1 + 1;

    // Select on both mq's and verify that only mq1 is selected
    FD_ZERO(&mq_set);
    FD_SET(mq1, &mq_set);

    sigemptyset(&sigset);
    sigaddset(&sigset, USER_SIG_01);
    sigaddset(&sigset, USER_SIG_02);
    sigaddset(&sigset, USER_SIG_03);
    sigaddset(&sigset, USER_SIG_04);
    sigaddset(&sigset, USER_SIG_EXIT);

    /* waiting for mq */
    start_peg = qurt_get_core_pcycles();
    status = pselect(max_fd, &mq_set, NULL, NULL, NULL, &sigset);
    end_peg = qurt_get_core_pcycles();
    TF_ASSERT(status == 1);
    TF_ASSERT(FD_ISSET(mq1, &mq_set));
    TF_MSG("recver: pselect TCycles=%d\n", (end_peg-start_peg)/6);

    TF_MSG("recver: wake up from pselect %d times\n", count++);

    status = mq_receive(mq1, (char*) &msg_recvd, sizeof(msg_recvd), (unsigned int *) &priority);
    TF_MSG("recver: wake up from mq_receive %d times\n", 0);
    TF_ASSERT(status == sizeof(msg_recvd));
    TF_ASSERT(100 == msg_recvd.payload);

    /* send sig01 to the other thread */
    pthread_kill(pthread_sender, USER_SIG_01);
    TF_MSG("recver: sent pthread_kill %d times\n", 0);

    /* expecting signal */
    sigaction(USER_SIG_01, &sa_user_sig_01, NULL);
    sigaction(USER_SIG_02, &sa_user_sig_02, NULL);
    sigaction(USER_SIG_03, &sa_user_sig_03, NULL);
    sigaction(USER_SIG_04, &sa_user_sig_04, NULL);
    sigaction(USER_SIG_EXIT, &sa_user_sig_exit, NULL);

    while (receiver_quit == 0)
    {
        TF_MSG("recver: before pselect\n");
        status = pselect(max_fd, &mq_set, NULL, NULL, NULL, &sigset);
        TF_MSG("recver: wake up from pselect %d times\n", count++);

        TF_ASSERT(status == -1);
        TF_ASSERT(errno == EINTR);
    }

    /* send exit signal back to the other thread */
    pthread_kill(pthread_sender, USER_SIG_EXIT);
    TF_MSG("pthread_kill(pthread_sender, USER_SIG_EXIT);\n");

    ret = mq_close(mq1);
    TF_MSG("mq_close\n");
    
    TF_ASSERT(ret == 0);

    //pthread_exit((void *) exit_status);
    return (void*) exit_status;
}

void posix_test_mq_02()
{
    pthread_attr_t     thread_attr;
    struct sched_param param;
    mqd_t              mq_desc1      = 0;
    int                ret           = -1;
    int                tmp           = 0;
    int                * exit_status = &tmp;
    message_t          msg_recvd;
    uint32_t           priority;
    struct timespec    timeout;
    
    /* create queue */
    mq_desc1 = mq_open("/my_mq", POSIX_O_CREAT | POSIX_O_WRONLY | POSIX_O_EXCL, 0600, NULL);

    /* Setup the thread attributes */
    ret = pthread_attr_init(&thread_attr);
    TF_ASSERT(ret == 0);

    param.sched_priority = 20;
    pthread_attr_setschedparam(&thread_attr, &param);
#ifndef T_UNIX
    pthread_attr_setthreadname(&thread_attr, "receiver");
#endif // !T_UNIX
    pthread_create(&pthread_receiver, &thread_attr, thread_select_receiver, NULL);

    param.sched_priority = 10;
    pthread_attr_setschedparam(&thread_attr, &param);
#ifndef T_UNIX
    pthread_attr_setthreadname(&thread_attr, "sender");
#endif // !T_UNIX
    pthread_create(&pthread_sender, &thread_attr, thread_select_sender, NULL);

    ret = pthread_join(pthread_sender, (void**) &exit_status);
    TF_ASSERT(ret == 0);
    TF_ASSERT(exit_status == (void*) 107);
    TF_MSG("join 1\n");

    ret = pthread_join(pthread_receiver, (void**) &exit_status);
    TF_ASSERT(ret == 0);
    TF_ASSERT(exit_status == (void*) 108);
    TF_MSG("join 2\n");

    timeout.tv_sec  = 0;
    timeout.tv_nsec = 100000;

    /* test timedreceive */
    TF_MSG("testing mq_timedreceive\n");
    ret = mq_timedreceive(mq_desc1, (char*) &msg_recvd, sizeof(msg_recvd), (unsigned int *) &priority, &timeout);
    TF_ASSERT(ret == -1);
    TF_ASSERT(errno == ETIMEDOUT);
    TF_MSG("mq_timedreceive\n");
   
    ret = mq_close(mq_desc1);
    TF_ASSERT(ret == 0);
    TF_MSG("mq_close\n");

    ret = mq_unlink("/my_mq");
    TF_ASSERT(ret == 0);
    TF_MSG("mq_unlink\n");
}

pthread_t pthread_select;
void * thread_select(void * unused)
{
    // declaring message queue variable for this test
    mqd_t          mq1, mq2;
    message_t      msg_to_send, msg_recvd;
    int            status, max_fd;
    uint32_t       priority;
    struct timeval no_to;
    fd_set         mq_set;
    int            exit_status = 109;

    no_to.tv_sec  = 0;
    no_to.tv_usec = 0;

    FD_ZERO(&mq_set);

    //defining mq attributes
    struct mq_attr attr;
    memset(&attr, 0, sizeof(attr));
    attr.mq_maxmsg  = MQ_MAXMSG;
    attr.mq_msgsize = sizeof(message_t);

    // ------------------------------------------------------------------ //
    // Create/Open both message queues
    mq1 = mq_open(MY_MQ_NAME, POSIX_O_RDWR | POSIX_O_CREAT | POSIX_O_EXCL, MY_MQ_PERMS, &attr);
    TF_MSG("mq open(POSIX_O_RDWR|POSIX_O_CREAT|POSIX_O_EXCL -> mqd_t = %d, errno = %d", mq1, errno);
    TF_ASSERT(mq1 != (mqd_t) -1);

    mq2 = mq_open(MY_MQ_NAME2, POSIX_O_RDWR | POSIX_O_CREAT | POSIX_O_EXCL, MY_MQ_PERMS, &attr);
    TF_MSG("mq open(POSIX_O_RDWR|POSIX_O_CREAT|POSIX_O_EXCL -> mqd_t = %d, errno = %d", mq2, errno);
    TF_ASSERT(mq2 != (mqd_t) -1);

    max_fd = ((mq1 > mq2) ? mq1 : mq2) + 1;

    // Send a message to mq1
    msg_to_send.payload = 17;
    TF_MSG("Sending message w/ payload(%d) to mq1(%d)", msg_to_send.payload, mq1);
    TF_ASSERT(mq_send(mq1, (char*) &msg_to_send, sizeof(msg_to_send), MY_MQ_PRIORITY) == 0);

    // Select on both mq's and verify that only mq1 is selected
    FD_ZERO(&mq_set);
    FD_SET(mq1, &mq_set);
    FD_SET(mq2, &mq_set);
    TF_MSG("Blocking on select()");
    status = select(max_fd, &mq_set, NULL, NULL, NULL);
    TF_MSG("status= %d\n", status);
    TF_ASSERT(status == 1);
    TF_ASSERT(FD_ISSET(mq1, &mq_set));
    TF_ASSERT(!FD_ISSET(mq2, &mq_set));

    // Select again and verify that mq1 is still selected
    FD_ZERO(&mq_set);
    FD_SET(mq1, &mq_set);
    FD_SET(mq2, &mq_set);
    TF_MSG("Blocking again on select()");
    status = select(max_fd, &mq_set, NULL, NULL, NULL);
    TF_ASSERT(status == 1);
    TF_ASSERT(FD_ISSET(mq1, &mq_set));
    TF_ASSERT(!FD_ISSET(mq2, &mq_set));

    // Receive the message off of mq1
    TF_MSG("Receiving message off of mq1");
    status = mq_receive(mq1, (char*) &msg_recvd, sizeof(msg_recvd), (unsigned int *) &priority);
    TF_ASSERT(status == sizeof(msg_to_send));
    TF_ASSERT(msg_to_send.payload == msg_recvd.payload);

    // Select again and verify that neither mq1 or mq2 is still selected
    FD_ZERO(&mq_set);
    FD_SET(mq1, &mq_set);
    FD_SET(mq2, &mq_set);
    TF_MSG("Checking with select()");
    status = select(max_fd, &mq_set, NULL, NULL, &no_to);
    TF_ASSERT(status == 0);
    TF_ASSERT(!FD_ISSET(mq1, &mq_set));
    TF_ASSERT(!FD_ISSET(mq2, &mq_set));

    // Send a message to mq2
    msg_to_send.payload = 13;
    TF_MSG("Sending message w/ payload(%d) to mq2(%d)", msg_to_send.payload, mq2);
    TF_ASSERT(mq_send(mq2, (char*) &msg_to_send, sizeof(msg_to_send), MY_MQ_PRIORITY) == 0);

    // Select on both mq's and verify that only mq2 is selected
    FD_ZERO(&mq_set);
    FD_SET(mq1, &mq_set);
    FD_SET(mq2, &mq_set);
    TF_MSG("Blocking on select(mq1,mq2), max_fd(%d)", max_fd);
    status = select(max_fd, &mq_set, NULL, NULL, NULL);
    TF_ASSERT(status == 1);
    TF_ASSERT(!FD_ISSET(mq1, &mq_set));
    TF_ASSERT(FD_ISSET(mq2, &mq_set));

    // Select again and verify that mq2 is still selected
    FD_ZERO(&mq_set);
    FD_SET(mq1, &mq_set);
    FD_SET(mq2, &mq_set);
    TF_MSG("Blocking again on select(mq1,mq2)");
    status = select(max_fd, &mq_set, NULL, NULL, NULL);
    TF_ASSERT(status == 1);
    TF_ASSERT(!FD_ISSET(mq1, &mq_set));
    TF_ASSERT(FD_ISSET(mq2, &mq_set));

    // Receive the message off of mq2
    TF_MSG("Receiving message off of mq2");
    status = mq_receive(mq2, (char*) &msg_recvd, sizeof(msg_recvd), (unsigned int *) &priority);
    TF_ASSERT(status == sizeof(msg_to_send));
    TF_ASSERT(msg_to_send.payload == msg_recvd.payload);

    // Select again and verify that neither mq1 or mq2 is still selected
    FD_ZERO(&mq_set);
    FD_SET(mq1, &mq_set);
    FD_SET(mq2, &mq_set);
    TF_MSG("Checking with select()");
    status = select(max_fd, &mq_set, NULL, NULL, &no_to);
    TF_ASSERT(status == 0);
    TF_ASSERT(!FD_ISSET(mq1, &mq_set));
    TF_ASSERT(!FD_ISSET(mq2, &mq_set));

    // Send a message to both mq1 and mq2
    msg_to_send.payload = 19;
    TF_MSG("Sending message w/ payload(%d) to mq1(%d)", msg_to_send.payload, mq1);
    TF_ASSERT(mq_send(mq1, (char*) &msg_to_send, sizeof(msg_to_send), MY_MQ_PRIORITY) == 0);

    msg_to_send.payload = 11;
    TF_MSG("Sending message w/ payload(%d) to mq2(%d)", msg_to_send.payload, mq2);
    TF_ASSERT(mq_send(mq2, (char*) &msg_to_send, sizeof(msg_to_send), MY_MQ_PRIORITY) == 0);

    // Select on both mq's and verify that both are selected
    FD_ZERO(&mq_set);
    FD_SET(mq1, &mq_set);
    FD_SET(mq2, &mq_set);
    TF_MSG("Blocking on select(mq1,mq2), max_fd(%d)", max_fd);
    status = select(max_fd, &mq_set, NULL, NULL, NULL);
    TF_ASSERT(status == 2);
    TF_ASSERT(FD_ISSET(mq1, &mq_set));
    TF_ASSERT(FD_ISSET(mq2, &mq_set));

    // Select again and verify that both are still selected
    FD_ZERO(&mq_set);
    FD_SET(mq1, &mq_set);
    FD_SET(mq2, &mq_set);
    TF_MSG("Blocking again on select(mq1,mq2)");
    status = select(max_fd, &mq_set, NULL, NULL, NULL);
    TF_ASSERT(status == 2);
    TF_ASSERT(FD_ISSET(mq1, &mq_set));
    TF_ASSERT(FD_ISSET(mq2, &mq_set));

    // Receive the message off of mq1
    TF_MSG("Receiving message off of mq1");
    status = mq_receive(mq1, (char*) &msg_recvd, sizeof(msg_recvd), (unsigned int *) &priority);
    TF_ASSERT(status == sizeof(msg_to_send));
    TF_ASSERT(msg_recvd.payload == 19);

    // Select to verify only mq2 is still selected
    FD_ZERO(&mq_set);
    FD_SET(mq1, &mq_set);
    FD_SET(mq2, &mq_set);
    TF_MSG("Blocking again on select(mq1,mq2)");
    status = select(max_fd, &mq_set, NULL, NULL, NULL);
    TF_ASSERT(status == 1);
    TF_ASSERT(!FD_ISSET(mq1, &mq_set));
    TF_ASSERT(FD_ISSET(mq2, &mq_set));

    // Receive the message off of mq2
    TF_MSG("Receiving message off of mq2");
    status = mq_receive(mq2, (char*) &msg_recvd, sizeof(msg_recvd), (unsigned int *) &priority);
    TF_ASSERT(status == sizeof(msg_to_send));
    TF_ASSERT(msg_recvd.payload == 11);

    // Select again and verify that both mq1 and mq2 are not selected
    FD_ZERO(&mq_set);
    FD_SET(mq1, &mq_set);
    FD_SET(mq2, &mq_set);
    TF_MSG("Checking with select()");
    status = select(max_fd, &mq_set, NULL, NULL, &no_to);
    TF_ASSERT(status == 0);
    TF_ASSERT(!FD_ISSET(mq1, &mq_set));
    TF_ASSERT(!FD_ISSET(mq2, &mq_set));

    // to clear as well as delete the mqd_t set
    FD_ZERO(&mq_set);

    // Closing and Unlinking the message queue
    TF_ASSERT(mq_close(mq1) == 0);
    TF_ASSERT(mq_unlink(MY_MQ_NAME) == 0);
    TF_ASSERT(mq_close(mq2) == 0);
    TF_ASSERT(mq_unlink(MY_MQ_NAME2) == 0);
    // ------------------------------------------------------------------ //

    pthread_exit((void *) exit_status);
    return NULL;
}

void posix_test_mq_03()
{
    pthread_attr_t thread_attr;
    mqd_t          mq_desc1      = 0;
    int            ret           = -1;
    int            tmp           = 0;
    int            * exit_status = &tmp;

    // Setup the thread attributes
    ret = pthread_attr_init(&thread_attr);
    TF_ASSERT(ret == 0);

#ifndef T_UNIX
    pthread_attr_setthreadname(&thread_attr, "select_tester");
#endif // THREAD_ATTR

    pthread_create(&pthread_select, &thread_attr, thread_select, NULL);

    ret = pthread_join(pthread_select, (void**) &exit_status);
    TF_ASSERT(ret == 0);
    TF_ASSERT(exit_status == (void*) 109);
}

#define MSG_NUM    100

static pthread_t rt_sender[3];
pthread_t        rt_receiver[5];
pthread_t        rt_other[5];

static int       sendcycles1[MSG_NUM];
static int       sendcycles2[MSG_NUM];
static int       sendcycles3[MSG_NUM];

static int       pselectcycles[MSG_NUM*3+MSG_NUM];
static int       receivecycles[MSG_NUM*3+MSG_NUM];

static int       quit_others  = 0;
static int       already_sent = 0;

void* rt_receiver_main(void *arg)
{
    static mqd_t       mq1;
    static mqd_t       mq2;
    static mqd_t       mq3;
    int                max_fd;
    fd_set             mq_set, init_set;
    int                ready_queue_no;
    mqd_t              read_queue;
    message_t          msg_recvd;
    unsigned int       priority;
    int                ret;
    int                exit_status = 111;
    sigset_t           sigset;
    int                i = 0;
    struct timespec    timeout;
    unsigned long long start_peg, end_peg;

    //A do nothing line of code to suppress compiler warnings
    (void) arg;
    mq1 = mq_open("/mq1", POSIX_O_WRONLY | POSIX_O_EXCL);
    TF_ASSERT(mq1 != 0 && mq1 != -1);
    mq2 = mq_open("/mq2", POSIX_O_WRONLY | POSIX_O_EXCL);
    TF_ASSERT(mq2 != 0 && mq2 != -1);
    mq3 = mq_open("/mq3", POSIX_O_WRONLY | POSIX_O_EXCL);
    TF_ASSERT(mq3 != 0 && mq3 != -1);

    max_fd = mq3;
    if (mq2 > max_fd) max_fd = mq2;
    if (mq1 > max_fd) max_fd = mq1;

    FD_ZERO(&init_set);
    FD_SET(mq1, &init_set);
    FD_SET(mq2, &init_set);
    FD_SET(mq3, &init_set);

    sigemptyset(&sigset);
    sigaddset(&sigset, USER_SIG_01);

    while (1)
    {
        FD_COPY(&init_set, &mq_set);
        /* waiting for mq */
        start_peg = qurt_get_core_pcycles();
        ready_queue_no = pselect(max_fd + 1, &mq_set, NULL, NULL, NULL, &sigset);
        end_peg = qurt_get_core_pcycles();

        if (i< MSG_NUM*3+MSG_NUM)//try not to overflow the buffer
        {
            pselectcycles[i++] = (end_peg - start_peg) / 6;
        }
        
        TF_ASSERT(ready_queue_no != 0);

        /* any signal will break the while loop */
        if (ready_queue_no == -1)
        {
            break;
        }

        if (FD_ISSET(mq1, &mq_set))
        {
            read_queue = mq1;
        }
        else if (FD_ISSET(mq2, &mq_set))
        {
            read_queue = mq2;
        }
        else if (FD_ISSET(mq3, &mq_set))
        {
            read_queue = mq3;
        }
        else
        {
            TF_ASSERT(0);
        }
        //TF_MSG ("received msg from queue %d\n", read_queue);
        start_peg = qurt_get_core_pcycles();
        //ret = mq_receive(read_queue, (char*) &msg_recvd, sizeof(msg_recvd), (unsigned int *) &priority);

        timeout.tv_sec  = 0;
        timeout.tv_nsec = 0;
        ret = mq_timedreceive(read_queue, (char*) &msg_recvd, sizeof(msg_recvd), (unsigned int *) &priority, &timeout);

        end_peg = qurt_get_core_pcycles();
        if (i< MSG_NUM*3+MSG_NUM)
        {
            if(ret == sizeof(msg_recvd))
                receivecycles[i] = (end_peg - start_peg) / 6;
            else
                receivecycles[i] = 0;
        }

        /* exit when reading the last msg */
        if ((MSG_NUM - 1) == msg_recvd.payload)
        {
            quit_others = 1;
            if (!already_sent)
            {
                already_sent = 1;
                for (i = 0; i < 5; i++)
                {
                    pthread_kill(rt_receiver[i], USER_SIG_01);
                }
            }
        }
    }
    ret = mq_close(mq1);
    TF_ASSERT(ret == 0);
    ret = mq_close(mq2);
    TF_ASSERT(ret == 0);
    ret = mq_close(mq3);
    TF_ASSERT(ret == 0);

    //pthread_exit((void *) exit_status);
    return (void*) exit_status;
}

void* rt_sender_main(void *arg)
{
    mqd_t              mq1 = 0;
    mqd_t              mq2 = 0;
    mqd_t              mq3 = 0;
    message_t          msg_to_send;
    int                i;
    int                ret;
    int                sleep_ms;
    int                exit_status = 110;
    unsigned long long start_peg, end_peg;

    switch (*(int*) arg)
    {
    case 1: sleep_ms  = 1; break;
    case 2: sleep_ms  = 1; break;
    case 3: sleep_ms  = 1; break;
    default: sleep_ms = 1;
    }

    mq1 = mq_open("/mq1", POSIX_O_WRONLY | POSIX_O_EXCL);
    TF_ASSERT(mq1 != 0 && mq1 != -1);
    mq2 = mq_open("/mq2", POSIX_O_WRONLY | POSIX_O_EXCL);
    TF_ASSERT(mq2 != 0 && mq2 != -1);
    mq3 = mq_open("/mq3", POSIX_O_WRONLY | POSIX_O_EXCL);
    TF_ASSERT(mq3 != 0 && mq3 != -1);

    for (i = 0; i < MSG_NUM; i++)
    {
        msg_to_send.payload = i;

        /* send msg to each mq */
        start_peg = qurt_get_core_pcycles();
        ret       = mq_send(mq1, (char*) &msg_to_send, sizeof(msg_to_send), 0);
        end_peg   = qurt_get_core_pcycles();
        if (ret == 0)
            sendcycles1[i] = (end_peg - start_peg) / 6;
        //TF_ASSERT( ret == 0 );
        TF_USLEEP(150);

        start_peg = qurt_get_core_pcycles();
        ret       = mq_send(mq2, (char*) &msg_to_send, sizeof(msg_to_send), 0);
        end_peg   = qurt_get_core_pcycles();
        if (ret == 0)
            sendcycles2[i] = (end_peg - start_peg) / 6;
        //TF_ASSERT( ret == 0 );
        TF_USLEEP(150);

        start_peg = qurt_get_core_pcycles();
        ret       = mq_send(mq3, (char*) &msg_to_send, sizeof(msg_to_send), 0);
        end_peg   = qurt_get_core_pcycles();
        if (ret == 0)
            sendcycles3[i] = (end_peg - start_peg) / 6;
        //TF_ASSERT( ret == 0 );

        TF_SLEEP(sleep_ms);
    }

    ret = mq_close(mq1);
    TF_ASSERT(ret == 0);
    ret = mq_close(mq2);
    TF_ASSERT(ret == 0);
    ret = mq_close(mq3);
    TF_ASSERT(ret == 0);

    pthread_exit((void *) exit_status);
    return NULL;
}

void* rt_other_main(void *arg)
{
    int sleep_ms;
    int exit_status = 112;
    int thread_no;

    thread_no = *(int*) arg;

    switch (thread_no)
    {
    case 1: sleep_ms  = 2; break;
    case 2: sleep_ms  = 2; break;
    case 3: sleep_ms  = 2; break;
    case 4: sleep_ms  = 4; break;
    default: sleep_ms = 2;
    }

    while (!quit_others)
    {
        TF_MSG("other thread No %d is running\n", thread_no);
        TF_SLEEP(sleep_ms);
    }
    //pthread_exit((void *) exit_status);
    return (void*) exit_status;
}

void posix_test_mq_realtime()
{
    mqd_t              mq_desc1 = 0;
    mqd_t              mq_desc2 = 0;
    mqd_t              mq_desc3 = 0;
    pthread_attr_t     thread_attr;
    struct sched_param param;
    int                ret = -1;
    int                arg;
    struct mq_attr     attr;
    int                i;
    int                tmp           = 0;
    int                * exit_status = &tmp;

    /* create queue */
    memset(&attr, 0, sizeof(attr));
    attr.mq_maxmsg  = 250;
    attr.mq_msgsize = sizeof(message_t);

    mq_desc1 = mq_open("/mq1", POSIX_O_CREAT | POSIX_O_WRONLY | POSIX_O_EXCL, 0600, &attr);
    mq_desc2 = mq_open("/mq2", POSIX_O_CREAT | POSIX_O_WRONLY | POSIX_O_EXCL, 0600, &attr);
    mq_desc3 = mq_open("/mq3", POSIX_O_CREAT | POSIX_O_WRONLY | POSIX_O_EXCL, 0600, &attr);

    for (i = 0; i < MSG_NUM; i++)
    {
        sendcycles1[i] = 0;
        sendcycles2[i] = 0;
        sendcycles3[i] = 0;
    }

    for (i = 0; i < MSG_NUM*4; i++)
    {
        pselectcycles[i] = 0;
        receivecycles[i] = 0;
    }

    /* Setup the thread attributes */
    ret = pthread_attr_init(&thread_attr);
    TF_ASSERT(ret == 0);
    //pthread_attr_setstacksize( &thread_attr, 4096);

    param.sched_priority = 1;
    pthread_attr_setschedparam(&thread_attr, &param);
#ifndef T_UNIX
    pthread_attr_setthreadname(&thread_attr, "Receiver1");
#endif // !T_UNIX
    arg = 1;
    pthread_create(&rt_receiver[0], &thread_attr, rt_receiver_main, (void*) &arg);

    param.sched_priority = 2;
    pthread_attr_setschedparam(&thread_attr, &param);
#ifndef T_UNIX
    pthread_attr_setthreadname(&thread_attr, "Receiver2");
#endif // !T_UNIX
    arg = 2;
    pthread_create(&rt_receiver[1], &thread_attr, rt_receiver_main, (void*) &arg);

    param.sched_priority = 3;
    pthread_attr_setschedparam(&thread_attr, &param);
#ifndef T_UNIX
    pthread_attr_setthreadname(&thread_attr, "Receiver3");
#endif // !T_UNIX
    arg = 3;
    pthread_create(&rt_receiver[2], &thread_attr, rt_receiver_main, (void*) &arg);

    param.sched_priority = 4;
    pthread_attr_setschedparam(&thread_attr, &param);
#ifndef T_UNIX
    pthread_attr_setthreadname(&thread_attr, "Receiver4");
#endif // !T_UNIX
    arg = 4;
    pthread_create(&rt_receiver[3], &thread_attr, rt_receiver_main, (void*) &arg);

    param.sched_priority = 5;
    pthread_attr_setschedparam(&thread_attr, &param);
#ifndef T_UNIX
    pthread_attr_setthreadname(&thread_attr, "Receiver5");
#endif // !T_UNIX
    arg = 5;
    pthread_create(&rt_receiver[4], &thread_attr, rt_receiver_main, (void*) &arg);

    param.sched_priority = 11;
    pthread_attr_setschedparam(&thread_attr, &param);
#ifndef T_UNIX
    pthread_attr_setthreadname(&thread_attr, "Other1");
#endif // !T_UNIX
    arg = 1;
    pthread_create(&rt_other[0], &thread_attr, rt_other_main, (void*) &arg);

    param.sched_priority = 12;
    pthread_attr_setschedparam(&thread_attr, &param);
#ifndef T_UNIX
    pthread_attr_setthreadname(&thread_attr, "Other2");
#endif // !T_UNIX
    arg = 2;
    pthread_create(&rt_other[1], &thread_attr, rt_other_main, (void*) &arg);

    param.sched_priority = 13;
    pthread_attr_setschedparam(&thread_attr, &param);
#ifndef T_UNIX
    pthread_attr_setthreadname(&thread_attr, "Other3");
#endif // !T_UNIX
    arg = 3;
    pthread_create(&rt_other[2], &thread_attr, rt_other_main, (void*) &arg);

    param.sched_priority = 14;
    pthread_attr_setschedparam(&thread_attr, &param);
#ifndef T_UNIX
    pthread_attr_setthreadname(&thread_attr, "Other4");
#endif // !T_UNIX
    arg = 4;
    pthread_create(&rt_other[3], &thread_attr, rt_other_main, (void*) &arg);

    param.sched_priority = 15;
    pthread_attr_setschedparam(&thread_attr, &param);
#ifndef T_UNIX
    pthread_attr_setthreadname(&thread_attr, "Other5");
#endif // !T_UNIX
    arg = 5;
    pthread_create(&rt_other[4], &thread_attr, rt_other_main, (void*) &arg);

    param.sched_priority = 21;
    pthread_attr_setschedparam(&thread_attr, &param);
#ifndef T_UNIX
    pthread_attr_setthreadname(&thread_attr, "Sender1");
#endif // !T_UNIX
    arg = 1;
    pthread_create(&rt_sender[0], &thread_attr, rt_sender_main, (void*) &arg);

    param.sched_priority = 22;
    pthread_attr_setschedparam(&thread_attr, &param);
#ifndef T_UNIX
    pthread_attr_setthreadname(&thread_attr, "Sender2");
#endif // !T_UNIX
    arg = 2;
    pthread_create(&rt_sender[1], &thread_attr, rt_sender_main, (void*) &arg);

    param.sched_priority = 23;
    pthread_attr_setschedparam(&thread_attr, &param);
#ifndef T_UNIX
    arg = 3;
    pthread_attr_setthreadname(&thread_attr, "Sender3");
#endif // !T_UNIX
    pthread_create(&rt_sender[2], &thread_attr, rt_sender_main, (void*) &arg);

    ret = mq_close(mq_desc1);
    TF_ASSERT(ret == 0);
    ret = mq_close(mq_desc2);
    TF_ASSERT(ret == 0);
    ret = mq_close(mq_desc3);
    TF_ASSERT(ret == 0);


    /* tear down */

    for (i = 0; i < 3; i++)
    {
        ret = pthread_join(rt_sender[i], (void**) &exit_status);
        TF_ASSERT(ret == 0);
        TF_ASSERT(exit_status == (void*) 110);
    }
    for (i = 0; i < 5; i++)
    {
        pthread_join(rt_receiver[i], (void**) &exit_status);
        TF_ASSERT(ret == 0);
        TF_ASSERT(exit_status == (void*) 111);
    }
    for (i = 0; i < 5; i++)
    {
        pthread_join(rt_other[i], (void**) &exit_status);
        TF_ASSERT(ret == 0);
        TF_ASSERT(exit_status == (void*) 112);
    }

    ret = mq_unlink("/mq1");
    TF_ASSERT(ret == 0);
    ret = mq_unlink("/mq2");
    TF_ASSERT(ret == 0);
    ret = mq_unlink("/mq3");
    TF_ASSERT(ret == 0);

    TF_MSG("POSIX MQ send1 Cycles: \n");
    for (i = 0; i < MSG_NUM; i++)
    {
        TF_MSG("%d, ", sendcycles1[i]);
    }
    TF_MSG("\n");
    TF_MSG("POSIX MQ send2 Cycles: \n");
    for (i = 0; i < MSG_NUM; i++)
    {
        TF_MSG("%d, ", sendcycles2[i]);
    }
    TF_MSG("\n");
    TF_MSG("POSIX MQ send3 Cycles: \n");
    for (i = 0; i < MSG_NUM; i++)
    {
        TF_MSG("%d, ", sendcycles3[i]);
    }
    TF_MSG("\n");

    TF_MSG("POSIX MQ pselect Cycles: \n");
    for (i = 0; i < MSG_NUM*4; i++)
    {
        TF_MSG("%d, ", pselectcycles[i]);
    }
    TF_MSG("\n");

    TF_MSG("POSIX MQ receive Cycles: \n");
    for (i = 0; i < MSG_NUM*4; i++)
    {
        TF_MSG("%d, ", receivecycles[i]);
    }
    TF_MSG("\n");    
}

typedef fd_set AudMqMask;
typedef mqd_t AudMq;
typedef pthread_t AudThread;
typedef atomic_word_t AudAtomicWord_t;

static const int g_threadPriority = 100;
typedef struct
{
   AudMq cmdQ;
   AudMq dataQ;
   AudMq bufQ;
   AudAtomicWord_t anRxDataCnt;
   AudAtomicWord_t anRxBufCnt;
   int nRxThreads;
   int nTotalMsgs;
} PselectThis_t;

AudAtomicWord_t totalRxBufCnt={0};
AudAtomicWord_t totalRxDataCnt={0};

// allocate instance
PselectThis_t me_struct;
unsigned int thread_datamsg_cnt[100];
unsigned int thread_bufmsg_cnt[100];

#define AUD_FAILED  

#define AudMqMaskInit  FD_ZERO
#define AudMqAddToMask FD_SET
#define AudMqCheckMask FD_ISSET
#define STD_MAX(a,b)   ((a)>(b)?(a):(b))
#define AudAtomicIncrement atomic_inc_return
#define AudAtomicDerement  atomic_dec_return

static inline int AudMqWaitOnMask(int nMaxDescPlusOne, AudMqMask* pMask)
{
   sigset_t nEmptySig = 0;
   return pselect(nMaxDescPlusOne, pMask, NULL, NULL, NULL, &nEmptySig);
}

static inline void AudThreadJoin(AudThread nTid, int* nStatus)
{
   // note- this double-pointer cast is actually correct, if you look at pthread_join...
   *nStatus = 0;
   (void) pthread_join(nTid, (void**)nStatus);
}

static inline int AudMqClose(AudMq nDesc)
{
   return (mq_close(nDesc) ? errno : 0);
}

static inline int AudMqUnlink(char* pzName)
{
   return (mq_unlink(pzName) ? errno : 0);
}

static inline int AudMqSend(AudMq nDesc, void* pPayload, int nPayloadSize, int nPrio)
{
   return (mq_send(nDesc, (const char *)pPayload, nPayloadSize, nPrio) ? errno : 0);
}

static inline int AudMqReceive(AudMq nDesc, void* pPayload, int nPayloadSize, int *nActualSize)
{
   *nActualSize = mq_receive(nDesc, (char*)pPayload, nPayloadSize, NULL);
   //TF_MSG("mq=%d, payload=0x%x\n", nDesc, (int)pPayload);
   return (-1 == *nActualSize ? (EAGAIN == errno ? 47 : 1) : 0);
}

int AudThreadLaunch(AudThread *pTid, char *pzThreadName, size_t nStackSize, int nPriority,
      int (*pfStartRoutine)(void *), void *arg)
{
    pthread_attr_t  tAttr;
    int             tStatus;
    sched_param     tSchedParam;

    tSchedParam.sched_priority = nPriority;

    // Create thread with attributes. Note that setthreadname is a QCOM extension.
    // May not work on other POSIX implementations.
    TF_MSG("Creating Thread: %s\n", pzThreadName);
    if (AUD_FAILED(tStatus = pthread_attr_init(&tAttr))
       || AUD_FAILED (tStatus = pthread_attr_setthreadname(&tAttr, pzThreadName))
       || AUD_FAILED (tStatus = pthread_attr_setstacksize(&tAttr, nStackSize))
       || AUD_FAILED (tStatus = pthread_attr_setschedparam(&tAttr, &tSchedParam))
       || AUD_FAILED (tStatus = pthread_create((AudThread *)pTid, &tAttr,
       		(void*(*)(void*))pfStartRoutine, arg)))
    {
       TF_MSG("Thread Creation Failed!! \n");
       return tStatus;
    }
    return 0;
}

int AudMqCreate(char *pzMqName, int nMaxSize, int nMaxMsgs, int fBlocking, AudMq *mqDesc)
{
    struct mq_attr    newAttr;

    memset( &newAttr, 0, sizeof(newAttr) );
    int oflag = POSIX_O_CREAT | POSIX_O_RDWR | (fBlocking ? 0 : POSIX_O_NONBLOCK);

    newAttr.mq_maxmsg   = nMaxMsgs;
    newAttr.mq_msgsize  = nMaxSize;
    newAttr.mq_flags    = oflag;

    TF_MSG("Creating Message Queue: %s\n", pzMqName);

    *mqDesc = mq_open(pzMqName, oflag, 0777, &newAttr);

    if (-1 == *((int*)mqDesc))
    {
       TF_MSG("Could not create queue, POSIX errno = %d!!\n", errno);
       return errno;
    }

    TF_MSG("Created MQ %s, descriptor = %d\n", pzMqName, *mqDesc);

    return 0;
}

int AudMqAttach(char *pzMqName, AudMq *mqDesc)
{
    struct mq_attr    newAttr;

    memset( &newAttr, 0, sizeof(newAttr) );
    int oflag = POSIX_O_RDWR;

    *mqDesc = mq_open(pzMqName, oflag, 0777, NULL);

    if (-1 == (int) *mqDesc)
    {
       TF_MSG("Could not find queue, POSIX errno = %d!!\n", errno);
       return errno;
    }

    TF_MSG("Attached to MQ %s, descriptor = %d\n", pzMqName, *mqDesc);

    return 0;
}


static int PselectSender(void* arg)
{
   PselectThis_t *me = (PselectThis_t*)&me_struct;
   int result, i, j;

   TF_MSG("PselectSender thread started...\n");

   for (i=0; i< me->nTotalMsgs / me->nRxThreads / 2; i++)
   {
      int nPayload;

      // send address of mq as payload, to ensure there are no msgs ending up in wrong q's.
      nPayload = (int) (&(me->dataQ));
      for (j = 0; j < me->nRxThreads; j++)
      {
         while (AUD_FAILED(result = AudMqSend(me->dataQ, &nPayload, sizeof(nPayload), 0)))
         {
             TF_MSG("PselectSender thread encountered AudMqSend error = %d!!. Probably overflowed the queue.\n", result);
             //break;
             //return(result);
             TF_USLEEP(100);
         }
      }

      TF_USLEEP(100);
      
      nPayload = (int) (&(me->bufQ));
      for (j = 0; j < me->nRxThreads; j++)
      {
         while (AUD_FAILED(result = AudMqSend(me->bufQ, &nPayload, sizeof(nPayload), 0)))
         {
             TF_MSG("PselectSender thread encountered AudMqSend error = %d!!\n", result);
             //break;
             //return(result);
             TF_USLEEP(100);             
         }
      }
   }

   TF_MSG("PselectSender thread exited...\n");
   return 0;
}

static int PselectReceiver(void* arg)
{
   int nSize;
   int nPayload1, nPayload2;
   int result;
   AudMqMask mask, dataMask, bufMask;
   int nDataMaxDesc = 0, nBufMaxDesc = 0;
   int index = (int)arg;

   PselectThis_t *me = (PselectThis_t*)&me_struct;

   // clear MQ masks (no message queues selected)
   AudMqMaskInit(&dataMask);
   AudMqMaskInit(&bufMask);

   // set up masks for listening
   //AudMqAddToMask(me->cmdQ, &dataMask);
   AudMqAddToMask(me->dataQ, &dataMask);

   nDataMaxDesc = STD_MAX((int)(me->cmdQ), (int)(me->dataQ)) + 1;

   //AudMqAddToMask(me->cmdQ, &bufMask);
   AudMqAddToMask(me->bufQ, &bufMask);
   nBufMaxDesc = STD_MAX((int)(me->cmdQ), (int)(me->bufQ)) + 1;

   for (;;)
   {
      // if all data msgs have been received, exit
#ifdef TESTOPTION1
      if((me->nTotalMsgs / 2 / me->nRxThreads) == thread_datamsg_cnt[index])
#else      
      if((me->nTotalMsgs / 2) == (me->anRxDataCnt.value))
#endif
      {
         //TF_MSG("Exiting receiver thread successfully after all data bufs received.\n");
         //TF_MSG("totalRxDataCnt=%d\n", totalRxDataCnt.value);
         //TF_MSG("me->anRxDataCnt=%d\n", me->anRxDataCnt.value);
      
         goto waitforbuf;
      }

      mask = dataMask;
      result = AudMqWaitOnMask(nDataMaxDesc, &mask);

      if (1 != result)
      {
         TF_MSG("AudMqWaitOnMask returned wrong number of ready queues = %d!!\n", result);
         TF_ASSERT(0);
         return result;
      }

      //Make sure data msg was the received.
      TF_ASSERT(AudMqCheckMask(me->dataQ, &mask));

      result = AudMqReceive(me->dataQ, &nPayload1, sizeof(nPayload1), &nSize);
      if (result) 
      {
          TF_MSG("Thread %d failed to receive data msg %d: 0x%8x, 0x%8x\n", index, me->anRxDataCnt.value, nPayload1, (int)(&(me->dataQ)));
          //AudAtomicDerement(&(me->anRxDataCnt));
      }
      else
      {
          TF_ASSERT((int)(&(me->dataQ)) == nPayload1);
          TF_ASSERT (4 == nSize);

          AudAtomicIncrement(&(me->anRxDataCnt));
          thread_datamsg_cnt[index]++;
      }

waitforbuf:
      // if all buf msgs have been received, exit
#ifdef TESTOPTION1
      if((me->nTotalMsgs / 2 / me->nRxThreads) == thread_bufmsg_cnt[index])      
#else      
      if((me->nTotalMsgs / 2) == (me->anRxBufCnt.value))
#endif      
      {
         //TF_MSG("Exiting receiver thread successfully after all empty bufs received.\n");
         //TF_MSG("totalRxBufCnt=%d\n", totalRxBufCnt.value);
         //TF_MSG("me->anRxBufCnt=%d\n", me->anRxBufCnt.value);
         
         goto check_data_and_buf;
      }

      mask = bufMask;
      result = AudMqWaitOnMask(nBufMaxDesc, &mask);

      if (1 != result)
      {
          TF_MSG("AudMqWaitOnMask returned wrong number of ready queues = %d!!\n", result);
          TF_ASSERT(0);
          return result;
      }

      //Make sure buf msg was the received.
      TF_ASSERT(AudMqCheckMask(me->bufQ, &mask));

      result = AudMqReceive(me->bufQ, &nPayload2, sizeof(nPayload2), &nSize);
      if (result) 
      {
          TF_MSG("Thread %d failed to receive buf msg %d: result=%d\n", index, me->anRxBufCnt.value, result);
          //AudAtomicDerement(&(me->anRxBufCnt));
      }
      else
      {
          TF_ASSERT((int)(&(me->bufQ)) == nPayload2);
          TF_ASSERT (4 == nSize);
          
          AudAtomicIncrement(&(me->anRxBufCnt));
          thread_bufmsg_cnt[index]++;
      }

check_data_and_buf:

#ifdef TESTOPTION1
      TF_MSG("thread_datamsg_cnt[%d]=%d, thread_bufmsg_cnt[%d]=%d\n", index, thread_datamsg_cnt[index], index, thread_bufmsg_cnt[index]);
      if( ((me->nTotalMsgs / 2 / me->nRxThreads) == thread_datamsg_cnt[index]) && 
          ((me->nTotalMsgs / 2 / me->nRxThreads) == thread_bufmsg_cnt[index]) )
#else      
      TF_MSG("thread %d: anRxBufCnt=%d, anRxDataCnt=%d\n", index, me->anRxBufCnt.value, me->anRxDataCnt.value);
      if(((me->nTotalMsgs / 2) == (me->anRxBufCnt.value)) && ((me->nTotalMsgs / 2) == (me->anRxDataCnt.value)))
#endif          
      {
          TF_MSG("Exiting rx thread %d\n", index);
          return 0;
      }
   }
}

/* this test case came from Audio team */
int posix_test_mq_audio()
{
    int j;

    char* pzCmd="AudTstCmd";
    char* pzData="AudTstData";
    char* pzBuf="AudTstBuf";
    const int fBlocking = 0;
    int result;

    // set up atomic counters to count total received msgs.
    if (0 != atomic_set(&(me_struct.anRxDataCnt), 0)
    || 0 != atomic_set(&(me_struct.anRxBufCnt), 0))
    {
       TF_MSG("Failed to initialize atomic counter\n");
       TF_ASSERT(0);
       return -1;
    }

    // configure test for 10 Rx threads and 1000 total messages
    me_struct.nRxThreads = 10;
    me_struct.nTotalMsgs = 1000;
    for (j=0; j<me_struct.nRxThreads ; j++)
    {
       thread_datamsg_cnt[j]=0;
       thread_bufmsg_cnt[j] =0;
    }

    AudThread tSender, tReceiver[me_struct.nRxThreads];

    // Create blocking queues, write needs to block if full
    if (  AUD_FAILED(result = AudMqCreate(pzCmd,  4, 50, fBlocking, &me_struct.cmdQ))
       || AUD_FAILED(result = AudMqCreate(pzData, 4, 50, fBlocking, &me_struct.dataQ))
       || AUD_FAILED(result = AudMqCreate(pzBuf,  4, 50, fBlocking, &me_struct.bufQ)))
    {
       TF_MSG("AUD_FAILED to create mq's with result = %d!!\n", result);
       TF_ASSERT(0);       
       return(result);
    }

    // launch Rx threads
    for (j = 0; j < me_struct.nRxThreads; j++)
    {
       if (AUD_FAILED (result = AudThreadLaunch(&tReceiver[j], "PselectReceiver", 4096, g_threadPriority + j, PselectReceiver, (void*)j)))
       {
          TF_MSG("AUD_FAILED to launch threads with result = %d!!\n", result);
          TF_ASSERT(0);
          return(result);
       }
    }

    if (AUD_FAILED (result = AudThreadLaunch(&tSender, "PselectSender", 4096, g_threadPriority +j -10, PselectSender, (void*)&me_struct)))
    {
       TF_MSG("AUD_FAILED to launch threads with result = %d!!\n", result);
       TF_ASSERT(0);
       return(result);
    }

    // wait for end
    AudThreadJoin(tSender, &result);
    if (AUD_FAILED (result))
    {
       TF_MSG("joining thread returned failure code = %d!!\n", result);
       TF_ASSERT(0);
       return result;
    }
    for (j = 0; j < me_struct.nRxThreads; j++)
    {
       AudThreadJoin(tReceiver[j], &result);

       if (AUD_FAILED (result))
       {
          TF_MSG("PselectReceiver returned failure code = %d!!\n", result);
          TF_ASSERT(0);
          return result;
       }
    }

    if (AUD_FAILED(result = AudMqClose(me_struct.cmdQ))
      || AUD_FAILED(result = AudMqClose(me_struct.dataQ))
      || AUD_FAILED(result = AudMqClose(me_struct.bufQ)))
    {
       TF_MSG("Failed to close mq's error = %d!!\n", result);
       TF_ASSERT(0);
       return result;
    }

    if (AUD_FAILED (result = AudMqUnlink(pzCmd))
      || AUD_FAILED (result = AudMqUnlink(pzData))
      || AUD_FAILED (result = AudMqUnlink(pzBuf)))
    {
       TF_MSG("failed to unlink message queue error = %d!!\n", result);
       TF_ASSERT(0);
       return result;
    }

    return 0;
}
