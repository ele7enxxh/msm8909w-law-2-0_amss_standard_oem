/*****************************************************************
# Copyright (c) $Date: 2016/12/13 $ Qualcomm Technologies INCORPORATED.
# All Rights Reserved.
# Modified by Qualcomm Technologies INCORPORATED on $Date: 2016/12/13 $
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "passfail.h"
#include <qube.h>
#include <assert.h>
#include <qinterrupt.h>
#include <qthread.h>
#include <qmutex.h>
#include "qtimer.h"
#include "blast.h"
#include "qthread.h"

#ifndef ITERATIONS
#define ITERATIONS 200
#endif

#if defined(__qdsp6__) || defined(__hexagon__)
#define NOP __asm__ __volatile("nop")
#else
#define NOP // pthread_yield()
#endif

#ifndef null
#define null 0
#endif

#define MAX_TIMERS 5
#define MAX_MSG_SIZE 40
#define MAX_NO_MAGS 100
#define MY_MSG_SIZE 4

static const int DONE = 5678;
static const int  MAX_MUTEX_THREADS = 6;
static const int TID_BASE = 0x20;

qobject_t objQ;
/* Message queue channel */
qch_t    app1_msgq_channel;

/* Message queue owned by this PD */
qmsgq_t  app1_msgQ1;

/* Message queue owned by other PD */
qmsgq_t  app1_msgQ2;

/* Attributes for creating message queue */
qmsgq_attr_t app1_msgq_attr;

/* Interrupt Callback */
qmsgq_callback_t int_callback;

/* Intra-pd and inter-pd mutex */
qmutex_t  app1_intra_mutex;

/* Message type expected */
static const int MY_MSG_TYPE  = 1234;

/* Message type for INTER-PD MUTEX */
static const int MUTEX_MSG_TYPE = 1122;

/* Interrupt callback message type */
static const int MY_CALLBACK_MSG_TYPE  = 1234;

/* Maximum message size in the queue */
static const size_t MY_MAX_MSG_SIZE = 12;

static const int START_VAL = 15;


/* Function proto-types*/
//void Timer_Task1( void *);
void ThreadMain1( void *);
void ThreadMain2( void *);

void isr_callback_fun( void * args); 

/* Function proto-types  */
void Timer_Task1(void *);
static void callback_init ( int timer_index );
static void timer_init ( qtimer_t* timer, int timer_index, unsigned int duration, qtimer_type_t type );
static void timer_cancel ( qtimer_t timer );
static void receive ( int timer_index );
static void no_receive ( int timer_index );
static void delete (int index);
static void timer_setup ( void );

/* message queue channel for timer messages */
static qch_t timer_channel;

/* Message queue for timer messages */
static qmsgq_t timer_msgQ1;

/* Timer tokens */
static qtimer_t timers [ MAX_TIMERS ];

/* Sginals for each timer */
static qurt_anysignal_t timer_signal [ MAX_TIMERS ];

/* Message type for each timer */
static int MSG_TYPE [ MAX_TIMERS ];

/* Pointer to the received message */
static void *timer_recvptr[ MAX_NO_MAGS ]; 

/* Message buffer for timer messages */
static int timer_buf[MAX_TIMERS][MY_MSG_SIZE];

/* Pointer to the message buffer */
static void *timer_ptr;

/* Initialize callback */
static void callback_init ( int timer_index )
{
    qurt_anysignal_init (&timer_signal[timer_index]);
}

/* Initialize timer */
static void timer_init ( qtimer_t* timer, int timer_index, unsigned int duration, qtimer_type_t type )
{
    int i =0;
    qtimer_attr_t attr;

    /* Initialize timer */
    qtimer_attr_init ( &attr );
    qtimer_attr_setduration ( &attr, duration );
    qtimer_attr_settype ( &attr, type );

    /* Send message before any receiver */
    i = sclk_timer_create ( timer, &attr, &timer_signal[timer_index], 0x1);

    fail_if (i != EOK );

}

/* Cancel existing timer */
static void timer_cancel ( qtimer_t timer )
{
    int i =0;

    /* Send message before any receiver */
    i = qtimer_delete ( timer );

    fail_if (i != EOK );

}

/* Receive one message */
static void receive ( int timer_index )
{
   int rc;

   rc = qurt_anysignal_wait (&timer_signal[timer_index], 0x1);
   if (rc != 0x1) {
        printf ("Any Signal wait failed\n" );
        failnow ("Any Signal wait failed\n" );
   }
   qurt_anysignal_clear (&timer_signal[timer_index], 0x1);
 
}

/* Setup signals for timers */
static void timer_setup ( void )
{
    int i = 0;

    /* Setup all callbacks  */
    for ( i =0; i < MAX_TIMERS; i++)
       callback_init ( i );

}

void Timer_Task1(void * args) 
{
  qtimer_t a;
  qtimer_t b;
  qtimer_t c;
  
  qtimer_t timer_array[MAX_TIMERS];
  qtimer_duration_t durations [MAX_TIMERS];

  int i;

  /* Steup Timer Signals*/
  timer_setup ();

  /* Timer test 1 -- Simplicity      */
  /***********************************/

  /* Start Timer */
  timer_init ( &a, 0, 10000, QTIMER_ONESHOT ); //10 Milliseconds

  /* Receive and verify message in location 0 with timer_callback 0 */
  receive ( 0 );

  printf("*** PD1: Test1 Passed Successfully***\n");

  /* Timer test 2 -- Double Trouble  */
  /***********************************/

  /* Start Timers */
  timer_init ( &a, 0, 10000, QTIMER_ONESHOT ); //10 Milliseconds
  timer_init ( &b, 1, 100000, QTIMER_ONESHOT ); //100 Milliseconds

  /* Receive and verify message in location 0 with timer_callback 0 */
  receive ( 0 );
  receive ( 1 );

  
  printf("*** PD1: Test2 Passed Successfully***\n");

  /* Timer test 3 */
  /***********************************/
  /* Start Timers */
  timer_init ( &a, 0, 10000, QTIMER_ONESHOT ); //10 Milliseconds
  timer_init ( &b, 1, 200000, QTIMER_ONESHOT ); //100 Milliseconds
  timer_init ( &c, 2, 100000, QTIMER_ONESHOT ); //100 Milliseconds

  /* Receive and verify message in location 0 with timer_callback 0 */
  receive ( 0 );
  receive ( 2 );
  receive ( 1 );

  printf("*** PD1: Test3 Passed Successfully***\n");
 
  /* Timer test 4 -- Start Stop      */
  /***********************************/

  /* Start Timers */
  timer_init ( &a, 0, 10000, QTIMER_ONESHOT ); //10 Milliseconds
  timer_init ( &b, 1, 100000, QTIMER_ONESHOT ); //100 Milliseconds

  /* Stop Timer */
  timer_cancel ( a );

  /* Verify that message for timer a was not received */
  receive ( 1 );
  //receive ( 0 );
  
  printf("*** PD1: Test4 Passed Successfully***\n");

  /* Timer test 5 -- Start & Start smaller */
  /*****************************************/
  /* Start Timers */
  timer_init ( &a, 0, 10000, QTIMER_ONESHOT ); //10 Milliseconds
  timer_init ( &b, 1, 5000, QTIMER_ONESHOT ); //5 Milliseconds

  /* Verify that message for timer b was received before a*/
  receive ( 1 );
  receive ( 0 );

  printf("*** PD1: Test5 Passed Successfully***\n");

  /* Timer test 6 -- Periodicity     */
  /***********************************/
  /* Start Timers */
  timer_init ( &a, 0, 100000, QTIMER_PERIODIC ); //100 Milliseconds

  /* Cancel timer */
  timer_cancel ( a );

  /* Now ensure it's actually cancelled by setting a ONE_SHOT timer */
  timer_init ( &b, 1, 200000, QTIMER_ONESHOT ); //200 Milliseconds

  receive ( 1 );

  printf("*** PD1: Test6 Passed Successfully***\n");

  /* Timer test 7 -- The Twelve Days of Timers */
  /*********************************************/
  /* Initialize the Timers */
  for( i = 0; i < MAX_TIMERS; i++ ) 
  {
    durations[i] = (i+1) * 20000;
  }

  /* Activate The Timers */
  for( i = 0; i < MAX_TIMERS; i++ ) 
  {
      timer_init ( &timer_array [ i ], i,  durations[i], QTIMER_ONESHOT );
  }

  /* Wait for the Responses */
  for( i = 0; i < MAX_TIMERS; i++ ) 
  {
      receive ( i );
  }

  printf("*** PD1: Test7 Passed Successfully***\n");

  /* Timer test 8 -- Long Timers in reverse order */
  /*********************************************/

    /* Initialize the Timers */
  for( i = MAX_TIMERS; i > 0; i-- ) 
  {
    durations[i-1] = i * 50000;
  }

  /* Activate The Timers */
  for( i = MAX_TIMERS-1; i >=0; i-- ) 
  {
    timer_init ( &timer_array [ i ], i,  durations[i], QTIMER_ONESHOT );
  }

  /* Wait for the Responses */
  for( i = 0; i < MAX_TIMERS; i++ ) 
  {
      receive ( i );
  }
  printf("*** PD1: Test8 Passed Successfully***\n");

}
#if defined(__qdsp6__) || defined(__hexagon__)
void isr_callback_fun( void * args) 
{
	//int status, int_number;
	int status;
	unsigned int int_number;
	status = qinterrupt_register(15);
	fail_if(status != EOK);
	status = qinterrupt_receive(&int_number);
	fail_if(status != EOK);
	if(int_number != 15)
		failnow("Unexpected interrupt :%d received", int_number);
    	qmutex_lock ( app1_intra_mutex );
    	printf( "from isr callback function \n" );
    	qmutex_unlock ( app1_intra_mutex );
}

#endif
/* Thread one */
void ThreadMain1( void * args)
{
	int i = 0;
	int j = 0;
    size_t index;
    
    void *recvptr;
    int msgtype, my_tid;
    unsigned int msgsize;
    uintptr_t sec_base;

    /* Allocate memory in SMI and use it for message queue */
//    qmem_group_t group;
    qmem_region_t region;
    qmem_region_attr_t   mem_attr;

    qthread_attr_t attr;
    qthread_get_attr(qthread_myself(), &attr);
    qthread_attr_gettid(&attr, &my_tid);
    fail_if(my_tid != TID_BASE);

    /* Output from multiple threads can be protected using mutex */
    printf ("Starting Thread 1 tid=%d\n",my_tid);

    /* Create a msgq_channel */
    i = qchannel_create ( &app1_msgq_channel );
    
    fail_if ( EOK != i );
    
    /* Allocate buffer for 4K and use first 320 bytes as message buffer */

    /* Initialize attributes */
    qmem_region_attr_init ( &mem_attr );

    /* Create memory region */
    i = qmem_region_create ( &region, 4096, qmem_default_pool, &mem_attr );

    fail_if ( EOK != i);

    /* Get virtual Address */
    i = qmem_region_get_attr( region, &mem_attr ); 
    fail_if ( EOK != i);
    qmem_region_attr_getvirtaddr( &mem_attr, &sec_base );

    /* Initialize attributes for message queue */
    qmsgq_attr_init ( &app1_msgq_attr );

    /* Initialize all the attributes */
    qmsgq_attr_setchannel ( &app1_msgq_attr, app1_msgq_channel );
    
    qmsgq_attr_setpriority ( &app1_msgq_attr, 20 );
    qmsgq_attr_setbuffer ( &app1_msgq_attr, (void*)sec_base, 400 );
    qmsgq_attr_setmaxmsgsize ( &app1_msgq_attr, MY_MAX_MSG_SIZE );

    /* Create message queue */
    i = qmsgq_create ( "msgQ1", &app1_msgQ1, &app1_msgq_attr );

    fail_if (i != EOK );
   
    /* Initialize interrupt callback */
#if defined(__qdsp6__) || defined(__hexagon__)
	qthread_t isrR;
	qthread_attr_t attrR;

	qthread_attr_init(&attrR);
	qthread_attr_setpriority(&attrR, 99);
	qthread_attr_setname(&attrR, "ISR");

	i = qthread_create(&isrR, &attrR);
	fail_if(i != EOK);
	i = qthread_start(isrR, isr_callback_fun, NULL);
	fail_if(i != EOK);
#endif

    /* Receive 200 messages from the message queue */
    for (j=0; j < ITERATIONS; j++ )
    {
        /* Receive message in non-blocking mode */
        i = qchannel_receive ( app1_msgq_channel, QCHANNEL_BLOCK, &msgtype, &recvptr, &msgsize , &objQ);
        fail_if (i != EOK );

        /* Verify Message size */
        fail_if ( MY_MAX_MSG_SIZE != msgsize );
    
        /* Verify Message type */
        fail_if ( MY_MSG_TYPE != msgtype );

        /* Verify Message */
        for ( index = 0; index < msgsize; index++ )
        {
            fail_if ( *((uint8_t*)recvptr+index) != ( j + START_VAL + index ) );
        }
        /* Free message */
        i=qmsgq_free_message ( recvptr );
        fail_if ( i != EOK );
    }

    qmutex_lock ( app1_intra_mutex );
    printf("*** PD1: Test9 Passed Successfully***\n");
    qmutex_unlock ( app1_intra_mutex );

}

/* Thread two */
void ThreadMain2( void * args)
{
    int i =0;
    int j =0;
    int k =0;
    uint8_t msg [ 32 ];
    int index;
    size_t value;
    
    /* Output from multiple threads can be protected using mutex */
    printf ("Starting Thread 2 \n");

    i = 1;

    while ( i != EOK )
    {
        /* wait for some time */
        for (i=0; i<5000; i++)
        {
            NOP;
        }
        /* Attach to the message queue */
        i = qmsgq_attach ( "msgQ1", &app1_msgQ1 );

    }

    value = app1_intra_mutex;
    printf("PD1: inta pd mutex value =%p\n", value);

    /* Send inter-pd mutex to other PD */
    //i = qmsgq_send ( app1_msgQ2, MUTEX_MSG_TYPE, NULL, 0, app1_intra_mutex, QPERM_FULL);

    /* Verify Success */
    fail_if (i != EOK );

    /* Add Message */
    for ( index = 0; index < 32; index++ )
    {
       msg [ index ] = j+(START_VAL+index);
    }

    /* Send 200 messages */
    for ( j = 0; j < ITERATIONS; j++)
    {
        /* Send one message */
        i = qmsgq_send ( app1_msgQ1, MY_MSG_TYPE, (void *)msg, MY_MAX_MSG_SIZE , NULL, QPERM_FULL);

        /* If send fails, wait for some time and try it again */ 
        if (i == EMAX_MSGS)
        {
	        printf ( "Sending msg failed, waiting for some time before continuing \n" );

            /* wait for some time */
            for (k=0; k<500; k++)
            {
                NOP;
            }

            j--;

        }

        fail_if( i != EOK && i != EMAX_MSGS );

        /* Add Message */
        for ( index = 0; index < 32; index++ )
        {
            msg [ index ] = (j+1)+(START_VAL+index);
        }
    }

    qmutex_lock ( app1_intra_mutex );
    printf("*** PD1: Test10 Passed Successfully***\n");
    qmutex_unlock ( app1_intra_mutex );

}

static void ThreadMutexIntraPD(void * args);

static void ThreadMutexIntraPD(void * args)
{
    static int val1 = -1, val2 = -1;
    int i,j, k;

    int num = (int)args;

    printf("PD1 Thread%d  (%x) \n", num, qthread_myself());
    for (i=0; i<30; i++)
    {
        qmutex_lock( app1_intra_mutex );
        printf("PD1 Thread%d: (%x) - iteration %d \n", num, qthread_myself(), i+1);
        val1 = -1;
        val2 = -1;
        for (j=0; j<10000; j++)
        {
            val1++;
            val2++;
            fail_if (val1 != j);
            fail_if (val2 != j);
        }
        val1 = -1;
        val2 = -1;
        /* wait for some time */
        for (k=0; k<3000; k++)
        {
            NOP;
        }
        for (j=0; j<10000; j++)
        {
            val1++;
            val2++;
            fail_if (val1 != j);
            fail_if (val2 != j);
        }
        qmutex_unlock( app1_intra_mutex );
    }
    qmutex_lock( app1_intra_mutex );
    printf("PD1 Thread%d  is done \n", num );
    qmutex_unlock( app1_intra_mutex );
}

int main(int argc, char **argv)
{
    qube_init();
    qurt_timer_init();
    /* Create three threads */
    qthread_attr_t thread_attr1, thread_attr2, thread_attr3;
    qthread_t thread1,thread2, thread3;

    qthread_attr_t thread_attr[MAX_MUTEX_THREADS];
    qthread_t threads[MAX_MUTEX_THREADS];

    int status1, status2, status3;
    int result, index;

    char name[10];
    qmutex_attr_t attrMintra, attrMinter;
    printf("Message Queue Test PD1 Started \n");
    //attrMintra.type = QMUTEX_LOCAL;
    qmutex_attr_init(&attrMintra);
    qmutex_attr_settype(&attrMintra, QMUTEX_LOCAL);
    /* Create Mutex */
    status1 = qmutex_create ( &app1_intra_mutex, &attrMintra );
    printf ("status1 %d, EOK %d\n", status1, EOK);
    //fail_if ( status1 != EOK );

    /* First Thread */
    qthread_attr_init( &thread_attr1 );

    qthread_attr_setname( &thread_attr1, "thread1" );
    qthread_attr_setpriority( &thread_attr1, 103 );
    qthread_attr_settid( &thread_attr1, TID_BASE );
    status1 = qthread_create( &thread1, &thread_attr1 );

    fail_if ( status1 != EOK );

    status1 = qthread_start( thread1, ThreadMain1, null );

    fail_if ( status1 != EOK );

    /* Second Thread */
    qthread_attr_init( &thread_attr2 );

    qthread_attr_setname( &thread_attr2, "thread2" );
    qthread_attr_setpriority( &thread_attr2, 101 );
    status2 = qthread_create( &thread2, &thread_attr2 );

    fail_if ( status2 != EOK );

    status2 = qthread_start( thread2, ThreadMain2, null );

    fail_if ( status2 != EOK );

    /* Third Thread */
    qthread_attr_init( &thread_attr3 );

    qthread_attr_setname( &thread_attr3, "thread3" );
    qthread_attr_setpriority( &thread_attr3, 101 );
    status3 = qthread_create( &thread3, &thread_attr3 );

    fail_if ( status3 != EOK );

    status3 = qthread_start( thread3, Timer_Task1, null );

    fail_if ( status3 != EOK );

    /* Wait for all the threads to exit */
    qthread_join ( thread1, &status1);
    qthread_join ( thread2, &status2);
    qthread_join ( thread3, &status3);
  
    /* Create Threads */
    for ( index = 0; index < MAX_MUTEX_THREADS; index++)
    {
        qthread_attr_init( &thread_attr[index] );
        snprintf ( name, sizeof(name), "pd1_t%d", index);
        qthread_attr_setname( &thread_attr[index], name );
        qthread_attr_setpriority( &thread_attr[index], 100 + index + 1 );
        status1 = qthread_create( &threads[index], &thread_attr[index] );
    }
   
    /* Lock Inter-PD mutex */
    qmutex_lock ( app1_intra_mutex );

    /* Start Threads */
    for ( index = 0; index < MAX_MUTEX_THREADS; index++)
    {
        status1 = qthread_start( threads[index], ThreadMutexIntraPD, (void *)index );
    }
    qmutex_unlock ( app1_intra_mutex );
    for ( index = 0; index < MAX_MUTEX_THREADS; index++)
    {
        qthread_join ( threads[index], &status1);
    }

    printf("*** PD1: Test11 Passed Successfully***\n");

    /* Unlock Inter-PD mutex */
    //qmutex_unlock ( app1_intra_mutex );

//    printf("*** PD1: Sending Done msg to PD2***\n");
    /* Send Final message to Pd2 */
//    result = qmsgq_send ( app1_msgQ2, DONE, null, 0 , NULL, QPERM_FULL);

//    printf("*** PD1: Sent Done msg to PD2***\n");

#if !defined(__qdsp6__) && !defined(__hexagon__)
//    qthread_join ( thr_id, &stat);
#endif
//    fail_if( result != EOK );

    /* Wait forever */
//    for(;;)
//      NOP;
        
}
