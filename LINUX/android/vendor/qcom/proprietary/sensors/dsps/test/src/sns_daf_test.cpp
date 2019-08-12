/*============================================================================
  @file sns_daf_test.c

  @brief
    This is designed to test basic features of the Driver Access Framework

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ==========================================================================*/
#define __STDC_FORMAT_MACROS

#include "sensor1.h"
#include "sns_common_v01.h"
#include "sns_smgr_restricted_api_v01.h"
#include "driver_access_framework.h"
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <pthread.h>
#include <sched.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <utils/SystemClock.h>

static const char usage_fmt[] =
  "\nUsage: %s [-u usage_scenario] [-d duration_in_seconds] \n"
  "Usage Scenarios:\n"
  "    1) APDS9960 Who Am I request\n"
  "    2) APDS9960 Prox Polling request @ 10 Hz\n"
  "    3) APDS9960 ALS/RGB Polling request @ 20 Hz\n"
  "Set the duration to -1 for unlimited streaming.\n"
  "Default setting: -u 1 -d 1\n";

static const char bmi160_uuid[SNS_SMGR_UUID_LENGTH_V01] =
  {0xa5,0xd5,0x0c,0xec,0x83,0xcb,0x46,0xd6,0xb5,0xea,0xc7,0x54,0xd6,0x0f,0x4d,0x0f};
static const char apds9960_uuid[SNS_SMGR_UUID_LENGTH_V01] =
  {0xf8,0x39,0x74,0x7a,0x04,0x7c,0x49,0x6f,0xb6,0x79,0x96,0xe1,0xcf,0x87,0x02,0x0f};

#define SENSOR1_RESP_WAIT_TIME_NS   1000000000 // 1 second

pthread_mutex_t my_mutex;
pthread_cond_t my_cond;
int my_predicate = 0;

// DAF Parameters
const char *daf_uuid_ptr = apds9960_uuid;
int         daf_req_id   = SNS_DAF_GENERAL_WHO_AM_I_V01;

void parent_notify_data_cb( intptr_t data,
                            sensor1_msg_header_s *msg_hdr_ptr,
                            sensor1_msg_type_e msg_type,
                            void *msg_ptr )
{
  if( NULL == msg_hdr_ptr ) {
    printf("\ndaf_test: received NULL msg_hdr_ptr!\n");
  }

  if( msg_type == SENSOR1_MSG_TYPE_RESP ) {
    // If a DAF response is received, process the response contents
    if( ( NULL != msg_hdr_ptr ) &&
        ( SNS_SMGR_RESTRICTED_SVC_ID_V01 == msg_hdr_ptr->service_number ) &&
        ( SNS_SMGR_DRIVER_ACCESS_RESP_V01 == msg_hdr_ptr->msg_id ) ) {
      sns_smgr_driver_access_resp_msg_v01* daf_resp =
        (sns_smgr_driver_access_resp_msg_v01*) msg_ptr;

      printf("\nReceived a DAF response\n");

      if( NULL == msg_ptr ) {
        printf("\ndaf_test: received NULL msg_ptr!\n");
        return;
      }

      // Parse the response if the result and error code within the response are both SUCCESS
      if( ( 0 == daf_resp->Resp.sns_result_t) &&
          ( SENSOR1_SUCCESS == daf_resp->Resp.sns_err_t) ) {
        // Check the DAF response status
        if( true == daf_resp->ResponseStatus_valid ) {
          if( SNS_SMGR_DRIVER_ACCESS_STATUS_SUCCESS_V01 == daf_resp->ResponseStatus ) {
            // Parse out the DAF response message
            switch( daf_req_id )
            {
              case( SNS_DAF_GENERAL_WHO_AM_I_V01 ):
              {
                if( ( true == daf_resp->ResponseMsg_valid ) &&
                    ( sizeof(sns_daf_general_who_am_i_resp_v01) <= daf_resp->ResponseMsg_len ) ) {
                  sns_daf_general_who_am_i_resp_v01* whoami_resp =
                    (sns_daf_general_who_am_i_resp_v01*) daf_resp->ResponseMsg;
                  printf("\nWHOAMI Response:\n    hardware_id: %d (0x%08x)\n    firmware_id: %d (0x%08x)\n",
                         whoami_resp->hardware_id,
                         whoami_resp->hardware_id,
                         whoami_resp->firmware_id,
                         whoami_resp->firmware_id);
                }
                else {
                  printf("\ndaf_test: Unexpected error when parsing DAF WHO_AM_I ResponseMsg! valid %d len %d\n",
                         daf_resp->ResponseMsg_valid,
                         daf_resp->ResponseMsg_len);
                }
                break;
              }
              case( SNS_DAF_PROX_POLLING_START_V01 ):
              {
                if( ( true == daf_resp->ResponseMsg_valid ) &&
                    ( sizeof(sns_daf_prox_polling_start_resp_v01) <= daf_resp->ResponseMsg_len ) ) {
                  sns_daf_prox_polling_start_resp_v01* prox_polling_resp =
                    (sns_daf_prox_polling_start_resp_v01*) daf_resp->ResponseMsg;
                  printf("\nProx Polling Response:\n    actual sampling period (us): %d\n",
                         prox_polling_resp->actual_sampling_period_us);
                }
                else {
                  printf("\ndaf_test: Unexpected error when parsing DAF Prox Polling ResponseMsg! valid %d len %d\n",
                         daf_resp->ResponseMsg_valid,
                         daf_resp->ResponseMsg_len);
                }
                break;
              }
              case( SNS_DAF_ALS_RGB_POLLING_START_V01 ):
              {
                if( ( true == daf_resp->ResponseMsg_valid ) &&
                    ( sizeof(sns_daf_als_rgb_polling_start_req_v01) <= daf_resp->ResponseMsg_len ) ) {
                  sns_daf_als_rgb_polling_start_resp_v01* als_rgb_polling_resp =
                    (sns_daf_als_rgb_polling_start_resp_v01*) daf_resp->ResponseMsg;
                  printf("\nALS/RGB Polling Response:\n    actual sampling period (us): %d\n",
                         als_rgb_polling_resp->actual_sampling_period_us);
                }
                else {
                  printf("\ndaf_test: Unexpected error when parsing DAF ALS/RGB Polling ResponseMsg! valid %d len %d\n",
                         daf_resp->ResponseMsg_valid,
                         daf_resp->ResponseMsg_len);
                }
                break;
              }
              case( SNS_DAF_PROX_POLLING_STOP_V01 ):
              {
                printf("\nProx Polling Stop request returned successfully\n");
                break;
              }
              case( SNS_DAF_ALS_RGB_POLLING_STOP_V01 ):
              {
                printf("\nALS/RGB Polling Stop request returned successfully\n");
                break;
              }
              default:
              {
                printf("\nSuccessful response to request ID %d\n", daf_req_id);
                break;
              }
            }
          } else {
            printf("\ndaf_test: DAF error in response! ResponseStatus %d\n",
                   daf_resp->ResponseStatus);
          }
        }
      } else {
        printf("\ndaf_test: Error found in common Resp! sns_result %d sns_err %d\n",
               daf_resp->Resp.sns_result_t,
               daf_resp->Resp.sns_err_t);
      }
    }
    pthread_mutex_lock( &my_mutex );
    pthread_cond_signal( &my_cond );
    pthread_mutex_unlock( &my_mutex );
  } else if( msg_type == SENSOR1_MSG_TYPE_IND ) {
    // If a DAF indication is received, process the indication contents
    if( ( NULL != msg_hdr_ptr ) &&
        ( SNS_SMGR_RESTRICTED_SVC_ID_V01 == msg_hdr_ptr->service_number ) &&
        ( SNS_SMGR_DRIVER_ACCESS_IND_V01 == msg_hdr_ptr->msg_id ) ) {
      sns_smgr_driver_access_ind_msg_v01* daf_ind =
        (sns_smgr_driver_access_ind_msg_v01*) msg_ptr;
      switch( daf_ind->IndicationId )
      {
        case SNS_DAF_PROX_POLLING_REPORT_V01:
        {
          if( ( true == daf_ind->IndicationMsg_valid ) &&
              ( sizeof(sns_daf_prox_polling_report_v01) <= daf_ind->IndicationMsg_len ) ) {
            sns_daf_prox_polling_report_v01* prox_polling_ind =
              (sns_daf_prox_polling_report_v01*) daf_ind->IndicationMsg;
            printf("\nProx Polling Indication:\n    prox_adc: %d\n",
                   prox_polling_ind->prox_adc);
          }
          else {
            printf("\ndaf_test: Unexpected error when parsing DAF Prox Polling IndicationMsg! valid %d len %d\n",
                   daf_ind->IndicationMsg_valid,
                   daf_ind->IndicationMsg_len);
          }
          break;
        }
        case SNS_DAF_ALS_RGB_POLLING_REPORT_V01:
        {
          if( ( true == daf_ind->IndicationMsg_valid ) &&
              ( sizeof(sns_daf_als_rgb_polling_report_v01) <= daf_ind->IndicationMsg_len ) ) {
            sns_daf_als_rgb_polling_report_v01* als_rgb_polling_ind =
              (sns_daf_als_rgb_polling_report_v01*) daf_ind->IndicationMsg;
            printf("\nALS & RGB Polling Indication:\n    als_data: %d    r_data: %d    g_data: %d    b_data: %d\n",
                   als_rgb_polling_ind->als_data,
                   als_rgb_polling_ind->r_data,
                   als_rgb_polling_ind->g_data,
                   als_rgb_polling_ind->b_data);
          }
          else {
            printf("\ndaf_test: Unexpected error when parsing DAF ALS & RGB Polling IndicationMsg! valid %d len %d\n",
                   daf_ind->IndicationMsg_valid,
                   daf_ind->IndicationMsg_len);
          }
          break;
        }
        default:
        {
          printf("\ndaf_test: received unknown indication ID %d\n", daf_ind->IndicationId);
          break;
        }
      }
    }
  } else if( msg_type == SENSOR1_MSG_TYPE_BROKEN_PIPE ) {
    printf("\ndaf_test: received BROKEN_PIPE!!!\n");
  } else {
    printf("\ncm_test: received INVALID MSG type!!!\n");
  }
  if( NULL != msg_ptr ) {
    sensor1_free_msg_buf( *((sensor1_handle_s**)data), msg_ptr );
  }
}

int main( int argc, char * const argv[])
{
  sensor1_handle_s                   *hndl;
  int                                opt;
  sensor1_error_e                    error;
  sensor1_msg_header_s               msg_hdr;
  sns_smgr_driver_access_req_msg_v01 *daf_req_msg_ptr;

  struct timespec                    ts;

  // Input Argument State Variables
  int usage_scenario = 0;
  int duration = 1;

  while( (opt = getopt(argc, argv, "u:d:" ))!= -1 ) {
    switch(opt) {
      case 'u':
        usage_scenario = atoi(optarg);
        break;
      case 'd':
        duration = atoi(optarg);
        break;
      case '?':
        fprintf(stderr, usage_fmt, argv[0]);
        exit(0);
      default:
        printf("\ndaf_test: Ignoring unknown option %c\n", opt);
        break;
    }
  }

  pthread_mutex_init( &my_mutex, NULL );
  pthread_cond_init( &my_cond, NULL );

  // Initialize the Sensor1 Framework
  error = sensor1_init();
  if( SENSOR1_SUCCESS != error ) {
    printf("\ndaf_test: sensor1_init returned %d\n", error);
    exit(error);
  }

  // Open a new Sensor1 connection
  printf("\nOpening a Sensor1 connection\n");
  error = sensor1_open( &hndl,
                        parent_notify_data_cb,
                        (intptr_t)&hndl );
  if( SENSOR1_SUCCESS != error ) {
    printf("\ndaf_test: sensor1_open returned %d\n", error);
    exit(error);
  }

  // Allocate the request message through Sensor1
  error = sensor1_alloc_msg_buf( hndl,
                                 sizeof(sns_smgr_driver_access_req_msg_v01),
                                 (void**)&daf_req_msg_ptr );
  if( SENSOR1_SUCCESS != error ) {
    printf("\ndaf_test: sensor1_alloc_msg_buf returned %d\n", error);
    exit(error);
  }

  // Zero out the Sensor1 request
  memset( daf_req_msg_ptr, 0, sizeof(sns_smgr_driver_access_req_msg_v01) );

  // Fill in the Sensor1 request
  switch( usage_scenario ) {
    default:
    {
      printf("\ndaf_test: Unknown usage scenario %d!\n", usage_scenario);
      // Fall through to the default usage scenario
    }
    // 1) APDS9960 Who Am I request
    case 1:
    {
      printf("\nPreparing SNS_DAF_GENERAL_WHO_AM_I_V01 message, req_id %d\n", SNS_DAF_GENERAL_WHO_AM_I_V01);
      daf_uuid_ptr = apds9960_uuid;
      daf_req_id   = SNS_DAF_GENERAL_WHO_AM_I_V01;

      memcpy( daf_req_msg_ptr->Uuid, daf_uuid_ptr, SNS_SMGR_UUID_LENGTH_V01);
      daf_req_msg_ptr->RequestId = daf_req_id;
      daf_req_msg_ptr->RequestMsg_valid = false; // There is no accompanying DAF request message
      daf_req_msg_ptr->TransactionId_valid = false; // We are not assigning a transaction ID
      break;
    }
    // 2) APDS9960 Prox Polling request @ 10 Hz
    case 2:
    {
      printf("\nPreparing SNS_DAF_PROX_POLLING_START_V01 message, req_id %d\n", SNS_DAF_PROX_POLLING_START_V01);
      sns_daf_prox_polling_start_req_v01 daf_req_msg;
      daf_req_msg.sampling_period_us = 100000; // 100000 us period => 10 Hz

      daf_uuid_ptr = apds9960_uuid;
      daf_req_id   = SNS_DAF_PROX_POLLING_START_V01;

      memcpy( daf_req_msg_ptr->Uuid, daf_uuid_ptr, SNS_SMGR_UUID_LENGTH_V01);
      daf_req_msg_ptr->RequestId = daf_req_id;
      daf_req_msg_ptr->RequestMsg_valid = true;
      daf_req_msg_ptr->RequestMsg_len = sizeof(daf_req_msg);
      memcpy( daf_req_msg_ptr->RequestMsg, &daf_req_msg, sizeof(daf_req_msg) );
      daf_req_msg_ptr->TransactionId_valid = false; // We are not assigning a transaction ID
      break;
    }
    // 3) APDS9960 ALS/RGB Polling request @ 20 Hz
    case 3:
    {
      printf("\nPreparing SNS_DAF_ALS_RGB_POLLING_START_V01 message, req_id %d\n", SNS_DAF_ALS_RGB_POLLING_START_V01);
      sns_daf_als_rgb_polling_start_req_v01 daf_req_msg;
      daf_req_msg.sampling_period_us = 50000; // 50000 us period => 20 Hz

      daf_uuid_ptr = apds9960_uuid;
      daf_req_id   = SNS_DAF_ALS_RGB_POLLING_START_V01;

      memcpy( daf_req_msg_ptr->Uuid, daf_uuid_ptr, SNS_SMGR_UUID_LENGTH_V01);
      daf_req_msg_ptr->RequestId = daf_req_id;
      daf_req_msg_ptr->RequestMsg_valid = true;
      daf_req_msg_ptr->RequestMsg_len = sizeof(daf_req_msg);
      memcpy( daf_req_msg_ptr->RequestMsg, &daf_req_msg, sizeof(daf_req_msg) );
      daf_req_msg_ptr->TransactionId_valid = false; // We are not assigning a transaction ID
      break;
    }
  }

  // Fill in the message header for the Sensor1 request
  msg_hdr.service_number = SNS_SMGR_RESTRICTED_SVC_ID_V01;
  msg_hdr.msg_id = SNS_SMGR_DRIVER_ACCESS_REQ_V01;
  msg_hdr.msg_size = sizeof(sns_smgr_driver_access_req_msg_v01);
  msg_hdr.txn_id = 123;

  // Send the Sensor1 request message
  printf("\nSending DAF request\n");
  error = sensor1_write( hndl,
                         &msg_hdr,
                         daf_req_msg_ptr );
  if( SENSOR1_SUCCESS != error ) {
    printf("\ndaf_test: sensor1_write returned %d\n", error);
    if( SENSOR1_EWOULDBLOCK != error ) {
      exit(error);
    }
  }

  // Wait for the Sensor1 response message
  pthread_mutex_lock( &my_mutex );
  clock_gettime( CLOCK_REALTIME, &ts );
  ts.tv_sec += SENSOR1_RESP_WAIT_TIME_NS; // wait for SENSOR1_RESP_WAIT_TIME_NS nanoseconds
  error = (sensor1_error_e)pthread_cond_timedwait( &my_cond, &my_mutex, &ts );
  pthread_mutex_unlock( &my_mutex );

  if( SENSOR1_SUCCESS != error ) {
    printf("\nError while waiting for response message %d\n", error);
    //exit(error);
  }

  // Wait the requested duration
  if( duration > 0 ) {
    sleep( (unsigned int) duration );
  } else {
    while( duration == -1 ) {
      // Sleep forever
      sleep(1);
    }
  }

  // Send the stop command, if applicable
  bool send_stop_command = true;
  switch( usage_scenario ) {
    default:
    {
      send_stop_command = false;
      break;
    }
    // 1) APDS9960 Who Am I request
    case 1:
    {
      // No stop request to be sent for the Who Am I request
      send_stop_command = false;
      break;
    }
    // 2) APDS9960 Prox Polling request @ 10 Hz
    case 2:
    {
      error = sensor1_alloc_msg_buf( hndl,
                                     sizeof(sns_smgr_driver_access_req_msg_v01),
                                     (void**)&daf_req_msg_ptr );
      if( SENSOR1_SUCCESS != error ) {
        printf("\ndaf_test: sensor1_alloc_msg_buf returned %d\n", error);
        exit(error);
      }
      memset( daf_req_msg_ptr, 0, sizeof(sns_smgr_driver_access_req_msg_v01) );

      printf("\nPreparing SNS_DAF_PROX_POLLING_STOP_V01 message, req_id %d\n", SNS_DAF_PROX_POLLING_STOP_V01);
      daf_uuid_ptr = apds9960_uuid;
      daf_req_id   = SNS_DAF_PROX_POLLING_STOP_V01;

      memcpy( daf_req_msg_ptr->Uuid, daf_uuid_ptr, SNS_SMGR_UUID_LENGTH_V01);
      daf_req_msg_ptr->RequestId = daf_req_id;
      daf_req_msg_ptr->RequestMsg_valid = false; // The stop command has an empty message
      daf_req_msg_ptr->TransactionId_valid = false; // We are not assigning a transaction ID
      break;
    }
    // 3) APDS9960 ALS/RGB Polling request @ 20 Hz
    case 3:
    {
      error = sensor1_alloc_msg_buf( hndl,
                                     sizeof(sns_smgr_driver_access_req_msg_v01),
                                     (void**)&daf_req_msg_ptr );
      if( SENSOR1_SUCCESS != error ) {
        printf("\ndaf_test: sensor1_alloc_msg_buf returned %d\n", error);
        exit(error);
      }
      memset( daf_req_msg_ptr, 0, sizeof(sns_smgr_driver_access_req_msg_v01) );

      printf("\nPreparing SNS_DAF_ALS_RGB_POLLING_STOP_V01 message, req_id %d\n", SNS_DAF_ALS_RGB_POLLING_STOP_V01);
      daf_uuid_ptr = apds9960_uuid;
      daf_req_id   = SNS_DAF_ALS_RGB_POLLING_STOP_V01;

      memcpy( daf_req_msg_ptr->Uuid, daf_uuid_ptr, SNS_SMGR_UUID_LENGTH_V01);
      daf_req_msg_ptr->RequestId = daf_req_id;
      daf_req_msg_ptr->RequestMsg_valid = false; // The stop command has an empty message
      daf_req_msg_ptr->TransactionId_valid = false; // We are not assigning a transaction ID
      break;
    }
  }

  if( send_stop_command )
  {
    // Send the Sensor1 request message
    printf("\nSending DAF request\n");
    error = sensor1_write( hndl,
                           &msg_hdr,
                           daf_req_msg_ptr );
    if( SENSOR1_SUCCESS != error ) {
      printf("\ndaf_test: sensor1_write returned %d\n", error);
      if( SENSOR1_EWOULDBLOCK != error ) {
        exit(error);
      }
    }

    // Wait for the Sensor1 response message
    pthread_mutex_lock( &my_mutex );
    clock_gettime( CLOCK_REALTIME, &ts );
    ts.tv_sec += SENSOR1_RESP_WAIT_TIME_NS; // wait for SENSOR1_RESP_WAIT_TIME_NS nanoseconds
    error = (sensor1_error_e)pthread_cond_timedwait( &my_cond, &my_mutex, &ts );
    pthread_mutex_unlock( &my_mutex );

    if( SENSOR1_SUCCESS != error ) {
      printf("\nError while waiting for response message %d\n", error);
      //exit(error);
    }
  }

  // Close the Sensor1 connection
  printf("\nClosing the Sensor1 connection\n");
  error = sensor1_close( hndl );
  if( SENSOR1_SUCCESS != error ) {
    printf("\ndaf_test: sensor1_close returned %d\n", error);
    exit(error);
  }

  return 0;
}
