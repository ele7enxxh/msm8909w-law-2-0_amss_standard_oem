/*=============================================================================
  @file   smgr_query.c

  @brief  Test application for SMGR Buffering (Query) using Sensor1 interface.

  *   Copyright (c) 2016 Qualcomm Technologies, Inc.
  *   All Rights Reserved.
  *   Confidential and Proprietary - Qualcomm Technologies, Inc.
  *
  =============================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sensor1.h>
#include <sns_smgr_api_v01.h>
#include <sns_time_api_v02.h>
#include <sns_reg_api_v02.h>
#include <pthread.h>

#define SMGR_BUF_VERSION    "1.0"

#define LOG_TAG     "smgr_buf"
#define LOG_NDEBUG  0
#define SNS_LA
#ifdef SNS_LA
#include <android/log.h>
#define LOG_TAG  "smgr_buf"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#else
#include <stdio.h>
#define LOGE(args...) printf(args); printf("\n");
#define LOGD(args...) LOGE(args)
#endif
#define NSEC_PER_SEC       1000000000LL
#define USEC_PER_SEC       1000000LL
#define DSPS_HZ            32768LL

#define NSEC_PER_USEC   1000
#define LAST_IND_WAIT   900       // Wait this long after the last req 
                                  // before closing connection to allow time 
                                  // for indications to come back
                                  // Caution : should be <= 999 ms
#define MAX_NUM_SENSOR  2
#define STR_OPTIONS     "Dd:f:i:q:v"

#define PRINTV(args...)     \
  do {                      \
    if(flag_verbose) {      \
      LOGE(args);           \
    }                       \
  } while(0)

#define DEBUG(args...)     \
  do {                      \
    if(flag_debug) {      \
      LOGE(args);           \
    }                       \
  } while(0)

#ifndef UNREFERENCED_PARAMETER
# define UNREFERENCED_PARAMETER(x) (void)x;
#endif /* UNREFERENCED_PARAMETER */

/* ------------------------------------------------------------------------------ */
typedef struct {
  uint8_t id;   /* sensor ID */
  uint8_t sr;   /* sampling rate */
} sensor_s;

typedef struct {
  uint32_t num_requests_sent;
  uint32_t num_resp_rcd; 
  uint32_t seq_no;
  struct timeval init_time;
  sensor1_handle_s *sensor1_handle;
  sensor1_handle_s *sensor1_handle_timesync;
  uint8_t instance_id;
} test_state_s;

typedef struct {
  test_state_s test_state;
  struct itimerval tout_val;
  uint64_t t1_prev;

  uint8_t report_id;

  /* Buffering configuration */
  uint8_t   num_sensor;
  sensor_s  sensor[MAX_NUM_SENSOR]; 

  /* Sensors to query */
  sensor_s  sensor_query[MAX_NUM_SENSOR];

  /* Parameters: Query requests */
  uint32_t query_duration;     // Frame duration to simulate (ms)
  int32_t  sof_req_delay;      // delay to add between SOF & the report reqs (ms)
  uint32_t query_start_delay;
  uint32_t num_query;          // number of frames to run the test
  uint32_t num_query_success;
  uint32_t t0_frame1;
  int32_t  t12now_frame1;      // gap between dsp time to the end of t1 of the first frame 

  /* AP-DSPS time sync */
  uint32_t dsps_rollover_cnt;
  uint32_t dsps_rollover_cnt_rcv;
  uint32_t dsps_ts_last;
  uint64_t timestamp_offset_apps_dsps;    /* nsec */
  bool     offset_init;
} buf_query_s;

/* ------------------------------------------------------------------------------ */
buf_query_s buf_query;
static int flag_verbose = 0;
static int flag_debug = 0;
static bool done_already = false;
static struct option long_options[] = {
  {0, 0, 0, 0}
};

pthread_mutex_t   smgr_cb_mutex;

void cleanup_and_exit(buf_query_s *bq);

/* ------------------------------------------------------------------------------ */
void usage(void) {
  LOGE("\n\n");
}

void dump_test_params(buf_query_s *bq) {
  uint8_t i;
  LOGE("\n========= SMGR Buffering Test App    ver. %s ===================", SMGR_BUF_VERSION);
  LOGE("        report_id = %d", bq->report_id);
  LOGE("       num_sensor = %u", bq->num_sensor);
  for(i = 0; i < bq->num_sensor; i++)
    LOGE(" sensor[%u]  id,sr = %u,%u", i, bq->sensor[i].id, bq->sensor[i].sr);
  LOGE("        num_query = %d", bq->num_query);
  LOGE("   query_duration = %d", bq->query_duration);
  LOGE("   offset from t0 = %d", bq->sof_req_delay);
  LOGE("==================================================================\n");
}

int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1) {
  uint64_t ta = t1->tv_usec  + (t1->tv_sec * 1000000L);
  uint64_t tb = t2->tv_usec  + (t2->tv_sec * 1000000L);
  uint64_t diff = tb - ta;

  result->tv_sec = diff / 1000000L;
  result->tv_usec = diff % 1000000L;

  return (tb < ta);
}

uint64_t get_test_time() {
//  struct timeval t_now, t_diff;
  struct timespec t_now, t_diff;
  uint64_t t;

//  gettimeofday(&t_now, NULL);
  clock_gettime( CLOCK_REALTIME, &t_now );
  t_diff = t_now;
  //timeval_subtract(&t_diff, &t_now, &test_state.init_time);
  t = t_diff.tv_sec;
  t = t * 1000000 + t_diff.tv_nsec/1000;

  return(t);  // usec
}

void process_buffering_resp(sensor1_msg_header_s *msg_hdr, void *msg_ptr) {
  uint64_t test_time_now; 

  test_time_now = get_test_time();

  switch(msg_hdr->msg_id) {
    case (SNS_SMGR_BUFFERING_RESP_V01):
    {
      sns_smgr_buffering_resp_msg_v01 *resp_ptr = msg_ptr;
      PRINTV("RX: Resp: res %d, rpt_id_valid %d, rpt_id %d, AckNak_valid %d, AckNak %d", 
             resp_ptr->Resp.sns_result_t, resp_ptr->ReportId_valid, resp_ptr->ReportId,
             resp_ptr->AckNak_valid, resp_ptr->AckNak);
      PRINTV("RX: Resp: timeAPnow=%lu", test_time_now/1000);
      buf_query.report_id = resp_ptr->ReportId;
      break;
    }
    case (SNS_SMGR_BUFFERING_QUERY_RESP_V01):
    {
      sns_smgr_buffering_query_resp_msg_v01 *resp_ptr = msg_ptr;
      uint8_t seqnum = (resp_ptr->QueryId & 0xFF00) >> 8;
      PRINTV("RX: QueryResp (seqnum %u): res %d, QueryId_valid %d, QueryId %d",
             seqnum, resp_ptr->Resp.sns_result_t, resp_ptr->QueryId_valid, resp_ptr->QueryId);
      break;
    }
    default:
      LOGE("%s: Cannot parse msg id %d", __FUNCTION__, msg_hdr->msg_id);
      break;
  }
}

void process_buffering_ind(sensor1_msg_header_s *msg_hdr, void *msg_ptr) {
  uint64_t test_time_now; 

  test_time_now = get_test_time();

  switch(msg_hdr->msg_id) {
    case(SNS_SMGR_BUFFERING_QUERY_IND_V01):
    {
      sns_smgr_buffering_query_ind_msg_v01 *ind_ptr = msg_ptr;
      uint8_t seqnum = (ind_ptr->QueryId & 0xFF00) >> 8;
      uint32_t i, ts;
      uint64_t offset;
      pthread_mutex_lock(&smgr_cb_mutex);
      offset = buf_query.timestamp_offset_apps_dsps / 1000; /* usec */
      pthread_mutex_unlock(&smgr_cb_mutex);

      DEBUG("RX: QueryInd: seqnum %u, num_sample %u, timeAPnow=%lu, timeDSPSnow=%llu (ticks)",
            seqnum, ind_ptr->Samples_len, test_time_now/1000, (test_time_now-offset)*DSPS_HZ/NSEC_PER_SEC);

      ts = ind_ptr->FirstSampleTimestamp;
      for(i = 0; i < ind_ptr->Samples_len; i++) {
        ts += ind_ptr->Samples[i].TimeStampOffset;
        DEBUG("timestampoffset[%u]=%u", i, ind_ptr->Samples[i].TimeStampOffset);
        PRINTV("\t timeDSPSdata=%llu, x=%d y=%d z=%d",
               ((uint64_t)(ts) * USEC_PER_SEC / DSPS_HZ)/1000,
               ind_ptr->Samples[i].Data[0],
               ind_ptr->Samples[i].Data[1],
               ind_ptr->Samples[i].Data[2]);
      }
      break;
    }
    default:
      LOGE("%s: ERROR invalid msg id %d", __FUNCTION__, msg_hdr->msg_id);
      break;
  }
}

static void process_time_resp( sensor1_msg_header_s *msg_hdr, void *msg_ptr )
{
  bool error = false;

  if( SNS_TIME_TIMESTAMP_RESP_V02 == msg_hdr->msg_id )
  {
    sns_time_timestamp_resp_msg_v02 *msgPtr =
           (sns_time_timestamp_resp_msg_v02*) msg_ptr;

    if( 0 == msgPtr->resp.sns_result_t )
    {
      /*
      if( msgPtr->dsps_rollover_cnt_valid )
      {
        if( 0 == buf_query.dsps_rollover_cnt && 0 == buf_query.dsps_ts_last )
        {
          buf_query.dsps_rollover_cnt = msgPtr->dsps_rollover_cnt;
          DEBUG( "%s: Resetting rollover count from %u to %u", __FUNCTION__,
                buf_query.dsps_rollover_cnt, msgPtr->dsps_rollover_cnt );
        }
        buf_query.dsps_rollover_cnt_rcv = msgPtr->dsps_rollover_cnt;
      }
      */

      if( msgPtr->timestamp_dsps_valid && msgPtr->timestamp_apps_valid )
      {
        uint64_t dsps_ns = (uint64_t)(((uint64_t)msgPtr->timestamp_dsps * NSEC_PER_SEC)/DSPS_HZ);

        if(!buf_query.offset_init)
        {
           buf_query.offset_init = true;
           buf_query.t0_frame1 = dsps_ns/1000;
        }
        pthread_mutex_lock(&smgr_cb_mutex);
        buf_query.timestamp_offset_apps_dsps = msgPtr->timestamp_apps - dsps_ns;
        pthread_mutex_unlock(&smgr_cb_mutex);
        DEBUG( "%s (nsec): Apps: %lu; DSPS: %lu; Offset: %lu", __FUNCTION__,
                msgPtr->timestamp_apps, dsps_ns,
                buf_query.timestamp_offset_apps_dsps );
      }
      else if( msgPtr->error_code_valid )
      {
        LOGE( "%s: Error in RESP: %i", __FUNCTION__, msgPtr->error_code );
      }
      else
      {
        LOGE( "%s: Unknown erorr in RESP. DSPS ts valid: %i; APPS: %i",
              __FUNCTION__, msgPtr->timestamp_dsps_valid, msgPtr->timestamp_apps_valid );
      }
    }
    else
    {
       LOGE( "%s: Received 'Failed' in response result", __FUNCTION__ );
    }
  }
  else
  {
    LOGE( "%s: Unhandled message id received: %i", __FUNCTION__, msg_hdr->msg_id );
  }
}

void smgr_cb (intptr_t cb_data,
              sensor1_msg_header_s *msg_hdr,
              sensor1_msg_type_e msg_type,
              void *msg_ptr)
{
  UNREFERENCED_PARAMETER(cb_data);
  DEBUG("%s: Type %d, SVC %d, ID %d, txn_id %d", __FUNCTION__, 
         msg_type, msg_hdr->service_number, msg_hdr->msg_id, msg_hdr->txn_id);

  switch(msg_type) {
    case SENSOR1_MSG_TYPE_RESP:
      if(msg_hdr->service_number == SNS_SMGR_SVC_ID_V01) {
        process_buffering_resp( msg_hdr, msg_ptr );
      }
      else if ( msg_hdr->service_number == SNS_TIME2_SVC_ID_V01 )
      {
        process_time_resp( msg_hdr, msg_ptr );
      }
      break;
    case SENSOR1_MSG_TYPE_IND:
      if(msg_hdr->service_number == SNS_SMGR_SVC_ID_V01) {
        process_buffering_ind( msg_hdr, msg_ptr );
      }
      break;
    case SENSOR1_MSG_TYPE_BROKEN_PIPE:
      LOGE("%s: ERROR Broken Pipe !!", __FUNCTION__);
      cleanup_and_exit(&buf_query);
      break;
    case SENSOR1_MSG_TYPE_RESP_INT_ERR:
      if(msg_hdr->service_number == SNS_SMGR_SVC_ID_V01) {
        //signal_response(true, &gp_cmsd_config->cb_arrived_cond);
      }
      LOGE("%s: SENSOR1_MSG_TYPE_RESP_INT_ERR ", __func__);
      break;
    case SENSOR1_MSG_TYPE_REQ:
    default:
      LOGE("%s: ERROR invalid msg type: %u", __func__, msg_type );
      break;
  }

  if( NULL != msg_ptr && buf_query.test_state.sensor1_handle != NULL) {
    sensor1_free_msg_buf( buf_query.test_state.sensor1_handle, msg_ptr );
  }
}

void init_test_state(buf_query_s *bq) {
  bq->test_state.num_requests_sent = 0;
  bq->test_state.num_resp_rcd      = 0;
  bq->test_state.seq_no            = 0;
  bq->test_state.sensor1_handle    = NULL;
  bq->test_state.sensor1_handle_timesync = NULL;
}

void init_test_defaults(buf_query_s *bq) {
  /* Defaults */
  bq->query_duration = 30;  // 30 ms frames
  bq->query_start_delay = 0;
  bq->num_query = 900; // ~ a 1/2 minute @30fpx
  bq->num_query_success = 0;
  bq->num_sensor = 0;
  bq->report_id = 1;

  bq->offset_init = false;
}

void parse_cmd_arg(buf_query_s* bq, int argc, char **argv) {
  int c, option_index, num_option; 

  num_option = 0;

  while((c=getopt_long(argc, argv, STR_OPTIONS, long_options, &option_index)) != -1) {
    switch (c) {
      case 'D':
        flag_debug = 1;
        break;
      case 'd':
        bq->query_start_delay = (uint32_t) atoi(optarg);
        break;
      case 'f':
        break;
      case 'i':
      {
        char argstr[64], *saveptr1, *token, *strptr;
        int i = 0;

        strncpy(argstr, optarg, 63);

        for(strptr = argstr; ; strptr = NULL) {
          token = strtok_r(strptr, ",", &saveptr1);
          //DEBUG("%c, i=%d, token='%s'", c, i, token);
          if(token==NULL) break;
          if(i==0) {
            bq->report_id = atoi(token);
          } else if(i==1) {
            switch(atoi(token)) {
              case SNS_SMGR_ID_GYRO_V01: 
                bq->sensor[bq->num_sensor].id = SNS_SMGR_ID_GYRO_V01;
                break;
              default:
                bq->sensor[bq->num_sensor].id = SNS_SMGR_ID_ACCEL_V01;
                break;
            }
          } else if(i==2) {
            bq->sensor[bq->num_sensor].sr = atoi(token);
            (bq->num_sensor)++;
          } else {
            LOGE("-%c option contains too many arguments", c);
            exit(1);
          }
          i++;
        }
        num_option++;
        break;
      }
      case 'q': 
      {
        char argstr[512], *saveptr1, *token, *strptr;
        int i = 0;
        strncpy(argstr, optarg, 511);

        for(strptr = argstr; ; strptr = NULL) {
          token = strtok_r(strptr, ":,;", &saveptr1);
          //DEBUG("%c, i=%d, token='%s'", c, i, token);          
          if(token==NULL) break;
          if(i==0) {
            switch(atoi(token)) {
              case SNS_SMGR_ID_GYRO_V01: 
                bq->sensor_query[0].id = SNS_SMGR_ID_GYRO_V01;
                break;
              default:
                bq->sensor_query[0].id = SNS_SMGR_ID_ACCEL_V01;
                break;
            }         
          } else if(i==1) {
            bq->sof_req_delay = atoi(token);
          } else if(i==2) {
            bq->query_duration = atoi(token);
          } else if(i==3) {
            bq->num_query = atoi(token);
          } else {
            LOGE("-%c option contains too many arguments", c);
            exit(1);
          }
          i++;
        }
        num_option++;
        break;
      }
      case 'v':
        flag_verbose = 1;
        break;
      case '?':
        /* getopt_long already printed an error message. */
        LOGE("argument not supported : %c", c);
      default:
        usage();
        exit(1);
        break;
    }
  } // while()

  if(num_option < 2) {
    LOGE("Command does not have sufficient number of required options.");
    usage();
    exit(1);
  }
}


void init_test_params(buf_query_s *bq, int argc, char **argv) {
  init_test_defaults(bq);
  parse_cmd_arg(bq, argc, argv);
  dump_test_params(bq);
}

int send_buffering_query_req(buf_query_s *bq) {
  int err; 
  struct timespec current_time;
  uint64_t t_in_usecs, t0, t1;
  sensor1_msg_header_s hdr;
  sns_smgr_buffering_query_req_msg_v01* queryMsgPtr = NULL;
  test_state_s *st = &(bq->test_state);
  uint64_t offset;

  err = sensor1_alloc_msg_buf(st->sensor1_handle, 
                              sizeof(sns_smgr_buffering_query_req_msg_v01), 
                              (void**) &queryMsgPtr );

  if(err != SENSOR1_SUCCESS) {
    LOGE("Cannot allocate memory to generate report request (err=%d)", err);
    return err;
  }

  hdr.txn_id         = st->seq_no;
  hdr.service_number = SNS_SMGR_SVC_ID_V01;
  hdr.msg_id         = SNS_SMGR_BUFFERING_QUERY_REQ_V01;
  hdr.msg_size       = sizeof(sns_smgr_buffering_query_req_msg_v01);

  clock_gettime( CLOCK_REALTIME, &current_time );
  t_in_usecs = current_time.tv_sec;
  t_in_usecs *= 1000000;
  t_in_usecs += current_time.tv_nsec/1000;

  if(bq->test_state.seq_no == 1) {
    //t0 = bq->t0_frame1;
    
    if(bq->sof_req_delay > 0) {
      t0 = t_in_usecs + (uint64_t)(bq->sof_req_delay)*1000;
    } else {
      t0 = t_in_usecs - (uint64_t)(-1*bq->sof_req_delay)*1000;
    }
    t1 = t0 + (bq->query_duration * 1000);
    bq->t12now_frame1 = t1 - t_in_usecs;
  } else {
    t0 = bq->t1_prev;
    t1 = t_in_usecs + bq->t12now_frame1;
  }
  
  bq->t1_prev = t1;

  pthread_mutex_lock(&smgr_cb_mutex);
  offset = bq->timestamp_offset_apps_dsps / 1000;
  pthread_mutex_unlock(&smgr_cb_mutex);

  t0 -= offset;
  t1 -= offset;

  if(queryMsgPtr) {
    queryMsgPtr->TimePeriod[0]  = (t0 * DSPS_HZ)/USEC_PER_SEC;
    queryMsgPtr->TimePeriod[1]  = (t1 * DSPS_HZ)/USEC_PER_SEC;
    queryMsgPtr->QueryId  = bq->report_id & 0x00FF;
    queryMsgPtr->QueryId |= (((uint16_t)(st->seq_no) << 8) & 0xFF00);
    queryMsgPtr->SensorId = bq->sensor_query[0].id;
    queryMsgPtr->DataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
  }

  err = sensor1_write(st->sensor1_handle, &hdr, queryMsgPtr);
  if(err != SENSOR1_SUCCESS) {
    if(queryMsgPtr) {
      sensor1_free_msg_buf(st->sensor1_handle, queryMsgPtr);
    }

    PRINTV("TX: QueryReq %u of %u, timeAPnow=%lu <--> timeDSPSnow=%lu ==> t0=%lu, t1=%lu, FAILURE (err=%d)",
           bq->test_state.seq_no, bq->num_query, t_in_usecs/1000, (t_in_usecs-offset)/1000, t0/1000, t1/1000, err);
    return(err);
  }

  PRINTV("TX: QueryReq %u of %u, timeAPnow=%lu <--> timeDSPSnow=%lu ==> t0=%lu, t1=%lu, SUCCESS",
         bq->test_state.seq_no, bq->num_query, t_in_usecs/1000, (t_in_usecs-offset)/1000, t0/1000, t1/1000);
  bq->num_query_success++;

  return(SENSOR1_SUCCESS);
}

int send_smgr_buffering_enable_req(buf_query_s *bq) {
  int err;
  uint32_t i; 
  intptr_t dummy = 0;
  sensor1_msg_header_s hdr;
  sns_smgr_buffering_req_msg_v01* msg;
  test_state_s *st = &(bq->test_state);

  PRINTV("%s: Opening Sensor1 session", __FUNCTION__);
  err = sensor1_open(&(st->sensor1_handle), &smgr_cb, dummy);
  if(err != SENSOR1_SUCCESS) {
    LOGE("Cannot open sensor1 connection - err %d", err);
    return(err);
  }

  hdr.txn_id    = 0;
  hdr.msg_id    = SNS_SMGR_BUFFERING_REQ_V01;
  hdr.msg_size  = sizeof(sns_smgr_buffering_req_msg_v01);
  hdr.service_number = SNS_SMGR_SVC_ID_V01;

  err = sensor1_alloc_msg_buf(st->sensor1_handle, hdr.msg_size, (void**) &msg);

  if(msg) {
    msg->ReportId = bq->report_id;
    msg->Action = SNS_SMGR_BUFFERING_ACTION_ADD_V01;
    msg->ReportRate = SNS_SMGR_BUFFERING_REPORT_RATE_NONE_V01; // using Query mode
    msg->Item_len = bq->num_sensor;
    for(i = 0; i < msg->Item_len; i++) {
      msg->Item[i].SensorId      = bq->sensor[i].id;
      msg->Item[i].DataType      = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
      msg->Item[i].SamplingRate  = bq->sensor[i].sr;
      msg->Item[i].SampleQuality = SNS_SMGR_SAMPLE_QUALITY_ACCURATE_TIMESTAMP_V01;
      msg->Item[i].Decimation    = SNS_SMGR_DECIMATION_FILTER_V01;
      msg->Item[i].Calibration   = ((msg->Item[i].SensorId == SNS_SMGR_ID_ACCEL_V01) ?
                                    SNS_SMGR_CAL_SEL_FACTORY_CAL_V01 :
                                    SNS_SMGR_CAL_SEL_FULL_CAL_V01);
    }
  }

  err = sensor1_write(st->sensor1_handle, &hdr, msg);
  if(err != SENSOR1_SUCCESS) {
    if(msg) {
      sensor1_free_msg_buf(st->sensor1_handle, msg);
    }

    LOGE("Cannot enable buffering via Sensor1 (err=%d)", err);
    return(err);
  }

  return 0;
}

int send_smgr_buffering_disable_req(buf_query_s *bq) {
  int err; 
  sensor1_msg_header_s hdr;
  intptr_t dummy = 0;
  sns_smgr_buffering_req_msg_v01 *msg;
  test_state_s *st = &(bq->test_state);

  err = sensor1_alloc_msg_buf(st->sensor1_handle, 
                              sizeof(sns_smgr_buffering_req_msg_v01), 
                              (void**) &msg);

  if(msg) {
    msg->ReportId = bq->report_id;
    msg->Action = SNS_SMGR_BUFFERING_ACTION_DELETE_V01;
    msg->Item_len = 0;
  }
  hdr.txn_id    = 0;
  hdr.msg_id    = SNS_SMGR_BUFFERING_REQ_V01;
  hdr.msg_size  = sizeof(sns_smgr_buffering_req_msg_v01);
  hdr.service_number = SNS_SMGR_SVC_ID_V01;

  err = sensor1_write(st->sensor1_handle, &hdr, msg);
  if(err != SENSOR1_SUCCESS) {
    if(msg) {
      sensor1_free_msg_buf(st->sensor1_handle, msg);
    }
    LOGE("Cannot disable buffering via Sensor1 (err=%d)", err);
    return(err);
  }

  return SENSOR1_SUCCESS;
}

static bool send_time_sync_req(buf_query_s *bq) {
  sensor1_error_e                 error;
  sensor1_msg_header_s            hdr;
  sns_reg_single_read_req_msg_v02 *msg = NULL;
  test_state_s *st = &(bq->test_state);
  bool rv = true;
  intptr_t dummy = 0;

#if 0
  PRINTV("%s: Opening Sensor1 session", __func__);
  error = sensor1_open(&(st->sensor1_handle_timesync), &smgr_cb, dummy);
  if(error != SENSOR1_SUCCESS) {
    LOGE("Cannot open sensor1 connection - err %d", error);
    return(error);
  }
  sleep(1);
#endif

  st->sensor1_handle_timesync = st->sensor1_handle;
  error = sensor1_alloc_msg_buf( st->sensor1_handle_timesync, hdr.msg_size, (void**) &msg );
  if( SENSOR1_SUCCESS != error ) {
    LOGE("%s: sensor1_alloc_msg_buf returned(get) %d", __FUNCTION__, error );
    return error;
  }
  hdr.service_number = SNS_TIME2_SVC_ID_V01;
  hdr.msg_id = SNS_TIME_TIMESTAMP_REQ_V02;
  //hdr.msg_size = 0;
  hdr.txn_id = 1;
  hdr.msg_size = sizeof(sns_time_timestamp_req_msg_v02);

  error = sensor1_write(st->sensor1_handle_timesync, &hdr, msg);
  if( SENSOR1_SUCCESS != error ) {
    if(msg) {
      sensor1_free_msg_buf(st->sensor1_handle_timesync, msg);
    }
    LOGE( "%s: sensor1_write returned %d", __FUNCTION__, error );
    return error;
  }
#if 0
  sleep(1);

  if(st->sensor1_handle_timesync != NULL) {
    LOGE("%s: Closing Sensor1 session", __FUNCTION__);
    error = sensor1_close(st->sensor1_handle_timesync);
    if(error != SENSOR1_SUCCESS) {
      LOGE("Cannot close sensor1 connection - err %d", error);
      return(error);
    }
    st->sensor1_handle_timesync = NULL;
  }
  sleep(1);
#endif
  return SENSOR1_SUCCESS;
}

void cleanup_and_exit(buf_query_s *bq) {
  int error;

  test_state_s *st = &(bq->test_state);

  send_smgr_buffering_disable_req(bq);
  sleep(1);     // enough time for RESP

  if(st->sensor1_handle != NULL) {
    LOGE("%s: Closing Sensor1 session", __FUNCTION__);
    error = sensor1_close(st->sensor1_handle);
    if(error != SENSOR1_SUCCESS) {
      LOGE("Cannot close sensor1 connection - err %d", error);
      return;
    }
    st->sensor1_handle = NULL;
  }
  sleep(1);

  exit(0);
}

int init_test(buf_query_s *bq) {
  test_state_s *st = &(bq->test_state);
  struct itimerval *time = &(bq->tout_val);

  pthread_mutex_init(&smgr_cb_mutex, NULL);

  /* Set our timer */
  time->it_interval.tv_sec = 0;
  time->it_interval.tv_usec = 0;
  time->it_value.tv_sec = 0; 
  time->it_value.tv_usec = bq->query_duration * 1000;

  /* Set an init time - convenience only */
  gettimeofday(&(st->init_time), NULL);

  return send_smgr_buffering_enable_req(bq);
}

void alarm_wakeup (int i) {
  UNREFERENCED_PARAMETER(i);
  signal(SIGALRM,alarm_wakeup);

  buf_query.test_state.seq_no++;

  if(buf_query.test_state.seq_no <= buf_query.num_query) {
    if(SENSOR1_SUCCESS == send_buffering_query_req(&buf_query)) {
       buf_query.test_state.num_requests_sent++;
    }

    if((buf_query.test_state.seq_no%512) == 511) {
      send_time_sync_req(&buf_query);
    }
  } else {
    /* Wait for responses */
//    PRINTV("TX: All requests completed, small slack time for response messages to arrive");
    buf_query.tout_val.it_interval.tv_usec = LAST_IND_WAIT * 1000;
  }

  setitimer(ITIMER_REAL, &(buf_query.tout_val), 0);

  if(buf_query.test_state.seq_no > (buf_query.num_query+1)) {
    if(!done_already) {
      PRINTV("\n--- Summary ---");
      PRINTV("num_query_sent=%u", buf_query.test_state.num_requests_sent);
      PRINTV("num_query_success=%u", buf_query.num_query_success);
      PRINTV("--- ------- ---\n");
      LOGE("Final clean up. Program terminates...");
      done_already = true;
      cleanup_and_exit(&buf_query);
    }
  }
}

int main(int argc, char *argv[]) {
  int err;

  init_test_state(&buf_query);   // initialize test statistics
  init_test_params(&buf_query, argc, argv);


  {
    PRINTV("%s: Enabling SMGR Buffering Request...", __func__);
    if((err=init_test(&buf_query)) < 0) {
      LOGE("Cannot enable SMGR buffering (err=%d)", err);
      return 1;
    }
    PRINTV("%s: Initial time sync before querying...", __func__);
    send_time_sync_req(&buf_query);
  }

  PRINTV("%s: Waiting for %u ms before first query...\n", __func__, buf_query.query_start_delay);
  usleep(buf_query.query_start_delay*1000);

  PRINTV("%s: Beginning to send query requests...", __func__);
  setitimer(ITIMER_REAL, &(buf_query.tout_val), 0);
  signal(SIGALRM, alarm_wakeup);   /* set the Alarm signal capture */
  while(buf_query.test_state.num_requests_sent < buf_query.num_query)
  {
    ;
  }

  LOGE("%s: Abnormal exit. Cleaning up...", __func__);
  sleep(1);
  cleanup_and_exit(&buf_query);
  return 0;
}

