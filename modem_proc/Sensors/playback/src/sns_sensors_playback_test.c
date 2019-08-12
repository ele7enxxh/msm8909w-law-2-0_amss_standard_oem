/*============================================================================
  FILE: sns_sensors_playback_test.c

  This file contains the playback task implementation

  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
============================================================================*/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/playback/src/sns_sensors_playback_test.c#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-08-13  gju  Remove smr_util
  2014-09-17  ps   Fix compiler warnings
  2014-06-02  ps   Update few variables to access from trace32 for flexibility in testing
  2014-05-29  ps   Add support for Buffering and test cases
  2013-12-18  MW   Add support for Hall Effect sensor
  2013-10-23  MW   Add support for SAR sensor
  2013-10-04  sc   Release QMI client notifier handler after being notified for service registration
  2013-07-17  MW   Add support for RGB sensor
  2013-07-15  dc   Support for humidity sensor type
  2013-03-03  ps   Include Pressure Sensor case under BRINGUP_8974
  2013-01-29  gju  Include different file for SMR utlity functions.
  2012-12-10  ag   Initialize ext_signal to NULL
  2012-11-05  vh   Added Relative Samples Per Second Print Message
  2012-11-02  vh   Change the task create to sns_os_task_create_ext for SNS_DSPS_BUILD
  2012-10-23  vh   Processing the messages in task loop
  2012-10-17  vh   Eliminated memory leak issue
  2012-10-14  sc   Use AMD enable request structure instead of common QMD struct
  2012-09-28  vh   Added playback task initialization in sns_sensors_playback_test()
  2012-09-21  vh   Modified Playback for Stress Test function
  2012-09-10  vh   Modified callbacks to print output in task loop
  2012-08-20  vh   Enable all the sensors & added test cases for SMGR & SAM
  2012-07-27  vh   Added QMI code, Enabled three sensors (ACCEL, GYRO, MAG).
  2012-07-11  sc   Added additional test for SAM.
  2012-06-18  vh   Added Run time switch for Concurrency test & selecting sampling frquency, changed decimation to recent value
  2012-05-10  vh   Modified sns_playback_task.c to remove memory issue
  2011-11-18  ad   Updated free and alloc calls to use new API
  2011-07-01  sc   Added enabling logging in initialization routine.
  2011-03-07  sc   Enabling algo is part of initialization.
  2011-02-18  sc   Added synchronization in task initialization.
  2011-01-17  sc   Initial version

============================================================================*/

/*---------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/
#include <stdio.h>

#include "sns_common.h"
#include "sns_osa.h"
#include "sns_init.h"
#include "sns_playback.h"
#include "sns_smgr_api_v01.h"
#include "sns_smgr_sensor_config.h"
#include "sns_sam_qmd_v01.h"
#include "sns_sam_amd_v01.h"
#include "fixed_point.h"
#include "sns_common.h"
#include "sns_sam_sensor_thresh_v01.h"
#include "sns_smgr_main.h"
#include "sns_smgr_priv.h"
#include "sns_sam_priv.h"
#include "sns_memmgr.h"
#include "sns_em.h"

#include "ULog.h"
#include "ULogFront.h"

#include <qmi_client.h>
#include <qmi_idl_lib_internal.h>

/*---------------------------------------------------------------------------
* Preprocessor Definitions and Constants
* -------------------------------------------------------------------------*/

#define SNS_SENSORS_PLAYBACK_TEST_COUNT  5
#define SNS_PLAYBACK_SLEEP_TIME          150    //5Hz read is configured so keep less than 200ms

#define SNS_PLBK_TEST_SLEEP_TIME         10000
#define SNS_PLBK_THR_TEST_VAL            2
#define SNS_PLBK_MAX_REPORTS             0xFF
#define SNS_PLBK_REPORT_ID               0x10

#define PLAYBACK_HIGH_PRIORIY_MSG_REQ    (0x1 << 0)
#define PLAYBACK_LOW_PRIORIY_MSG_REQ     (0x1 << 1)

#define PLAYBACK_SAM_QCSI_WAIT_SIG       (0x1 << 2)
#define PLAYBACK_SAM_QCSI_TIMER_SIG      (0x1 << 3)
#define PLAYBACK_SMGR_QCSI_WAIT_SIG      (0x1 << 4)
#define PLAYBACK_SMGR_QCSI_TIMER_SIG     (0x1 << 5)
#define PLAYBACK_MSG_SIGNAL              (0x1 << 6)

#define SNS_TEST_SIG_FLAG                (PLAYBACK_MSG_SIGNAL + PLAYBACK_HIGH_PRIORIY_MSG_REQ + \
                                          PLAYBACK_LOW_PRIORIY_MSG_REQ)

#define PLAYBACK_SAM_IND_MSG              0x5A
#define PLAYBACK_SAM_RESP_MSG             0xA5
#define PLAYBACK_SMGR_IND_MSG             0xAA
#define PLAYBACK_SMGR_RESP_MSG            0x55

#define SNS_PLBK_PROX_LIGHT_REPORT_RATE_MAX   20
#define SNS_PLBK_SAM_DEF_CLIENT_REPORT_PERIOD (FX_FLTTOFIX_Q16(1.0)) /*in secs */
#define SNS_PLBK_REQ                     0
#define SNS_PLBK_RESP                    1
#define FREQUENCY_PATTERNS               3
#define PLBK_BREAK_TIME                  (DSPS_SLEEP_CLK * 10)
#define SNS_PLBK_CLIENT_SMGR             0
#define SNS_PLBK_CLIENT_SAM              1

#define SNS_PLBK_SENSORS_DEFINED         SNS_SMGR_NUM_SENSORS_DEFINED

/*---------------------------------------------------------------------------
* Type Declarations
* -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Structure Definitions
 * -------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
* Global Data Definitions
* -------------------------------------------------------------------------*/
extern sns_smgr_s    sns_smgr;

/*---------------------------------------------------------------------------
* Static Variable Definitions
* -------------------------------------------------------------------------*/
static qmi_client_os_params playback_smgr_os_params;
#if SNS_PLBK_SAM_CLNT
static qmi_client_os_params playback_sam_os_params;
#endif

static qmi_client_type playback_smgr_user_handle;
static qmi_client_type playback_sam_user_handle;

static sns_q_s  plbk_msg_q;
static sns_q_s  plbk_high_pri_msg_req_q;
static sns_q_s  plbk_low_pri_msg_req_q;

static OS_EVENT *palyback_mutex;
static BOOLEAN sns_plbk_init_done = FALSE;
static uint8 g_Report_id = 0;

static OS_STK sns_playback_stk[SNS_MODULE_STK_SIZE_DSPS_PLAYBACK];
static OS_FLAG_GRP *sns_test_sig_grp;

static volatile uint8_t ConcurrencyEnable = FALSE;
static volatile uint8_t SamplingFreq = 50;
static uint8 report_id_length[SNS_PLBK_MAX_REPORTS];
static volatile uint32_t sns_success_cnt[SNS_PLBK_MAX_REPORTS];

static volatile int8 test_sam_amd_plbk  = TRUE;
static volatile int8 test_sam_thr_plbk  = TRUE;
static volatile int8 test_smgr_plbk     = TRUE;
static volatile int8 plbk_sam_smgr_test = FALSE;
static volatile int8 plbk_stress_test   = FALSE;
static volatile int8 plbk_sns_dev_test  = FALSE;
static volatile int8 plbk_sns_cal_test  = FALSE;

static volatile int8 start_playback  = FALSE;
static volatile uint32 stress_entries = 10;
static volatile plbk_stream_cnt_s stream_cont_str_tst;
static plbk_strm_res_s plbk_steam_test_results;

static uint8 sam_amd_test[2][6];
static uint8 sam_thr_test[2][6];
static uint8 smgr_test[2][SNS_SMGR_BUFFERING_QUERY_IND_V01];
static uint32 sam_amd_ind_test[2];
static uint32 sam_thr_ind_test[2];
static uint32 smgr_ind_test[4];

/* set pattern of sensors to be enabled */
static volatile uint8 sensors_pattern = SNS_PLBK_ACCEL;

/* List of testcases to execute. The logs are stored in Sensors ulog.
 * Ulog size sns_log_size is sufficient to hold the results of testcases defined.
 * {Sensor, Sampling rate, duration in Sec} */
static plbk_test_fmt_s plbk_test_case[] =
{
  { SNS_PLBK_ACCEL, 30, 1 },
  { SNS_PLBK_ACCEL, 100, 1 },
  { SNS_PLBK_GYRO, 30, 1 },
  { SNS_PLBK_GYRO, 200, 1 },
  { SNS_PLBK_GYRO, 250, 1 },
  { SNS_PLBK_MAG, 30, 1 },
  { SNS_PLBK_MAG, 50, 1 },
  { SNS_PLBK_PROX, 5, 1 },
  { SNS_PLBK_LIG, 10, 1 },
  { SNS_PLBK_HUMID, 2, 1 },
  { SNS_PLBK_ACCEL_GYRO, 100, 1 },
  { SNS_PLBK_ACCEL_GYRO_MAG, 50, 1 },
  { SNS_PLBK_SAR, 30, 1 },
  { SNS_PLBK_RGB, 30, 1 },
  { SNS_PLBK_ACCEL_GYRO_MAG_PROX, 30, 1 },
  { SNS_PLBK_ACCEL_GYRO_MAG_PROX_LIG, 30, 1 },
  { SNS_PLBK_HALL_EFFECT, 30, 1 },
};

static uint8_t sensor_id[] =
{
  SNS_SMGR_ID_ACCEL_V01,
  SNS_SMGR_ID_GYRO_V01,
  SNS_SMGR_ID_MAG_V01,
  SNS_SMGR_ID_PRESSURE_V01,
  SNS_SMGR_ID_PROX_LIGHT_V01,
  SNS_SMGR_ID_HUMIDITY_V01,
  SNS_SMGR_ID_RGB_V01,
  SNS_SMGR_ID_SAR_V01,
  SNS_SMGR_ID_HALL_EFFECT_V01,
};

static uint32_t sns_sam_qmd_report_count;
static sns_sam_qmd_report_ind_msg_v01 sns_sam_qmd_report_buf[2];
static uint8_t sns_sam_instance_id;

/* Global Ulog handle to for Sensor test logging */
ULogHandle sns_log_handle;
static uint32_t sns_log_size = 0x20000;
uint8_t plbk_buffer_stream = true;
uint8_t plbk_buffer_report_rate = 30;

/* SAM AMD Requests */
static char *testAmdReqMsg[] =
{
  "SNS_SAM_AMD_CANCEL_REQ_V01",
  "SNS_SAM_AMD_VERSION_REQ_V01",
  "SNS_SAM_AMD_ENABLE_REQ_V01",
  "SNS_SAM_AMD_DISABLE_REQ_V01",
  "SNS_SAM_AMD_GET_REPORT_REQ_V01"
};

/* SAM AMD Responses */
static char *testAmdRespMsg[] =
{
  "SNS_SAM_AMD_CANCEL_RESP_V01",
  "SNS_SAM_AMD_VERSION_RESP_V01",
  "SNS_SAM_AMD_ENABLE_RESP_V01",
  "SNS_SAM_AMD_DISABLE_RESP_V01",
  "SNS_SAM_AMD_GET_REPORT_RESP_V01"
};

/* SAM AMD Indications */
static char *testAmdIndMsg[] =
{
  "SNS_SAM_AMD_REPORT_IND_V01",
  "SNS_SAM_AMD_ERROR_IND_V01"
};

/* SAM Threshold Requests */
static char *testThrReqMsg[] =
{
  "SNS_SAM_SENSOR_THRESH_CANCEL_REQ_V01",
  "SNS_SAM_SENSOR_THRESH_VERSION_REQ_V01",
  "SNS_SAM_SENSOR_THRESH_ENABLE_REQ_V01",
  "SNS_SAM_SENSOR_THRESH_DISABLE_REQ_V01",
  "SNS_SAM_SENSOR_THRESH_GET_REPORT_REQ_V01"
};

/* SAM Threshold Responses */
static char *testThrRespMsg[] =
{
  "SNS_SAM_SENSOR_THRESH_CANCEL_RESP_V01",
  "SNS_SAM_SENSOR_THRESH_VERSION_RESP_V01",
  "SNS_SAM_SENSOR_THRESH_ENABLE_RESP_V01",
  "SNS_SAM_SENSOR_THRESH_DISABLE_RESP_V01",
  "SNS_SAM_SENSOR_THRESH_GET_REPORT_RESP_V01"
};

/* SAM Threshold Indications */
static char *testThrIndMsg[] =
{
  "SNS_SAM_SENSOR_THRESH_REPORT_IND_V01",
  "SNS_SAM_SENSOR_THRESH_ERROR_IND_V01"
};

/* SMGR Requests */
static char *testSmgrReqMsg[] =
{
  "SNS_SMGR_CANCEL_REQ_V01",
  "SNS_SMGR_VERSION_REQ_V01",
  "SNS_SMGR_REPORT_REQ_V01",
  "",
  "SNS_SMGR_CAL_REQ_V01",
  "SNS_SMGR_ALL_SENSOR_INFO_REQ_V01",
  "SNS_SMGR_SINGLE_SENSOR_INFO_REQ_V01",
  "SNS_SMGR_SENSOR_TEST_REQ_V01",
  "SNS_SMGR_SENSOR_POWER_STATUS_REQ_V01",
  "",
  "SNS_SMGR_SENSOR_POWER_CONTROL_REQ_V01",
  "SNS_SMGR_SENSOR_STATUS_REQ_V01",
  "",
  "SNS_SMGR_SINGLE_SENSOR_TEST_REQ_V01"
};

/* SMGR Responses */
static char *testSmgrRespMsg[] =
{
  "SNS_SMGR_CANCEL_RESP_V01",
  "SNS_SMGR_VERSION_RESP_V01",
  "SNS_SMGR_REPORT_RESP_V01",
  "",
  "SNS_SMGR_CAL_RESP_V01",
  "SNS_SMGR_ALL_SENSOR_INFO_RESP_V01",
  "SNS_SMGR_SINGLE_SENSOR_INFO_RESP_V01",
  "SNS_SMGR_SENSOR_TEST_RESP_V01",
  "SNS_SMGR_SENSOR_POWER_STATUS_RESP_V01",
  "",
  "SNS_SMGR_SENSOR_POWER_CONTROL_RESP_V01",
  "SNS_SMGR_SENSOR_STATUS_RESP_V01",
  "",
  "SNS_SMGR_SINGLE_SENSOR_TEST_RESP_V01"
};

/* SMGR Indications */
static char *testSmgrIndMsg[] =
{
  "SNS_SMGR_REPORT_IND_V01",
  "SNS_SMGR_SENSOR_POWER_STATUS_IND_V01",
  "SNS_SMGR_SENSOR_STATUS_IND_V01",
  "SNS_SMGR_SINGLE_SENSOR_TEST_IND_V01"
};

/* sensor combination */
static char *snsComb[] =
{
  "SNS_PLBK_ACCEL",
  "SNS_PLBK_GYRO",
  "SNS_PLBK_MAG",
  "SNS_PLBK_PRESS",
  "SNS_PLBK_PROX",
  "SNS_PLBK_LIG",
  "SNS_PLBK_PROX_LIG",
  "SNS_PLBK_ACCEL_GYRO",
  "SNS_PLBK_ACCEL_GYRO_MAG",
  "SNS_PLBK_ACCEL_GYRO_MAG_PRESS",
  "SNS_PLBK_ACCEL_GYRO_MAG_PRESS_PROX",
  "SNS_PLBK_ACCEL_GYRO_MAG_PRESS_PROX_LIG"
  "SNS_PLBK_ACCEL_GYRO_MAG_PROX",
  "SNS_PLBK_ACCEL_GYRO_MAG_PROX_LIG",
  "SNS_PLBK_HUMID",
  "SNS_PLBK_RGB",
  "SNS_PLBK_SAR",
  "SNS_PLBK_HALL_EFFECT",
};

void sns_playback_put_msg_req_to_queue (
   plbk_msg_req_s *plbkReq, OS_FLAGS sigFlag);

static void sns_playback_smgr_resp_cb (qmi_client_type user_handle,
                           unsigned int msg_id,
                           void *resp_buf,
                           unsigned int resp_buf_len,
                           void *user_cb_data, qmi_client_error_type
                           transp_err);

static void sns_playback_sam_resp_cb (qmi_client_type user_handle,
                          unsigned int msg_id,
                          void *resp_buf,
                          unsigned int resp_buf_len,
                          void *user_cb_data, qmi_client_error_type transp_err);

void sns_sensors_calibration_test (void);

BOOLEAN sns_plbk_stress_test (void);

/*---------------------------------------------------------------------------
* Static Function Definitions
* -------------------------------------------------------------------------*/

static void* sns_plbk_q_get (OS_FLAGS qflags)
{
  void    *msg_ptr = NULL;
  uint8_t os_err   = 0;

  sns_os_mutex_pend(palyback_mutex, 0, &os_err);

  if (PLAYBACK_MSG_SIGNAL == qflags)
  {
    msg_ptr = sns_q_get(&plbk_msg_q);
  }
  else if (PLAYBACK_HIGH_PRIORIY_MSG_REQ == qflags)
  {
    msg_ptr = sns_q_get(&plbk_high_pri_msg_req_q);
  }
  else
  {
    msg_ptr = sns_q_get(&plbk_low_pri_msg_req_q);
  }

  os_err = sns_os_mutex_post(palyback_mutex);

  return msg_ptr;
}

void sns_playback_put_msg_req_to_queue (
   plbk_msg_req_s *plbkReq, OS_FLAGS sigFlag)
{
  uint8_t        os_err;

  sns_q_link(plbkReq, &plbkReq->q_link);

  sns_os_mutex_pend(palyback_mutex, 0, &os_err);

  if (sigFlag == PLAYBACK_HIGH_PRIORIY_MSG_REQ)
    sns_q_put(&plbk_high_pri_msg_req_q, &plbkReq->q_link);
  else
    sns_q_put(&plbk_low_pri_msg_req_q, &plbkReq->q_link);

  os_err = sns_os_mutex_post(palyback_mutex);

  sns_os_sigs_post (sns_test_sig_grp, sigFlag, OS_FLAG_SET, &os_err );
}

void sns_playback_send_msg_requests (plbk_msg_req_s *plbkReq)
{
  qmi_txn_handle txn_handle;
  qmi_client_error_type err;

  if ((plbkReq != NULL) && (plbkReq->rep_req_data != NULL) &&
      (plbkReq->rep_resp_data != NULL))
  {
    if (plbkReq->plbk_cient == SNS_PLBK_CLIENT_SMGR)
    {
      err = qmi_client_send_msg_async (playback_smgr_user_handle,
                                       plbkReq->report_req,
                                       plbkReq->rep_req_data,
                                       plbkReq->rep_req_size,
                                       plbkReq->rep_resp_data,
                                       plbkReq->rep_resp_size,
                                       sns_playback_smgr_resp_cb,
                                       plbkReq->resp_clbk_data,
                                       &txn_handle);
      if (QMI_NO_ERR != err)
      {
        /* extra count indicates failure to send */
        smgr_test[SNS_PLBK_REQ][plbkReq->report_req]++;

        SNS_PLBK_MSG_4("SMGR request = %d, qmi_client_send_msg_async fail error = %d @: %d, %s\n",
                        plbkReq->report_req, err, __LINE__, __FILE__);
      }
      SNS_PLBK_MSG_1("sent PLBK <-> SMGR request# %d\n", plbkReq->report_req);
    }
    else
    {
      err = qmi_client_send_msg_async (playback_sam_user_handle,
                                       plbkReq->report_req,
                                       plbkReq->rep_req_data,
                                       plbkReq->rep_req_size,
                                       plbkReq->rep_resp_data,
                                       plbkReq->rep_resp_size,
                                       sns_playback_sam_resp_cb,
                                       plbkReq->resp_clbk_data,
                                       &txn_handle);
      if (QMI_NO_ERR != err)
      {
        /* extra count indicates failure to send */
        smgr_test[SNS_PLBK_REQ][plbkReq->report_req]++;

        SNS_PLBK_MSG_4("SAM request = %d, qmi_client_send_msg_async fail error = %d @: %d, %s\n",
                        plbkReq->report_req, err, __LINE__, __FILE__);
      }
      SNS_PLBK_MSG_1("sent PLBK <-> SAM request# %d\n", plbkReq->report_req);
    }

    /* free the report request data */
    SNS_OS_FREE(plbkReq->rep_req_data);
    SNS_OS_FREE(plbkReq);
    plbkReq = NULL;
  }
}

static void sns_playback_process_sam_resp (void *samRespPtr, int32_t msgId, uint32 thr_amd)
{
  if (thr_amd == SNS_PLBK_THR_TEST_VAL)
  {
    switch (msgId)
    {
      case SNS_SAM_SENSOR_THRESH_CANCEL_RESP_V01:
        sam_thr_test[SNS_PLBK_RESP][SNS_SAM_SENSOR_THRESH_CANCEL_RESP_V01]++;
        SNS_PLBK_MSG("PLAYBACK received SNS_SAM_SENSOR_THRESH_CANCEL_RESP_V01\n");
        break;
      case SNS_SAM_SENSOR_THRESH_VERSION_RESP_V01:
        sam_thr_test[SNS_PLBK_RESP][SNS_SAM_SENSOR_THRESH_VERSION_RESP_V01]++;
        SNS_PLBK_MSG("PLAYBACK received SNS_SAM_SENSOR_THRESH_VERSION_RESP_V01\n");
        break;
      case SNS_SAM_SENSOR_THRESH_ENABLE_RESP_V01:
        {
          sns_sam_sensor_thresh_enable_resp_msg_v01 *respPtr =
                            (sns_sam_sensor_thresh_enable_resp_msg_v01 *)samRespPtr;

          sam_thr_test[SNS_PLBK_RESP][SNS_SAM_SENSOR_THRESH_ENABLE_RESP_V01]++;
          SNS_PLBK_MSG("PLAYBACK received SNS_SAM_SENSOR_THRESH_ENABLE_RESP_V01\n");

          if (respPtr->resp.sns_result_t != SNS_SUCCESS)
          {
            /* extra count indicates wrong data */
            sam_thr_test[SNS_PLBK_RESP][SNS_SAM_SENSOR_THRESH_ENABLE_RESP_V01]++;

            SNS_PLBK_MSG_1("error in SNS_SAM_SENSOR_THRESH_ENABLE_RESP_V01 = %d\n", respPtr->resp.sns_err_t);
          }
          else
          {
            SNS_PLBK_MSG_2("instance_id_valid = %d, instance_id = %d\n",
                        respPtr->instance_id_valid, respPtr->instance_id);
          }
        }
        break;
      case SNS_SAM_SENSOR_THRESH_DISABLE_RESP_V01:
        {
          sns_sam_sensor_thresh_disable_resp_msg_v01 *respPtr =
                            (sns_sam_sensor_thresh_disable_resp_msg_v01 *)samRespPtr;

          sam_thr_test[SNS_PLBK_RESP][SNS_SAM_SENSOR_THRESH_DISABLE_RESP_V01]++;
          SNS_PLBK_MSG("PLAYBACK received SNS_SAM_SENSOR_THRESH_DISABLE_RESP_V01\n");

          if (respPtr->resp.sns_result_t != SNS_SUCCESS)
          {
            /* extra count indicates wrong data */
            sam_thr_test[SNS_PLBK_RESP][SNS_SAM_SENSOR_THRESH_DISABLE_RESP_V01]++;

            SNS_PLBK_MSG_1("error in SNS_SAM_SENSOR_THRESH_DISABLE_RESP_V01 = %d\n", respPtr->resp.sns_err_t);
          }
          else
          {
            SNS_PLBK_MSG_2("instance_id_valid = %d, instance_id = %d\n",
                        respPtr->instance_id_valid, respPtr->instance_id);
          }
        }
        break;
      case SNS_SAM_SENSOR_THRESH_GET_REPORT_RESP_V01:
        {
          sns_sam_sensor_thresh_get_report_resp_msg_v01 *respPtr =
                        (sns_sam_sensor_thresh_get_report_resp_msg_v01 *)samRespPtr;

          sam_thr_test[SNS_PLBK_RESP][SNS_SAM_SENSOR_THRESH_GET_REPORT_RESP_V01]++;
          SNS_PLBK_MSG("PLAYBACK received SNS_SAM_SENSOR_THRESH_GET_REPORT_RESP_V01\n");

          if (respPtr->resp.sns_result_t != SNS_SUCCESS)
          {
            /* extra count indicates wrong data */
            sam_thr_test[SNS_PLBK_RESP][SNS_SAM_SENSOR_THRESH_GET_REPORT_RESP_V01]++;

            SNS_PLBK_MSG_1("error in SNS_SAM_SENSOR_THRESH_GET_REPORT_RESP_V01 = %d\n", respPtr->resp.sns_err_t);
          }
          else
          {
            SNS_PLBK_MSG_4("instance_id_valid = %d, instance_id = %d, timestamp_valid = %d, timestamp = %lu\n",
                     respPtr->instance_id_valid, respPtr->instance_id,
                     respPtr->timestamp_valid, respPtr->timestamp);

            SNS_PLBK_MSG_4("sample_value_valid = %d, sample_value[0] = %lu, sample_value[0] = %lu, sample_value[0] = %lu\n",
                     respPtr->sample_value_valid, respPtr->sample_value[0],
                     respPtr->sample_value[1], respPtr->sample_value[2]);
          }
        }
        break;
      default:
        SNS_PLBK_MSG_1("PLAYBACK %s received invalid response\n", __func__);
        break;
    }
  }
  else
  {
    switch (msgId)
    {
      case SNS_SAM_AMD_CANCEL_RESP_V01:
        sam_amd_test[SNS_PLBK_RESP][SNS_SAM_AMD_CANCEL_RESP_V01]++;
        SNS_PLBK_MSG("PLAYBACK received SNS_SAM_AMD_CANCEL_RESP_V01\n");
        break;
      case SNS_SAM_AMD_VERSION_RESP_V01:
        sam_amd_test[SNS_PLBK_RESP][SNS_SAM_AMD_VERSION_RESP_V01]++;
        SNS_PLBK_MSG("PLAYBACK received SNS_SAM_AMD_VERSION_RESP_V01\n");
        break;
      case SNS_SAM_AMD_ENABLE_RESP_V01:
        {
          sns_sam_qmd_enable_resp_msg_v01 *respPtr =
                        (sns_sam_qmd_enable_resp_msg_v01 *)samRespPtr;

          sam_amd_test[SNS_PLBK_RESP][SNS_SAM_AMD_ENABLE_RESP_V01]++;
          SNS_PLBK_MSG("PLAYBACK received SNS_SAM_AMD_ENABLE_RESP_V01\n");

          if (respPtr->resp.sns_result_t != SNS_SUCCESS)
          {
            /* extra count indicates wrong data */
            sam_amd_test[SNS_PLBK_RESP][SNS_SAM_AMD_ENABLE_RESP_V01]++;

            SNS_PLBK_MSG_1("error in SNS_SAM_AMD_ENABLE_RESP_V01 = %d\n", respPtr->resp.sns_err_t);
          }
          else
          {
            SNS_PLBK_MSG_1("instance_id = %d\n", respPtr->instance_id);
            sns_sam_instance_id = respPtr->instance_id;
          }
        }
        break;
      case SNS_SAM_AMD_DISABLE_RESP_V01:
        {
          sns_sam_qmd_disable_resp_msg_v01 *respPtr =
                              (sns_sam_qmd_disable_resp_msg_v01 *)samRespPtr;

          sam_amd_test[SNS_PLBK_RESP][SNS_SAM_AMD_DISABLE_RESP_V01]++;
          SNS_PLBK_MSG("PLAYBACK received SNS_SAM_AMD_DISABLE_RESP_V01\n");

          if (respPtr->resp.sns_result_t != SNS_SUCCESS)
          {
            /* extra count indicates wrong data */
            sam_amd_test[SNS_PLBK_RESP][SNS_SAM_AMD_DISABLE_RESP_V01]++;

            SNS_PLBK_MSG_1("error in SNS_SAM_AMD_DISABLE_RESP_V01 = %d\n", respPtr->resp.sns_err_t);
          }
          else
          {
            SNS_PLBK_MSG_1("instance_id = %d\n", respPtr->instance_id);
            sns_sam_instance_id = respPtr->instance_id;
          }
        }
        break;
      case SNS_SAM_AMD_GET_REPORT_RESP_V01:
        {
          sns_common_resp_s_v01 sns_sam_qmd_result_buf[2];
          sns_sam_qmd_get_report_resp_msg_v01 *respPtr =
                            (sns_sam_qmd_get_report_resp_msg_v01 *) samRespPtr;

          sam_amd_test[SNS_PLBK_RESP][SNS_SAM_AMD_GET_REPORT_RESP_V01]++;
          SNS_PLBK_MSG("PLAYBACK received SNS_SAM_AMD_GET_REPORT_RESP_V01\n");

          if (respPtr->resp.sns_result_t != SNS_SUCCESS)
          {
            /* extra count indicates wrong data */
            sam_amd_test[SNS_PLBK_RESP][SNS_SAM_AMD_GET_REPORT_RESP_V01]++;

            SNS_PLBK_MSG_1("error in SNS_SAM_AMD_GET_REPORT_RESP_V01 = %d\n", respPtr->resp.sns_err_t);
          }
          else
          {
            sns_sam_qmd_result_buf[1] = sns_sam_qmd_result_buf[0];
            sns_sam_qmd_result_buf[0].sns_result_t = respPtr->resp.sns_result_t;
            sns_sam_qmd_result_buf[0].sns_err_t = respPtr->resp.sns_err_t;

            sns_sam_qmd_report_buf[1] = sns_sam_qmd_report_buf[0];
            sns_sam_qmd_report_buf[0].instance_id = respPtr->instance_id;
            sns_sam_qmd_report_buf[0].state = respPtr->state;
            sns_sam_qmd_report_buf[0].timestamp = respPtr->timestamp;

            SNS_PLBK_MSG_3("algo instance id %d, algo state %d, report timestamp %lu\n",
                    sns_sam_qmd_report_buf[0].instance_id, sns_sam_qmd_report_buf[0].state,
                    sns_sam_qmd_report_buf[0].timestamp);

            SNS_PLBK_MSG_2("result %d, error code %d\n", sns_sam_qmd_result_buf[0].sns_result_t,
                           sns_sam_qmd_result_buf[0].sns_err_t);
          }
        }
        break;
      default:
        SNS_PLBK_MSG_1("PLAYBACK %s received invalid response\n", __func__);
        break;
    }
  }
}

static void sns_playback_process_smgr_resp (void *smgrRespPtr, int32_t msgId, uint32 user_data)
{
  switch (msgId)
  {
    case SNS_SMGR_CANCEL_RESP_V01:
      {
        sns_common_cancel_resp_msg_v01 *respPtr;
        respPtr = (sns_common_cancel_resp_msg_v01 *)smgrRespPtr;

        smgr_test[SNS_PLBK_RESP][SNS_SMGR_CANCEL_RESP_V01]++;
        SNS_PLBK_MSG("PLAYBACK received SNS_SMGR_CANCEL_RESP_V01\n");

        if (respPtr->resp.sns_result_t != SNS_SUCCESS)
        {
           /* extra count indicates wrong data */
           smgr_test[SNS_PLBK_RESP][SNS_SMGR_CANCEL_RESP_V01]++;

           SNS_PLBK_MSG_1("error in SNS_SMGR_CANCEL_RESP_V01 = %d\n", respPtr->resp.sns_err_t);
        }
      }
      break;
    case SNS_SMGR_VERSION_RESP_V01:
      {
        sns_common_version_resp_msg_v01 *respPtr;
        respPtr = (sns_common_version_resp_msg_v01 *)smgrRespPtr;

        smgr_test[SNS_PLBK_RESP][SNS_SMGR_VERSION_RESP_V01]++;
        SNS_PLBK_MSG("PLAYBACK received SNS_SMGR_VERSION_RESP_V01\n");

        if (respPtr->resp.sns_result_t != SNS_SUCCESS)
        {
           /* extra count indicates wrong data */
           smgr_test[SNS_PLBK_RESP][SNS_SMGR_VERSION_RESP_V01]++;

           SNS_PLBK_MSG_1("error in SNS_SMGR_VERSION_RESP_V01 = %d\n", respPtr->resp.sns_err_t);
        }
        else
        {
          SNS_PLBK_MSG_2("interface_version_number = %lu, max_message_id = %u\n",
                   respPtr->interface_version_number, respPtr->max_message_id);
        }
      }
      break;
    case SNS_SMGR_REPORT_RESP_V01:
      {
        uint32_t index;
        sns_smgr_periodic_report_resp_msg_v01 *respPtr;
        respPtr = (sns_smgr_periodic_report_resp_msg_v01 *) smgrRespPtr;

        smgr_test[SNS_PLBK_RESP][SNS_SMGR_REPORT_RESP_V01]++;

        if (respPtr->Resp.sns_result_t != SNS_SUCCESS)
        {
           /* extra count indicates wrong data */
           smgr_test[SNS_PLBK_RESP][SNS_SMGR_REPORT_RESP_V01]++;

           SNS_PLBK_MSG_2("error in SNS_SMGR_REPORT_RESP_V01, ReportId = %d, error = %d\n",
                           respPtr->ReportId, respPtr->Resp.sns_err_t);
        }
        else
        {
          SNS_PLBK_MSG_1("PLAYBACK received SNS_SMGR_REPORT_RESP_V01, ReportId = %d\n", respPtr->ReportId);
        }

        SNS_PLBK_MSG_2("AckNak = %d, ReasonPair_len = %lu\n", respPtr->AckNak, respPtr->ReasonPair_len);

        for (index = 0; index < respPtr->ReasonPair_len; index++)
        {
          SNS_PLBK_MSG_2("ItemNum = %d, Reason = %d\n", respPtr->ReasonPair[index].ItemNum,
                          respPtr->ReasonPair[index].Reason);
        }
      }
      break;
    case SNS_SMGR_BUFFERING_RESP_V01:
      {
        uint32_t index;
        sns_smgr_buffering_resp_msg_v01 *respPtr;
        respPtr = (sns_smgr_buffering_resp_msg_v01 *) smgrRespPtr;

        smgr_test[SNS_PLBK_RESP][SNS_SMGR_BUFFERING_RESP_V01]++;

        if (respPtr->Resp.sns_result_t != SNS_SUCCESS)
        {
           /* extra count indicates wrong data */
           smgr_test[SNS_PLBK_RESP][SNS_SMGR_REPORT_RESP_V01]++;

           SNS_PLBK_MSG_2("error in SNS_SMGR_BUFFERING_RESP_V01, ReportId = %d, error = %d\n",
                           respPtr->ReportId, respPtr->Resp.sns_err_t);
        }
        else
        {
          SNS_PLBK_MSG_1("PLAYBACK received SNS_SMGR_BUFFERING_RESP_V01, ReportId = %d\n", respPtr->ReportId);
        }

        SNS_PLBK_MSG_2("AckNak = %d, ReasonPair_len = %lu\n", respPtr->AckNak, respPtr->ReasonPair_len);

        for (index = 0; index < respPtr->ReasonPair_len; index++)
        {
          SNS_PLBK_MSG_2("ItemNum = %d, Reason = %d\n", respPtr->ReasonPair[index].ItemNum,
                          respPtr->ReasonPair[index].Reason);
        }
      }
      break;
    case SNS_SMGR_CAL_RESP_V01:
      {
        sns_smgr_sensor_cal_resp_msg_v01 *respPtr;
        respPtr = (sns_smgr_sensor_cal_resp_msg_v01 *) smgrRespPtr;

        smgr_test[SNS_PLBK_RESP][SNS_SMGR_CAL_RESP_V01]++;
        SNS_PLBK_MSG("PLAYBACK received SNS_SMGR_CAL_RESP_V01\n");

        if (respPtr->Resp.sns_result_t != SNS_SUCCESS)
        {
           /* extra count indicates wrong data */
           smgr_test[SNS_PLBK_RESP][SNS_SMGR_CAL_RESP_V01]++;

           SNS_PLBK_MSG_1("error in SNS_SMGR_CAL_RESP_V01 = %d\n", respPtr->Resp.sns_err_t);
        }
      }
      break;
    case SNS_SMGR_ALL_SENSOR_INFO_RESP_V01:
      {
        uint32_t index;
        sns_smgr_all_sensor_info_resp_msg_v01 *respPtr;
        respPtr = (sns_smgr_all_sensor_info_resp_msg_v01 *)smgrRespPtr;

        smgr_test[SNS_PLBK_RESP][SNS_SMGR_ALL_SENSOR_INFO_RESP_V01]++;
        SNS_PLBK_MSG("PLAYBACK received SNS_SMGR_ALL_SENSOR_INFO_RESP_V01\n");

        if (respPtr->Resp.sns_result_t != SNS_SUCCESS)
        {
           /* extra count indicates wrong data */
           smgr_test[SNS_PLBK_RESP][SNS_SMGR_ALL_SENSOR_INFO_RESP_V01]++;

           SNS_PLBK_MSG_1("error in SNS_SMGR_ALL_SENSOR_INFO_RESP_V01 = %d\n", respPtr->Resp.sns_err_t);
        }
        else
        {
          SNS_PLBK_MSG_1("SensorInfo_len = %lu\n", respPtr->SensorInfo_len);
          for (index = 0; index < respPtr->SensorInfo_len; index++)
          {
            SNS_PLBK_MSG_3("SensorID = %d, SensorShortName_len = %lu, SensorID = %s\n",
                        respPtr->SensorInfo[index].SensorID,
                        respPtr->SensorInfo[index].SensorShortName_len,
                        respPtr->SensorInfo[index].SensorShortName);
          }
        }
      }
      break;
    case SNS_SMGR_SINGLE_SENSOR_INFO_RESP_V01:
      {
        uint32_t index;
        sns_smgr_single_sensor_info_resp_msg_v01 *respPtr;
        respPtr = (sns_smgr_single_sensor_info_resp_msg_v01 *)smgrRespPtr;

        smgr_test[SNS_PLBK_RESP][SNS_SMGR_SINGLE_SENSOR_INFO_RESP_V01]++;
        SNS_PLBK_MSG("PLAYBACK received SNS_SMGR_SINGLE_SENSOR_INFO_RESP_V01\n");

        if (respPtr->Resp.sns_result_t != SNS_SUCCESS)
        {
           /* extra count indicates wrong data */
           smgr_test[SNS_PLBK_RESP][SNS_SMGR_SINGLE_SENSOR_INFO_RESP_V01]++;

           SNS_PLBK_MSG_1("error in SNS_SMGR_SINGLE_SENSOR_INFO_RESP_V01 = %d\n", respPtr->Resp.sns_err_t);
        }
        else
        {
          SNS_PLBK_MSG_1("data_type_info_len = %lu\n", respPtr->SensorInfo.data_type_info_len);
          for (index = 0; index < respPtr->SensorInfo.data_type_info_len; index++)
          {
            SNS_PLBK_MSG_4("SensorID = %d, DataType = %d, SensorName_len = %lu, SensorName = %s\n",
            respPtr->SensorInfo.data_type_info[index].SensorID,
            respPtr->SensorInfo.data_type_info[index].DataType,
            respPtr->SensorInfo.data_type_info[index].SensorName_len,
            respPtr->SensorInfo.data_type_info[index].SensorName);

            SNS_PLBK_MSG_3("VendorName_len = %lu, VendorName =  %s, Version = %lu\n",
            respPtr->SensorInfo.data_type_info[index].VendorName_len,
            respPtr->SensorInfo.data_type_info[index].VendorName,
            respPtr->SensorInfo.data_type_info[index].Version);

            SNS_PLBK_MSG_5("MaxSampleRate = %u, IdlePower = %u, MaxPower = %u, MaxRange = %lu, Resolution = %lu\n",
            respPtr->SensorInfo.data_type_info[index].MaxSampleRate,
            respPtr->SensorInfo.data_type_info[index].IdlePower,
            respPtr->SensorInfo.data_type_info[index].MaxPower,
            respPtr->SensorInfo.data_type_info[index].MaxRange,
            respPtr->SensorInfo.data_type_info[index].Resolution);
          }
        }
      }
      break;
    case SNS_SMGR_SENSOR_TEST_RESP_V01:
      {
        uint32_t index;
        sns_smgr_sensor_test_resp_msg_v01  *respPtr;
        respPtr = (sns_smgr_sensor_test_resp_msg_v01 *)smgrRespPtr;

        smgr_test[SNS_PLBK_RESP][SNS_SMGR_SENSOR_TEST_RESP_V01]++;
        SNS_PLBK_MSG("PLAYBACK received SNS_SMGR_SENSOR_TEST_RESP_V01\n");

        if (respPtr->Resp.sns_result_t != SNS_SUCCESS)
        {
           /* extra count indicates wrong data */
           smgr_test[SNS_PLBK_RESP][SNS_SMGR_SENSOR_TEST_RESP_V01]++;

           SNS_PLBK_MSG_1("error in SNS_SMGR_SENSOR_TEST_RESP_V01 = %d\n", respPtr->Resp.sns_err_t);
        }
        else
        {
          SNS_PLBK_MSG_1("SNS_SMGR_SENSOR_TEST_RESP_V01 length = %lu\n", respPtr->result_len);
          for (index = 0; index < respPtr->result_len; index++)
          {
             SNS_PLBK_MSG_5("SensorID = %d, BusCanAccessSensor = %d, CanCommandSensor = %d, CanReadSensorStatus = %d, CanReadSensorData = %d\n",
                     respPtr->result[index].SensorID, respPtr->result[index].BusCanAccessSensor,
                     respPtr->result[index].CanCommandSensor, respPtr->result[index].CanReadSensorStatus, respPtr->result[index].CanReadSensorData);

             SNS_PLBK_MSG_4("DataShowsNoise = %d, CanReadFactoryCalibrationROM = %d, ValidSelfTestReport = %d, CanReceiveInterrupt = %d\n",
                     respPtr->result[index].DataShowsNoise,  respPtr->result[index].CanReadFactoryCalibrationROM,
                     respPtr->result[index].ValidSelfTestReport, respPtr->result[index].CanReceiveInterrupt);
          }
        }
      }
      break;
    case SNS_SMGR_SENSOR_POWER_STATUS_RESP_V01:
      {
        sns_smgr_sensor_power_status_resp_msg_v01 *respPtr;
        respPtr = (sns_smgr_sensor_power_status_resp_msg_v01 *)smgrRespPtr;

        smgr_test[SNS_PLBK_RESP][SNS_SMGR_SENSOR_POWER_STATUS_RESP_V01]++;
        SNS_PLBK_MSG("PLAYBACK received SNS_SMGR_SENSOR_POWER_STATUS_RESP_V01\n");

        if (respPtr->Resp.sns_result_t != SNS_SUCCESS)
        {
           /* extra count indicates wrong data */
           smgr_test[SNS_PLBK_RESP][SNS_SMGR_SENSOR_POWER_STATUS_RESP_V01]++;

           SNS_PLBK_MSG_1("error in SNS_SMGR_SENSOR_POWER_STATUS_RESP_V01 = %d\n", respPtr->Resp.sns_err_t);
        }
      }
      break;
    case SNS_SMGR_SENSOR_POWER_CONTROL_RESP_V01:
      {
        sns_smgr_sensor_power_control_resp_msg_v01 *respPtr;
        respPtr = (sns_smgr_sensor_power_control_resp_msg_v01 *)smgrRespPtr;

        SNS_PLBK_MSG("PLAYBACK received SNS_SMGR_SENSOR_POWER_CONTROL_RESP_V01\n");
        smgr_test[SNS_PLBK_RESP][SNS_SMGR_SENSOR_POWER_CONTROL_RESP_V01]++;

        if (respPtr->Resp.sns_result_t != SNS_SUCCESS)
        {
           /* extra count indicates wrong data */
           smgr_test[SNS_PLBK_RESP][SNS_SMGR_SENSOR_POWER_CONTROL_RESP_V01]++;

           SNS_PLBK_MSG_1("error in SNS_SMGR_SENSOR_POWER_CONTROL_RESP_V01 = %d\n", respPtr->Resp.sns_err_t);
        }
      }
      break;
    case SNS_SMGR_SENSOR_STATUS_RESP_V01:
      {
        sns_smgr_sensor_status_resp_msg_v01* respPtr;
        respPtr = (sns_smgr_sensor_status_resp_msg_v01 *)smgrRespPtr;

        smgr_test[SNS_PLBK_RESP][SNS_SMGR_SENSOR_STATUS_RESP_V01]++;
        SNS_PLBK_MSG("PLAYBACK received SNS_SMGR_SENSOR_STATUS_RESP_V01\n");

        if (respPtr->Resp.sns_result_t != SNS_SUCCESS)
        {
           /* extra count indicates wrong data */
           smgr_test[SNS_PLBK_RESP][SNS_SMGR_SENSOR_STATUS_RESP_V01]++;

           SNS_PLBK_MSG_1("error in SNS_SMGR_SENSOR_STATUS_RESP_V01 = %d\n", respPtr->Resp.sns_err_t);
        }
      }
      break;
    case SNS_SMGR_SINGLE_SENSOR_TEST_RESP_V01:
      {
        sns_smgr_single_sensor_test_resp_msg_v01 *respPtr;
        respPtr = (sns_smgr_single_sensor_test_resp_msg_v01 *)smgrRespPtr;

        smgr_test[SNS_PLBK_RESP][SNS_SMGR_SINGLE_SENSOR_TEST_RESP_V01]++;
        SNS_PLBK_MSG("PLAYBACK received SNS_SMGR_SINGLE_SENSOR_TEST_RESP_V01\n");

        if (respPtr->Resp.sns_result_t != SNS_SUCCESS)
        {
           /* extra count indicates wrong data */
           smgr_test[SNS_PLBK_RESP][SNS_SMGR_SINGLE_SENSOR_TEST_RESP_V01]++;

           SNS_PLBK_MSG_1("error in SNS_SMGR_SINGLE_SENSOR_TEST_RESP_V01 = %d\n", respPtr->Resp.sns_err_t);
        }
        else
        {
          SNS_PLBK_MSG_4("SensorID = %d, DataType = %d, TestType = %d, TestStatus = %d\n",
                   respPtr->SensorID, respPtr->DataType, respPtr->TestType, respPtr->TestStatus);
        }
      }
      break;
    default:
        SNS_PLBK_MSG_1("PLAYBACK %s received invalid response\n", __func__);
      break;
  }
}

static void sns_playback_process_sam_ind (void *samIndPtr, int32_t msgId, uint32 thr_amd)
{
  if (thr_amd == SNS_PLBK_THR_TEST_VAL)
  {
    switch (msgId)
    {
      case SNS_SAM_SENSOR_THRESH_REPORT_IND_V01:
        {
          sns_sam_sensor_thresh_report_ind_msg_v01 *ind_ptr =
                             (sns_sam_sensor_thresh_report_ind_msg_v01 *) samIndPtr;

          sam_thr_ind_test[0]++;
          SNS_PLBK_MSG("PLAYBACK received SNS_SAM_SENSOR_THRESH_REPORT_IND_V01\n");

          SNS_PLBK_MSG_2("instace id = %d, time stamp = %lu\n", ind_ptr->instance_id, ind_ptr->timestamp);

          SNS_PLBK_MSG_3("sample vales = %lu, %lu, %lu\n", ind_ptr->sample_value[0],
                               ind_ptr->sample_value[1], ind_ptr->sample_value[2]);
        }
        break;
      case SNS_SAM_SENSOR_THRESH_ERROR_IND_V01:
        {
          sns_sam_sensor_thresh_error_ind_msg_v01 *ind_ptr =
                              (sns_sam_sensor_thresh_error_ind_msg_v01 *) samIndPtr;

          sam_thr_ind_test[1]++;
          SNS_PLBK_MSG("PLAYBACK received SNS_SAM_SENSOR_THRESH_ERROR_IND_V01\n");

          SNS_PLBK_MSG_2("Error = %d, instance id = %d\n", ind_ptr->error, ind_ptr->instance_id);
        }
        break;
      default:
        SNS_PLBK_MSG_1("PLAYBACK %s received invalid indication\n", __func__);
        break;
    }
  }
  else
  {
    switch (msgId)
    {
      case SNS_SAM_AMD_ERROR_IND_V01:
        {
          sns_sam_qmd_error_ind_msg_v01 *ind_ptr = (sns_sam_qmd_error_ind_msg_v01 *) samIndPtr;

          sam_amd_ind_test[0]++;
          SNS_PLBK_MSG("PLAYBACK received SNS_SAM_AMD_ERROR_IND_V01\n");

          SNS_PLBK_MSG_2("Error = %d, instance id = %d\n", ind_ptr->error, ind_ptr->instance_id);
        }
        break;
      case SNS_SAM_AMD_REPORT_IND_V01:
        {
          uint32_t sns_sam_qmd_report_period;
          sns_sam_qmd_report_ind_msg_v01 *indPtr = (sns_sam_qmd_report_ind_msg_v01 *) samIndPtr;

          sam_amd_ind_test[1]++;
          SNS_PLBK_MSG("PLAYBACK received SNS_SAM_AMD_REPORT_IND_V01\n");

          if (sns_sam_qmd_report_count & 1)
          {
            sns_sam_qmd_report_buf[1].instance_id = indPtr->instance_id;
            sns_sam_qmd_report_buf[1].state = indPtr->state;
            sns_sam_qmd_report_buf[1].timestamp = indPtr->timestamp;
            sns_sam_qmd_report_period = (sns_sam_qmd_report_buf[1].timestamp - sns_sam_qmd_report_buf[0].timestamp);
            SNS_PLBK_MSG_3("algo instance id %d, algo state %d, report timestamp %lu\n",
                    sns_sam_qmd_report_buf[1].instance_id,
                    sns_sam_qmd_report_buf[1].state,
                    sns_sam_qmd_report_buf[1].timestamp);
          }
          else
          {
            sns_sam_qmd_report_buf[0].instance_id = indPtr->instance_id;
            sns_sam_qmd_report_buf[0].state = indPtr->state;
            sns_sam_qmd_report_buf[0].timestamp = indPtr->timestamp;
            sns_sam_qmd_report_period = (sns_sam_qmd_report_buf[0].timestamp - sns_sam_qmd_report_buf[1].timestamp);
            SNS_PLBK_MSG_3("algo instance id %d, algo state %d, report timestamp %lu\n",
                    sns_sam_qmd_report_buf[0].instance_id,
                    sns_sam_qmd_report_buf[0].state,
                    sns_sam_qmd_report_buf[0].timestamp);
          }
          sns_sam_qmd_report_count++;

          SNS_PLBK_MSG_2("report count %lu, report period %lu\n", sns_sam_qmd_report_count, sns_sam_qmd_report_period);
        }
        break;
      default:
        SNS_PLBK_MSG_1("PLAYBACK %s received invalid indication\n", __func__);
        break;
    }
  }
}

static void sns_playback_process_smgr_ind (void *smgrIndPtr, int32_t msgId, uint32 user_data)
{
  switch (msgId)
  {
    case SNS_SMGR_REPORT_IND_V01:
      {
        uint32_t index;
        sns_smgr_periodic_report_ind_msg_v01 *ind_ptr;
        ind_ptr = (sns_smgr_periodic_report_ind_msg_v01 *) smgrIndPtr;

        smgr_ind_test[0]++;
        SNS_PLBK_MSG("PLAYBACK received SNS_SMGR_REPORT_IND_V01\n");

        /* success only if it has valid data length */
        if (report_id_length[ind_ptr->ReportId] == ind_ptr->Item_len)
        {
          sns_success_cnt[ind_ptr->ReportId]++;

          SNS_PLBK_MSG_3("PLAYBACK ReportId = %d, status = %d, success count = %lu\n",
                          ind_ptr->ReportId, ind_ptr->status, sns_success_cnt[ind_ptr->ReportId]);
          SNS_PLBK_MSG_2("PLAYBACK CurrentRate = %d, Item_len = %lu\n", ind_ptr->CurrentRate, ind_ptr->Item_len);
        }
        else
        {
          SNS_PLBK_MSG("Playback received invalid data @function sns_playback_process_smgr_ind\n");

          SNS_PLBK_MSG_2("PLAYBACK ReportId = %d, status = %d\n", ind_ptr->ReportId, ind_ptr->status);

          SNS_PLBK_MSG_3("PLAYBACK CurrentRate = %d, Requested Item_len = %d, Received Item_len = %lu\n",
                          ind_ptr->CurrentRate, report_id_length[ind_ptr->ReportId], ind_ptr->Item_len);
        }

        for (index = 0; index < ind_ptr->Item_len; index++)
        {
          SNS_PLBK_MSG_2("PLAYBACK SensorId = %d, Ts = %lu\n", ind_ptr->Item[index].SensorId,
                        ind_ptr->Item[index].TimeStamp);

          SNS_PLBK_MSG_3("PLAYBACK x = %f, y = %f, z = %f\n",
                        FX_FIXTOFLT_Q16(ind_ptr->Item[index].ItemData[0]),
                        FX_FIXTOFLT_Q16(ind_ptr->Item[index].ItemData[1]),
                        FX_FIXTOFLT_Q16(ind_ptr->Item[index].ItemData[2]));
           
          ULogFront_RealTimePrintf(sns_log_handle, 5, "ID=%d,ts=%lu, %d, %d, %d",
                        ind_ptr->Item[index].SensorId,
                        ind_ptr->Item[index].TimeStamp,
                        (ind_ptr->Item[index].ItemData[0]),
                        (ind_ptr->Item[index].ItemData[1]),
                        (ind_ptr->Item[index].ItemData[2]));
           
#ifdef SNS_PLBK_SAMPLES_PER_SEC
          static uint32 prev_time[SNS_SMGR_MAX_ITEMS_PER_REPORT_V01] = {0};
          SNS_PLBK_MSG_1("samples per second = %lu\n", (DSPS_SLEEP_CLK /(ind_ptr->Item[index].TimeStamp - prev_time[index])));
          prev_time[index] = ind_ptr->Item[index].TimeStamp;
#endif
         }
         SNS_PLBK_MSG("\n");
      }
      break;
    case SNS_SMGR_BUFFERING_IND_V01:
      {
        uint32_t index;
        sns_smgr_buffering_ind_msg_v01 *ind_ptr;
        ind_ptr = (sns_smgr_buffering_ind_msg_v01 *) smgrIndPtr;

        smgr_ind_test[0]++;
        SNS_PLBK_MSG("PLAYBACK received SNS_SMGR_BUFFERING_IND_V01\n");
        ULogFront_RealTimePrintf(sns_log_handle, 1, "PLAYBACK received Buffer IND for report ID %d", ind_ptr->ReportId);

        /* success only if it has valid data length */
        if (report_id_length[ind_ptr->ReportId] == ind_ptr->Indices_len)
        {
          sns_success_cnt[ind_ptr->ReportId]++;

          SNS_PLBK_MSG_2("PLAYBACK ReportId = %d, success count = %lu\n",
                          ind_ptr->ReportId, sns_success_cnt[ind_ptr->ReportId]);
          SNS_PLBK_MSG_2("PLAYBACK Samples_len = %d, Item_len = %lu\n", ind_ptr->Samples_len, ind_ptr->Indices_len);
        }
        else
        {
          SNS_PLBK_MSG("Playback received invalid data @function sns_playback_process_smgr_ind\n");
          SNS_PLBK_MSG_3("PLAYBACK CurrentRate = %d, Requested Item_len = %d, Received Item_len = %lu\n",
                          ind_ptr->Indices[0].SamplingRate, report_id_length[ind_ptr->ReportId], ind_ptr->Indices_len);
        }

        ULogFront_RealTimePrintf(sns_log_handle, 1, "PLAYBACK Indices_len = %lu", ind_ptr->Indices_len);

        for (index = 0; index < ind_ptr->Indices_len; index++)
	{
          SNS_PLBK_MSG_3("PLAYBACK SensorId = %u, First sample index = %u, sample count %u\n",
			  ind_ptr->Indices[index].SensorId,
			  ind_ptr->Indices[index].FirstSampleIdx,
			  ind_ptr->Indices[index].SampleCount);
          ULogFront_RealTimePrintf(sns_log_handle, 3,
			  "PLAYBACK SensorId = %u, First sample index = %u, sample count %u",
			  ind_ptr->Indices[index].SensorId,
			  ind_ptr->Indices[index].FirstSampleIdx,
			  ind_ptr->Indices[index].SampleCount);
	}

        for (index = 0; index < ind_ptr->Samples_len; index++)
        {
          SNS_PLBK_MSG_3("PLAYBACK x = %f, y = %f, z = %f\n",
                        FX_FIXTOFLT_Q16(ind_ptr->Samples[index].Data[0]),
                        FX_FIXTOFLT_Q16(ind_ptr->Samples[index].Data[1]),
                        FX_FIXTOFLT_Q16(ind_ptr->Samples[index].Data[2]));
           
          ULogFront_RealTimePrintf(sns_log_handle, 4, "tsoffset=%lu, %d, %d, %d",
                        ind_ptr->Samples[index].TimeStampOffset,
                        (ind_ptr->Samples[index].Data[0]),
                        (ind_ptr->Samples[index].Data[1]),
                        (ind_ptr->Samples[index].Data[2]));
           
#ifdef SNS_PLBK_SAMPLES_PER_SEC
          static uint32 prev_time[SNS_SMGR_MAX_ITEMS_PER_REPORT_V01] = {0};
          SNS_PLBK_MSG_1("samples per second = %lu\n", (DSPS_SLEEP_CLK /(ind_ptr->Item[index].TimeStamp - prev_time[index])));
          prev_time[index] = ind_ptr->Item[index].TimeStamp;
#endif
         }
         SNS_PLBK_MSG("\n");
      }
      break;
    case SNS_SMGR_SENSOR_POWER_STATUS_IND_V01:
      {
        uint32_t index;
        sns_smgr_sensor_power_status_ind_msg_v01 *ind_ptr =
                        (sns_smgr_sensor_power_status_ind_msg_v01 *) smgrIndPtr;

        smgr_ind_test[1]++;
        SNS_PLBK_MSG("PLAYBACK received SNS_SMGR_SENSOR_POWER_STATUS_IND_V01\n");

        SNS_PLBK_MSG_2("ReportId = %d, PowerStatus_len = %lu\n",
                  ind_ptr->ReportId, ind_ptr->PowerStatus_len);

        for (index = 0; index < ind_ptr->PowerStatus_len; index++)
        {
          SNS_PLBK_MSG_5("SensorID = %d, PowerStatus = %d, ActiveTimeStamp = %lu, LowPowerTimeStamp = %lu, CycleCount = %lu\n",
                   ind_ptr->PowerStatus[index].SensorID,
                   ind_ptr->PowerStatus[index].PowerAction,
                   ind_ptr->PowerStatus[index].ActiveTimeStamp,
                   ind_ptr->PowerStatus[index].LowPowerTimeStamp,
                   ind_ptr->PowerStatus[index].CycleCount);
        }
      }
      break;
    case SNS_SMGR_SENSOR_STATUS_IND_V01:
      {
        sns_smgr_sensor_status_ind_msg_v01 *ind_ptr =
                               (sns_smgr_sensor_status_ind_msg_v01 *) smgrIndPtr;

        smgr_ind_test[2]++;
        SNS_PLBK_MSG("PLAYBACK received SNS_SMGR_SENSOR_STATUS_IND_V01\n");

        SNS_PLBK_MSG_3("sensor Id = %d, state = %d, timestamp = %lu\n",
                 ind_ptr->SensorID, ind_ptr->SensorState, ind_ptr->TimeStamp);
      }
      break;
    case SNS_SMGR_SINGLE_SENSOR_TEST_IND_V01:
      {
        sns_smgr_single_sensor_test_ind_msg_v01 *ind_ptr =
                         (sns_smgr_single_sensor_test_ind_msg_v01 *) smgrIndPtr;

        smgr_ind_test[3]++;
        SNS_PLBK_MSG("PLAYBACK received SNS_SMGR_SINGLE_SENSOR_TEST_IND_V01\n");
        SNS_PLBK_MSG_6("SensorID = %d, DataType = %d, TestType = %d, TestResult = %d, ErrorCode_valid = %d, ErrorCode = %d\n",
                        ind_ptr->SensorID, ind_ptr->DataType, ind_ptr->TestType, ind_ptr->TestResult, ind_ptr->ErrorCode_valid, ind_ptr->ErrorCode);
      }
      break;
    default:
      SNS_PLBK_MSG_3("PLAYBACK received invalid indication = %ld @%d, %s\n", msgId, __LINE__, __func__);
      break;
  }
}

/*=========================================================================
  FUNCTION:  sns_playback_smgr_resp_cb
  =========================================================================*/
/*!
  @brief PLAYBACK - SMGR response callback

  @detail
  Handles the responses from SMGR to PLAYBACK.

  @param[i]   user_handle         Handle used by the infrastructure to identify different clients.
  @param[i]   msg_id                Message ID
  @param[i]   resp_buf              Pointer to the response buffer
  @param[i]   resp_buf_len        Length of the response buffer
  @param[i]   user_cb_data       Pointer to the User-data
  @param[i]   transp_err           Error code

  @return None
*/
/*=======================================================================*/
static void sns_playback_smgr_resp_cb (qmi_client_type user_handle,
                           unsigned int msg_id,
                           void *resp_buf,
                           unsigned int resp_buf_len,
                           void *user_cb_data, qmi_client_error_type transp_err)
{
  uint8_t               os_err = 0;
  plbk_q_item_s         *plbk_smgr_resp_q_ptr = NULL;

  if ((resp_buf != NULL) && (transp_err == QMI_NO_ERR))
  {
    plbk_smgr_resp_q_ptr = (plbk_q_item_s *) SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS, sizeof(plbk_q_item_s));

    plbk_smgr_resp_q_ptr->msg_id = msg_id;
    plbk_smgr_resp_q_ptr->user_data = (uint32) user_cb_data;
    plbk_smgr_resp_q_ptr->body_ptr = resp_buf;
    plbk_smgr_resp_q_ptr->msg_type = PLAYBACK_SMGR_RESP_MSG;
    sns_q_link(plbk_smgr_resp_q_ptr, &plbk_smgr_resp_q_ptr->q_link);

    sns_os_mutex_pend(palyback_mutex, 0, &os_err);
    sns_q_put(&plbk_msg_q, &plbk_smgr_resp_q_ptr->q_link);
    os_err = sns_os_mutex_post(palyback_mutex);

    sns_os_sigs_post (sns_test_sig_grp, PLAYBACK_MSG_SIGNAL, OS_FLAG_SET, &os_err );
  }
  else
  {
    SNS_PLBK_MSG_2("error in getting response @%d, %s\n", __LINE__, __FILE__);
  }
}

/*=========================================================================
  FUNCTION:  sns_playback_sam_resp_cb
  =========================================================================*/
/*!
  @brief PLAYBACK - SAM response callback

  @detail
  Handles the responses from SAM to PLAYBACK.

  @param[i]   user_handle         Handle used by the infrastructure to identify different clients.
  @param[i]   msg_id                Message ID
  @param[i]   resp_buf              Pointer to the response buffer
  @param[i]   resp_buf_len        Length of the response buffer
  @param[i]   user_cb_data       Pointer to the User-data
  @param[i]   transp_err           Error code

    @return None
*/
/*=======================================================================*/
static void sns_playback_sam_resp_cb (qmi_client_type user_handle,
                          unsigned int msg_id,
                          void *resp_buf,
                          unsigned int resp_buf_len,
                          void *user_cb_data, qmi_client_error_type transp_err)
{
  uint8_t               os_err = 0;
  plbk_q_item_s         *plbk_sam_resp_q_ptr = NULL;

  if ((resp_buf != NULL) && (transp_err == QMI_NO_ERR))
  {
    plbk_sam_resp_q_ptr = (plbk_q_item_s *) SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS, sizeof(plbk_q_item_s));

    plbk_sam_resp_q_ptr->msg_id = msg_id;
    plbk_sam_resp_q_ptr->user_data = (uint32) user_cb_data;
    plbk_sam_resp_q_ptr->body_ptr = resp_buf;
    plbk_sam_resp_q_ptr->msg_type = PLAYBACK_SAM_RESP_MSG;
    sns_q_link(plbk_sam_resp_q_ptr, &plbk_sam_resp_q_ptr->q_link);

    sns_os_mutex_pend(palyback_mutex, 0, &os_err);
    sns_q_put(&plbk_msg_q, &plbk_sam_resp_q_ptr->q_link);
    os_err = sns_os_mutex_post(palyback_mutex);

    sns_os_sigs_post (sns_test_sig_grp, PLAYBACK_MSG_SIGNAL, OS_FLAG_SET, &os_err );
  }
  else
  {
    SNS_PLBK_MSG_2("error in getting response @%d, %s\n", __LINE__, __FILE__);
  }
}

/*=========================================================================
  FUNCTION:  sns_playback_smgr_ind_cb
  =========================================================================*/
/*!
  @brief PLAYBACK - SMGR indication callback

  @detail
  Handles the indications from SMGR to PLAYBACK.

  @param[i]   user_handle         Handle used by the infrastructure to identify different clients.
  @param[i]   msg_id                Message ID
  @param[i]   ind_buf                Pointer to the indication buffer
  @param[i]   ind_buf_len          Length of the indication buffer
  @param[i]   user_cb_data       Pointer to the User-data

  @return None
*/
/*=======================================================================*/
static void sns_playback_smgr_ind_cb (qmi_client_type user_handle,
                          unsigned int msg_id,
                          void *ind_buf,
                          unsigned int ind_buf_len, void *user_cb_data)
{
  void                  *msgPtr =  NULL;
  unsigned int          decode_msg_len = 0;
  plbk_q_item_s         *plbk_smgr_ind_q_ptr = NULL;
  uint8_t               os_err = 0;

  switch (msg_id)
  {
    case SNS_SMGR_REPORT_IND_V01:
      decode_msg_len = sizeof(sns_smgr_periodic_report_ind_msg_v01);
      break;
    case SNS_SMGR_BUFFERING_IND_V01:
      decode_msg_len = sizeof(sns_smgr_buffering_ind_msg_v01);
      break;
    case SNS_SMGR_SENSOR_POWER_STATUS_IND_V01:
      decode_msg_len = sizeof(sns_smgr_sensor_power_status_ind_msg_v01);
      break;
    case SNS_SMGR_SENSOR_STATUS_IND_V01:
      decode_msg_len = sizeof(sns_smgr_sensor_status_ind_msg_v01);
      break;
    case SNS_SMGR_SINGLE_SENSOR_TEST_IND_V01:
      decode_msg_len = sizeof(sns_smgr_single_sensor_test_ind_msg_v01);
      break;
    default:
      SNS_PLBK_MSG_1("Inavlid msg_id at %s\n", __func__);
      return;
  }

  msgPtr = SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS, ind_buf_len);
  SNS_OS_MEMCOPY(msgPtr, ind_buf, ind_buf_len);

  plbk_smgr_ind_q_ptr = (plbk_q_item_s *) SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS, sizeof(plbk_q_item_s));

  plbk_smgr_ind_q_ptr->msg_id = msg_id;
  plbk_smgr_ind_q_ptr->user_data = (uint32) user_cb_data;
  plbk_smgr_ind_q_ptr->body_ptr = msgPtr;
  plbk_smgr_ind_q_ptr->plbk_msg_len = ind_buf_len;
  plbk_smgr_ind_q_ptr->plbk_decode_len = decode_msg_len;
  plbk_smgr_ind_q_ptr->msg_type = PLAYBACK_SMGR_IND_MSG;
  sns_q_link(plbk_smgr_ind_q_ptr, &plbk_smgr_ind_q_ptr->q_link);

  sns_os_mutex_pend(palyback_mutex, 0, &os_err);
  sns_q_put(&plbk_msg_q, &plbk_smgr_ind_q_ptr->q_link);
  os_err = sns_os_mutex_post(palyback_mutex);

  sns_os_sigs_post (sns_test_sig_grp, PLAYBACK_MSG_SIGNAL, OS_FLAG_SET, &os_err );
}

/*=========================================================================
  FUNCTION:  sns_playback_sam_ind_cb
  =========================================================================*/
/*!
  @brief PLAYBACK - SAM indication callback

  @detail
  Handles the indications from SAM to PLAYBACK.

  @param[i]   user_handle         Handle used by the infrastructure to identify different clients.
  @param[i]   msg_id                Message ID
  @param[i]   ind_buf                Pointer to the indication buffer
  @param[i]   ind_buf_len          Length of the indication buffer
  @param[i]   user_cb_data       Pointer to the User-data

  @return None
*/
/*=======================================================================*/
#if SNS_PLBK_SAM_CLNT
static void sns_playback_sam_ind_cb (qmi_client_type user_handle,
                         unsigned int msg_id,
                         void *ind_buf,
                         unsigned int ind_buf_len, void *user_cb_data)
{
  void                  *msgPtr =  NULL;
  unsigned int          decode_msg_len = 0;
  plbk_q_item_s         *plbk_sam_ind_q_ptr = NULL;
  uint8_t               os_err = 0;

  if ((uint32_t) user_cb_data == SNS_PLBK_THR_TEST_VAL)
  {
    switch(msg_id)
    {
      case SNS_SAM_SENSOR_THRESH_REPORT_IND_V01:
        {
          decode_msg_len = sizeof (sns_sam_sensor_thresh_report_ind_msg_v01);
          break;
        }
      case SNS_SAM_SENSOR_THRESH_ERROR_IND_V01:
        {
          decode_msg_len = sizeof (sns_sam_sensor_thresh_error_ind_msg_v01);
          break;
        }
      default:
        {
          SNS_PLBK_MSG_1("Inavlid msg_id at %s\n", __func__);
          return;
        }
    }
  }
  else
  {
    switch(msg_id)
    {
      case SNS_SAM_AMD_REPORT_IND_V01:
        {
          decode_msg_len = sizeof (sns_sam_qmd_report_ind_msg_v01);
          break;
        }
      case SNS_SAM_AMD_ERROR_IND_V01:
        {
          decode_msg_len = sizeof (sns_sam_qmd_error_ind_msg_v01);
          break;
        }
      default:
        {
          SNS_PLBK_MSG_1("Inavlid msg_id at %s\n", __func__);
          return;
        }
    }
  }
  msgPtr = SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS, ind_buf_len);
  SNS_OS_MEMCOPY(msgPtr, ind_buf, ind_buf_len);

  plbk_sam_ind_q_ptr = (plbk_q_item_s *) SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS, sizeof(plbk_q_item_s));

  plbk_sam_ind_q_ptr->msg_id = msg_id;
  plbk_sam_ind_q_ptr->user_data = (uint32) user_cb_data;
  plbk_sam_ind_q_ptr->body_ptr = msgPtr;
  plbk_sam_ind_q_ptr->plbk_msg_len = ind_buf_len;
  plbk_sam_ind_q_ptr->plbk_decode_len = decode_msg_len;
  plbk_sam_ind_q_ptr->msg_type = PLAYBACK_SAM_IND_MSG;
  sns_q_link(plbk_sam_ind_q_ptr, &plbk_sam_ind_q_ptr->q_link);

  sns_os_mutex_pend(palyback_mutex, 0, &os_err);
  sns_q_put(&plbk_msg_q, &plbk_sam_ind_q_ptr->q_link);
  os_err = sns_os_mutex_post(palyback_mutex);

  sns_os_sigs_post (sns_test_sig_grp, PLAYBACK_MSG_SIGNAL, OS_FLAG_SET, &os_err );
}
#endif

/*=========================================================================
  FUNCTION:  sns_sensors_buffering_data_fill
=========================================================================*/
/*!
    @brief Called to fill the sensors information

    @param[in/out] Msg_p           Pointer to the buffering report for sensor sampling structure
    @param[in]       sensor_comb  Combination of sensors to be enabled

    @return
    None
*/
/*=======================================================================*/
static void sns_sensors_buffering_data_fill (sns_smgr_buffering_req_msg_v01 * Msg_p,
                       uint8 sensor_comb, uint8 action, uint8 reportId, uint8 sample_rate)
{
  uint8 index = 0;
  memset(Msg_p, 0, sizeof(sns_smgr_buffering_req_msg_v01));

  Msg_p->Item_len = 1;
  Msg_p->ReportId = reportId;
  Msg_p->SrcModule_valid = true;
  Msg_p->SrcModule = SNS_MODULE_DSPS_PLAYBACK;

  switch (sensor_comb)
  {
    case SNS_PLBK_ACCEL:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_ACCEL_V01;
      break;
    case SNS_PLBK_GYRO:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_GYRO_V01;
      break;
    case SNS_PLBK_MAG:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_MAG_V01;
      break;
    case SNS_PLBK_PRESS:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_PRESSURE_V01;
      break;
    case SNS_PLBK_PROX:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_PROX_LIGHT_V01;
      break;
    case SNS_PLBK_LIG:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_PROX_LIGHT_V01;
      break;
    case SNS_PLBK_PROX_LIG:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_PROX_LIGHT_V01;
      Msg_p->Item[1].SensorId = SNS_SMGR_ID_PROX_LIGHT_V01;
      Msg_p->Item_len = 2;
      break;
    case SNS_PLBK_ACCEL_GYRO:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_ACCEL_V01;
      Msg_p->Item[1].SensorId = SNS_SMGR_ID_GYRO_V01;
      Msg_p->Item_len = 2;
      break;
    case SNS_PLBK_ACCEL_GYRO_MAG:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_ACCEL_V01;
      Msg_p->Item[1].SensorId = SNS_SMGR_ID_GYRO_V01;
      Msg_p->Item[2].SensorId = SNS_SMGR_ID_MAG_V01;
      Msg_p->Item_len = 3;
      break;
    case SNS_PLBK_ACCEL_GYRO_MAG_PRESS:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_ACCEL_V01;
      Msg_p->Item[1].SensorId = SNS_SMGR_ID_GYRO_V01;
      Msg_p->Item[2].SensorId = SNS_SMGR_ID_MAG_V01;
      Msg_p->Item[3].SensorId = SNS_SMGR_ID_PRESSURE_V01;
      Msg_p->Item_len = 4;
      break;
    case SNS_PLBK_ACCEL_GYRO_MAG_PRESS_PROX:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_ACCEL_V01;
      Msg_p->Item[1].SensorId = SNS_SMGR_ID_GYRO_V01;
      Msg_p->Item[2].SensorId = SNS_SMGR_ID_MAG_V01;
      Msg_p->Item[3].SensorId = SNS_SMGR_ID_PRESSURE_V01;
      Msg_p->Item[4].SensorId = SNS_SMGR_ID_PROX_LIGHT_V01;
      Msg_p->Item_len = 5;
      break;
    case SNS_PLBK_ACCEL_GYRO_MAG_PRESS_PROX_LIG:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_ACCEL_V01;
      Msg_p->Item[1].SensorId = SNS_SMGR_ID_GYRO_V01;
      Msg_p->Item[2].SensorId = SNS_SMGR_ID_MAG_V01;
      Msg_p->Item[3].SensorId = SNS_SMGR_ID_PRESSURE_V01;
      Msg_p->Item[4].SensorId = SNS_SMGR_ID_PROX_LIGHT_V01;
      //Msg_p->Item[5].SensorId = SNS_SMGR_ID_PROX_LIGHT_V01;
      Msg_p->Item_len = 5;
      break;
    case SNS_PLBK_ACCEL_GYRO_MAG_PROX:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_ACCEL_V01;
      Msg_p->Item[1].SensorId = SNS_SMGR_ID_GYRO_V01;
      Msg_p->Item[2].SensorId = SNS_SMGR_ID_MAG_V01;
      Msg_p->Item[3].SensorId = SNS_SMGR_ID_PROX_LIGHT_V01;
      Msg_p->Item_len = 4;
      break;
    case SNS_PLBK_ACCEL_GYRO_MAG_PROX_LIG:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_ACCEL_V01;
      Msg_p->Item[1].SensorId = SNS_SMGR_ID_GYRO_V01;
      Msg_p->Item[2].SensorId = SNS_SMGR_ID_MAG_V01;
      Msg_p->Item[3].SensorId = SNS_SMGR_ID_PROX_LIGHT_V01;
      Msg_p->Item[4].SensorId = SNS_SMGR_ID_PROX_LIGHT_V01;
      Msg_p->Item_len = 5;
      break;
    case SNS_PLBK_HUMID:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_HUMIDITY_V01;
      break;
    case SNS_PLBK_RGB:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_RGB_V01;
      break;
    case SNS_PLBK_SAR:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_SAR_V01;
      break;
    case SNS_PLBK_HALL_EFFECT:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_HALL_EFFECT_V01;
      break;
    default:
      break;
  }

  /* fill the data for sensors */
  for (index = 0; index < Msg_p->Item_len; index++)
  {
    Msg_p->Item[index].DataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
    Msg_p->Item[index].Decimation = SNS_SMGR_DECIMATION_RECENT_SAMPLE_V01;
    Msg_p->Item[index].SamplingRate = sample_rate;
  }

  /* verify for secondary */
  if (sensor_comb == SNS_PLBK_LIG)
    Msg_p->Item[0].DataType = SNS_SMGR_DATA_TYPE_SECONDARY_V01;
  else if (sensor_comb == SNS_PLBK_PROX_LIG)
    Msg_p->Item[1].DataType = SNS_SMGR_DATA_TYPE_SECONDARY_V01;
  else {
    //if (sensor_comb == SNS_PLBK_ACCEL_GYRO_MAG_PRESS_PROX_LIG)
    //  Msg_p->Item[5].DataType = SNS_SMGR_DATA_TYPE_SECONDARY_V01;

    if (sensor_comb == SNS_PLBK_ACCEL_GYRO_MAG_PROX_LIG)
      Msg_p->Item[4].DataType = SNS_SMGR_DATA_TYPE_SECONDARY_V01;
  }

  report_id_length[reportId] = Msg_p->Item_len;
}

/*=========================================================================
  FUNCTION:  sns_sensors_data_fill
=========================================================================*/
/*!
    @brief Called to fill the sensors inforormation

    @param[in/out] Msg_p           Pointer to the periodic report for sensor sampling structure
    @param[in]       sensor_comb  Combination of sensors to be enabled

    @return
    None
*/
/*=======================================================================*/
static void sns_sensors_data_fill (sns_smgr_periodic_report_req_msg_v01 * Msg_p,
                       uint8 sensor_comb, uint8 action, uint8 reportId)
{
  uint8 index = 0;
  memset(Msg_p, 0, sizeof(sns_smgr_periodic_report_req_msg_v01));

  Msg_p->BufferFactor = 0;
  Msg_p->Item_len = 1;
  Msg_p->ReportId = reportId;
  Msg_p->SrcModule_valid = true;
  Msg_p->SrcModule = SNS_MODULE_DSPS_PLAYBACK;

  switch (sensor_comb)
  {
    case SNS_PLBK_ACCEL:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_ACCEL_V01;
      break;
    case SNS_PLBK_GYRO:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_GYRO_V01;
      break;
    case SNS_PLBK_MAG:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_MAG_V01;
      break;
    case SNS_PLBK_PRESS:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_PRESSURE_V01;
      break;
    case SNS_PLBK_PROX:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_PROX_LIGHT_V01;
      break;
    case SNS_PLBK_LIG:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_PROX_LIGHT_V01;
      break;
    case SNS_PLBK_PROX_LIG:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_PROX_LIGHT_V01;
      Msg_p->Item[1].SensorId = SNS_SMGR_ID_PROX_LIGHT_V01;
      Msg_p->Item_len = 2;
      break;
    case SNS_PLBK_ACCEL_GYRO:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_ACCEL_V01;
      Msg_p->Item[1].SensorId = SNS_SMGR_ID_GYRO_V01;
      Msg_p->Item_len = 2;
      break;
    case SNS_PLBK_ACCEL_GYRO_MAG:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_ACCEL_V01;
      Msg_p->Item[1].SensorId = SNS_SMGR_ID_GYRO_V01;
      Msg_p->Item[2].SensorId = SNS_SMGR_ID_MAG_V01;
      Msg_p->Item_len = 3;
      break;
    case SNS_PLBK_ACCEL_GYRO_MAG_PRESS:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_ACCEL_V01;
      Msg_p->Item[1].SensorId = SNS_SMGR_ID_GYRO_V01;
      Msg_p->Item[2].SensorId = SNS_SMGR_ID_MAG_V01;
      Msg_p->Item[3].SensorId = SNS_SMGR_ID_PRESSURE_V01;
      Msg_p->Item_len = 4;
      break;
    case SNS_PLBK_ACCEL_GYRO_MAG_PRESS_PROX:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_ACCEL_V01;
      Msg_p->Item[1].SensorId = SNS_SMGR_ID_GYRO_V01;
      Msg_p->Item[2].SensorId = SNS_SMGR_ID_MAG_V01;
      Msg_p->Item[3].SensorId = SNS_SMGR_ID_PRESSURE_V01;
      Msg_p->Item[4].SensorId = SNS_SMGR_ID_PROX_LIGHT_V01;
      Msg_p->Item_len = 5;
      break;
    case SNS_PLBK_ACCEL_GYRO_MAG_PRESS_PROX_LIG:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_ACCEL_V01;
      Msg_p->Item[1].SensorId = SNS_SMGR_ID_GYRO_V01;
      Msg_p->Item[2].SensorId = SNS_SMGR_ID_MAG_V01;
      Msg_p->Item[3].SensorId = SNS_SMGR_ID_PRESSURE_V01;
      Msg_p->Item[4].SensorId = SNS_SMGR_ID_PROX_LIGHT_V01;
      Msg_p->Item[5].SensorId = SNS_SMGR_ID_PROX_LIGHT_V01;
      Msg_p->Item_len = 6;
      break;
    case SNS_PLBK_ACCEL_GYRO_MAG_PROX:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_ACCEL_V01;
      Msg_p->Item[1].SensorId = SNS_SMGR_ID_GYRO_V01;
      Msg_p->Item[2].SensorId = SNS_SMGR_ID_MAG_V01;
      Msg_p->Item[3].SensorId = SNS_SMGR_ID_PROX_LIGHT_V01;
      Msg_p->Item_len = 4;
      break;
    case SNS_PLBK_ACCEL_GYRO_MAG_PROX_LIG:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_ACCEL_V01;
      Msg_p->Item[1].SensorId = SNS_SMGR_ID_GYRO_V01;
      Msg_p->Item[2].SensorId = SNS_SMGR_ID_MAG_V01;
      Msg_p->Item[3].SensorId = SNS_SMGR_ID_PROX_LIGHT_V01;
      Msg_p->Item[4].SensorId = SNS_SMGR_ID_PROX_LIGHT_V01;
      Msg_p->Item_len = 5;
      break;
    case SNS_PLBK_HUMID:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_HUMIDITY_V01;
      break;
    case SNS_PLBK_RGB:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_RGB_V01;
      break;
    case SNS_PLBK_SAR:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_SAR_V01;
      break;
    case SNS_PLBK_HALL_EFFECT:
      Msg_p->Item[0].SensorId = SNS_SMGR_ID_HALL_EFFECT_V01;
      break;
    default:
      break;
  }

  /* fill the data for sensors */
  for (index = 0; index < Msg_p->Item_len; index++)
  {
    Msg_p->Item[index].DataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
    Msg_p->Item[index].Sensitivity = 50;
    Msg_p->Item[index].Decimation = SNS_SMGR_DECIMATION_RECENT_SAMPLE_V01;
    Msg_p->Item[index].MinSampleRate = 0;
    Msg_p->Item[index].StationaryOption = SNS_SMGR_REST_OPTION_REPORT_PRIOR_V01;
    Msg_p->Item[index].DoThresholdTest = 1;
    Msg_p->Item[index].ThresholdOutsideMinMax = 0;
    Msg_p->Item[index].ThresholdDelta = 0;
    Msg_p->Item[index].ThresholdAllAxes = 1;
    Msg_p->Item[index].ThresholdMinMax[0] = (int32_t) (0.731 * 65536);
    Msg_p->Item[index].ThresholdMinMax[1] = (int32_t) (1.009 * 65536);
  }

  /* verify for secondary */
  if (sensor_comb == SNS_PLBK_LIG)
    Msg_p->Item[0].DataType = SNS_SMGR_DATA_TYPE_SECONDARY_V01;
  else if (sensor_comb == SNS_PLBK_PROX_LIG)
    Msg_p->Item[1].DataType = SNS_SMGR_DATA_TYPE_SECONDARY_V01;
  else {
    if (sensor_comb == SNS_PLBK_ACCEL_GYRO_MAG_PRESS_PROX_LIG)
      Msg_p->Item[5].DataType = SNS_SMGR_DATA_TYPE_SECONDARY_V01;

    if (sensor_comb == SNS_PLBK_ACCEL_GYRO_MAG_PROX_LIG)
      Msg_p->Item[4].DataType = SNS_SMGR_DATA_TYPE_SECONDARY_V01;
  }

  report_id_length[reportId] = Msg_p->Item_len;
}

void sns_plbk_sam_amd_enable_test(uint32 report_period)
{
  sns_sam_qmd_enable_resp_msg_v01 *amd_en_respP = NULL;
  plbk_msg_req_s                 *plbkReq       = NULL;
  sns_sam_qmd_enable_req_msg_v01 *amd_en_reqP   = NULL;

  amd_en_reqP = (sns_sam_qmd_enable_req_msg_v01 *)
                     SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS,
                       sizeof(sns_sam_qmd_enable_req_msg_v01));

  /* update send request count */
  sam_amd_test[SNS_PLBK_REQ][SNS_SAM_AMD_ENABLE_REQ_V01]++;

  /* sam AMD enable */
  amd_en_reqP->report_period = report_period;
  amd_en_reqP->config_valid = FALSE;

  amd_en_respP = (sns_sam_qmd_enable_resp_msg_v01 *)
                     SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS,
                       sizeof(sns_sam_qmd_enable_resp_msg_v01));

  plbkReq = (plbk_msg_req_s *) SNS_OS_MALLOC(SNS_DBG_NUM_MOD_IDS, sizeof( plbk_msg_req_s));

  plbkReq->plbk_cient = SNS_PLBK_CLIENT_SAM;
  plbkReq->report_req = SNS_SAM_AMD_ENABLE_REQ_V01;
  plbkReq->rep_req_data = amd_en_reqP;
  plbkReq->rep_req_size = sizeof(sns_sam_qmd_enable_req_msg_v01);
  plbkReq->rep_resp_data = amd_en_respP;
  plbkReq->rep_resp_size = sizeof(sns_sam_qmd_enable_resp_msg_v01);
  plbkReq->resp_clbk_data = NULL;

  sns_playback_put_msg_req_to_queue (plbkReq, PLAYBACK_HIGH_PRIORIY_MSG_REQ);
}

void sns_plbk_sam_amd_disable_test(uint8_t instance_id)
{
  sns_sam_qmd_disable_req_msg_v01  *amd_dis_reqP  = NULL;
  sns_sam_qmd_disable_resp_msg_v01 *amd_dis_respP = NULL;
  plbk_msg_req_s                   *plbkReq       = NULL;

  amd_dis_reqP = (sns_sam_qmd_disable_req_msg_v01 *)
                     SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS,
                       sizeof(sns_sam_qmd_disable_req_msg_v01));

  /* update send request count */
  sam_amd_test[SNS_PLBK_REQ][SNS_SAM_AMD_DISABLE_REQ_V01]++;

  /*
   * send DISABLE message to SAM
   */
  amd_dis_reqP->instance_id = instance_id;

  amd_dis_respP = (sns_sam_qmd_disable_resp_msg_v01 *)
                     SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS,
                       sizeof(sns_sam_qmd_disable_resp_msg_v01));

  plbkReq = (plbk_msg_req_s *) SNS_OS_MALLOC(SNS_DBG_NUM_MOD_IDS, sizeof( plbk_msg_req_s));

  plbkReq->plbk_cient = SNS_PLBK_CLIENT_SAM;
  plbkReq->report_req = SNS_SAM_AMD_DISABLE_REQ_V01;
  plbkReq->rep_req_data = amd_dis_reqP;
  plbkReq->rep_req_size = sizeof(sns_sam_qmd_disable_req_msg_v01);
  plbkReq->rep_resp_data = amd_dis_respP;
  plbkReq->rep_resp_size = sizeof(sns_sam_qmd_disable_resp_msg_v01);
  plbkReq->resp_clbk_data = NULL;

  sns_playback_put_msg_req_to_queue (plbkReq, PLAYBACK_HIGH_PRIORIY_MSG_REQ);
}

void sns_plbk_smgr_sns_start_stop_stream(
         uint8 report_rate, uint8 report_id, uint8 sensor_comb, uint8 action)
{
  sns_smgr_periodic_report_req_msg_v01 *smgr_rep_reqP = NULL;
  sns_smgr_periodic_report_resp_msg_v01 *smgr_rep_respP = NULL;
  plbk_msg_req_s *plbkReq = NULL;

#ifdef PROX_LIGHT_LOWER_RATE
  if (report_rate > SNS_PLBK_PROX_LIGHT_REPORT_RATE_MAX)
  {
    switch (sensor_comb)
    {
      case SNS_PLBK_PROX:
      case SNS_PLBK_LIG:
      case SNS_PLBK_PROX_LIG:
      case SNS_PLBK_ACCEL_GYRO_MAG_PRESS_PROX:
      case SNS_PLBK_ACCEL_GYRO_MAG_PRESS_PROX_LIG:
        report_rate = SNS_PLBK_PROX_LIGHT_REPORT_RATE_MAX;
      break;
      default:
      break;
    }
  }
#endif //PROX_LIGHT_LOWER_RATE

  smgr_rep_reqP = (sns_smgr_periodic_report_req_msg_v01 *)
                     SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS,
                       sizeof(sns_smgr_periodic_report_req_msg_v01));

  /* fill the sensors data */
  sns_sensors_data_fill (smgr_rep_reqP, sensor_comb, action, report_id);

  /*
     * perodic report request
     */
  /* Initialise the message pointer action add request */
  smgr_rep_reqP->Action = action;
  smgr_rep_reqP->ReportRate = report_rate;
  smgr_rep_reqP->ReportId = report_id;

  /* update send request count */
  smgr_test[SNS_PLBK_REQ][SNS_SMGR_REPORT_REQ_V01]++;

  smgr_rep_respP = (sns_smgr_periodic_report_resp_msg_v01 *)
                     SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS,
                       sizeof(sns_smgr_periodic_report_resp_msg_v01));

  plbkReq = (plbk_msg_req_s *) SNS_OS_MALLOC(SNS_DBG_NUM_MOD_IDS, sizeof( plbk_msg_req_s));

  plbkReq->plbk_cient = SNS_PLBK_CLIENT_SMGR;
  plbkReq->report_req = SNS_SMGR_REPORT_REQ_V01;
  plbkReq->rep_req_data = smgr_rep_reqP;
  plbkReq->rep_req_size = sizeof(sns_smgr_periodic_report_req_msg_v01);
  plbkReq->rep_resp_data = smgr_rep_respP;
  plbkReq->rep_resp_size = sizeof(sns_smgr_periodic_report_resp_msg_v01);
  plbkReq->resp_clbk_data = NULL;

  sns_playback_put_msg_req_to_queue (plbkReq, PLAYBACK_HIGH_PRIORIY_MSG_REQ);
}

/* Buffering request and delete */
void sns_plbk_buffer_start_stop_stream(
         uint8 sample_rate, uint8 report_id, uint8 sensor_comb, uint8 action)
{
  sns_smgr_buffering_req_msg_v01 *smgr_rep_reqP = NULL;
  sns_smgr_buffering_resp_msg_v01 *smgr_rep_respP = NULL;
  plbk_msg_req_s *plbkReq = NULL;

#ifdef PROX_LIGHT_LOWER_RATE
  if (sample_rate > SNS_PLBK_PROX_LIGHT_REPORT_RATE_MAX)
  {
    switch (sensor_comb)
    {
      case SNS_PLBK_PROX:
      case SNS_PLBK_LIG:
      case SNS_PLBK_PROX_LIG:
      case SNS_PLBK_ACCEL_GYRO_MAG_PRESS_PROX:
      case SNS_PLBK_ACCEL_GYRO_MAG_PRESS_PROX_LIG:
        sample_rate = SNS_PLBK_PROX_LIGHT_REPORT_RATE_MAX;
      break;
      default:
      break;
    }
  }
#endif //PROX_LIGHT_LOWER_RATE

  smgr_rep_reqP = (sns_smgr_buffering_req_msg_v01 *)
                     SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS,
                       sizeof(sns_smgr_buffering_req_msg_v01));

  /* fill the sensors data */
  sns_sensors_buffering_data_fill (smgr_rep_reqP, sensor_comb, action, report_id, sample_rate);

  /*
     * perodic report request
     */
  /* Initialise the message pointer action add request */
  smgr_rep_reqP->Action = action;
  smgr_rep_reqP->ReportRate = plbk_buffer_report_rate;
  smgr_rep_reqP->ReportId = report_id;

  /* update send request count */
  smgr_test[SNS_PLBK_REQ][SNS_SMGR_BUFFERING_REQ_V01]++;

  smgr_rep_respP = (sns_smgr_buffering_resp_msg_v01 *)
                     SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS,
                       sizeof(sns_smgr_buffering_resp_msg_v01));

  plbkReq = (plbk_msg_req_s *) SNS_OS_MALLOC(SNS_DBG_NUM_MOD_IDS, sizeof( plbk_msg_req_s));

  plbkReq->plbk_cient = SNS_PLBK_CLIENT_SMGR;
  plbkReq->report_req = SNS_SMGR_BUFFERING_REQ_V01;
  plbkReq->rep_req_data = smgr_rep_reqP;
  plbkReq->rep_req_size = sizeof(sns_smgr_buffering_req_msg_v01);
  plbkReq->rep_resp_data = smgr_rep_respP;
  plbkReq->rep_resp_size = sizeof(sns_smgr_buffering_resp_msg_v01);
  plbkReq->resp_clbk_data = NULL;

  sns_playback_put_msg_req_to_queue (plbkReq, PLAYBACK_HIGH_PRIORIY_MSG_REQ);
}

void sns_plbk_sam_thr_enable_test(void)
{
  uint8 index;
  qmi_txn_handle txn_handle;
  qmi_client_error_type err;
  sns_sam_sensor_thresh_enable_req_msg_v01 *thr_en_reqP;
  sns_sam_sensor_thresh_enable_resp_msg_v01 *thr_en_respP;

  for (index = 0; index < 4; index++)
  {
    thr_en_reqP = (sns_sam_sensor_thresh_enable_req_msg_v01 *)
                      SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS,
                        sizeof(sns_sam_sensor_thresh_enable_req_msg_v01));

    thr_en_respP = (sns_sam_sensor_thresh_enable_resp_msg_v01 *)
                      SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS,
                        sizeof(sns_sam_sensor_thresh_enable_resp_msg_v01));

    /* Threshold enable test */
    thr_en_reqP->sensor_id = sensor_id[index];
    thr_en_reqP->data_type = SNS_SMGR_DATA_TYPE_SECONDARY_V01;
    thr_en_reqP->sample_rate = SamplingFreq;
    thr_en_reqP->threshold[0] = 0;
    thr_en_reqP->threshold[1] = 0;
    thr_en_reqP->threshold[2] = 0;

    /* update send request count */
    sam_thr_test[SNS_PLBK_REQ][SNS_SAM_SENSOR_THRESH_ENABLE_REQ_V01]++;

    err = qmi_client_send_msg_async(playback_sam_user_handle,
                                    SNS_SAM_SENSOR_THRESH_ENABLE_REQ_V01,
                                    thr_en_reqP,
                                    sizeof(sns_sam_sensor_thresh_enable_req_msg_v01),
                                    thr_en_respP,
                                    sizeof(sns_sam_sensor_thresh_enable_resp_msg_v01),
                                    sns_playback_sam_resp_cb,
                                    (void *)SNS_PLBK_THR_TEST_VAL,
                                    &txn_handle);
    if (QMI_NO_ERR != err)
    {
      /* extra count indicates failure to send */
      sam_thr_test[SNS_PLBK_REQ][SNS_SAM_SENSOR_THRESH_ENABLE_REQ_V01]++;

      SNS_PLBK_MSG_3("qmi_client_send_msg_async fail = %d @: %d, %s\n", err, __LINE__, __FILE__);
    }

    SNS_OS_FREE (thr_en_reqP);

    sns_em_sleep(SNS_PLBK_TEST_SLEEP_TIME);
  }
}

void sns_plbk_sam_thr_version_test(void)
{
  qmi_txn_handle txn_handle;
  qmi_client_error_type err;
  uint8 dummy_req;
  uint8 *dummy_respP;

  /* Threshold version request */
  dummy_respP = (uint8 *) SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS, sizeof(uint8));

  /* update send request count */
  sam_thr_test[SNS_PLBK_REQ][SNS_SAM_SENSOR_THRESH_VERSION_REQ_V01]++;

  err = qmi_client_send_msg_async(playback_sam_user_handle,
                                  SNS_SAM_SENSOR_THRESH_VERSION_REQ_V01,
                                  &dummy_req,
                                  0,
                                  dummy_respP,
                                  0,
                                  sns_playback_sam_resp_cb,
                                  (void *)SNS_PLBK_THR_TEST_VAL,
                                  &txn_handle);
  if (QMI_NO_ERR != err)
  {
    /* extra count indicates failure to send */
    sam_thr_test[SNS_PLBK_REQ][SNS_SAM_SENSOR_THRESH_VERSION_REQ_V01]++;

    SNS_PLBK_MSG_3("qmi_client_send_msg_async fail = %d @: %d, %s\n", err, __LINE__, __FILE__);
  }

  sns_em_sleep(SNS_PLBK_TEST_SLEEP_TIME);
}

void sns_plbk_sam_thr_disable_test(void)
{
  qmi_txn_handle txn_handle;
  qmi_client_error_type err;
  sns_sam_sensor_thresh_disable_req_msg_v01 thr_dis_req;
  sns_sam_sensor_thresh_disable_resp_msg_v01 *thr_dis_respP;

  thr_dis_respP = (sns_sam_sensor_thresh_disable_resp_msg_v01 *)
                     SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS,
                       sizeof(sns_sam_sensor_thresh_disable_resp_msg_v01));
  /* update send request count */
  sam_thr_test[SNS_PLBK_REQ][SNS_SAM_SENSOR_THRESH_DISABLE_REQ_V01]++;

  /* Threshold disable test */
  thr_dis_req.instance_id = 0;
  err = qmi_client_send_msg_async(playback_sam_user_handle,
                                  SNS_SAM_SENSOR_THRESH_DISABLE_REQ_V01,
                                  &thr_dis_req,
                                  sizeof(sns_sam_sensor_thresh_disable_req_msg_v01),
                                  thr_dis_respP,
                                  sizeof(sns_sam_sensor_thresh_disable_resp_msg_v01),
                                  sns_playback_sam_resp_cb,
                                  (void *)SNS_PLBK_THR_TEST_VAL,
                                  &txn_handle);
  if (QMI_NO_ERR != err)
  {
    /* extra count indicates failure to send */
    sam_thr_test[SNS_PLBK_REQ][SNS_SAM_SENSOR_THRESH_DISABLE_REQ_V01]++;

    SNS_PLBK_MSG_3("qmi_client_send_msg_async fail = %d @: %d, %s\n", err, __LINE__, __FILE__);
  }

  sns_em_sleep(SNS_PLBK_TEST_SLEEP_TIME);
}

void sns_plbk_sam_thr_report_req_test(void)
{
  qmi_txn_handle txn_handle;
  qmi_client_error_type err;
  sns_sam_sensor_thresh_get_report_req_msg_v01 thr_rep_req;
  sns_sam_sensor_thresh_get_report_resp_msg_v01 *thr_rep_respP;

  thr_rep_respP = (sns_sam_sensor_thresh_get_report_resp_msg_v01 *)
                     SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS,
                       sizeof(sns_sam_sensor_thresh_get_report_resp_msg_v01));

  /* update send request count */
  sam_thr_test[SNS_PLBK_REQ][SNS_SAM_SENSOR_THRESH_GET_REPORT_REQ_V01]++;

  /* Threshold report enable test */
  thr_rep_req.instance_id = 0;
  err = qmi_client_send_msg_async(playback_sam_user_handle,
                                  SNS_SAM_SENSOR_THRESH_GET_REPORT_REQ_V01,
                                  &thr_rep_req,
                                  sizeof(sns_sam_sensor_thresh_get_report_req_msg_v01),
                                  thr_rep_respP,
                                  sizeof(sns_sam_sensor_thresh_get_report_resp_msg_v01),
                                  sns_playback_sam_resp_cb,
                                  (void *)SNS_PLBK_THR_TEST_VAL,
                                  &txn_handle);
  if (QMI_NO_ERR != err)
  {
    /* extra count indicates failure to send */
    sam_thr_test[SNS_PLBK_REQ][SNS_SAM_SENSOR_THRESH_GET_REPORT_REQ_V01]++;

    SNS_PLBK_MSG_3("qmi_client_send_msg_async fail = %d @: %d, %s\n", err, __LINE__, __FILE__);
  }

  sns_em_sleep(SNS_PLBK_TEST_SLEEP_TIME);
}

void sns_plbk_sam_thr_cancel_test(void)
{
  qmi_txn_handle txn_handle;
  qmi_client_error_type err;
  uint8 dummy_req;
  uint8 *dummy_respP;

  dummy_respP = (uint8 *) SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS, sizeof(uint8));

  /* update send request count */
  sam_thr_test[SNS_PLBK_REQ][SNS_SAM_SENSOR_THRESH_CANCEL_REQ_V01]++;

  /* Threshold cancel request */
  err = qmi_client_send_msg_async(playback_sam_user_handle,
                                  SNS_SAM_SENSOR_THRESH_CANCEL_REQ_V01,
                                  &dummy_req,
                                  0,
                                  dummy_respP,
                                  0,
                                  sns_playback_sam_resp_cb,
                                  (void *)SNS_PLBK_THR_TEST_VAL,
                                  &txn_handle);
  if (QMI_NO_ERR != err)
  {
    /* extra count indicates failure to send */
    sam_thr_test[SNS_PLBK_REQ][SNS_SAM_SENSOR_THRESH_CANCEL_REQ_V01]++;

    SNS_PLBK_MSG_3("qmi_client_send_msg_async fail = %d @: %d, %s\n", err, __LINE__, __FILE__);
  }

  sns_em_sleep(SNS_PLBK_TEST_SLEEP_TIME);
}

void sns_plbk_sam_amd_report_req_test(void)
{
  qmi_txn_handle txn_handle;
  qmi_client_error_type err;
  sns_sam_qmd_get_report_req_msg_v01 amd_get_req;
  sns_sam_qmd_get_report_resp_msg_v01 *amd_get_respP;

  amd_get_respP = (sns_sam_qmd_get_report_resp_msg_v01 *)
                     SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS,
                       sizeof(sns_sam_qmd_get_report_resp_msg_v01));

  /* update send request count */
  sam_amd_test[SNS_PLBK_REQ][SNS_SAM_AMD_GET_REPORT_REQ_V01]++;

  /* report request test */
  amd_get_req.instance_id = 0;
  err = qmi_client_send_msg_async(playback_sam_user_handle,
                                  SNS_SAM_AMD_GET_REPORT_REQ_V01,
                                  &amd_get_req,
                                  sizeof(sns_sam_qmd_get_report_req_msg_v01),
                                  amd_get_respP,
                                  sizeof(sns_sam_qmd_get_report_resp_msg_v01),
                                  sns_playback_sam_resp_cb,
                                  NULL,
                                  &txn_handle);
  if (QMI_NO_ERR != err)
  {
    /* extra count indicates failure to send */
    sam_amd_test[SNS_PLBK_REQ][SNS_SAM_AMD_GET_REPORT_REQ_V01]++;

    SNS_PLBK_MSG_3("qmi_client_send_msg_async fail = %d @: %d, %s\n", err, __LINE__, __FILE__);
  }

  sns_em_sleep(SNS_PLBK_TEST_SLEEP_TIME);
}

void sns_plbk_sam_amd_version_test(void)
{
  qmi_txn_handle txn_handle;
  qmi_client_error_type err;
  uint8 dummy_req;
  uint8 *dummy_respP;

  /* AMD version request */
  dummy_respP = (uint8 *) SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS, sizeof(uint8));

  /* update send request count */
  sam_amd_test[SNS_PLBK_REQ][SNS_SAM_AMD_VERSION_REQ_V01]++;

  err = qmi_client_send_msg_async(playback_sam_user_handle,
                                  SNS_SAM_AMD_VERSION_REQ_V01,
                                  &dummy_req,
                                  0,
                                  dummy_respP,
                                  0,
                                  sns_playback_sam_resp_cb,
                                  NULL,
                                  &txn_handle);
  if (QMI_NO_ERR != err)
  {
    /* extra count indicates failure to send */
    sam_amd_test[SNS_PLBK_REQ][SNS_SAM_AMD_VERSION_REQ_V01]++;

    SNS_PLBK_MSG_3("qmi_client_send_msg_async fail = %d @: %d, %s\n", err, __LINE__, __FILE__);
  }
  sns_em_sleep(SNS_PLBK_TEST_SLEEP_TIME);
}

void sns_plbk_sam_amd_cancel_test(void)
{
  qmi_txn_handle txn_handle;
  qmi_client_error_type err;
  uint8 dummy_req;
  uint8 *dummy_respP;

  dummy_respP = (uint8 *) SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS, sizeof(uint8));

  /* update send request count */
  sam_amd_test[SNS_PLBK_REQ][SNS_SAM_AMD_CANCEL_REQ_V01]++;

  /* Threshold cancel request */
  err = qmi_client_send_msg_async(playback_sam_user_handle,
                                  SNS_SAM_AMD_CANCEL_REQ_V01,
                                  &dummy_req,
                                  0,
                                  dummy_respP,
                                  0,
                                  sns_playback_sam_resp_cb,
                                  NULL,
                                  &txn_handle);
  if (QMI_NO_ERR != err)
  {
    /* extra count indicates failure to send */
    sam_amd_test[SNS_PLBK_REQ][SNS_SAM_AMD_CANCEL_REQ_V01]++;

    SNS_PLBK_MSG_3("qmi_client_send_msg_async fail = %d @: %d, %s\n", err, __LINE__, __FILE__);
  }

  sns_em_sleep(SNS_PLBK_TEST_SLEEP_TIME);
}

plbk_strm_res_s sns_plbk_smgr_sns_stream_test(
         uint8 frequency, uint8 patterns, uint32 iterations,
         volatile plbk_stream_cnt_s *strm_cnt)
{
  uint8 index;
  uint8 base_index;
  int8 local_count = 0;
  uint32_t start_time;
  uint32_t end_time;
  uint64_t break_time;
  plbk_strm_res_s plbk_stream_result = {0};

  /* set of combinations with diffenet report ID but streamed one at a time */
  for (index = 0; index <= patterns; index++, g_Report_id++)
  {
    /* start streaming */
    sns_plbk_smgr_sns_start_stop_stream (frequency, g_Report_id, index, SNS_SMGR_REPORT_ACTION_ADD_V01);

    start_time = sns_em_get_timestamp();
    break_time = (PLBK_BREAK_TIME * iterations) / frequency;

    do
    {
      plbk_stream_result.one_at_a_time |= (1 << index);

      end_time = sns_em_get_timestamp();
    }
#ifdef SNS_PLBK_TIME_CONDITION
    while (((end_time - start_time) < break_time) && (sns_success_cnt[g_Report_id] < iterations));
#else
    while (sns_success_cnt[g_Report_id] < iterations);
#endif


    if (sns_success_cnt[g_Report_id] >= iterations)
    {
      SNS_PLBK_MSG_1("sensor combination %s streamed one at a time pass\n", snsComb[index]);
    }
    else
    {
      SNS_PLBK_MSG_1("sensor combination %s streamed one at a time fail\n", snsComb[index]);
    }

    strm_cnt->one_sns[g_Report_id % SNS_PLBK_COMBINATIONS] = sns_success_cnt[g_Report_id];

    /* stop streaming */
    sns_plbk_smgr_sns_start_stop_stream (frequency, g_Report_id, index, SNS_SMGR_REPORT_ACTION_DELETE_V01);
  }

  /*
     *  set of combinations with diffenet report ID but streamed all together
     */
  break_time = 0;
  for (index = 0; index <= patterns; index++, g_Report_id++)
  {
    /* start streaming */
    sns_plbk_smgr_sns_start_stop_stream (frequency, g_Report_id, index, SNS_SMGR_REPORT_ACTION_ADD_V01);

    break_time += (PLBK_BREAK_TIME * iterations) / frequency;
  }

  start_time = sns_em_get_timestamp();

  do
  {
    base_index = g_Report_id - patterns - 1;

    for (index = 0; index <= patterns; index++, base_index++)
    {
      /* consider unfinished data */
      if ((plbk_stream_result.all_together & (1 << index)) == 0)
      {
        /* delete the message request */
        if (sns_success_cnt[base_index] >= iterations)
        {
          strm_cnt->all_sns[base_index % SNS_PLBK_COMBINATIONS] = sns_success_cnt[base_index];
          plbk_stream_result.all_together |= (1 << index);
          SNS_PLBK_MSG_1("sensor combination %s, streamed all together at a time pass\n", snsComb[index]);

          /* stop streaming */
          sns_plbk_smgr_sns_start_stop_stream (frequency, base_index, index, SNS_SMGR_REPORT_ACTION_DELETE_V01);

          local_count++;
        }
      }
    }

    end_time = sns_em_get_timestamp();
  }
#ifdef SNS_PLBK_TIME_CONDITION
  while ((local_count <= patterns) && ((end_time - start_time) < break_time));
#else
  while (local_count <= patterns);
  SNS_PLBK_MSG_2("start_time %d, end_time %d\n", start_time, end_time);
#endif
  /* cancel the failed sensors streaming */
  base_index = g_Report_id - patterns - 1;
  for (index = 0; index <= patterns; index++, base_index++)
  {
    if ((plbk_stream_result.all_together & (1 << index)) == 0)
    {
      SNS_PLBK_MSG_1("sensor combination %s, streamed all together at a time fail\n", snsComb[index]);

      /* stop streaming */
      sns_plbk_smgr_sns_start_stop_stream (frequency, base_index, index, SNS_SMGR_REPORT_ACTION_DELETE_V01);
    }
  }

  return plbk_stream_result;
}

void sns_plbk_smgr_sns_status_add_delete(uint8 sensor, uint8 action)
{
  qmi_txn_handle txn_handle;
  qmi_client_error_type err;
  sns_smgr_sensor_status_req_msg_v01  sns_sts_req;
  sns_smgr_sensor_status_resp_msg_v01 *sns_sts_respP;

  /*
      * sensor status add request
      */
  sns_sts_respP = (sns_smgr_sensor_status_resp_msg_v01 *)
                        SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS,
                           sizeof(sns_smgr_sensor_status_resp_msg_v01));

  memset(sns_sts_respP, 0, sizeof(sns_smgr_sensor_status_resp_msg_v01));
  sns_sts_req.SensorID       = sensor_id[sensor];
  sns_sts_req.Action         = action;
  sns_sts_req.ReqDataTypeNum = 1;

  /* update send request count */
  smgr_test[SNS_PLBK_REQ][SNS_SMGR_SENSOR_STATUS_REQ_V01]++;

  err = qmi_client_send_msg_async (playback_smgr_user_handle,
                                   SNS_SMGR_SENSOR_STATUS_REQ_V01,
                                   &sns_sts_req,
                                   sizeof(sns_smgr_sensor_status_req_msg_v01),
                                   sns_sts_respP,
                                   sizeof(sns_smgr_sensor_status_resp_msg_v01),
                                   sns_playback_smgr_resp_cb,
                                   NULL,
                                   &txn_handle);
  if (QMI_NO_ERR != err)
  {
    /* extra count indicates failure to send */
    smgr_test[SNS_PLBK_REQ][SNS_SMGR_SENSOR_STATUS_REQ_V01]++;

    SNS_PLBK_MSG_3("error in qmi_client_send_msg_async %d: @%d, %s\n", err, __LINE__, __FILE__);
  }
}

void sns_plbk_smgr_sns_status_test (void)
{
  uint8 index;

  for (index = 0; index < 4; index++)
  {
    sns_plbk_smgr_sns_status_add_delete(index, SNS_SMGR_SENSOR_STATUS_ADD_V01);
    sns_em_sleep(SNS_PLBK_TEST_SLEEP_TIME);
    sns_plbk_smgr_sns_status_add_delete(index, SNS_SMGR_SENSOR_STATUS_DEL_V01);
    sns_em_sleep(SNS_PLBK_TEST_SLEEP_TIME);
  }
}

void sns_plbk_smgr_sns_calibration_test (
  uint8 usage, uint8 SensorID, uint8 DataType)
{
  sns_smgr_sensor_cal_req_msg_v01  *sns_cal_reqP  = NULL;
  sns_smgr_sensor_cal_resp_msg_v01 *sns_cal_respP = NULL;
  plbk_msg_req_s                   *plbkReq       = NULL;

  sns_cal_reqP = (sns_smgr_sensor_cal_req_msg_v01 *)
                    SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS,
                      sizeof(sns_smgr_sensor_cal_req_msg_v01));

  sns_cal_respP = (sns_smgr_sensor_cal_resp_msg_v01 *)
                     SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS,
                       sizeof(sns_smgr_sensor_cal_resp_msg_v01));

  plbkReq = (plbk_msg_req_s *) SNS_OS_MALLOC(SNS_DBG_NUM_MOD_IDS, sizeof(plbk_msg_req_s));

  /* Request calibration */
  sns_cal_reqP->usage = usage;
  sns_cal_reqP->SensorId = SensorID;
  sns_cal_reqP->DataType = DataType;

  /* Send bias correction values to SMGR */
  sns_cal_reqP->ZeroBias_len = 3;
  sns_cal_reqP->ZeroBias[0] = 0;
  sns_cal_reqP->ZeroBias[1] = 0;
  sns_cal_reqP->ZeroBias[2] = 0;
  sns_cal_reqP->ScaleFactor_len = 0;
  sns_cal_reqP->ScaleFactor[0] = 0;
  sns_cal_reqP->ScaleFactor[1] = 0;
  sns_cal_reqP->ScaleFactor[2] = 0;

  /* update send request count */
  smgr_test[SNS_PLBK_REQ][SNS_SMGR_CAL_REQ_V01]++;

  plbkReq->plbk_cient = SNS_PLBK_CLIENT_SMGR;
  plbkReq->report_req = SNS_SMGR_CAL_REQ_V01;
  plbkReq->rep_req_data = sns_cal_reqP;
  plbkReq->rep_req_size = sizeof(sns_smgr_sensor_cal_req_msg_v01);
  plbkReq->rep_resp_data = sns_cal_respP;
  plbkReq->rep_resp_size = sizeof(sns_smgr_sensor_cal_resp_msg_v01);
  plbkReq->resp_clbk_data = NULL;

  sns_playback_put_msg_req_to_queue (plbkReq, PLAYBACK_LOW_PRIORIY_MSG_REQ);
}

void sns_plbk_smgr_sns_pwr_status_add_delete(uint8 reportId, uint8 action)
{
  qmi_txn_handle txn_handle;
  qmi_client_error_type err;
  sns_smgr_sensor_power_status_req_msg_v01 pow_sts_req;
  sns_smgr_sensor_power_status_resp_msg_v01 *pow_sts_respP;

  /*
     * power status request
     */
  pow_sts_respP = (sns_smgr_sensor_power_status_resp_msg_v01 *)
                        SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS,
                           sizeof(sns_smgr_sensor_power_status_resp_msg_v01));

  pow_sts_req.ReportId = reportId;
  pow_sts_req.Action = action;

  /* update send request count */
  smgr_test[SNS_PLBK_REQ][SNS_SMGR_SENSOR_POWER_STATUS_REQ_V01]++;

  err = qmi_client_send_msg_async (playback_smgr_user_handle,
                                   SNS_SMGR_SENSOR_POWER_STATUS_REQ_V01,
                                   &pow_sts_req,
                                   sizeof(sns_smgr_sensor_power_status_req_msg_v01),
                                   pow_sts_respP,
                                   sizeof(sns_smgr_sensor_power_status_resp_msg_v01),
                                   sns_playback_smgr_resp_cb,
                                   NULL,
                                   &txn_handle);
  if (QMI_NO_ERR != err)
  {
    /* extra count indicates failure to send */
    smgr_test[SNS_PLBK_REQ][SNS_SMGR_SENSOR_POWER_STATUS_REQ_V01]++;

    SNS_PLBK_MSG_3("qmi_client_send_msg_async fail = %d @: %d, %s\n", err, __LINE__, __FILE__);
  }
}

void sns_plbk_smgr_sns_pwr_status_test(void)
{
  uint8 index;

  for (index = 0; index < 2; index++)
  {
    sns_plbk_smgr_sns_pwr_status_add_delete(index, SNS_SMGR_POWER_STATUS_ADD_V01);
    sns_em_sleep(SNS_PLBK_TEST_SLEEP_TIME);
    sns_plbk_smgr_sns_pwr_status_add_delete(index, SNS_SMGR_POWER_STATUS_DEL_V01);
    sns_em_sleep(SNS_PLBK_TEST_SLEEP_TIME);
  }
}

void sns_plbk_smgr_sns_pwr_control_test(void)
{
  sns_smgr_sensor_power_control_req_msg_v01 pwr_ctrl_req;
  sns_smgr_sensor_power_control_resp_msg_v01 *pwr_ctrl_respP;
  qmi_txn_handle txn_handle;
  qmi_client_error_type err;
  uint8 index;

  for (index = 0; index < 4; index++)
  {
    pwr_ctrl_respP = (sns_smgr_sensor_power_control_resp_msg_v01 *)
                          SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS,
                             sizeof(sns_smgr_sensor_power_control_resp_msg_v01));

    /* power control request */
    pwr_ctrl_req.SensorID = sensor_id[index];
    pwr_ctrl_req.Action = SNS_SMGR_POWER_CTRL_AUTO_V01;

    /* update send request count */
    smgr_test[SNS_PLBK_REQ][SNS_SMGR_SENSOR_POWER_CONTROL_REQ_V01]++;

    err = qmi_client_send_msg_async (playback_smgr_user_handle,
                                     SNS_SMGR_SENSOR_POWER_CONTROL_REQ_V01,
                                     &pwr_ctrl_req,
                                     sizeof(sns_smgr_sensor_power_control_req_msg_v01),
                                     pwr_ctrl_respP,
                                     sizeof(sns_smgr_sensor_power_control_resp_msg_v01),
                                     sns_playback_smgr_resp_cb,
                                     NULL,
                                     &txn_handle);
    if (QMI_NO_ERR != err)
    {
      /* extra count indicates failure to send */
      smgr_test[SNS_PLBK_REQ][SNS_SMGR_SENSOR_POWER_CONTROL_REQ_V01]++;

      SNS_PLBK_MSG_3("qmi_client_send_msg_async fail = %d @: %d, %s\n", err, __LINE__, __FILE__);
    }
    sns_em_sleep(SNS_PLBK_TEST_SLEEP_TIME);
  }
}

void sns_plbk_smgr_sns_single_sns_test(
     uint8 TestType, BOOLEAN SaveToRegistry, BOOLEAN ApplyCalNow,
     uint8 SensorID, uint8 DataType)
{
  sns_smgr_single_sensor_test_req_msg_v01  *sing_sns_test_reqP  = NULL;
  sns_smgr_single_sensor_test_resp_msg_v01 *sing_sns_test_respP = NULL;
  plbk_msg_req_s                           *plbkReq             = NULL;

  sing_sns_test_reqP = (sns_smgr_single_sensor_test_req_msg_v01 *)
                           SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS,
                             sizeof(sns_smgr_single_sensor_test_req_msg_v01));

  sing_sns_test_respP = (sns_smgr_single_sensor_test_resp_msg_v01 *)
                           SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS,
                             sizeof(sns_smgr_single_sensor_test_resp_msg_v01));

  plbkReq = (plbk_msg_req_s *) SNS_OS_MALLOC(SNS_DBG_NUM_MOD_IDS, sizeof(plbk_msg_req_s));

  /* Single sensor test */
  sing_sns_test_reqP->SensorID = SensorID;
  sing_sns_test_reqP->DataType = DataType;
  sing_sns_test_reqP->TestType = TestType;
  sing_sns_test_reqP->SaveToRegistry_valid = SaveToRegistry;
  sing_sns_test_reqP->SaveToRegistry = SaveToRegistry;
  sing_sns_test_reqP->ApplyCalNow_valid = ApplyCalNow;
  sing_sns_test_reqP->ApplyCalNow = ApplyCalNow;

  /* update send request count */
  smgr_test[SNS_PLBK_REQ][SNS_SMGR_SINGLE_SENSOR_TEST_REQ_V01]++;

  plbkReq->plbk_cient = SNS_PLBK_CLIENT_SMGR;
  plbkReq->report_req = SNS_SMGR_SINGLE_SENSOR_TEST_REQ_V01;
  plbkReq->rep_req_data = sing_sns_test_reqP;
  plbkReq->rep_req_size = sizeof(sns_smgr_single_sensor_test_req_msg_v01);
  plbkReq->rep_resp_data = sing_sns_test_respP;
  plbkReq->rep_resp_size = sizeof(sns_smgr_single_sensor_test_resp_msg_v01);
  plbkReq->resp_clbk_data = NULL;

  sns_playback_put_msg_req_to_queue (plbkReq, PLAYBACK_LOW_PRIORIY_MSG_REQ);
}

void sns_plbk_smgr_sns_single_sns_info_test(void)
{
  sns_smgr_single_sensor_info_req_msg_v01 sing_sns_info_req;
  sns_smgr_single_sensor_info_resp_msg_v01 *sing_sns_info_respP;
  qmi_txn_handle txn_handle;
  qmi_client_error_type err;
  uint8 index;

  for (index = 0; index < 4; index++)
  {
    /* single sensor info request */
    sing_sns_info_respP = (sns_smgr_single_sensor_info_resp_msg_v01 *)
                             SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS,
                               sizeof(sns_smgr_single_sensor_info_resp_msg_v01));

    /* update send request count */
    smgr_test[SNS_PLBK_REQ][SNS_SMGR_SINGLE_SENSOR_INFO_REQ_V01]++;

    sing_sns_info_req.SensorID = sensor_id[index];
    err = qmi_client_send_msg_async (playback_smgr_user_handle,
                                     SNS_SMGR_SINGLE_SENSOR_INFO_REQ_V01,
                                     &sing_sns_info_req,
                                     sizeof(sns_smgr_single_sensor_info_req_msg_v01),
                                     sing_sns_info_respP,
                                     sizeof(sns_smgr_single_sensor_info_resp_msg_v01),
                                     sns_playback_smgr_resp_cb,
                                     NULL,
                                     &txn_handle);
    if (QMI_NO_ERR != err)
    {
      /* extra count indicates failure to send */
      smgr_test[SNS_PLBK_REQ][SNS_SMGR_SINGLE_SENSOR_INFO_REQ_V01]++;

      SNS_PLBK_MSG_3("qmi_client_send_msg_async fail = %d @: %d, %s\n", err, __LINE__, __FILE__);
    }
    sns_em_sleep(SNS_PLBK_TEST_SLEEP_TIME);
  }
}

void sns_plbk_smgr_sns_all_sns_info_test(void)
{
  uint8 dummy = 0;
  sns_smgr_all_sensor_info_resp_msg_v01 *all_sns_info_respP;
  qmi_txn_handle txn_handle;
  qmi_client_error_type err;

  all_sns_info_respP = (sns_smgr_all_sensor_info_resp_msg_v01 *)
                           SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS,
                             sizeof(sns_smgr_all_sensor_info_resp_msg_v01));

  /* update send request count */
  smgr_test[SNS_PLBK_REQ][SNS_SMGR_ALL_SENSOR_INFO_REQ_V01]++;

  /* all sensor info request */
  err = qmi_client_send_msg_async (playback_smgr_user_handle,
                                   SNS_SMGR_ALL_SENSOR_INFO_REQ_V01,
                                   &dummy,
                                   0,
                                   all_sns_info_respP,
                                   sizeof(sns_smgr_all_sensor_info_resp_msg_v01),
                                   sns_playback_smgr_resp_cb,
                                   NULL,
                                   &txn_handle);
  if (QMI_NO_ERR != err)
  {
    /* extra count indicates failure to send */
    smgr_test[SNS_PLBK_REQ][SNS_SMGR_ALL_SENSOR_INFO_REQ_V01]++;

    SNS_PLBK_MSG_3("qmi_client_send_msg_async fail = %d @: %d, %s\n", err, __LINE__, __FILE__);
  }
  sns_em_sleep(SNS_PLBK_TEST_SLEEP_TIME);
}

void sns_plbk_smgr_sns_ver_test(void)
{
  uint8 dummy = 0;
  sns_common_version_resp_msg_v01  *sns_ver_respP;
  qmi_txn_handle txn_handle;
  qmi_client_error_type err;

  sns_ver_respP = (sns_common_version_resp_msg_v01 *)
                      SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS,
                        sizeof(sns_common_version_resp_msg_v01));

  /* update send request count */
  smgr_test[SNS_PLBK_REQ][SNS_SMGR_VERSION_REQ_V01]++;

  /* version request */
  err = qmi_client_send_msg_async (playback_smgr_user_handle,
                                   SNS_SMGR_VERSION_REQ_V01,
                                   &dummy,
                                   0,
                                   sns_ver_respP,
                                   sizeof(sns_common_version_resp_msg_v01),
                                   sns_playback_smgr_resp_cb,
                                   NULL,
                                   &txn_handle);
  if (QMI_NO_ERR != err)
  {
    /* extra count indicates failure to send */
    smgr_test[SNS_PLBK_REQ][SNS_SMGR_VERSION_REQ_V01]++;

    SNS_PLBK_MSG_3("qmi_client_send_msg_async fail = %d @: %d, %s\n", err, __LINE__, __FILE__);
  }
  sns_em_sleep(SNS_PLBK_TEST_SLEEP_TIME);
}


void sns_plbk_smgr_sns_test_test(void)
{
  uint8 dummy = 0;
  sns_smgr_sensor_test_resp_msg_v01  *sns_test_respP;
  qmi_txn_handle txn_handle;
  qmi_client_error_type err;

  sns_test_respP = (sns_smgr_sensor_test_resp_msg_v01 *)
                      SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS,
                        sizeof(sns_smgr_sensor_test_resp_msg_v01));

  /* update send request count */
  smgr_test[SNS_PLBK_REQ][SNS_SMGR_SENSOR_TEST_REQ_V01]++;

  err = qmi_client_send_msg_async (playback_smgr_user_handle,
                                   SNS_SMGR_SENSOR_TEST_REQ_V01,
                                   &dummy,
                                   0,
                                   sns_test_respP,
                                   sizeof(sns_smgr_sensor_test_resp_msg_v01),
                                   sns_playback_smgr_resp_cb,
                                   NULL,
                                   &txn_handle);
  if (QMI_NO_ERR != err)
  {
    /* extra count indicates failure to send */
    smgr_test[SNS_PLBK_REQ][SNS_SMGR_SENSOR_TEST_REQ_V01]++;

    SNS_PLBK_MSG_3("qmi_client_send_msg_async fail = %d @: %d, %s\n", err, __LINE__, __FILE__);
  }
  sns_em_sleep(SNS_PLBK_TEST_SLEEP_TIME);
}

void sns_plbk_smgr_sns_cancel_test(void)
{
  uint8 dummy = 0;
  sns_common_cancel_resp_msg_v01  *sns_can_respP;
  qmi_txn_handle txn_handle;
  qmi_client_error_type err;

  sns_can_respP = (sns_common_cancel_resp_msg_v01 *)
                      SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS,
                        sizeof(sns_common_cancel_resp_msg_v01));

  /* update send request count */
  smgr_test[SNS_PLBK_REQ][SNS_SMGR_CANCEL_REQ_V01]++;

  /* cancel request */
  err = qmi_client_send_msg_async (playback_smgr_user_handle,
                                   SNS_SMGR_CANCEL_REQ_V01,
                                   &dummy,
                                   0,
                                   sns_can_respP,
                                   sizeof(sns_common_cancel_resp_msg_v01),
                                   sns_playback_smgr_resp_cb,
                                   NULL,
                                   &txn_handle);
  if (QMI_NO_ERR != err)
  {
    /* extra count indicates failure to send */
    smgr_test[SNS_PLBK_REQ][SNS_SMGR_CANCEL_REQ_V01]++;

    SNS_PLBK_MSG_3("qmi_client_send_msg_async fail = %d @: %d, %s\n", err, __LINE__, __FILE__);
  }
  sns_em_sleep(SNS_PLBK_TEST_SLEEP_TIME);
}

void sns_plbk_sam_amd_test (void)
{
  sns_plbk_sam_amd_enable_test(SNS_PLBK_SAM_DEF_CLIENT_REPORT_PERIOD);
  sns_plbk_sam_amd_version_test();
  sns_plbk_sam_amd_report_req_test();
  sns_plbk_sam_amd_cancel_test();
  sns_plbk_sam_amd_disable_test(sns_sam_instance_id);
}

void sns_plbk_sam_threshold_test (void)
{
  sns_plbk_sam_thr_enable_test();
  sns_plbk_sam_thr_version_test();
  sns_plbk_sam_thr_report_req_test();
  sns_plbk_sam_thr_cancel_test();
  sns_plbk_sam_thr_disable_test();
}

void sns_plbk_smgr_test (void)
{
  sns_plbk_smgr_sns_status_test();
  sns_sensors_calibration_test();
  sns_plbk_smgr_sns_pwr_status_test();
  sns_plbk_smgr_sns_pwr_control_test();
  sns_plbk_smgr_sns_single_sns_test (SNS_SMGR_TEST_SELF_V01, TRUE, TRUE, sensor_id[0], SNS_SMGR_DATA_TYPE_PRIMARY_V01);
  sns_plbk_smgr_sns_single_sns_info_test();
  sns_plbk_smgr_sns_all_sns_info_test();
  sns_plbk_smgr_sns_ver_test();
  sns_plbk_smgr_sns_test_test();
  sns_plbk_smgr_sns_cancel_test();
  SamplingFreq = 100;
  stress_entries = 100;
  sns_plbk_stress_test();
}

void sns_plbk_sam_smgr_test(void)
{
  uint8 index;

  while (test_sam_amd_plbk | test_sam_thr_plbk | test_smgr_plbk)
  {
    if (test_sam_amd_plbk == TRUE)
    {
      sns_plbk_sam_amd_test();
      test_sam_amd_plbk = FALSE;
      sns_em_sleep(10000);

      SNS_PLBK_MSG("\n*****************SAM AMD TEST RESULTS*********************\n");

      for (index = 0; index <= SNS_SAM_AMD_ERROR_IND_V01; index++)
      {
        if (index == SNS_SAM_AMD_ERROR_IND_V01)
        {
          if (sam_amd_ind_test[0] == 0)
          {
             SNS_PLBK_MSG_2("Error in %s, indications received = %lu\n", testAmdIndMsg[0], sam_amd_ind_test[0]);
          }
        }
        else if (index == SNS_SAM_AMD_REPORT_IND_V01)
        {
          if (sam_amd_ind_test[1] == 0)
          {
             SNS_PLBK_MSG_2("Error in %s, indications received = %lu\n", testAmdIndMsg[1], sam_amd_ind_test[1]);
          }
        }
        else
        {
          if (sam_amd_test[SNS_PLBK_REQ][index] != sam_amd_test[SNS_PLBK_RESP][index])
          {
            SNS_PLBK_MSG_4("Error in %s <-> %s, Request sent = %d, response received = %d\n",
                    testAmdReqMsg[index], testAmdRespMsg[index],
                    sam_amd_test[SNS_PLBK_REQ][index], sam_amd_test[SNS_PLBK_RESP][index]);
          }
        }
      }

      SNS_PLBK_MSG("**********************************************************\n\n");
    }

    if (test_sam_thr_plbk == TRUE)
    {
      sns_plbk_sam_threshold_test();
      test_sam_thr_plbk = FALSE;
      sns_em_sleep(10000);

      SNS_PLBK_MSG("\n*****************SAM THR TEST RESULTS*********************\n");

      for (index = 0; index <= SNS_SAM_SENSOR_THRESH_ERROR_IND_V01; index++)
      {
        if (index == SNS_SAM_SENSOR_THRESH_REPORT_IND_V01)
        {
          if (sam_thr_ind_test[0] == 0)
          {
             SNS_PLBK_MSG_2("Error in %s, indications received = %lu\n", testThrIndMsg[0], sam_thr_ind_test[0]);
          }
        }
        else if (index == SNS_SAM_SENSOR_THRESH_ERROR_IND_V01)
        {
          if (sam_thr_ind_test[1] == 0)
          {
             SNS_PLBK_MSG_2("Error in %s, indications received = %lu\n", testThrIndMsg[1], sam_thr_ind_test[1]);
          }
        }
        else
        {
          if (sam_thr_test[SNS_PLBK_REQ][index] != sam_thr_test[SNS_PLBK_RESP][index])
          {
            SNS_PLBK_MSG_4("Error in %s <-> %s, Request sent = %d, response received = %d\n",
                    testThrReqMsg[index], testThrRespMsg[index],
                    sam_thr_test[SNS_PLBK_REQ][index], sam_thr_test[SNS_PLBK_RESP][index]);
          }
        }
      }

      SNS_PLBK_MSG("**********************************************************\n\n");
    }

    if (test_smgr_plbk == TRUE)
    {
      sns_plbk_smgr_test();
      test_smgr_plbk = FALSE;
      sns_em_sleep(10000);

      SNS_PLBK_MSG("\n*****************SMGR TEST RESULTS************************\n");

      for (index = 0; index <= SNS_SMGR_SINGLE_SENSOR_TEST_IND_V01; index++)
      {
        if (index == SNS_SMGR_REPORT_IND_V01)
        {
          if (smgr_ind_test[0] == 0)
          {
             SNS_PLBK_MSG_2("Error in %s, indications received = %lu\n", testSmgrIndMsg[0], smgr_ind_test[0]);
          }
        }
        else if (index == SNS_SMGR_SENSOR_POWER_STATUS_IND_V01)
        {
          if (smgr_ind_test[1] == 0)
          {
             SNS_PLBK_MSG_2("Error in %s, indications received = %lu\n", testSmgrIndMsg[1], smgr_ind_test[1]);
          }
        }
        else if (index == SNS_SMGR_SENSOR_STATUS_IND_V01)
        {
          if (smgr_ind_test[2] == 0)
          {
             SNS_PLBK_MSG_2("Error in %s, indications received = %lu\n", testSmgrIndMsg[2], smgr_ind_test[2]);
          }
        }
        else if (index == SNS_SMGR_SINGLE_SENSOR_TEST_IND_V01)
        {
          if (smgr_ind_test[3] == 0)
          {
             SNS_PLBK_MSG_2("Error in %s, indications received = %lu\n", testSmgrIndMsg[3], smgr_ind_test[3]);
          }
        }
        else
        {
          if (smgr_test[SNS_PLBK_REQ][index] != smgr_test[SNS_PLBK_RESP][index])
          {
            SNS_PLBK_MSG_4("Error in %s <-> %s, Request sent = %d, response received = %d\n",
                    testSmgrReqMsg[index], testSmgrRespMsg[index],
                    smgr_test[SNS_PLBK_REQ][index], smgr_test[SNS_PLBK_RESP][index]);
          }
        }
      }

      SNS_PLBK_MSG("**********************************************************\n\n");
    }
  }

  SNS_PLBK_MSG("SAM & SMGR -> QMI -> PLBK test done\n");
}

/*=========================================================================
  FUNCTION:  sns_plbk_stress_test
=========================================================================*/
/*!
    @brief
    Stress test for sensors
    View the variable stream_cont_str_tst for the combination of sensors stress test.

    @return
    TRUE/FALSE
*/
/*=======================================================================*/
BOOLEAN sns_plbk_stress_test (void)
{
  uint8    index = 0;
  BOOLEAN  result = TRUE;

  SNS_PLBK_MSG("Stress test started\n");

  while (plbk_stress_test == TRUE)
  {
    plbk_stress_test = FALSE;
    plbk_steam_test_results.one_at_a_time = 0;
    plbk_steam_test_results.all_together = 0;
    memset ((void *) &stream_cont_str_tst, 0, sizeof(plbk_stream_cnt_s));

    /* start streaming test */
    plbk_steam_test_results = sns_plbk_smgr_sns_stream_test(SamplingFreq, sensors_pattern,
                                               stress_entries, &stream_cont_str_tst);

    /* print the results */
    SNS_PLBK_MSG_1("******************Tesing at Frequency pattern %d******************\n", SamplingFreq);

    for (index = 0; index <= sensors_pattern; index++)
    {
      if ((plbk_steam_test_results.one_at_a_time & (1<< index)) == 0)
      {
        SNS_PLBK_MSG_2("sensor combination %s streamed one at a time fail, stream count = %lu\n",
                        snsComb[index], stream_cont_str_tst.one_sns[index]);
        result = FALSE;
      }
      else
      {
        SNS_PLBK_MSG_2("sensor combination %s streamed one at a time pass, stream count = %lu\n",
                        snsComb[index], stream_cont_str_tst.one_sns[index]);
      }
    }

    for (index = 0; index <= sensors_pattern; index++)
    {
      if ((plbk_steam_test_results.all_together & (1<< index)) == 0)
      {
        SNS_PLBK_MSG_2("sensor combination %s, streamed all together at a time fail, stream count = %lu\n",
                        snsComb[index], stream_cont_str_tst.all_sns[index]);
        result = FALSE;
      }
      else
      {
        SNS_PLBK_MSG_2("sensor combination %s, streamed all together at a time pass, stream count = %lu\n",
                        snsComb[index], stream_cont_str_tst.all_sns[index]);
      }
    }
  }

  if (result == TRUE)
  {
    SNS_PLBK_MSG("Stress test PASS\n");
  }
  else
  {
    SNS_PLBK_MSG("Stress test FAIL\n");
  }

  return result;
}

BOOLEAN plbk_process_all_data(OS_FLAGS *flags)
{
  OS_FLAGS               sigFlags     = 0;
  BOOLEAN                result       = FALSE;
  plbk_q_item_s          *msgPtr      = NULL;
  plbk_msg_req_s         *plbkReqP    = NULL;
  void                   *decodePtr   = NULL;
  qmi_client_error_type  err          = 0;

  sigFlags = *flags;

  /* Send Playback high priority requests */
  if (sigFlags & PLAYBACK_HIGH_PRIORIY_MSG_REQ)
  {
    /* high priority message so procss everything */
    while (NULL != (plbkReqP = sns_plbk_q_get(PLAYBACK_HIGH_PRIORIY_MSG_REQ)))
    {
      sns_playback_send_msg_requests (plbkReqP);

      result = TRUE;
    }
  }

  /* Process Playback messages */
  if (sigFlags & PLAYBACK_MSG_SIGNAL)
  {
    result = TRUE;

    /* Process SMGR indications */
    if (NULL != (msgPtr = sns_plbk_q_get(PLAYBACK_MSG_SIGNAL)))
    {
      if (msgPtr->msg_type == PLAYBACK_SMGR_IND_MSG)
      {
        decodePtr = SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS, msgPtr->plbk_decode_len);

        err = qmi_client_message_decode (playback_smgr_user_handle, QMI_IDL_INDICATION, msgPtr->msg_id,
                                         msgPtr->body_ptr, msgPtr->plbk_msg_len, decodePtr,
                                         msgPtr->plbk_decode_len);
        if (err == QMI_NO_ERR)
        {
          sns_playback_process_smgr_ind (decodePtr, msgPtr->msg_id, msgPtr->user_data);
        }

        SNS_OS_FREE(decodePtr);
      }
      /* Process SAM indications */
      else if (msgPtr->msg_type == PLAYBACK_SAM_IND_MSG)
      {
        decodePtr = SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS, msgPtr->plbk_decode_len);

        err = qmi_client_message_decode (playback_sam_user_handle, QMI_IDL_INDICATION, msgPtr->msg_id,
                                         msgPtr->body_ptr, msgPtr->plbk_msg_len, decodePtr,
                                         msgPtr->plbk_decode_len);
        if (err == QMI_NO_ERR)
        {
          sns_playback_process_sam_ind (decodePtr, msgPtr->msg_id, msgPtr->user_data);
        }

        SNS_OS_FREE(decodePtr);
      }
      /* Process SMGR responses  */
      else if (msgPtr->msg_type == PLAYBACK_SMGR_RESP_MSG)
      {
        sns_playback_process_smgr_resp (msgPtr->body_ptr, msgPtr->msg_id, msgPtr->user_data);
      }
      /* Process SAM responses  */
      else if (msgPtr->msg_type == PLAYBACK_SAM_RESP_MSG)
      {
        sns_playback_process_sam_resp (msgPtr->body_ptr, msgPtr->msg_id, msgPtr->user_data);
      }
      else
      {
        SNS_PLBK_MSG("Invalid PLBK Message type\n");
        result = TRUE;
      }
      SNS_OS_FREE(msgPtr->body_ptr);
      SNS_OS_FREE(msgPtr);
      msgPtr = NULL;
    }
    else
    {
      result = FALSE;
    }
  }

  /* Send Playback low priority requests */
  if (sigFlags & PLAYBACK_LOW_PRIORIY_MSG_REQ)
  {
    if (NULL != (plbkReqP = sns_plbk_q_get(PLAYBACK_LOW_PRIORIY_MSG_REQ)))
    {
      sns_playback_send_msg_requests (plbkReqP);

      result = TRUE;
    }
  }

  sigFlags &= ~SNS_TEST_SIG_FLAG;

  *flags = sigFlags;

  return result;
}

/* Create Sensors Ulog to store the testcase logs. Sensors ulog is sufficient
 * to hold the results of testcases defined by plbk_test_case[] */
static void sns_ulog_create(void)
{
  ULogResult sns_log_result;

  sns_log_result = ULogFront_RealTimeInit(&sns_log_handle,
                          "Sensor_test_log",
                          sns_log_size,
                          ULOG_MEMORY_LOCAL,
                          ULOG_LOCK_OS );
  printf("Sensor_test_log created with %u\n", (unsigned int)sns_log_result);
}

/*=========================================================================
  FUNCTION:  sns_playback_task
=========================================================================*/
/*!
    @brief
    Playback task

    @return
    None
*/
/*=======================================================================*/
void sns_playback_task (void *p_arg)
{
  OS_FLAGS sigFlags;
  uint8_t err = 0;
  BOOLEAN result = TRUE;
  qmi_client_error_type       qmi_err;

  /* SMGR variables */
  qmi_idl_service_object_type playback_smgr_service_obj = SNS_SMGR_SVC_get_service_object_v01();
  qmi_service_info            playback_smgr_service_info_array = { {0} };
  unsigned int                playback_smgr_num_entries = 1;
  unsigned int                playback_smgr_num_services = 0;

  /* SAM variables */
#if SNS_PLBK_SAM_CLNT
  qmi_idl_service_object_type playback_sam_service_obj = SNS_SAM_AMD_SVC_get_service_object_v01();
  qmi_service_info            playback_sam_service_info_array = { {0} };
  unsigned int                playback_sam_num_entries = 1;
  unsigned int                playback_sam_num_services = 0;
#endif  /* SNS_PLBK_SAM_CLNT */

  memset(&playback_smgr_os_params,0,sizeof(playback_smgr_os_params));

  /* Queue Initialize */
  sns_q_init(&plbk_msg_q);
  sns_q_init(&plbk_high_pri_msg_req_q);
  sns_q_init(&plbk_low_pri_msg_req_q);

  sns_ulog_create();

  /* add the low priority message request signals */
  sns_os_sigs_add (sns_test_sig_grp, PLAYBACK_LOW_PRIORIY_MSG_REQ);
  sns_os_sigs_add (sns_test_sig_grp, PLAYBACK_MSG_SIGNAL);

  /* PLAYBACK - SMGR client notifier init */
  sns_os_sigs_add (sns_test_sig_grp, PLAYBACK_SMGR_QCSI_WAIT_SIG);
  sns_os_sigs_add (sns_test_sig_grp, PLAYBACK_SMGR_QCSI_TIMER_SIG);

  memset(&playback_smgr_os_params,0,sizeof(playback_smgr_os_params));
  playback_smgr_os_params.ext_signal = NULL;
  playback_smgr_os_params.sig = PLAYBACK_SMGR_QCSI_WAIT_SIG;
  playback_smgr_os_params.timer_sig = PLAYBACK_SMGR_QCSI_TIMER_SIG;

  qmi_err = qmi_client_notifier_init (playback_smgr_service_obj,
                                      &playback_smgr_os_params,
                                      &playback_smgr_user_handle);
  if (QMI_NO_ERR != qmi_err)
  {
    SNS_PLBK_MSG_3("error in qmi_client_notifier_init %d: @%d %s\n", qmi_err, __LINE__, __FILE__);
  }

#if SNS_PLBK_SAM_CLNT
  /* PLAYBACK - SAM client notifier init */
  sns_os_sigs_add (sns_test_sig_grp, PLAYBACK_SAM_QCSI_WAIT_SIG);
  sns_os_sigs_add (sns_test_sig_grp, PLAYBACK_SAM_QCSI_TIMER_SIG);

  memset(&playback_sam_os_params,0,sizeof(playback_sam_os_params));
  playback_sam_os_params.ext_signal = NULL;
  playback_sam_os_params.sig = PLAYBACK_SAM_QCSI_WAIT_SIG;
  playback_sam_os_params.timer_sig = PLAYBACK_SAM_QCSI_TIMER_SIG;

  qmi_err = qmi_client_notifier_init (playback_sam_service_obj,
                                      &playback_sam_os_params,
                                      &playback_sam_user_handle);
  if (QMI_NO_ERR != qmi_err)
  {
    SNS_PLBK_MSG_3("error in qmi_client_notifier_init %d: @%d %s\n", qmi_err, __LINE__, __FILE__);
  }
#endif /* SNS_PLBK_SAM_CLNT */

  /* PLAYBACK - SMGR get service list & client init */
  QMI_CCI_OS_SIGNAL_WAIT (&playback_smgr_os_params, 0);
  QMI_CCI_OS_SIGNAL_CLEAR (&playback_smgr_os_params);
  qmi_client_release (playback_smgr_user_handle);

  qmi_err = qmi_client_get_service_list (playback_smgr_service_obj,
                                         &playback_smgr_service_info_array,
                                         &playback_smgr_num_entries,
                                         &playback_smgr_num_services);
  if ((QMI_NO_ERR != qmi_err)
      && (playback_smgr_num_entries != playback_smgr_num_services))
  {
    SNS_PLBK_MSG_3("error in qmi_client_get_service_list %d: @%d %s\n", qmi_err, __LINE__, __FILE__);
  }

  qmi_err = qmi_client_init (&playback_smgr_service_info_array,
                             playback_smgr_service_obj, sns_playback_smgr_ind_cb,
                             NULL, &playback_smgr_os_params,
                             &playback_smgr_user_handle);
  if (QMI_NO_ERR != qmi_err)
  {
    SNS_PLBK_MSG_3("error in qmi_client_init %d: @%d %s\n", qmi_err, __LINE__, __FILE__);
  }

#if SNS_PLBK_SAM_CLNT
  /* PLAYBACK - SAM get service list & client init */
  QMI_CCI_OS_SIGNAL_WAIT (&playback_sam_os_params, 0);
  QMI_CCI_OS_SIGNAL_CLEAR (&playback_sam_os_params);
  qmi_client_release (playback_sam_user_handle);

  qmi_err = qmi_client_get_service_list (playback_sam_service_obj,
                                         &playback_sam_service_info_array,
                                         &playback_sam_num_entries,
                                         &playback_sam_num_services);
  if ((QMI_NO_ERR != qmi_err)
      && (playback_sam_num_entries != playback_sam_num_services))
  {
    SNS_PLBK_MSG_3("error in qmi_client_get_service_list %d: @%d %s\n", qmi_err, __LINE__, __FILE__);
  }

  qmi_err = qmi_client_init (&playback_sam_service_info_array,
                             playback_sam_service_obj, sns_playback_sam_ind_cb, NULL,
                             &playback_sam_os_params, &playback_sam_user_handle);
  if (QMI_NO_ERR != qmi_err)
  {
    SNS_PLBK_MSG_3("error in qmi_client_init %d: @%d %s\n", qmi_err, __LINE__, __FILE__);
  }
#endif /* SNS_PLBK_SAM_CLNT */

   sns_em_sleep(100000);

  sns_plbk_init_done = TRUE;

  SNS_PLBK_MSG_1("PLAYBACK Starting..... id(%d)\n", SNS_MODULE_DSPS_PLAYBACK);

  while (1)
  {
#ifdef SNS_QMI_ENABLE
    /* poll for the data incase signal is missing */
    result = TRUE;
    while (result == TRUE)
    {
      sigFlags = SNS_TEST_SIG_FLAG;

      result = plbk_process_all_data(&sigFlags);
    }
#endif

    /* OS_FLAG_CONSUME will make the SMR clear the flag after the call */
    sigFlags = sns_os_sigs_pend (sns_test_sig_grp, SNS_TEST_SIG_FLAG,
                                 OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME, 0, &err);

    result = plbk_process_all_data(&sigFlags);

    if (sigFlags != 0)
    {
      SNS_PLBK_MSG_3("Error in signal = %d @%d, %s\n", sigFlags, __LINE__, __FILE__);
    }
  }
}

/*=========================================================================
  FUNCTION:  sns_playback_init
  =========================================================================*/
/*!
  @brief Playback module initialization.
         Creates the playback task.

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_playback_init (void)
{
  static BOOLEAN task_status = FALSE;
  uint8_t err;

  /* initialise the playback task */
  if (task_status == FALSE)
  {
    task_status = TRUE;
    sns_em_sleep(20000);

    sns_test_sig_grp = sns_os_sigs_create (PLAYBACK_HIGH_PRIORIY_MSG_REQ, &err);
    palyback_mutex = sns_os_mutex_create (SNS_DSPS_MODULE_PRI_BASE, &err);

    SNS_PLBK_MSG("PLAYBACK task create\n");

    //create the PLBK task
    err = sns_os_task_create_ext(sns_playback_task,
                                 NULL,
                                 &sns_playback_stk[SNS_MODULE_STK_SIZE_DSPS_PLAYBACK-1],
                                 SNS_MODULE_PRI_DSPS_PLAYBACK,
                                 SNS_MODULE_PRI_DSPS_PLAYBACK,
                                 &sns_playback_stk[0],
                                 SNS_MODULE_STK_SIZE_DSPS_PLAYBACK,
                                 (void *)0,
                                 OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR,
                                 (uint8_t *)"SNS_PLBK");

    while(sns_plbk_init_done == FALSE)
    {
      sns_em_sleep(100);
    }
  }

  return SNS_SUCCESS;
}

/*=========================================================================
  FUNCTION:  sns_sensors_stress_test
=========================================================================*/
/*!
    @brief
    Sensors stress test for external clients.

    @param[i]   samples    Number of samples to be streamed for each sensor combination
    @param[i]   frequency Frequency at which the sensors are to be streamed
    @return[i]   output       Stream test results bit values
    TRUE/FALSE
*/
/*=======================================================================*/
BOOLEAN sns_sensors_stress_test(uint32 samples, uint8 frequency, void *output)
{
  BOOLEAN result;

  sns_playback_init();

  plbk_stress_test = TRUE;
  stress_entries = samples;
  g_Report_id = 0;
  SamplingFreq = frequency;
  SNS_OS_MEMSET(&sns_success_cnt, 0, sizeof (sns_success_cnt));

  result = sns_plbk_stress_test ();

  if (result == TRUE)
  {
    SNS_PLBK_MSG("Stress test PASS\n");
  }
  else
  {
    SNS_PLBK_MSG("Stress test FAIL\n");
  }

  SNS_OS_MEMCOPY(output, &plbk_steam_test_results, sizeof (plbk_strm_res_s));

  return result;
}


/*=========================================================================
  FUNCTION:  sns_sensors_device_test
=========================================================================*/
/*!
    @brief
    Sensors device test.
    1. At present it does only Self-Test.

    TODO : rest of the Tests

    @return
    None
*/
/*=======================================================================*/
void sns_sensors_device_test(void)
{
  uint8 index;

  while (plbk_sns_dev_test == TRUE)
  {
    plbk_sns_dev_test = FALSE;

    for (index = 0; index < SNS_PLBK_SENSORS_DEFINED; index++)
    {
      sns_plbk_smgr_sns_single_sns_test (SNS_SMGR_TEST_SELF_V01, TRUE, TRUE,
                                           sensor_id[index], SNS_SMGR_DATA_TYPE_PRIMARY_V01);
      sns_em_sleep(40000);
    }
    sns_plbk_smgr_sns_single_sns_test (SNS_SMGR_TEST_SELF_V01, TRUE, TRUE,
                                       sensor_id[SNS_PLBK_SENSORS_DEFINED - 1], SNS_SMGR_DATA_TYPE_SECONDARY_V01);
    sns_em_sleep(40000);
  }
}

/*=========================================================================
  FUNCTION:  sns_sensors_calibration_test
=========================================================================*/
/*!
    @brief
    Sensors calibration test.

    @return
    None
*/
/*=======================================================================*/
void sns_sensors_calibration_test (void)
{
  uint8 index;

  while (plbk_sns_cal_test == TRUE)
  {
    plbk_sns_cal_test = FALSE;

    for (index = 0; index < SNS_PLBK_SENSORS_DEFINED; index++)
    {
      sns_plbk_smgr_sns_calibration_test (SNS_SMGR_CAL_DYNAMIC_V01, sensor_id[index],
                                          SNS_SMGR_DATA_TYPE_PRIMARY_V01);
      sns_em_sleep(20000);
    }
    sns_plbk_smgr_sns_calibration_test (SNS_SMGR_CAL_DYNAMIC_V01, sensor_id[SNS_PLBK_SENSORS_DEFINED - 1],
                                        SNS_SMGR_DATA_TYPE_SECONDARY_V01);
    sns_em_sleep(20000);
  }

}

/*=========================================================================
  FUNCTION:  sns_sensors_playback_test
=========================================================================*/
/*!
    @brief
    Playback test for sensors

    @return
    TRUE/FALSE
*/
/*=======================================================================*/
int sns_sensors_playback_test (void)
{
  int ix;
#ifdef SNS_PLBK_TIME_CONDITION
  uint32_t start_time = 0;
  uint32_t current_time = 0;
#endif

  sns_playback_init();

#ifdef BRINGUP_8974
  while (start_playback == TRUE)
  {
    sns_em_sleep(SNS_PLAYBACK_SLEEP_TIME);
  }
#endif //BRINGUP_8974

  /* start stress test */
  if (plbk_stress_test == TRUE)
  {
    stress_entries = 100;
    SamplingFreq = 100;
    if (sns_sensors_stress_test (stress_entries, SamplingFreq, &plbk_steam_test_results) == TRUE)
    {
      SNS_PLBK_MSG("Stress test PASS\n");
    }
    else
    {
      SNS_PLBK_MSG("Stress test FAIL\n");
    }
  }

  /* start SAM & SMGR test */
  if (plbk_sam_smgr_test == TRUE)
  {
    sns_plbk_sam_smgr_test();
  }

  /* start device tests */
  if (plbk_sns_dev_test == TRUE)
  {
    sns_sensors_device_test();
  }

  if (plbk_sns_cal_test == TRUE)
  {
    sns_sensors_calibration_test();
  }

#ifdef SNS_PLBK_TIME_CONDITION
  start_time = sns_em_get_timestamp();
#endif

#if !defined(VIRTIO_BRINGUP)
  /* send start message to SMGR */
  sns_plbk_smgr_sns_start_stop_stream (SamplingFreq, SNS_PLBK_REPORT_ID,
                              sensors_pattern, SNS_SMGR_REPORT_ACTION_ADD_V01);

    while (1)
    {
      /* check the message status count */
      if (sns_success_cnt[SNS_PLBK_REPORT_ID] >= SNS_SENSORS_PLAYBACK_TEST_COUNT)
      {
        /* dummy wait to finish debug messages in buffer */
        sns_em_sleep(SNS_PLAYBACK_SLEEP_TIME << 4);

        SNS_PLBK_MSG("PLAYBACK ===== SMGR test passed =====\n");
        break;
      }

#ifdef SNS_PLBK_TIME_CONDITION
      /* check for data */
      if (sns_success_cnt[SNS_PLBK_REPORT_ID] == 0)
      {
        current_time = sns_em_get_timestamp();

        if ((current_time - start_time) > PLBK_BREAK_TIME)
        {
          SNS_PLBK_MSG("Playback failed to get data return back\n");
          break;
        }
      }
#endif
      /*  wait to schedule other jobs  */
      sns_em_sleep(SNS_PLAYBACK_SLEEP_TIME);
    }

  /* send stop message to SMGR */
  sns_plbk_smgr_sns_start_stop_stream (SamplingFreq, SNS_PLBK_REPORT_ID,
                              sensors_pattern, SNS_SMGR_REPORT_ACTION_DELETE_V01);
#else

    for(ix = 0; ix < sizeof(plbk_test_case)/sizeof(plbk_test_case[0]); ix++ ) {

     if(plbk_buffer_stream) {
       sns_plbk_buffer_start_stop_stream (plbk_test_case[ix].samplingfreq,
             SNS_PLBK_REPORT_ID + ix, plbk_test_case[ix].sns_pattern,
             SNS_SMGR_BUFFERING_ACTION_ADD_V01);
     } else {
       sns_plbk_smgr_sns_start_stop_stream (plbk_test_case[ix].samplingfreq,
             SNS_PLBK_REPORT_ID + ix, plbk_test_case[ix].sns_pattern,
             SNS_SMGR_REPORT_ACTION_ADD_V01);
     }
             ULogFront_RealTimePrintf(sns_log_handle, 3,
             "PLAYBACK START:Test case=%d, RepID=%d SamplingRate %d",
             plbk_test_case[ix].sns_pattern, SNS_PLBK_REPORT_ID + ix,
             plbk_test_case[ix].samplingfreq);

      sns_em_sleep(plbk_test_case[ix].duration * 1000000);

      if(plbk_buffer_stream) {
        sns_plbk_buffer_start_stop_stream (plbk_test_case[ix].samplingfreq,
             SNS_PLBK_REPORT_ID + ix, plbk_test_case[ix].sns_pattern,
             SNS_SMGR_BUFFERING_ACTION_DELETE_V01);
      } else {
        sns_plbk_smgr_sns_start_stop_stream (plbk_test_case[ix].samplingfreq,
             SNS_PLBK_REPORT_ID + ix, plbk_test_case[ix].sns_pattern,
             SNS_SMGR_REPORT_ACTION_DELETE_V01);
      }
      ULogFront_RealTimePrintf(sns_log_handle, 2, "PLAYBACK STOP:Test case=%d, RepID=%d",
             plbk_test_case[ix].sns_pattern, SNS_PLBK_REPORT_ID + ix);

      sns_em_sleep(5000);
    }
#endif /* VIRTIO_BRINGUP */

#if SNS_PLBK_SAM_CLNT
  /* send ENABLE message to SAM */
  sns_plbk_sam_amd_enable_test (SNS_PLBK_SAM_DEF_CLIENT_REPORT_PERIOD);

  sns_em_sleep(3000000);

  /* print QMD status */
  if (sns_sam_qmd_report_count > 0)
  {
    SNS_PLBK_MSG_1("QMD success count = %lu\n", sns_sam_qmd_report_count);
  }
  else
  {
    SNS_PLBK_MSG("Playback failed to get QMD data return back\n");
  }

  /* send DISABLE message to SAM */
  sns_plbk_sam_amd_disable_test(sns_sam_instance_id);

#endif /* SNS_PLBK_SAM_CLNT */

  /* return the status */
  if (sns_success_cnt[SNS_PLBK_REPORT_ID] > 0)
    return TRUE;    /* success */
  else
    return FALSE;       /* fail */
}

