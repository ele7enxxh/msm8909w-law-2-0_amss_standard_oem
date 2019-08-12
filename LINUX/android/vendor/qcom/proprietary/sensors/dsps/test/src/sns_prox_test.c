/*============================================================================
  @file sns_prox_test.c

  @brief
    Tests background proximity operation, with periodic disable/re-enabling.

  Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ==========================================================================*/
#define __STDC_FORMAT_MACROS

#include "sensor1.h"
#include "sns_common_v01.h"
#include "sns_smgr_api_v01.h"
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/wait.h>

#define SNS_WAKELOCK_FILE       "/sys/power/wake_lock"
#define SNS_WAKEUNLOCK_FILE     "/sys/power/wake_unlock"
#define SNS_WAKELOCK_NAME       "sns_prox_test_wakelock"

#define RESP_PREDICATE 1
#define RETRY_OPEN_PREDICATE 2
#define ALARM_PREDICATE 4

static const char usage_fmt[] =
  "\nUsage: %s [-d delay (in seconds) between SMGR enable/disable]\n"
  "Default setting: -d 5 \n";

pthread_mutex_t my_mutex;
pthread_cond_t my_cond;
volatile int my_predicate = 0;
sensor1_error_e error;
sensor1_msg_header_s msg_hdr;
sns_smgr_periodic_report_req_msg_v01 *smgr_req;
struct timespec ts;
timer_t my_timer;
/** File descriptor for SNS_WAKELOCK_FILE */
static int g_wl_fd = 0;
static int g_wul_fd = 0;



static void wake_lock( void )
{
  int err;

  if( g_wl_fd >= 0 ) {
    printf("acquiring wakelock\n");
    err = write( g_wl_fd, SNS_WAKELOCK_NAME, sizeof(SNS_WAKELOCK_NAME) );
    if( err != (int)sizeof(SNS_WAKELOCK_NAME) ) {
      printf("wakelock write error: %d, %d\n", err, errno );
    }
  }
}

static void wake_unlock( void )
{
  int err;

  if( g_wul_fd >= 0 ) {
    printf( "releasing wakelock\n" );
    err = write( g_wul_fd, SNS_WAKELOCK_NAME, sizeof(SNS_WAKELOCK_NAME) );
    if( err != (int)sizeof(SNS_WAKELOCK_NAME) ) {
      printf("wakeunlock write error: %d, %d\n", err, errno );
    }
  }
}

void notify_data_cb( intptr_t data,
                     sensor1_msg_header_s *msg_hdr_ptr,
                     sensor1_msg_type_e msg_type,
                     void *msg_ptr )
{
  if( NULL == msg_hdr_ptr ) {

  } else {

  }

  if( msg_type == SENSOR1_MSG_TYPE_RESP ) {
    pthread_mutex_lock( &my_mutex );
    my_predicate = RESP_PREDICATE;
    pthread_cond_signal( &my_cond );
    pthread_mutex_unlock( &my_mutex );
  } else if( msg_type == SENSOR1_MSG_TYPE_IND ) {
    uint8_t id = ((sns_smgr_periodic_report_ind_msg_v01*)msg_ptr)->Item[0].SensorId;
    uint8_t type = ((sns_smgr_periodic_report_ind_msg_v01*)msg_ptr)->Item[0].DataType;
    uint8_t flag = ((sns_smgr_periodic_report_ind_msg_v01*)msg_ptr)->Item[0].ItemFlags;
    uint8_t quality =  ((sns_smgr_periodic_report_ind_msg_v01*)msg_ptr)->Item[0].ItemQuality;
    int32_t *data = ((sns_smgr_periodic_report_ind_msg_v01*)msg_ptr)->Item[0].ItemData;
    printf("ID=%d,Type=%d,Val=%d,%d,%d", id, type, data[0], data[1], data[2]);
    if (quality != SNS_SMGR_ITEM_QUALITY_FILTERED_V01)
      printf("   Quality=%d,Flag=%d\n", quality, flag);
    printf("\n");
  } else if( msg_type == SENSOR1_MSG_TYPE_BROKEN_PIPE ) {
    printf("\nreceived BROKEN_PIPE!!!\n");
  } else if( msg_type == SENSOR1_MSG_TYPE_RETRY_OPEN ) {
    printf("\nreceived RETRY_OPEN!!!\n");
    error = sensor1_open( (sensor1_handle_s **)data,
                          notify_data_cb,
                          (intptr_t)((sensor1_handle_s **)data) );
    if( SENSOR1_SUCCESS != error ) {
      printf("\nretry opened failed with %d\n", error);
    } else {
      pthread_mutex_lock( &my_mutex );
      my_predicate = RETRY_OPEN_PREDICATE;
      pthread_cond_signal( &my_cond );
      pthread_mutex_unlock( &my_mutex );
    }
  } else {
    printf("\nreceived INVALID MSG type!!!\n");
  }

  if( NULL != msg_ptr ) {
    sensor1_free_msg_buf( *((sensor1_handle_s**)data), msg_ptr );
  }
}

void write_smgr_cmd( sensor1_handle_s *hndl )
{
  printf("Writing SMGR command\n");
  error = sensor1_write( hndl,
                         &msg_hdr,
                         smgr_req );

  if( SENSOR1_SUCCESS != error ) {
    printf("\nssensor1_write returned %d\n", error);
    if( SENSOR1_EWOULDBLOCK != error ) {
      exit(error);
    }
  }
  // Make sure we get a response
  error = SENSOR1_SUCCESS;
  pthread_mutex_lock( &my_mutex );
  clock_gettime( CLOCK_REALTIME, &ts );
  while( my_predicate != RESP_PREDICATE && error == SENSOR1_SUCCESS ) {
    ts.tv_sec += 100000000; // wait 100msec
    error = (sensor1_error_e)pthread_cond_timedwait( &my_cond, &my_mutex, &ts );
  }
  my_predicate = 0;
  pthread_mutex_unlock( &my_mutex );
  if( SENSOR1_SUCCESS != error ) {
    printf("\nError while waiting for response message %d\n", error);
    exit(error);
  } else {
    printf("\nGot response to message\n");
  }
}

void open_sensor1( sensor1_handle_s **hndl_ptr )
{
  printf("Opening sensor1 connection\n");
  error = sensor1_open( hndl_ptr,
                        notify_data_cb,
                        (intptr_t)hndl_ptr );

  if( SENSOR1_EWOULDBLOCK == error ) {
    printf("\nsensor1_open returned EWOULDBLOCK. Waiting 60 sec for sensor availability\n");
    pthread_mutex_lock( &my_mutex );
    error = SENSOR1_SUCCESS;
    clock_gettime( CLOCK_REALTIME, &ts );
    while( my_predicate != RETRY_OPEN_PREDICATE && error == 0 ) {
      ts.tv_sec += 60; // wait 60 seconds
      error = (sensor1_error_e)pthread_cond_timedwait( &my_cond, &my_mutex, &ts );
    }
    if( my_predicate &= RETRY_OPEN_PREDICATE ) {
      printf("sensor1 now available\n");
    } else {
      printf("error waiting for sensor1\n");
      exit(1);
    }
    my_predicate = 0;
    pthread_mutex_unlock( &my_mutex );
  } else if( SENSOR1_SUCCESS != error ) {
    printf("\nsensor1_open returned %d\n", error);
    exit(error);
  }
}

void alloc_smgr_req( sensor1_handle_s *hndl)
{
  error = sensor1_alloc_msg_buf( hndl,
                                 sizeof(sns_smgr_periodic_report_req_msg_v01),
                                 (void**)&smgr_req );
  if( SENSOR1_SUCCESS != error ) {
    printf("\nsensor1_alloc_msg_buf returned %d\n", error);
    exit(error);
  }
  smgr_req->ReportId = 0;
  smgr_req->Action = SNS_SMGR_REPORT_ACTION_ADD_V01;
  smgr_req->ReportRate = 5;
  smgr_req->Item_len = 1;
  smgr_req->Item[0].SensorId = SNS_SMGR_ID_PROX_LIGHT_V01;
  smgr_req->Item[0].DataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
  smgr_req->Item[0].Decimation = SNS_SMGR_DECIMATION_RECENT_SAMPLE_V01;
}

static void fault_hndlr( int signum )
{
  if( signum == SIGUSR1 )
  {
    wake_lock();
    /* Not the best to do this outside of a mutex. But probably
     * shouldn't lock a mutex inside of the signal handler */
    my_predicate = ALARM_PREDICATE;
    pthread_cond_signal( &my_cond );
  }
}

void init( void )
{
  struct sigaction action;
  struct sigevent sev;

  sensor1_init();

  msg_hdr.service_number = SNS_SMGR_SVC_ID_V01;
  msg_hdr.msg_id = SNS_SMGR_REPORT_REQ_V01;
  msg_hdr.msg_size = sizeof(sns_smgr_periodic_report_req_msg_v01);
  msg_hdr.txn_id = 123;

  pthread_mutex_init( &my_mutex, NULL );
  pthread_cond_init( &my_cond, NULL );

  /* Open wakelock files: */
  g_wl_fd = open( SNS_WAKELOCK_FILE, O_WRONLY|O_APPEND );
  if( g_wl_fd < 0 )
  {
    printf("wakelock open error: %d", errno );
    g_wul_fd = 0;
  } else {
    g_wul_fd = open( SNS_WAKEUNLOCK_FILE, O_WRONLY|O_APPEND );
    if( g_wul_fd < 0 ) {
      printf("wakeunlock open error: %d", errno );
      close( g_wl_fd );
      g_wl_fd = 0;
    }
  }

  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIGUSR1;

  timer_create( CLOCK_REALTIME_ALARM, &sev, &my_timer );

  /* Set up the fault handler */
  action.sa_handler = fault_hndlr;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  sigaction( SIGUSR1, &action, NULL );

}

void send_smgr_req_delete( sensor1_handle_s *hndl )
{
  printf("Deleting SMGR request\n");
  alloc_smgr_req( hndl );
  smgr_req->Action = SNS_SMGR_REPORT_ACTION_DELETE_V01;
  write_smgr_cmd( hndl );
}

int main( int argc, char * const argv[] )
{
  sensor1_handle_s *hndl1;
  struct itimerspec timer =
    { .it_value = {0, 0},
      .it_interval = {0, 0}
    };
  int duration = 5; /* 5 seconds */
  int opt;

  while( (opt = getopt(argc, argv, "hd:" ))!= -1 ) {
    switch(opt) {
      case 'd':
        duration = atoi(optarg);
        break;
      case 'h':
      case '?':
        fprintf(stderr, usage_fmt, argv[0]);
        exit(0);
      default:
        break;
    }
  }

  init();
  open_sensor1( &hndl1 );
  while( true ) {
    alloc_smgr_req( hndl1 );
    write_smgr_cmd( hndl1 );

    pthread_mutex_lock( &my_mutex );
    my_predicate = 0;
    clock_gettime( CLOCK_REALTIME_ALARM, &ts );
    printf("Time now: %ld sec %ld nsec\n",
           (long)ts.tv_sec, (long)ts.tv_nsec );
    fflush(NULL);
    timer.it_value.tv_sec = ts.tv_sec + duration;
    timer.it_value.tv_nsec = ts.tv_nsec;
    timer_settime( my_timer, TIMER_ABSTIME, &timer, NULL );
    wake_unlock();
    while( my_predicate != ALARM_PREDICATE ) {
      pthread_cond_wait( &my_cond, &my_mutex );
    }
    my_predicate = 0;
    pthread_mutex_unlock( &my_mutex );
    send_smgr_req_delete( hndl1 );
  }

  return 0;
}
