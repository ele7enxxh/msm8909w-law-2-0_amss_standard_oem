/*============================================================================

   Copyright (c) 2012 - 2015 Qualcomm Technologies, Inc. All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.

   This file defines the media/module/master controller's interface with the
   DSPS modules. The functionalities od this module include:

   1. Control communication with the sensor module
   2. Process data received from the sensors

============================================================================*/
#ifndef _DSPS_HW_H_
#define _DSPS_HW_H_


#include <sensor1.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include "camera_dbg.h"
//#include "stats_debug.h"
#include "dsps_hw_interface.h"
#include <sns_sam_gyroint_v01.h>
#include <sns_sam_gravity_vector_v01.h>



#define SENSOR_TIME_OUT     1000  /* Timeout for response message */
#define MSEC_TO_SEC         1/1000
#define USEC_TO_NSEC        1000
#define SEC_TO_MSEC         1000
#define MSEC_TO_NSEC        1000000
#define SEC_TO_USEC         1000000
#define SEC_TO_NSEC         1000000000
#define INVALID_INSTANCE_ID 0xFF
#define DSPS_HZ             32768U
#define SNS_SAM_ALGO_REPORT_IND 0x05
#define TIMESYNC_FREQ       16

#define DSPS_PROC_DEBUG     0

#if(DSPS_PROC_DEBUG)
  #include <utils/Log.h>
  #undef LOG_NIDEBUG
  #undef LOG_TAG
  #define LOG_NIDEBUG 0
  #define LOG_TAG "mm-camera-DSPS"
#else
#endif

#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

typedef struct {
  uint64_t t_start;
  uint64_t t_end;
  uint8_t seqnum;
  uint32_t gyro_sample_rate;
} sensor1_req_data_gyro_t;

typedef struct {
  uint32_t report_period;
  uint8_t sample_rate_valid;
  uint32_t sample_rate;
} sensor1_req_data_gravity_t;

typedef struct {
  dsps_data_type sensor_type;
  dsps_req_msg_type_t msg_type;
  union {
    sensor1_req_data_gyro_t gyro;
    sensor1_req_data_gravity_t gravity;
  } u;
} sensor1_req_data_t;

typedef enum {
  DSPS_RUNNING = 1,
  DSPS_STOPPED = 0,
  DSPS_BROKEN_PIPE = -1,
  DSPS_FAILED_CREATE = -2,
} dsps_thread_status_t;

/** sensor1_config_t:
 *    @thread_id: thread id used to join
 *    @handle: sensor1 handle
 *    @callback_mutex: mutex for use with sensor1 callback
 *    @callback_condvar: cvar for callback from sensor
 *    @callback_arrived: flag to indicate callback arrived
 *    @dsps_callback: callback into upper gyro layer
 *    @thread_mutex: mutex for DSPS thread
 *    @thread_condvar: cvar for DSPS thread
 *    @error: error indication
 *    @instance_id: GYROINT service instance id
 *    @status: current status of DSPS thread
 *    @queue: gyro data circular queue
 *    @seqnum: DSPS layer - sensor1 transaction id
 **/
typedef struct _sensor1_config {
  pthread_t thread_id;
  sensor1_handle_s *handle;
  pthread_mutex_t callback_mutex;
  pthread_cond_t callback_condvar;
  int callback_arrived;
  dsps_callback_func dsps_callback;
  pthread_mutex_t thread_mutex;
  pthread_cond_t thread_condvar;
  int error;
  int instance_id;
  int instance_id_gravity;
  dsps_thread_status_t status;
  void *port;
  uint8_t seqnum;
} sensor1_config_t;


/* Function Prototypes */
int dsps_send_request(void *sensor_config, void *req_data, int wait);
int dsps_disconnect(void *sensor_config);
int dsps_open(void *sensor_config);

#endif /* _DSPS_HW_H_ */
