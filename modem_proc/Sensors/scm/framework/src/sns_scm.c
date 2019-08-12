/*============================================================================
  FILE: sns_scm.c

  This file contains the Sensors Calibration Manager implementation

  Copyright (c) 2011-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
 ============================================================================*/

#define __SNS_MODULE__ SNS_SCM

/*---------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "sns_scm_priv.h"
#include "gyro_cal.h"
#include "qmag_cal.h"
#include "sns_queue.h"
#include "sns_reg_common.h"
#include "sns_reg_api_v02.h"

#include "sns_init.h"
#include "sns_memmgr.h"
#include "sns_debug_str.h"
#include "fixed_point.h"
#include "sns_log_api.h"
#include "sns_log_types.h"
#include "sns_profiling.h"

#include <qmi_client.h>
#include <qmi_idl_lib_internal.h>

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

//Gryo sampling rate for gyro bias calibration
#define SNS_SCM_GYRO_CAL_SAMP_RATE_HZ   (25)
//Gyro maximum absolute bias
#define SNS_SCM_GYRO_CAL_MAX_ABS_BIAS   FX_FLTTOFIX_Q16(0.4363)

//QMD instance id when request is pending
#define SNS_SCM_QMD_PEND_ID             (0xFE)

#define SNS_SCM_MAG_CAL
#define SCM_MAX_IND_Q_LEN 30
/*---------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
sns_q_s scm_reg_q;
OS_EVENT *scm_mutex;

scm_qmi_client_if_s        scm_smgr_qmi_client_if;
scm_qmi_client_if_s        scm_smgr_int_qmi_client_if;

#ifndef ADSP_STANDALONE
scm_qmi_client_if_s        scm_reg_qmi_client_if;
#endif /* ADSP_STANDALONE */

/*---------------------------------------------------------------------------
 * STATIC Variable Definitions
 * -------------------------------------------------------------------------*/
#if defined(SNS_PLAYBACK_SKIP_SMGR) || defined(SNS_QDSP_SIM)
STATIC int sns_scm_pb_gyro_cal_algo_svc_id=0;
STATIC uint32_t sns_scm_pb_report_timeout=1966600; // dsps clock ticks corresponding to 1 min timeout of SCM
STATIC uint32_t sns_scm_pb_next_report_time=0;
STATIC bool sns_scm_pb_update_next_report_time=false;
#endif

//SCM task stack
STATIC OS_STK sns_scm_task_stk[SNS_SCM_MODULE_STK_SIZE];

//SCM event signal
STATIC OS_FLAG_GRP *sns_scm_sig_event;

//algorithm database
STATIC sns_scm_algo_s* sns_scm_algo_dbase[SNS_SCM_NUM_ALGO_SVCS];

//algorithm instance database
STATIC sns_scm_algo_inst_s* sns_scm_algo_inst_dbase[SNS_SCM_MAX_ALGO_INSTS];
STATIC uint8_t sns_scm_algo_inst_count;

//sensor data request database
STATIC sns_scm_data_req_s* sns_scm_data_req_dbase[SNS_SCM_MAX_DATA_REQS];
STATIC uint8_t sns_scm_data_req_count;

//sensor database
STATIC sns_scm_sensor_s sns_scm_sensor_dbase[SNS_SCM_MAX_SNS_MON];

STATIC scm_motion_state_s scm_qmd_state;

// QMD motion state related information
STATIC sns_q_s                    scm_sam_smgr_q;

STATIC scm_qmi_client_if_s        scm_sam_qmi_client_if;

//SMGR Buffering Support flag
STATIC bool sns_scm_smgr_buffering_flag = false;
STATIC bool sns_scm_sensor_report_rate_available = false;

/*---------------------------------------------------------------------------
 * Function Definitions
 * -------------------------------------------------------------------------*/

/*=========================================================================
  FUNCTION:  sns_scm_qmi_client_send_msg_async
  =========================================================================*/
/*!
  @brief    Wrapper function to qmi_client_send_msg_async to prevent memory
            leaks

  @param[i]   user_handle:         Handle used by the infrastructure to identify different clients.
  @param[i]   msg_id:              Message ID
  @param[i]   req_c_struct:        Pointer to the request parameters
  @param[i]   req_c_struct_len:    Length of the request buffer
  @param[i]   resp_c_struct:       Pointer to the response parameters
  @param[i]   resp_c_struct_len:   Length of the response buffer
  @param[i]   resp_cb:             Callback function to handle response parameters
  @param[i]   resp_cb_data:        Callback user data
  @param[i]   txn_handle:          Handle used to identify the transaction

  @return   QMI error type
*/
/*=======================================================================*/
qmi_client_error_type sns_scm_qmi_client_send_msg_async (
   qmi_client_type                 user_handle,
   unsigned int                    msg_id,
   void                            *req_c_struct,
   unsigned int                    req_c_struct_len,
   void                            *resp_c_struct,
   unsigned int                    resp_c_struct_len,
   qmi_client_recv_msg_async_cb    resp_cb,
   void                            *resp_cb_data,
   qmi_txn_handle                  *txn_handle )
{
   qmi_client_error_type qmi_err;

   qmi_err = qmi_client_send_msg_async(user_handle, msg_id,
                                      req_c_struct, req_c_struct_len,
                                      resp_c_struct, resp_c_struct_len,
                                      resp_cb, resp_cb_data, txn_handle);

   if (QMI_NO_ERR != qmi_err)
   {
      /* If QMI returns an error, free response struct and callback data to prevent memory leaks */
      if (resp_c_struct)
      {
         SNS_OS_FREE(resp_c_struct);
      }

      if (resp_cb_data)
      {
         SNS_OS_FREE(resp_cb_data);
      }

      SNS_SCM_PRINTF1(ERROR, "error in sns_scm_qmi_client_send_msg_async: QMI error:%d", qmi_err);
   }

   if (req_c_struct)
   {
      SNS_OS_FREE(req_c_struct);
   }

   return qmi_err;
}

/*=========================================================================
  FUNCTION:  sns_scm_sensor_report_rate_available
  =========================================================================*/
/*!
  @brief    Detects sensor report rate availability in Registry

  @return   'true' if sensor report rate is expected to be available;
            'false' otherwise.
*/
/*=======================================================================*/
bool sns_scm_is_sensor_report_rate_available()
{
   return sns_scm_sensor_report_rate_available;
}

/*=========================================================================
  FUNCTION:  sns_scm_sensor_status_hyst_timer_cb
  =========================================================================*/
/*!
  @brief Callback registered for sensor status timer expiry

  @param[i] argPtr: pointer to argument for callback function

  @return None
*/
/*=======================================================================*/
STATIC void sns_scm_sensor_status_hyst_timer_cb(
   void *argPtr)
{
   uint8_t err;
   uint32_t sensorIndex = (uint32_t)(uintptr_t)argPtr;

   if (sensorIndex < SNS_SCM_MAX_SNS_MON)
   {
      sns_scm_sensor_dbase[sensorIndex].timeout = true;

      sns_os_sigs_post(sns_scm_sig_event,
                       SNS_SCM_SENSOR_STATUS_HYST_TIMER_SIG,
                       OS_FLAG_SET,
                       &err);
      if (err != OS_ERR_NONE)
      {
         SNS_SCM_PRINTF1(ERROR, "sensor status timer callback err:%d",err);
      }
   }
}

/*=========================================================================
  FUNCTION:  sns_scm_timer_cb
  =========================================================================*/
/*!
  @brief Callback registered for timer expiry

  @param[i] argPtr: pointer to argument for callback function

  @return None
*/
/*=======================================================================*/
STATIC void sns_scm_timer_cb(
   void *argPtr)
{
   uint8_t err;
   uint32_t algoIndex = (uint32_t)(uintptr_t)argPtr;

   if (algoIndex < SNS_SCM_NUM_ALGO_SVCS &&
       sns_scm_algo_dbase[algoIndex] != NULL)
   {
      sns_scm_algo_dbase[algoIndex]->timeout = true;

      sns_os_sigs_post(sns_scm_sig_event,
                       SNS_SCM_REPORT_TIMER_SIG,
                       OS_FLAG_SET,
                       &err);
      if (err != OS_ERR_NONE)
      {
         SNS_SCM_DEBUG1(ERROR, DBG_SCM_TIMER_CB_SIGNALERR, err);
      }
   }
}

/*=========================================================================
  FUNCTION:  sns_scm_reg_sensor_status_hyst_timer
  =========================================================================*/
/*!
  @brief Register timer for sensor status hysteresis

  @param[i] sensorIndex: Id of sensor in database

  @return Sensors error code
*/
/*=======================================================================*/
STATIC sns_err_code_e sns_scm_reg_sensor_status_hyst_timer(
   uint32_t sensorIndex)
{
   sns_err_code_e err;
   sns_scm_sensor_s* sensorPtr = &sns_scm_sensor_dbase[sensorIndex];

   if (sensorPtr->timer == NULL)
   {
      err = sns_em_create_timer_obj(sns_scm_sensor_status_hyst_timer_cb,
                                    (void*)(intptr_t)sensorIndex,
                                    SNS_EM_TIMER_TYPE_ONESHOT,
                                    &(sensorPtr->timer));
   }
   else
   {
      err = sns_em_cancel_timer(sensorPtr->timer);
   }

#if !defined(SNS_PLAYBACK_SKIP_SMGR) && !defined(SNS_QDSP_SIM)
   if (err == SNS_SUCCESS)
   {
      err = sns_em_register_timer(sensorPtr->timer,
                                  sns_em_convert_usec_to_localtick(SNS_SCM_SENSOR_STATUS_HYST_PERIOD_USEC));
      if (err == SNS_SUCCESS)
      {
         SNS_SCM_PRINTF2(MEDIUM, "sensor status hysteresis timer started sensorId:%d, period:%d",
                         sensorPtr->sensorId,SNS_SCM_SENSOR_STATUS_HYST_PERIOD_USEC);
      }
      else
      {
         SNS_SCM_PRINTF3(ERROR, "sensor status hysteresis timer start error sensorId:%d, period:%d, err:%d",
                         sensorPtr->sensorId,SNS_SCM_SENSOR_STATUS_HYST_PERIOD_USEC,err);
      }
   }
#endif

   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_reg_timer
  =========================================================================*/
/*!
  @brief Register timer for client reports

  @param[i] algoIndex: algorithm instance id

  @return Sensors error code
*/
/*=======================================================================*/
STATIC sns_err_code_e sns_scm_reg_timer(
   uint32_t algoIndex)
{
   sns_err_code_e err = SNS_SUCCESS;
   sns_scm_algo_s* algoPtr = sns_scm_algo_dbase[algoIndex];

   if(algoPtr->period == 0)
   {
      // timer disabled
      return SNS_SUCCESS;
   }

   if (algoPtr->timer == NULL)
   {
      err = sns_em_create_timer_obj(sns_scm_timer_cb,
                                    (void*)(intptr_t)algoIndex,
                                    SNS_EM_TIMER_TYPE_PERIODIC,
                                    &(algoPtr->timer));
   }

#if !defined(SNS_PLAYBACK_SKIP_SMGR) && !defined(SNS_QDSP_SIM)
   if (err == SNS_SUCCESS)
   {
      err = sns_em_register_timer(algoPtr->timer,
                                  algoPtr->period);
      if (err == SNS_SUCCESS)
      {
         SNS_SCM_DEBUG2(MEDIUM, DBG_SCM_REG_TIMER_STARTED,
                        algoIndex, algoPtr->period);
      }
      else
      {
         SNS_SCM_DEBUG1(ERROR, DBG_SCM_REG_TIMER_FAILED, err);
      }
   }
#endif

   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_dereg_timer
  =========================================================================*/
/*!
  @brief Deregister timer

  @param[i] algoIndex: client request id

  @return Sensors error code
*/
/*=======================================================================*/
STATIC sns_err_code_e sns_scm_dereg_timer(
   uint8_t algoIndex)
{
   sns_err_code_e err;

   if (sns_scm_algo_dbase[algoIndex]->timer == NULL)
   {
      return SNS_SUCCESS;
   }


   err = sns_em_delete_timer_obj(sns_scm_algo_dbase[algoIndex]->timer);
   if (err == SNS_SUCCESS)
   {
      sns_scm_algo_dbase[algoIndex]->timer = NULL;
      SNS_SCM_DEBUG1(MEDIUM, DBG_SCM_DEREG_TIMER_DELETED, sns_em_get_timestamp());
   }
   else
   {
      SNS_SCM_DEBUG1(ERROR, DBG_SCM_DEREG_TIMER_FAILED, err);
   }

   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_find_data_req
  =========================================================================*/
/*!
  @brief Searches the active sensor data request database for an instance
  with the same sensor id and report rate

  @param[i] algoInstId: algorithm instance id
  @param[i] reportRate: report generation rate
  @param[i] sensorReqCnt: number of sensors in data request
  @param[i] sensorReq: sensors for which data is requested

  @return  Index to algorithm instance matching the specified configuration
  SNS_SCM_INVALID_ID if match is not found
*/
/*=======================================================================*/
STATIC uint8_t sns_scm_find_data_req(
   uint8_t algoInstId,
   uint32_t reportRate,
   uint8_t sensorReqCnt,
   sns_scm_sensor_req_s sensorReq[])
{
   uint8_t dataReqId, dataReqCnt;

   for (dataReqId = 0, dataReqCnt = 0;
       dataReqCnt < sns_scm_data_req_count &&
       dataReqId < SNS_SCM_MAX_DATA_REQS;
       dataReqId++)
   {
      if (sns_scm_data_req_dbase[dataReqId] != NULL)
      {
         sns_scm_data_req_s *dataReq = sns_scm_data_req_dbase[dataReqId];

         if (dataReq->sensorCount == sensorReqCnt &&
             dataReq->reportRate == reportRate)
         {
            uint8_t i, j;
            for (j = 0; j < sensorReqCnt; j++)
            {
               //ordered match done on requests
               if (dataReq->sensorDbase[j].sensorId != sensorReq[j].sensorId ||
                   dataReq->sensorDbase[j].dataType != sensorReq[j].dataType ||
                   dataReq->sensorDbase[j].sampleRate != sensorReq[j].sampleRate ||
                   dataReq->sensorDbase[j].sampleQual != sensorReq[j].sampleQual)
               {
                  break;
               }
            }

            //Found matching request
            if (j >= sensorReqCnt)
            {
               //Avoid duplicate
               for (i = 0; i < dataReq->algoInstCount; i++)
               {
                  if (dataReq->algoInstDbase[i] == algoInstId)
                  {
                     return dataReqId;
                  }
               }

               //Add request
               if (i < SNS_SCM_MAX_ALGO_INSTS_PER_DATA_REQ)
               {
                  dataReq->algoInstDbase[i] = algoInstId;
                  dataReq->algoInstCount++;
                  return dataReqId;
               }
            }
         }

         dataReqCnt++;
      }
   }

   return SNS_SCM_INVALID_ID;
}

/*=========================================================================
  FUNCTION:  sns_scm_send_qmd_start_req
  =========================================================================*/
/*!
  @brief Send a request to sensors algorithm manager for QMD

  @return Sensors error code
*/
/*=======================================================================*/
STATIC sns_err_code_e sns_scm_send_qmd_start_req(void)
{
   qmi_client_error_type           err;
   sns_sam_qmd_enable_req_msg_v01  *msgReqP;
   sns_sam_qmd_enable_resp_msg_v01 *msgRespP;
   qmi_txn_handle                  txn_handle;

   msgReqP = (sns_sam_qmd_enable_req_msg_v01 *)
             SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_sam_qmd_enable_req_msg_v01));
   SNS_ASSERT(msgReqP != NULL);

   msgRespP = (sns_sam_qmd_enable_resp_msg_v01 *)
              SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_sam_qmd_enable_resp_msg_v01));
   SNS_ASSERT(msgRespP != NULL);

   msgReqP->report_period = 0;  //async report
   msgReqP->config_valid = 0;
   msgReqP->notify_suspend_valid = true;
   msgReqP->notify_suspend.proc_type = SNS_PROC_SSC_V01;
   msgReqP->notify_suspend.send_indications_during_suspend = true;

   err = sns_scm_qmi_client_send_msg_async (scm_sam_qmi_client_if.user_handle,
                                    SNS_SAM_AMD_ENABLE_REQ_V01,
                                    msgReqP, sizeof(sns_sam_qmd_enable_req_msg_v01),
                                    msgRespP, sizeof(sns_sam_qmd_enable_resp_msg_v01),
                                    sns_scm_sam_resp_cb, NULL, &txn_handle);

   //avoid duplicate qmd start requests
   scm_qmd_state.qmd_enabled = true;

   SNS_SCM_DEBUG3(MEDIUM, DBG_SCM_REQ_ALGO_SVC,
                  SNS_SAM_AMD_SVC_ID_V01,
                  SNS_SAM_AMD_ENABLE_REQ_V01,
                  err);

   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_send_qmd_stop_req
  =========================================================================*/
/*!
  @brief Send a request to sensors algorithm manager for QMD

  @return None
*/
/*=======================================================================*/
STATIC void sns_scm_send_qmd_stop_req(void)
{
   qmi_client_error_type            err;
   sns_sam_qmd_disable_req_msg_v01  *msgReqP;
   sns_sam_qmd_disable_resp_msg_v01 *msgRespP;
   qmi_txn_handle                   txn_handle;

   msgReqP = (sns_sam_qmd_disable_req_msg_v01 *)
             SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_sam_qmd_disable_req_msg_v01));
   SNS_ASSERT(msgReqP != NULL);

   msgRespP = (sns_sam_qmd_disable_resp_msg_v01 *)
              SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_sam_qmd_disable_resp_msg_v01));
   SNS_ASSERT(msgRespP != NULL);

   msgReqP->instance_id = scm_qmd_state.qmd_inst_id;

   err = sns_scm_qmi_client_send_msg_async (scm_sam_qmi_client_if.user_handle,
                                    SNS_SAM_AMD_DISABLE_REQ_V01,
                                    msgReqP, sizeof(sns_sam_qmd_disable_req_msg_v01),
                                    msgRespP, sizeof(sns_sam_qmd_disable_resp_msg_v01),
                                    sns_scm_sam_resp_cb, NULL, &txn_handle);

   scm_qmd_state.qmd_state = SNS_SAM_MOTION_UNKNOWN_V01;

   SNS_SCM_DEBUG3(MEDIUM, DBG_SCM_REQ_ALGO_SVC,
                  SNS_SAM_AMD_SVC_ID_V01,
                  SNS_SAM_AMD_DISABLE_REQ_V01,
                  err);
}

/*=========================================================================
  FUNCTION:  sns_scm_get_smgr_msg_req_type
  =========================================================================*/
/*!
  @brief  Determines the type of message request used to communicate with
          Sensor Manager.

  @param[i] algoSvcId: algorithm service ID

  @return  message request ID
*/
/*=======================================================================*/
uint8_t sns_scm_get_smgr_msg_req_type(
   uint8_t algoSvcId)
{
   uint8_t buff_flag = SNS_SMGR_REPORT_REQ_V01;  // default

   if(!sns_scm_smgr_buffering_flag)
   {
      SNS_SCM_PRINTF0(MED, "SMGR does not support buffering" );
   }
   else
   {
      if(algoSvcId == SNS_SCM_GYRO_CAL_SVC )
      {
         sns_scm_algo_s* algoPtr = sns_scm_get_algo_handle(algoSvcId);
         if( algoPtr != NULL && algoPtr->defSensorReportRate != 0 )
         {
            buff_flag =  SNS_SMGR_BUFFERING_REQ_V01;
         }
      }
      else if(algoSvcId == SNS_SCM_MAG_CAL_SVC )
      {
         /* Always use buffering */
         buff_flag =  SNS_SMGR_BUFFERING_REQ_V01;
      }
   }

   SNS_SCM_PRINTF1(MED, "Returning buff flag %d", buff_flag);
   return (buff_flag);
}

/*=========================================================================
  FUNCTION:  sns_scm_send_smgr_start_req
  =========================================================================*/
/*!
  @brief Send a request to sensors manager for sensor data

  @param[i] dataReqId: Index of data request in database
  @param[i] svcId: Service Id

  @return Sensors error code
*/
/*=======================================================================*/
STATIC sns_err_code_e sns_scm_send_smgr_start_req(
   uint8_t dataReqId,
   uint8_t svcId)
{
   if(sns_scm_get_smgr_msg_req_type(svcId) == SNS_SMGR_BUFFERING_REQ_V01)
   {
      sns_smgr_buffering_req_msg_v01 *msgPtr = NULL;
      uint8_t i;
      qmi_client_error_type err;
      sns_smgr_buffering_resp_msg_v01 *msgRespP = NULL;
      qmi_txn_handle txn_handle;

      msgPtr   = (sns_smgr_buffering_req_msg_v01 *)
                 SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_smgr_buffering_req_msg_v01));
      SNS_ASSERT(msgPtr != NULL);

      msgRespP = (sns_smgr_buffering_resp_msg_v01 *)
                 SNS_OS_MALLOC (SNS_SCM_DBG_MOD, sizeof(sns_smgr_buffering_resp_msg_v01));
      SNS_ASSERT(msgRespP != NULL);

      msgPtr->ReportId = dataReqId;
      msgPtr->ReportRate = sns_scm_data_req_dbase[dataReqId]->reportRate;
      msgPtr->Action = SNS_SMGR_BUFFERING_ACTION_ADD_V01;
      msgPtr->Item_len = sns_scm_data_req_dbase[dataReqId]->sensorCount;
      for(i = 0; i < msgPtr->Item_len; i++)
      {
         msgPtr->Item[i].SensorId =
            sns_scm_data_req_dbase[dataReqId]->sensorDbase[i].sensorId;
         msgPtr->Item[i].DataType =
            sns_scm_data_req_dbase[dataReqId]->sensorDbase[i].dataType;
         msgPtr->Item[i].SamplingRate =
            sns_scm_data_req_dbase[dataReqId]->sensorDbase[i].sampleRate;
         msgPtr->Item[i].SampleQuality =
            sns_scm_data_req_dbase[dataReqId]->sensorDbase[i].sampleQual;
         msgPtr->Item[i].Decimation =
            sns_scm_data_req_dbase[dataReqId]->sensorDbase[i].decimationType;
         msgPtr->Item[i].Calibration = SNS_SMGR_CAL_SEL_FACTORY_CAL_V01;
      }

      msgPtr->notify_suspend_valid = false;
      msgPtr->SrcModule_valid = true;
      msgPtr->SrcModule = SNS_SCM_MODULE;

      err = sns_scm_qmi_client_send_msg_async (scm_smgr_qmi_client_if.user_handle,
                                       SNS_SMGR_BUFFERING_REQ_V01,
                                       msgPtr, sizeof(sns_smgr_buffering_req_msg_v01),
                                       msgRespP, sizeof(sns_smgr_buffering_resp_msg_v01),
                                       sns_scm_smgr_resp_cb, NULL, &txn_handle);

      SNS_SCM_PRINTF3(MEDIUM, "Sensor Buffering data req %d to SMGR for %d sensors for algo SVC %d",
                     dataReqId,
                     sns_scm_data_req_dbase[dataReqId]->sensorCount,
                     svcId);

      return err;
   }
   else
   {
      sns_smgr_periodic_report_req_msg_v01 *msgPtr = NULL;
      uint8_t i;
      qmi_client_error_type                 err;
      sns_smgr_periodic_report_resp_msg_v01 *msgRespP = NULL;
      qmi_txn_handle                        txn_handle;

      msgPtr   = (sns_smgr_periodic_report_req_msg_v01 *)
                 SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_smgr_periodic_report_req_msg_v01));
      SNS_ASSERT(msgPtr != NULL);

      msgRespP = (sns_smgr_periodic_report_resp_msg_v01 *)
                 SNS_OS_MALLOC (SNS_SCM_DBG_MOD, sizeof(sns_smgr_periodic_report_resp_msg_v01));
      SNS_ASSERT(msgRespP != NULL);

      msgPtr->BufferFactor = 1;

      msgPtr->ReportId = dataReqId;
      msgPtr->Action = SNS_SMGR_REPORT_ACTION_ADD_V01;

      msgPtr->ReportRate = sns_scm_data_req_dbase[dataReqId]->reportRate;

      msgPtr->cal_sel_valid = true;
      msgPtr->cal_sel_len = sns_scm_data_req_dbase[dataReqId]->sensorCount;

      msgPtr->Item_len = sns_scm_data_req_dbase[dataReqId]->sensorCount;
      for (i = 0; i < sns_scm_data_req_dbase[dataReqId]->sensorCount; i++)
      {
         msgPtr->Item[i].SensorId =
         sns_scm_data_req_dbase[dataReqId]->sensorDbase[i].sensorId;
         msgPtr->Item[i].DataType =
         sns_scm_data_req_dbase[dataReqId]->sensorDbase[i].dataType;

         msgPtr->Item[i].Decimation =
            sns_scm_data_req_dbase[dataReqId]->sensorDbase[i].decimationType;

         msgPtr->cal_sel[i] = SNS_SMGR_CAL_SEL_FACTORY_CAL_V01;
      }

      msgPtr->notify_suspend_valid = false;
      msgPtr->SrcModule_valid = true;
      msgPtr->SrcModule = SNS_SCM_MODULE;

      err = sns_scm_qmi_client_send_msg_async (scm_smgr_qmi_client_if.user_handle,
                                       SNS_SMGR_REPORT_REQ_V01,
                                       msgPtr, sizeof(sns_smgr_periodic_report_req_msg_v01),
                                       msgRespP, sizeof(sns_smgr_periodic_report_resp_msg_v01),
                                       sns_scm_smgr_resp_cb, NULL, &txn_handle);

      SNS_SCM_PRINTF3(MEDIUM, "Sensor Periodic data req %d to SMGR for %d sensors for algo SVC %d",
                     dataReqId,
                     sns_scm_data_req_dbase[dataReqId]->sensorCount,
                     sns_scm_data_req_dbase[dataReqId]->sensorDbase[0].sensorId);

      return err;
   }
}

/*=========================================================================
  FUNCTION:  sns_scm_find_max_sample_rate
  =========================================================================*/
/*!
  @brief Finds max sample rate in a data request

  @param[i] dataReqId: Index of data request in data request database

  @return sample rate in Hz (Q16 format)
*/
/*=======================================================================*/
STATIC int32_t sns_scm_find_max_sample_rate(
   const uint8_t dataReqId)
{
   int32_t maxSampleRate = 0;

   if( dataReqId < SNS_SCM_MAX_DATA_REQS &&
       sns_scm_data_req_dbase[dataReqId] != NULL )
   {
      int i;
      for( i = 0; i < sns_scm_data_req_dbase[dataReqId]->sensorCount; i++ )
      {
         if( sns_scm_data_req_dbase[dataReqId]->sensorDbase[i].sampleRate >
             maxSampleRate )
         {
            maxSampleRate =
               sns_scm_data_req_dbase[dataReqId]->sensorDbase[i].sampleRate;
         }
      }
   }
   return FX_CONV_Q16(maxSampleRate,0);
}

/*=========================================================================
  FUNCTION:  sns_scm_switch_smgr_data_req
  =========================================================================*/
/*!
  @brief Switch between buffering and periodic data requests to SMGR

  @param[i] dataReqId: Index of data request in data request database

  @return Sensors error code
*/
/*=======================================================================*/
STATIC sns_err_code_e sns_scm_switch_smgr_data_req(
   const uint8_t dataReqId)
{
   sns_err_code_e err = SNS_ERR_FAILED;
   if( dataReqId < SNS_SCM_MAX_DATA_REQS &&
       sns_scm_data_req_dbase[dataReqId] != NULL &&
       sns_scm_data_req_dbase[dataReqId]->algoInstCount > 0 )
   {
      uint8_t algoInstId, svcId = 0;

      sns_scm_data_req_dbase[dataReqId]->reportRate =
          sns_scm_find_max_sample_rate(dataReqId);

      // Find one service associated with this data request
      algoInstId = sns_scm_data_req_dbase[dataReqId]->algoInstDbase[0];
      if( algoInstId < SNS_SCM_MAX_ALGO_INSTS &&
          sns_scm_algo_inst_dbase[algoInstId] != NULL )
      {
         sns_scm_algo_inst_s * algoInstPtr = sns_scm_algo_inst_dbase[algoInstId];
         svcId = sns_scm_algo_dbase[algoInstPtr->algoIndex]->serviceId;

         // Send request to switch to new message type
         SNS_SCM_PRINTF2(MED, "Switching data req %d for service %d", dataReqId, svcId);
         err = sns_scm_send_smgr_start_req( dataReqId, svcId );
      }
   }
   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_send_smgr_stop_req
  =========================================================================*/
/*!
  @brief Send a request to sensors manager to stop sensor data received by
  specified algorithm

  @param[i] dataReqId: Index of data request in database
  @param[i] svcId: Service Id

  @return None
*/
/*=======================================================================*/
STATIC void sns_scm_send_smgr_stop_req(
   uint8_t dataReqId,
   uint8_t svcId)
{
   if(sns_scm_get_smgr_msg_req_type(svcId) == SNS_SMGR_BUFFERING_REQ_V01)
   {
      sns_smgr_buffering_req_msg_v01  *msgReqP = NULL;
      sns_smgr_buffering_resp_msg_v01 *msgRespP = NULL;
      qmi_txn_handle                   txn_handle;

      msgReqP  = (sns_smgr_buffering_req_msg_v01 *)
                 SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_smgr_buffering_req_msg_v01));
      SNS_ASSERT(msgReqP != NULL);

      msgRespP = (sns_smgr_buffering_resp_msg_v01 *)
                 SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_smgr_buffering_resp_msg_v01));
      SNS_ASSERT(msgRespP != NULL);

      msgReqP->ReportId = dataReqId;
      msgReqP->Action = SNS_SMGR_BUFFERING_ACTION_DELETE_V01;
      msgReqP->Item_len = 0;
      msgReqP->notify_suspend_valid = false;
      msgReqP->SrcModule_valid = true;
      msgReqP->SrcModule = SNS_SCM_MODULE;

      sns_scm_qmi_client_send_msg_async (scm_smgr_qmi_client_if.user_handle,
                                       SNS_SMGR_BUFFERING_REQ_V01,
                                       msgReqP, sizeof(sns_smgr_buffering_req_msg_v01),
                                       msgRespP, sizeof(sns_smgr_buffering_resp_msg_v01),
                                       sns_scm_smgr_resp_cb, NULL, &txn_handle);

      SNS_SCM_DEBUG3(MEDIUM, DBG_SCM_STOP_SNSR_DATA,
                     dataReqId,
                     sns_scm_data_req_dbase[dataReqId]->sensorCount,
                     sns_scm_data_req_dbase[dataReqId]->sensorDbase[0].sensorId);
   }
   else
   {
      sns_smgr_periodic_report_req_msg_v01  *msgReqP = NULL;
      sns_smgr_periodic_report_resp_msg_v01 *msgRespP = NULL;
      qmi_txn_handle                        txn_handle;

      msgReqP  = (sns_smgr_periodic_report_req_msg_v01 *)
                 SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_smgr_periodic_report_req_msg_v01));
      SNS_ASSERT(msgReqP != NULL);

      msgRespP = (sns_smgr_periodic_report_resp_msg_v01 *)
                 SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_smgr_periodic_report_resp_msg_v01));
      SNS_ASSERT(msgRespP != NULL);

      msgReqP->Item_len = sns_scm_data_req_dbase[dataReqId]->sensorCount;
      msgReqP->BufferFactor = 1;

      msgReqP->ReportId = dataReqId;
      msgReqP->Action = SNS_SMGR_REPORT_ACTION_DELETE_V01;
      msgReqP->SrcModule_valid = true;
      msgReqP->SrcModule = SNS_SCM_MODULE;

      sns_scm_qmi_client_send_msg_async (scm_smgr_qmi_client_if.user_handle,
                                       SNS_SMGR_REPORT_REQ_V01,
                                       msgReqP, sizeof(sns_smgr_periodic_report_req_msg_v01),
                                       msgRespP, sizeof(sns_smgr_periodic_report_resp_msg_v01),
                                       sns_scm_smgr_resp_cb, NULL, &txn_handle);

      SNS_SCM_DEBUG3(MEDIUM, DBG_SCM_STOP_SNSR_DATA,
                     dataReqId,
                     sns_scm_data_req_dbase[dataReqId]->sensorCount,
                     sns_scm_data_req_dbase[dataReqId]->sensorDbase[0].sensorId);
   }
}

/*=========================================================================
  FUNCTION:  sns_scm_start_sensor_data
  =========================================================================*/
/*!
  @brief Request sensor data according to the algorithm needs

  @param[i] algoInstId: algorithm instance id
  @param[i] reportRate: report generation rate
  @param[i] sensorReqCnt: number of sensors in data request
  @param[i] sensorReq: sensors for which data is requested

  @return Sensors error code
*/
/*=======================================================================*/
STATIC sns_err_code_e sns_scm_start_sensor_data(
   uint8_t algoInstId,
   uint32_t reportRate,
   uint8_t sensorReqCnt,
   sns_scm_sensor_req_s sensorIndex[])
{
   sns_err_code_e err = SNS_ERR_FAILED;
   uint8_t dataReqId, i;
   sns_scm_algo_inst_s * algoInstPtr = sns_scm_algo_inst_dbase[algoInstId];

   //check if there is an identical existing request
   dataReqId = sns_scm_find_data_req(algoInstId,
                                     reportRate,
                                     sensorReqCnt,
                                     sensorIndex);

   //add request if none exists
   if (dataReqId >= SNS_SCM_MAX_DATA_REQS)
   {
      if (sns_scm_data_req_count >= SNS_SCM_MAX_DATA_REQS)
      {
         return err;
      }

      for (dataReqId = 0; dataReqId < SNS_SCM_MAX_DATA_REQS; dataReqId++)
      {
         if (sns_scm_data_req_dbase[dataReqId] == NULL)
         {
            sns_scm_data_req_dbase[dataReqId] =
            SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_scm_data_req_s));
            SNS_ASSERT(sns_scm_data_req_dbase[dataReqId] != NULL);

            for (i = 0; i < SNS_SCM_MAX_ALGO_INSTS_PER_DATA_REQ; i++)
            {
               sns_scm_data_req_dbase[dataReqId]->algoInstDbase[i] =
               SNS_SCM_INVALID_ID;
            }
            sns_scm_data_req_dbase[dataReqId]->reportRate = reportRate;
            for (i = 0; i < sensorReqCnt; i++)
            {
               sns_scm_data_req_dbase[dataReqId]->sensorDbase[i].sensorId = sensorIndex[i].sensorId;
               sns_scm_data_req_dbase[dataReqId]->sensorDbase[i].dataType = sensorIndex[i].dataType;
               sns_scm_data_req_dbase[dataReqId]->sensorDbase[i].sampleRate = sensorIndex[i].sampleRate;
               sns_scm_data_req_dbase[dataReqId]->sensorDbase[i].sampleQual = sensorIndex[i].sampleQual;
               sns_scm_data_req_dbase[dataReqId]->sensorDbase[i].decimationType = sensorIndex[i].decimationType;
            }
            sns_scm_data_req_dbase[dataReqId]->sensorCount = sensorReqCnt;
            sns_scm_data_req_dbase[dataReqId]->algoInstDbase[0] = algoInstId;
            sns_scm_data_req_dbase[dataReqId]->algoInstCount = 1;

            //send message to sensors manager requesting required sensor data
            err = sns_scm_send_smgr_start_req(dataReqId,
                sns_scm_algo_dbase[algoInstPtr->algoIndex]->serviceId);

            sns_scm_data_req_count++;

            break;
         }
      }
   }
   else
   {
      SNS_SCM_PRINTF0(LOW, "Request already exists, not adding a new one");
      err = SNS_SUCCESS;
   }

   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_stop_sensor_data
  =========================================================================*/
/*!
  @brief Stop sensor data received for specified algorithm instance

  @param[i] algoInstId: Index of algorithm instance in database

  @return None
*/
/*=======================================================================*/
STATIC void sns_scm_stop_sensor_data(
   uint8_t algoInstId)
{
   uint8_t dataReqId, dataReqCnt, i;

   for (dataReqId = 0, dataReqCnt = 0;
       dataReqCnt < sns_scm_data_req_count &&
       dataReqId < SNS_SCM_MAX_DATA_REQS;
       dataReqId++)
   {
      if (sns_scm_data_req_dbase[dataReqId] != NULL)
      {
         sns_scm_data_req_s *dataReq = sns_scm_data_req_dbase[dataReqId];
         for (i = 0; i < dataReq->algoInstCount; i++)
         {
            if (dataReq->algoInstDbase[i] == algoInstId)
            {
               dataReq->algoInstCount--;
               dataReq->algoInstDbase[i] =
               dataReq->algoInstDbase[dataReq->algoInstCount];
               dataReq->algoInstDbase[dataReq->algoInstCount] =
               SNS_SCM_INVALID_ID;

               if (dataReq->algoInstCount == 0)
               {
                  sns_scm_algo_inst_s * algoInstPtr = sns_scm_algo_inst_dbase[algoInstId];
                  sns_scm_send_smgr_stop_req(dataReqId,sns_scm_algo_dbase[algoInstPtr->algoIndex]->serviceId);
                  SNS_OS_FREE(sns_scm_data_req_dbase[dataReqId]);
                  sns_scm_data_req_dbase[dataReqId] = NULL;

                  sns_scm_data_req_count--;
                  break;
               }
            }
         }
         dataReqCnt++;
      }
   }
}

/*=========================================================================
  FUNCTION:  sns_scm_req_sensor_data
  =========================================================================*/
/*!
  @brief Request sensor data for algorithm

  @param[i] algoInstId: algorithm instance id

  @return None
*/
/*=======================================================================*/
STATIC void sns_scm_req_sensor_data(
   uint8_t algoInstId)
{
   uint8_t sensorCnt = 0;
   uint32_t reportRate = 0;
   sns_scm_sensor_req_s sensorReq[SNS_SCM_MAX_SENSORS_PER_DATA_REQ];
   uint8_t algoIndex = sns_scm_algo_inst_dbase[algoInstId]->algoIndex;
   int i;

   SNS_SCM_PRINTF2(LOW, "Requesting sensor data for algoInst %d, %d",
                   algoInstId, 0);

   for( i = 0; i < SNS_SCM_MAX_SENSORS_PER_DATA_REQ; i++ )
   {
      sensorReq[i].sampleRate = SNS_SCM_INVALID_SAMPLE_RATE;
      sensorReq[i].sampleQual = SNS_SCM_DEFAULT_SAMPLE_QUALITY;
   }

   if (sns_scm_algo_dbase[algoIndex]->serviceId == SNS_SCM_GYRO_CAL_SVC)
   {
      gyro_cal_config_s *cfgPtr = (gyro_cal_config_s *)
         (sns_scm_algo_inst_dbase[algoInstId]->configData.memPtr);

       sensorReq[0].sensorId =
          sns_scm_sensor_dbase[sns_scm_algo_dbase[algoIndex]->sensorIndex].sensorId;
      sensorReq[0].dataType =
          sns_scm_sensor_dbase[sns_scm_algo_dbase[algoIndex]->sensorIndex].dataType;
      sensorReq[0].decimationType = SNS_SMGR_DECIMATION_FILTER_V01;
      sensorCnt = 1;
      if( sns_scm_get_smgr_msg_req_type(SNS_SCM_GYRO_CAL_SVC) !=
          SNS_SMGR_BUFFERING_REQ_V01 )
      {
         reportRate = FX_CONV(cfgPtr->sampleRate, 16, 0);
      }
      else
      {
         reportRate = sns_scm_algo_dbase[algoIndex]->defSensorReportRate;
         sensorReq[0].sampleRate = FX_CONV(cfgPtr->sampleRate, 16, 0);
      }
   }
   else if (sns_scm_algo_dbase[algoIndex]->serviceId == SNS_SCM_MAG_CAL_SVC)
   {
      qmag_cal_config_s *cfgPtr = (qmag_cal_config_s *)
         (sns_scm_algo_inst_dbase[algoInstId]->configData.memPtr);

      sensorReq[0].sensorId = SNS_SMGR_ID_MAG_V01;
      sensorReq[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
      sensorReq[0].sampleRate = FX_CONV(cfgPtr->sample_rate_mag, 16, 0);
      sensorReq[0].decimationType = SNS_SMGR_DECIMATION_RECENT_SAMPLE_V01;

      sensorReq[1].sensorId = SNS_SMGR_ID_ACCEL_V01;
      sensorReq[1].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
      sensorReq[1].sampleRate = FX_CONV(cfgPtr->sample_rate_accel, 16, 0);
      sensorReq[1].decimationType = SNS_SMGR_DECIMATION_FILTER_V01;

      sensorCnt = 2;
      if( sns_scm_get_smgr_msg_req_type(SNS_SCM_MAG_CAL_SVC) !=
          SNS_SMGR_BUFFERING_REQ_V01 )
      {
         reportRate = FX_CONV(cfgPtr->sample_rate_mag, 16, 0);
      }
      else
      {
         reportRate = sns_scm_algo_dbase[algoIndex]->defSensorReportRate;
         sensorReq[0].sampleRate = FX_CONV(cfgPtr->sample_rate_mag, 16, 0);
      }
   }
   else
   {
      SNS_SCM_PRINTF2(LOW, "Cannot Handle sensor algo %d, %d",
                      sns_scm_algo_dbase[algoIndex]->serviceId, 0);

   }

   //send requests for sensor data to sensor manager
   if (sensorCnt > 0)
   {
      SNS_SCM_PRINTF2(LOW, "Requesting sensor data at %d Hz (Q16) for svc %d",
          reportRate, sns_scm_algo_dbase[algoIndex]->serviceId );
      sns_scm_start_sensor_data(algoInstId, reportRate, sensorCnt, sensorReq);
   }
}

/*=========================================================================
  FUNCTION:  sns_scm_log_gyro_cal_config
  =========================================================================*/
/*!
  @brief Log gyro calibration configuration

  @param[i] algoInstId: algorithm instance id

  @return Sensors error code
*/
/*=======================================================================*/
STATIC sns_err_code_e sns_scm_log_gyro_cal_config(
   uint8_t algoInstId)
{
   sns_err_code_e err;
   sns_log_gyro_cal_config_s* logPtr;
   gyro_cal_config_s* algoCfgPtr =
   sns_scm_algo_inst_dbase[algoInstId]->configData.memPtr;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_GYRO_CAL_CONFIG,
                           sizeof(sns_log_gyro_cal_config_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate log packet
      logPtr->version = SNS_LOG_GYRO_CAL_CONFIG_VERSION;

      logPtr->algo_inst_id = algoInstId;
      logPtr->sample_rate = algoCfgPtr->sampleRate;

      //Timestamp
      logPtr->timestamp = sns_em_get_timestamp();

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_GYRO_CAL_CONFIG, logPtr);
   }

   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_log_gyro_cal_result
  =========================================================================*/
/*!
  @brief Log gyro calibration result

  @param[i] algoInstId: algorithm instance id
  @param[i] timestamp: timestamp on the algorithm input data

  @return Sensors error code
*/
/*=======================================================================*/
STATIC sns_err_code_e sns_scm_log_gyro_cal_result(
   uint8_t algoInstId,
   uint32_t timestamp)
{
   sns_err_code_e err;
   sns_log_gyro_cal_result_s* logPtr;
   gyro_cal_input_s* algoInPtr =
   sns_scm_algo_inst_dbase[algoInstId]->inputData.memPtr;
   gyro_cal_output_s* algoOutPtr =
   sns_scm_algo_inst_dbase[algoInstId]->outputData.memPtr;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_GYRO_CAL_RESULT,
                           sizeof(sns_log_gyro_cal_result_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate log packet
      logPtr->version = SNS_LOG_GYRO_CAL_RESULT_VERSION;

      logPtr->algo_inst_id = algoInstId;

      logPtr->gyro_x = algoInPtr->data[0];
      logPtr->gyro_y = algoInPtr->data[1];
      logPtr->gyro_z = algoInPtr->data[2];
      logPtr->gyro_zero_bias_x = algoOutPtr->bias[0];
      logPtr->gyro_zero_bias_y = algoOutPtr->bias[1];
      logPtr->gyro_zero_bias_z = algoOutPtr->bias[2];

      //Timestamp
      logPtr->timestamp = timestamp;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_GYRO_CAL_RESULT, logPtr);
   }

   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_log_qmag_cal_config
  =========================================================================*/
/*!
  @brief Log mag calibration configuration

  @param[i] algoInstId: algorithm instance id

  @return Sensors error code
*/
/*=======================================================================*/
STATIC sns_err_code_e sns_scm_log_qmag_cal_config(
   uint8_t algoInstId)
{
   sns_err_code_e err;
   sns_log_qmag_cal_config_s* logPtr;
   qmag_cal_config_s* algoCfgPtr =
   sns_scm_algo_inst_dbase[algoInstId]->configData.memPtr;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_QMAG_CAL_CONFIG,
                           sizeof(sns_log_qmag_cal_config_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate log packet
      logPtr->version = SNS_LOG_QMAG_CAL_CONFIG_VERSION;
      logPtr->sample_rate_mag = algoCfgPtr->sample_rate_mag;
      logPtr->sample_rate_accel = algoCfgPtr->sample_rate_accel;

      logPtr->cal_algo_type = 0;
      logPtr->cal_cycle_rate = 2.0;
      //Timestamp
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;
      logPtr->timestamp = sns_em_get_timestamp();

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_QMAG_CAL_CONFIG, logPtr);
   }
   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_log_qmag_cal_result
  =========================================================================*/
/*!
  @brief Log mag calibration result

  @param[i] algoInstId: algorithm instance id
  @param[i] timestamp: timestamp on the algorithm input data

  @return Sensors error code
*/
/*=======================================================================*/
STATIC sns_err_code_e sns_scm_log_qmag_cal_result(
   uint8_t algoInstId,
   uint32_t timestamp)
{

   sns_err_code_e err;
   sns_log_qmag_cal_result_s* logPtr;

   qmag_cal_input_s* algoInPtr =
   sns_scm_algo_inst_dbase[algoInstId]->inputData.memPtr;

   qmag_cal_state_s* algoStatPtr =
   sns_scm_algo_inst_dbase[algoInstId]->stateData.memPtr;

   qmag_cal_output_s* algoOutPtr =
   sns_scm_algo_inst_dbase[algoInstId]->outputData.memPtr;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_QMAG_CAL_RESULT,
                           sizeof(sns_log_qmag_cal_result_s),
                           (void**)&logPtr);


   //below is yet to be changed
   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate log packet
      logPtr->version = SNS_LOG_QMAG_CAL_RESULT_VERSION;

      logPtr->input_type =  algoInPtr->input_type;

      if (algoInPtr->input_type == QMAG_CAL_INPUT_TYPE_MAG)
      {
         logPtr->input_mag_x = algoInPtr->sensor_data[0];
         logPtr->input_mag_y = algoInPtr->sensor_data[1];
         logPtr->input_mag_z = algoInPtr->sensor_data[2];
      }else if(algoInPtr->input_type == QMAG_CAL_INPUT_TYPE_ACCEL)
      {
         logPtr->input_accel_x = algoInPtr->sensor_data[0];
         logPtr->input_accel_y = algoInPtr->sensor_data[1];
         logPtr->input_accel_z = algoInPtr->sensor_data[2];
      }

      logPtr-> accel_input_variance = algoStatPtr->accel_raw_variance;
      logPtr-> mag_input_variance = algoStatPtr->mag_raw_variance;
      logPtr-> offset_HI_unfiltered_x = algoStatPtr->offset_HI[0];
      logPtr-> offset_HI_unfiltered_y = algoStatPtr->offset_HI[1];
      logPtr-> offset_HI_unfiltered_z = algoStatPtr->offset_HI[2];
      logPtr-> radius_HI  = algoStatPtr->radius_HI;
      logPtr-> unfiltered_accuracy = algoStatPtr ->accuracy_HI;
      logPtr->accuracy_published = algoStatPtr->persistent_state.published_accuracy_HI;

      logPtr->cal_algo_type = 0;
      logPtr->mag_zero_bias_x = algoOutPtr->offset_HI[0];
      logPtr->mag_zero_bias_y = algoOutPtr->offset_HI[1];
      logPtr->mag_zero_bias_z = algoOutPtr->offset_HI[2];

      logPtr->offset_time = algoOutPtr->offset_time;

      //Timestamp
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;
      logPtr->timestamp = timestamp;

      logPtr -> soft_reset = 0;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_QMAG_CAL_RESULT, logPtr);
   }

   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_log_algo_config
  =========================================================================*/
/*!
  @brief Log algorithm configuration

  @param[i] algoInstId: algorithm instance id

  @return None
*/
/*=======================================================================*/
STATIC void sns_scm_log_algo_config(
   uint8_t algoInstId)
{
   sns_err_code_e err = SNS_SUCCESS;
   uint8_t serviceId =
   sns_scm_algo_dbase[sns_scm_algo_inst_dbase[algoInstId]->algoIndex]->
   serviceId;

   if (serviceId == SNS_SCM_GYRO_CAL_SVC)
   {
      err = sns_scm_log_gyro_cal_config(algoInstId);
   }else if (serviceId == SNS_SCM_MAG_CAL_SVC)
   {
      err = sns_scm_log_qmag_cal_config(algoInstId);
   }

   if (err != SNS_SUCCESS && err != SNS_ERR_NOTALLOWED)
   {
      SNS_SCM_DEBUG3(ERROR,DBG_SCM_LOG_CONFIG_ERR, serviceId, algoInstId, err);
   }
}

/*=========================================================================
  FUNCTION:  sns_scm_log_algo_result
  =========================================================================*/
/*!
  @brief Log algorithm result

  @param[i] algoInstId: algorithm instance id
  @param[i] timestamp: timestamp on the algorithm input data

  @return None
*/
/*=======================================================================*/
STATIC void sns_scm_log_algo_result(
   uint8_t algoInstId,
   uint32_t timestamp)
{
   sns_err_code_e err = SNS_SUCCESS;
   uint8_t serviceId =
   sns_scm_algo_dbase[sns_scm_algo_inst_dbase[algoInstId]->algoIndex]->
   serviceId;

   if (serviceId == SNS_SCM_GYRO_CAL_SVC)
   {
      err = sns_scm_log_gyro_cal_result(algoInstId, timestamp);
   }else if(serviceId == SNS_SCM_MAG_CAL_SVC)
   {

      err = sns_scm_log_qmag_cal_result(algoInstId, timestamp);
   }

   if (err != SNS_SUCCESS && err != SNS_ERR_NOTALLOWED)
   {
      SNS_SCM_DEBUG3(ERROR,DBG_SCM_LOG_RESULT_ERR, serviceId, algoInstId, err);
   }
}

/*=========================================================================
  FUNCTION:  sns_scm_find_algo_inst
  =========================================================================*/
/*!
  @brief Searches the active algorithm database for an instance
  of an algorithm with specified configuration

  @param[i] algoSvcId: algorithm service id
  @param[i] algoCfgPtr: pointer to configuration of specified algorithm

  @return  Index to algorithm instance matching the specified configuration
  if successful, SNS_SCM_INVALID_ID otherwise
*/
/*=======================================================================*/
STATIC uint8_t sns_scm_find_algo_inst(
   uint8_t algoSvcId,
   void *algoCfgPtr)
{
   uint8_t algoInstId, algoInstCnt;

   for (algoInstId = 0, algoInstCnt = 0;
       algoInstCnt < sns_scm_algo_inst_count &&
       algoInstId < SNS_SCM_MAX_ALGO_INSTS;
       algoInstId++)
   {
      sns_scm_algo_inst_s *algoInstPtr = sns_scm_algo_inst_dbase[algoInstId];
      if (algoInstPtr == NULL)
      {
         continue;
      }

      if (sns_scm_algo_dbase[algoInstPtr->algoIndex]->serviceId == algoSvcId)
      {
         if (!SNS_OS_MEMCMP(algoCfgPtr,
                            algoInstPtr->configData.memPtr,
                            algoInstPtr->configData.memSize))
         {
            return algoInstId;
         }
      }
      algoInstCnt++;
   }
   return SNS_SCM_INVALID_ID;
}

/*=========================================================================
  FUNCTION:  sns_scm_get_algo_inst
  =========================================================================*/
/*!
  @brief
  If an instance of the specified algorithm doesnt exist,
  creates an instance and initializes the reference count.
  If an instance with identical configuration exists,
  updates the reference count.

  @param[i] algoIndex: index to algorithm in the algorithm database

  @return index of the algorithm instance in database if successful,
          SNS_SCM_INVALID_ID otherwise
*/
/*=======================================================================*/
STATIC uint8_t sns_scm_get_algo_inst(
   uint8_t algoIndex)
{
   uint8_t algoSvcId = sns_scm_algo_dbase[algoIndex]->serviceId;

   void *algoCfgPtr = sns_scm_algo_dbase[algoIndex]->defConfigData.memPtr;

   uint8_t algoInstId = sns_scm_find_algo_inst(algoSvcId, algoCfgPtr);

   if (algoInstId >= SNS_SCM_MAX_ALGO_INSTS)
   {
      uint8_t instId;

      //check if number of algo instances already reaches max
      if (sns_scm_algo_inst_count >= SNS_SCM_MAX_ALGO_INSTS)
      {
         return SNS_SCM_INVALID_ID;
      }

      for (instId = 0; instId < SNS_SCM_MAX_ALGO_INSTS; instId++)
      {
         if (sns_scm_algo_inst_dbase[instId] == NULL)
         {
            sns_scm_algo_inst_s* algoInstPtr;
            void *statePtr = NULL;
            uint32_t memSize = sizeof(sns_scm_algo_inst_s);

            //create algorithm instance
            sns_scm_algo_inst_dbase[instId] = SNS_OS_MALLOC(SNS_SCM_DBG_MOD, memSize);
            SNS_ASSERT(sns_scm_algo_inst_dbase[instId] != NULL);
            algoInstPtr = sns_scm_algo_inst_dbase[instId];

            //initialize the algorithm instance
            algoInstPtr->configData.memPtr = NULL;
            algoInstPtr->stateData.memPtr = NULL;
            algoInstPtr->inputData.memPtr = NULL;
            algoInstPtr->outputData.memPtr = NULL;

            memSize = sns_scm_algo_dbase[algoIndex]->defConfigData.memSize;
            if (memSize > 0)
            {
               algoInstPtr->configData.memPtr = SNS_OS_MALLOC(SNS_SCM_DBG_MOD, memSize);
               SNS_ASSERT(algoInstPtr->configData.memPtr != NULL);
               SNS_OS_MEMCOPY(algoInstPtr->configData.memPtr,
                              algoCfgPtr, memSize);
            }
            algoInstPtr->configData.memSize = memSize;

            memSize = sns_scm_algo_dbase[algoIndex]->algoApi.
                      sns_scm_algo_mem_req(algoCfgPtr);

            //Algorithm indicates configuration error with size 0
            if (memSize == 0)
            {
               SNS_OS_FREE(algoInstPtr->configData.memPtr);
               algoInstPtr->configData.memPtr = NULL;
               SNS_OS_FREE(sns_scm_algo_inst_dbase[instId]);
               sns_scm_algo_inst_dbase[instId] = NULL;
               return SNS_SCM_INVALID_ID;
            }

            algoInstPtr->stateData.memPtr = SNS_OS_MALLOC(SNS_SCM_DBG_MOD, memSize);
            SNS_ASSERT(algoInstPtr->stateData.memPtr != NULL);
            SNS_OS_MEMZERO(algoInstPtr->stateData.memPtr, memSize);
            algoInstPtr->stateData.memSize = memSize;

            memSize = sns_scm_algo_dbase[algoIndex]->defInputDataSize;
            if (memSize > 0)
            {
               algoInstPtr->inputData.memPtr = SNS_OS_MALLOC(SNS_SCM_DBG_MOD, memSize);
               SNS_ASSERT(algoInstPtr->inputData.memPtr != NULL);
               SNS_OS_MEMZERO(algoInstPtr->inputData.memPtr, memSize);
            }
            algoInstPtr->inputData.memSize = memSize;

            memSize = sns_scm_algo_dbase[algoIndex]->defOutputDataSize;
            if (memSize > 0)
            {
               algoInstPtr->outputData.memPtr = SNS_OS_MALLOC(SNS_SCM_DBG_MOD, memSize);
               SNS_ASSERT(algoInstPtr->outputData.memPtr != NULL);
               SNS_OS_MEMZERO(algoInstPtr->outputData.memPtr, memSize);
            }
            algoInstPtr->outputData.memSize = memSize;

            statePtr = sns_scm_algo_dbase[algoIndex]->algoApi.
                       sns_scm_algo_reset(algoInstPtr->configData.memPtr,
                                          algoInstPtr->stateData.memPtr);
            if (statePtr == NULL)
            {
               SNS_SCM_DEBUG0(ERROR, DBG_SCM_ENABLE_ALGO_STATE_NULL);
            }

            algoInstPtr->algoIndex = algoIndex;

            algoInstId = instId;

            sns_scm_algo_inst_count++;

            //request sensor data
            sns_scm_req_sensor_data(algoInstId);

            //Log algorithm configuration
            sns_scm_log_algo_config(algoInstId);

            SNS_SCM_DEBUG3(LOW, DBG_SCM_ALGO_MEM_INFO,
                           sns_scm_algo_dbase[algoInstPtr->algoIndex]->serviceId,
                           sizeof(sns_scm_algo_inst_s),
                           algoInstPtr->configData.memSize);
            SNS_SCM_DEBUG3(LOW, DBG_SCM_ALGO_STATE_MEM_INFO,
                           algoInstPtr->stateData.memSize,
                           algoInstPtr->inputData.memSize,
                           algoInstPtr->outputData.memSize);

            break;
         }
      }
   }

   return algoInstId;
}

/*=========================================================================
  FUNCTION:  sns_scm_delete_algo_inst
  =========================================================================*/
/*!
  @brief Deletes the specified algorithm instance

  @param[i] algoInstId: index to algorithm instance in database

  @return None
*/
/*=======================================================================*/
STATIC void sns_scm_delete_algo_inst(
   uint8_t algoInstId)
{
   sns_scm_algo_inst_s* algoInstPtr = sns_scm_algo_inst_dbase[algoInstId];

   //free memory reserved for this algorithm instance
   SNS_OS_FREE(algoInstPtr->configData.memPtr);
   algoInstPtr->configData.memPtr = NULL;
   algoInstPtr->configData.memSize = 0;

   SNS_OS_FREE(algoInstPtr->stateData.memPtr);
   algoInstPtr->stateData.memPtr = NULL;
   algoInstPtr->stateData.memSize = 0;

   SNS_OS_FREE(algoInstPtr->inputData.memPtr);
   algoInstPtr->inputData.memPtr = NULL;
   algoInstPtr->inputData.memSize = 0;

   SNS_OS_FREE(algoInstPtr->outputData.memPtr);
   algoInstPtr->outputData.memPtr = NULL;
   algoInstPtr->outputData.memSize = 0;

   SNS_OS_FREE(sns_scm_algo_inst_dbase[algoInstId]);
   sns_scm_algo_inst_dbase[algoInstId] = NULL;

   sns_scm_algo_inst_count--;
}

/*=========================================================================
  FUNCTION:  sns_scm_enable_algo
  =========================================================================*/
/*!
  @brief
  enables specified algorithm with the specified configuration

  @param[i] algoIndex: index to algorithm in the algorithm database

  @return index of the algorithm instance in database if successful,
          SNS_SCM_INVALID_ID otherwise
*/
/*=======================================================================*/
STATIC uint8_t sns_scm_enable_algo(
   uint8_t algoIndex)
{
   uint8_t algoInstId;

   algoInstId = sns_scm_get_algo_inst(algoIndex);
   if (algoInstId >= SNS_SCM_MAX_ALGO_INSTS ||
       sns_scm_algo_inst_dbase[algoInstId] == NULL)
   {
      return SNS_SCM_INVALID_ID;
   }

   if (sns_scm_algo_dbase[algoIndex]->timer == NULL)
   {
      sns_scm_reg_timer(algoIndex);
   }

   SNS_SCM_DEBUG3(MEDIUM, DBG_SCM_ENABLE_ALGO,
                  sns_scm_algo_dbase[algoIndex]->serviceId,
                  sns_scm_sensor_dbase[sns_scm_algo_dbase[algoIndex]->sensorIndex].sensorId,
                  algoInstId);

   return algoInstId;
}

/*=========================================================================
  FUNCTION:  sns_scm_store_auto_cal_params_registry
  =========================================================================*/
/*!
  @brief Stores last estimated auto cal parameters from SCM algo dbase to registry

  @param[i] algoIndex: index to algorithm in the algorithm database

  @return Sensors error code
*/
/*=======================================================================*/
STATIC sns_err_code_e sns_scm_store_auto_cal_params_registry(
   uint8_t algoIndex)
{
   sns_err_code_e err = SNS_SUCCESS;
   sns_scm_algo_s* algoPtr;

   if (sns_scm_algo_dbase[algoIndex] == NULL)
   {
      SNS_SCM_PRINTF1(ERROR, "algoIndex: %d has NULL entry in database", algoIndex);
      return SNS_ERR_FAILED;
   }

   algoPtr = sns_scm_algo_dbase[algoIndex];

   if (algoPtr->serviceId == SNS_SCM_GYRO_CAL_SVC)
   {
      err = sns_scm_store_gyro_auto_cal_params_registry(algoPtr->persistAutoCalParam.memPtr);
   }
   else if (algoPtr->serviceId == SNS_SCM_MAG_CAL_SVC)
   {
      err = sns_scm_store_qmag_auto_cal_params_registry(algoPtr->persistAutoCalParam.memPtr);
   }

   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_store_auto_cal_state_registry
  =========================================================================*/
/*!
  @brief Store auto cal algorithm state in registry

  @param[i] algoIndex: index to algorithm in the algorithm database

  @return Sensors error code
*/
/*=======================================================================*/
STATIC sns_err_code_e sns_scm_store_auto_cal_state_registry(
   uint8_t algoIndex)
{
   sns_err_code_e err = SNS_SUCCESS;
   sns_scm_algo_s* algoPtr;

   if (sns_scm_algo_dbase[algoIndex] == NULL)
   {
      SNS_SCM_PRINTF1(ERROR, "algoIndex: %d has NULL entry in database", algoIndex);
      return SNS_ERR_FAILED;
   }

   algoPtr = sns_scm_algo_dbase[algoIndex];

   if (algoPtr->serviceId == SNS_SCM_MAG_CAL_SVC)
   {
      err = sns_scm_store_qmag_auto_cal_state_registry(algoPtr->defConfigData.memPtr);
   }

   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_store_auto_cal_params_algodbase
  =========================================================================*/
/*!
  @brief Stores the auto cal param and state to scm algo database

  @param[i] algoIndex: index to algorithm in the algorithm database
  @param[i] algoInstId: index to algorithm instance in the database

  @return Sensors error code
*/
/*=======================================================================*/
STATIC sns_err_code_e sns_scm_store_auto_cal_params_algodbase(
   uint8_t algoIndex, uint8_t algoInstId)
{
   sns_scm_algo_s *algoPtr;
   sns_scm_algo_inst_s * algoInstPtr;

   if (sns_scm_algo_dbase[algoIndex] == NULL)
   {
      SNS_SCM_PRINTF1(ERROR, "algoIndex: %d has NULL entry in database", algoIndex);
      return SNS_ERR_FAILED;
   }

   algoPtr = sns_scm_algo_dbase[algoIndex];

   if (algoInstId >= SNS_SCM_MAX_ALGO_INSTS ||
       sns_scm_algo_inst_dbase[algoInstId] == NULL)
   {
      SNS_SCM_DEBUG1(ERROR, DBG_SCM_DISABLE_ALGO_INSTANCE_ERR, algoInstId);
      return SNS_ERR_FAILED;
   }

   algoInstPtr = sns_scm_algo_inst_dbase[algoInstId];

   if (algoPtr->persistAutoCalParam.memPtr != NULL)
   {
      if ( algoPtr->serviceId == SNS_SCM_MAG_CAL_SVC)
      {
         qmag_cal_output_s *qmagOpAlgoInstPtr = (qmag_cal_output_s *)(algoInstPtr->outputData.memPtr);
         qmag_cal_output_s *qmagOpAlgoDBPtr = (qmag_cal_output_s *)(algoPtr->persistAutoCalParam.memPtr);

         //skip storing to algo dbase if calibration did not occur
         if (qmagOpAlgoInstPtr->offset_HI[0] != 0 && qmagOpAlgoInstPtr->offset_HI[1] != 0 && qmagOpAlgoInstPtr->offset_HI[2] != 0)
         {
            SNS_OS_MEMCOPY(algoPtr->persistAutoCalParam.memPtr,
                           algoInstPtr->outputData.memPtr,
                           algoPtr->persistAutoCalParam.memSize);

            SNS_SCM_PRINTF3(MEDIUM, " stored qmag auto cal params to scm algo dbase offsets: %d, %d, %d",
                            qmagOpAlgoDBPtr->offset_HI[0],
                            qmagOpAlgoDBPtr->offset_HI[1],
                            qmagOpAlgoDBPtr->offset_HI[2]);

            sns_scm_store_qmag_auto_cal_state_algodbase(algoInstPtr->stateData.memPtr,
                                                        algoPtr->defConfigData.memPtr);
         }
      }
      else if ( algoPtr->serviceId == SNS_SCM_GYRO_CAL_SVC)
      {
         gyro_cal_output_s *gyroOpAlgoInstPtr = (gyro_cal_output_s *)(algoInstPtr->outputData.memPtr);
         gyro_cal_output_s *gyroOpAlgoDBPtr = (gyro_cal_output_s *)(algoPtr->persistAutoCalParam.memPtr);

         //skip storing to algo dbase if calibration did not occur
         if (gyroOpAlgoInstPtr->bias[0] != 0 && gyroOpAlgoInstPtr->bias[1] != 0 && gyroOpAlgoInstPtr->bias[2] != 0)
         {
            SNS_OS_MEMCOPY(algoPtr->persistAutoCalParam.memPtr,
                           algoInstPtr->outputData.memPtr,
                           algoPtr->persistAutoCalParam.memSize);

            SNS_SCM_PRINTF3(HIGH, " stored gyro auto cal params to persistent algo dbase offset: %d, %d, %d",
                            gyroOpAlgoDBPtr->bias[0],
                            gyroOpAlgoDBPtr->bias[1],
                            gyroOpAlgoDBPtr->bias[2]);
         }
      }
   }

   return SNS_SUCCESS;
}

/*=========================================================================
  FUNCTION:  sns_scm_disable_algo
  =========================================================================*/
/*!
  @brief Decrements the reference count of the algorithm instance.
         If count is zero, deletes an instance and frees up its resources

  @param[i] algoIndex: index to algorithm in the algorithm database

  @return Sensors error code
*/
/*=======================================================================*/
STATIC sns_err_code_e sns_scm_disable_algo(
   uint8_t algoIndex)
{
   uint8_t algoInstId;

   if (sns_scm_algo_dbase[algoIndex] == NULL)
   {
      SNS_SCM_PRINTF1(ERROR, "algoIndex: %d has NULL entry in database", algoIndex);
      return SNS_ERR_FAILED;
   }

   algoInstId = sns_scm_find_algo_inst(sns_scm_algo_dbase[algoIndex]->serviceId,
                                       sns_scm_algo_dbase[algoIndex]->defConfigData.memPtr);

   if (algoInstId == SNS_SCM_INVALID_ID)
   {
      return SNS_ERR_BAD_PARM;
   }
   // store auto cal params to scm's algo database
   sns_scm_store_auto_cal_params_algodbase(algoIndex, algoInstId);

   //Deregister timer
   sns_scm_dereg_timer(algoIndex);

   //stop sensor data
   sns_scm_stop_sensor_data(algoInstId);

   //delete the algo instance
   sns_scm_delete_algo_inst(algoInstId);

   SNS_SCM_DEBUG3(MEDIUM, DBG_SCM_DISABLE_ALGO,
                  sns_scm_algo_dbase[algoIndex]->serviceId,
                  sns_scm_sensor_dbase[sns_scm_algo_dbase[algoIndex]->sensorIndex].sensorId,
                  algoInstId);

   return SNS_SUCCESS;
}

/*=========================================================================
  FUNCTION:  sns_scm_update_sensor_status
  =========================================================================*/
/*!
  @brief    Updates overall status of sensor calibration service based on
            sensor status of each sensor ID.

  @param[i] algoPtr: pointer to algo database

  @return   Overall status
*/
/*=======================================================================*/
STATIC uint8_t sns_scm_update_sensor_status(
   sns_scm_algo_s *algoPtr)
{
   unsigned int one_client_cnt = 0;
   unsigned int active_cnt = 0;
   uint8_t sensorStatus0 = SNS_SMGR_SENSOR_STATUS_UNKNOWN_V01;
   uint8_t status = SNS_SMGR_SENSOR_STATUS_UNKNOWN_V01, sensorIndex = 0;
   int k;

   for(k = 0; k < algoPtr->monitorSensorCount; k++)
   {
      sensorIndex = sns_scm_get_sensor_index(algoPtr->monitorSensorId[k]);
      if(sensorIndex != SNS_SCM_INVALID_ID && sensorIndex < SNS_SCM_MAX_SNS_MON)
      {
         switch(sns_scm_sensor_dbase[sensorIndex].status)
         {
            case SNS_SMGR_SENSOR_STATUS_ACTIVE_V01:
               active_cnt++;
               break;
            case SNS_SMGR_SENSOR_STATUS_ONE_CLIENT_V01:
               one_client_cnt++;
               break;
            default:
               break;
         }

         if(k == 0)
         {
            sensorStatus0 = sns_scm_sensor_dbase[sensorIndex].status;
         }
      }
   }

   status = (active_cnt >= 1)     ? SNS_SMGR_SENSOR_STATUS_ACTIVE_V01 :
            (one_client_cnt > 1)  ? SNS_SMGR_SENSOR_STATUS_ACTIVE_V01 :
            (sensorStatus0 ==
             SNS_SMGR_SENSOR_STATUS_UNKNOWN_V01)
                                  ? SNS_SMGR_SENSOR_STATUS_UNKNOWN_V01 :
            (sensorStatus0 ==
             SNS_SMGR_SENSOR_STATUS_ONE_CLIENT_V01)
                                  ? SNS_SMGR_SENSOR_STATUS_ONE_CLIENT_V01 :
                                    SNS_SMGR_SENSOR_STATUS_IDLE_V01 ;

   SNS_SCM_PRINTF3(MED, "active=%d 1client=%d status=%d",
                   active_cnt, one_client_cnt, status);

   algoPtr->monitorSensorStatus = status;

   return status;
}

/*=========================================================================
  FUNCTION:  sns_scm_active_qmd_client
  =========================================================================*/
/*!
  @brief Determines if any algorithm requires QMD.

  @return true if at least one algorithm requires QMD
          false otherwise
*/
/*=======================================================================*/
STATIC bool sns_scm_active_qmd_client(void)
{
   uint8_t i;
   bool active = false;

   for(i = 0; i < SNS_SCM_NUM_ALGO_SVCS; i++)
   {
      if(sns_scm_algo_dbase[i] != NULL)
      {
         if(sns_scm_algo_dbase[i]->monitorSensorStatus != SNS_SMGR_SENSOR_STATUS_IDLE_V01 &&
            sns_scm_algo_dbase[i]->monitorSensorStatus != SNS_SMGR_SENSOR_STATUS_UNKNOWN_V01)
         {
            active = true;
            break;
         }
      }
   }

   return active;
}

/*=========================================================================
  FUNCTION:  sns_scm_process_algo_result
  =========================================================================*/
/*!
  @brief Process specified algorithm result

  @param[i] clientReqId: index to client request in the database

  @return None
*/
/*=======================================================================*/
STATIC void sns_scm_process_algo_result(
   uint8_t algoInstId)
{
   if (sns_scm_algo_dbase[sns_scm_algo_inst_dbase[algoInstId]->algoIndex]->
       serviceId == SNS_SCM_GYRO_CAL_SVC)
   {
      gyro_cal_output_s* gyroCalResult;
      sns_smgr_sensor_cal_req_msg_v01 *msgPtr = NULL;

      sns_smgr_sensor_cal_resp_msg_v01 *msgRespP;
      qmi_txn_handle                   txn_handle;

      msgRespP = (sns_smgr_sensor_cal_resp_msg_v01 *)
                 SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_smgr_sensor_cal_resp_msg_v01));
      SNS_ASSERT(msgRespP != NULL);

      msgPtr = (sns_smgr_sensor_cal_req_msg_v01 *)
               SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_smgr_sensor_cal_req_msg_v01));
      SNS_ASSERT(msgPtr != NULL);

      gyroCalResult = (gyro_cal_output_s*)
                      (sns_scm_algo_inst_dbase[algoInstId]->outputData.memPtr);

      msgPtr->usage = SNS_SMGR_CAL_DYNAMIC_V01;
      msgPtr->SensorId = SNS_SMGR_ID_GYRO_V01;
      msgPtr->DataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;

      //Send bias correction values to SMGR
      msgPtr->ZeroBias_len = 3;
      msgPtr->ZeroBias[0] = -(gyroCalResult->bias[0]);
      msgPtr->ZeroBias[1] = -(gyroCalResult->bias[1]);
      msgPtr->ZeroBias[2] = -(gyroCalResult->bias[2]);

      msgPtr->ScaleFactor_len = 0;
      sns_scm_qmi_client_send_msg_async (scm_smgr_qmi_client_if.user_handle,
                                       SNS_SMGR_CAL_REQ_V01,
                                       msgPtr, sizeof(sns_smgr_sensor_cal_req_msg_v01),
                                       msgRespP, sizeof(sns_smgr_sensor_cal_resp_msg_v01),
                                       sns_scm_smgr_resp_cb, NULL, &txn_handle);

      SNS_SCM_DEBUG3(MEDIUM, DBG_SCM_GYRO_CAL_REQ_INFO,
                     -(gyroCalResult->bias[0]),
                     -(gyroCalResult->bias[1]),
                     -(gyroCalResult->bias[2]));
   }
   else if (sns_scm_algo_dbase[sns_scm_algo_inst_dbase[algoInstId]->algoIndex]->
       serviceId == SNS_SCM_MAG_CAL_SVC)
   {
      qmag_cal_output_s* qmagCalResult;
      sns_smgr_sensor_cal_req_msg_v01 *msgPtr = NULL;

      sns_smgr_sensor_cal_resp_msg_v01 *msgRespP;
      qmi_txn_handle                   txn_handle;

      msgRespP = (sns_smgr_sensor_cal_resp_msg_v01 *)
                 SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_smgr_sensor_cal_resp_msg_v01));
      SNS_ASSERT(msgRespP != NULL);

      msgPtr = (sns_smgr_sensor_cal_req_msg_v01 *)
               SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_smgr_sensor_cal_req_msg_v01));
      SNS_ASSERT(msgPtr != NULL);

      qmagCalResult = (qmag_cal_output_s*)
                      (sns_scm_algo_inst_dbase[algoInstId]->outputData.memPtr);

      msgPtr->usage = SNS_SMGR_CAL_DYNAMIC_V01;
      msgPtr->SensorId = SNS_SMGR_ID_MAG_V01;
      msgPtr->DataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;

      //Send bias correction values to SMGR
      msgPtr->ZeroBias_len = 3;
      msgPtr->ZeroBias[0] = -(qmagCalResult->offset_HI[0]);
      msgPtr->ZeroBias[1] = -(qmagCalResult->offset_HI[1]);
      msgPtr->ZeroBias[2] = -(qmagCalResult->offset_HI[2]);

      msgPtr->ScaleFactor_len = 0;
      msgPtr->CompensationMatrix_valid = false;
      msgPtr->CalibrationAccuracy_valid = true;
      msgPtr->CalibrationAccuracy = qmagCalResult->accuracy_HI;
      sns_scm_qmi_client_send_msg_async (scm_smgr_qmi_client_if.user_handle,
                                       SNS_SMGR_CAL_REQ_V01,
                                       msgPtr, sizeof(sns_smgr_sensor_cal_req_msg_v01),
                                       msgRespP, sizeof(sns_smgr_sensor_cal_resp_msg_v01),
                                       sns_scm_smgr_resp_cb, NULL, &txn_handle);

      SNS_SCM_PRINTF3(MEDIUM, "Sending correction values to Smgr %d %d %d",
          msgPtr->ZeroBias[0],
          msgPtr->ZeroBias[1],
          msgPtr->ZeroBias[2]);
      SNS_SCM_PRINTF1(MEDIUM, "Sending Cal Accuracy Smgr %d ", msgPtr->CalibrationAccuracy);
   }
}


/*=========================================================================
  FUNCTION:  sns_scm_process_sam_resp
  =========================================================================*/
/*!
  @brief Processes the response received from sensors manager

  @param[i] smgrRespPtr: Pointer to sensors manager response message
  @param[i] msgId: Message ID

  @return Sensors error code
*/
/*=======================================================================*/
STATIC sns_err_code_e sns_scm_process_sam_resp(
   const void *samRespPtr,
   int32_t msgId)
{
   if (msgId == SNS_SAM_AMD_ENABLE_RESP_V01)
   {
      sns_sam_qmd_enable_resp_msg_v01* respPtr =
      (sns_sam_qmd_enable_resp_msg_v01*)samRespPtr;

      if (respPtr->resp.sns_result_t != SNS_SUCCESS)
      {
         SNS_SCM_PRINTF0(ERROR, "SCM : error in SNS_SAM_AMD_ENABLE_RESP_V01");
         scm_qmd_state.qmd_enabled = false;
         scm_qmd_state.qmd_inst_id_valid = false;
         return((sns_err_code_e)(respPtr->resp.sns_err_t));
      }

      SNS_SCM_DEBUG3(MEDIUM, DBG_SCM_SAM_RESP_INFO,
                     msgId, respPtr->instance_id, scm_qmd_state.qmd_inst_id);

      //Update the qmd instance id
      scm_qmd_state.qmd_inst_id = respPtr->instance_id;
      scm_qmd_state.qmd_inst_id_valid = true;
   }
   else if (msgId == SNS_SAM_AMD_DISABLE_RESP_V01)
   {
      sns_sam_qmd_disable_resp_msg_v01* respPtr =
      (sns_sam_qmd_disable_resp_msg_v01*)samRespPtr;

      if (respPtr->resp.sns_result_t != SNS_SUCCESS)
      {
         SNS_SCM_PRINTF0(ERROR, "SCM : error in SNS_SAM_AMD_DISABLE_RESP_V01");

         return((sns_err_code_e)(respPtr->resp.sns_err_t));
      }

      scm_qmd_state.qmd_enabled = false;
      scm_qmd_state.qmd_inst_id_valid = false;
      scm_qmd_state.qmd_state = SNS_SAM_MOTION_UNKNOWN_V01;


      SNS_SCM_DEBUG3(MEDIUM, DBG_SCM_SAM_RESP_INFO,
                     msgId, respPtr->instance_id, scm_qmd_state.qmd_inst_id);
   }
   else
   {
      SNS_SCM_DEBUG1(HIGH, DBG_SCM_SAM_RESP_DROPPED, msgId);
   }

   return SNS_SUCCESS;
}

/*=========================================================================
  FUNCTION:  sns_scm_process_sam_ind
  =========================================================================*/
/*!
  @brief Processes sensor status indication received from sensors manager

  @param[i] smgrIndPtr: Pointer to sensors manager indication message

  @return Sensors error code
*/
/*=======================================================================*/
STATIC sns_err_code_e sns_scm_process_sam_ind(
   const void *samIndPtr)
{
   sns_sam_qmd_report_ind_msg_v01* indPtr;
   uint8_t i;
   uint8_t algoInstCnt = sns_scm_algo_inst_count;

   indPtr = (sns_sam_qmd_report_ind_msg_v01*)samIndPtr;

   if (scm_qmd_state.qmd_enabled)
   {
      scm_qmd_state.qmd_state = indPtr->state;
      if( !scm_qmd_state.qmd_inst_id_valid )
      {
         SNS_SCM_PRINTF0(HIGH, "SCM recieved QMD indication but qmd_inst_id_valid is false");
      }
   }

   /* In case of SAM instance is already created then the INDICATION msg
  	  arrives before RESPONSE msg. This may delay the state change &
  	  block auto calibration for significant amount of time. To avoid this
  	  we enable algorithms when we receive request PENDING indication.    */

   if ( (SNS_SCM_QMD_PEND_ID == scm_qmd_state.qmd_inst_id) ||
  	    (indPtr->instance_id == scm_qmd_state.qmd_inst_id) )
   {
	   {
		   scm_qmd_state.qmd_state = indPtr->state;
		   SNS_SCM_PRINTF1(MEDIUM, "scm qmd inst id : %d",scm_qmd_state.qmd_inst_id);
	   }
   }

   SNS_SCM_PRINTF1(MEDIUM, "scm qmd state change: %d",scm_qmd_state.qmd_state);

   //enable algos
   for (i = 0;
        (i < SNS_SCM_NUM_ALGO_SVCS) && (sns_scm_algo_dbase[i] != NULL);
        i++)
   {
      if (sns_scm_algo_dbase[i]->enableAlgo)
      {
         if (sns_scm_algo_dbase[i]->monitorSensorStatus == SNS_SMGR_SENSOR_STATUS_ACTIVE_V01 ||
             sns_scm_algo_dbase[i]->monitorSensorStatus == SNS_SMGR_SENSOR_STATUS_ONE_CLIENT_V01)
         {
            if ( (scm_qmd_state.qmd_state == SNS_SAM_MOTION_REST_V01 &&
                 sns_scm_algo_dbase[i]->motionTrigFlags == MOTION_DETECT_FLAG_ACTIVE_IN_REST) ||
                 (scm_qmd_state.qmd_state == SNS_SAM_MOTION_MOVE_V01 &&
                 sns_scm_algo_dbase[i]->motionTrigFlags == MOTION_DETECT_FLAG_ACTIVE_IN_MOTION)
                )
            {
               sns_scm_enable_algo(i);
               SNS_SCM_PRINTF1(MEDIUM, "scm enable algo svc: %d",sns_scm_algo_dbase[i]->serviceId);
            }
         }
      }
   }

   algoInstCnt = sns_scm_algo_inst_count; // reinitialize to account for any new enables
   //disable algos
   for (i=0; i < SNS_SCM_MAX_ALGO_INSTS && algoInstCnt > 0; i++)
   {
      if (sns_scm_algo_inst_dbase[i] != NULL)
      {
         uint8_t algoIndex = sns_scm_algo_inst_dbase[i]->algoIndex;

         SNS_SCM_PRINTF3(HIGH, "scm algo dbase scan i: %d, algo index: %d, algo svc: %d",
                         i,algoIndex,sns_scm_algo_dbase[algoIndex]->serviceId);
         SNS_SCM_PRINTF1(HIGH, "scm motionTrigFlags: %d",sns_scm_algo_dbase[algoIndex]->motionTrigFlags);

         if( (sns_scm_algo_dbase[algoIndex]->motionTrigFlags == MOTION_DETECT_FLAG_ACTIVE_IN_REST &&
              scm_qmd_state.qmd_state == SNS_SAM_MOTION_MOVE_V01) ||
             (sns_scm_algo_dbase[algoIndex]->motionTrigFlags == MOTION_DETECT_FLAG_ACTIVE_IN_MOTION &&
              scm_qmd_state.qmd_state == SNS_SAM_MOTION_REST_V01))
         {
            sns_scm_disable_algo(algoIndex);
            algoInstCnt --;
            SNS_SCM_PRINTF1(MEDIUM, "scm disable algo svc: %d",sns_scm_algo_dbase[algoIndex]->serviceId);
         }
      }
   }

   SNS_SCM_DEBUG3(MEDIUM, DBG_SCM_SAM_IND_INFO,
                  scm_qmd_state.qmd_inst_id, indPtr->instance_id, indPtr->state);

#if defined(SNS_PLAYBACK_SKIP_SMGR) || defined(SNS_QDSP_SIM)
   if (scm_qmd_state.qmd_state == SNS_SAM_MOTION_REST_V01)
   {
      sns_scm_pb_next_report_time=indPtr->timestamp + sns_scm_pb_report_timeout;
      sns_scm_pb_update_next_report_time=true;
   }
   else if (scm_qmd_state.qmd_state == SNS_SAM_MOTION_MOVE_V01)
   {
      sns_scm_pb_update_next_report_time=false;
   }
#endif

   return SNS_SUCCESS;
}

/*=========================================================================
  FUNCTION:  sns_scm_process_smgr_resp
  =========================================================================*/
/*!
  @brief Processes the response received from sensors manager

  @param[i] smgrRespPtr: Pointer to sensors manager response message
  @param[i] msgId: Message ID

  @return Sensors error code
*/
/*=======================================================================*/
STATIC sns_err_code_e sns_scm_process_smgr_resp(
   const void *smgrRespPtr,
   int32_t msgId)
{
   uint8_t dataReqId = SNS_SCM_INVALID_ID;
   uint32_t i;

   if (msgId == SNS_SMGR_BUFFERING_RESP_V01)
   {
      sns_smgr_buffering_resp_msg_v01* respPtr =
         (sns_smgr_buffering_resp_msg_v01*) smgrRespPtr;

      if (respPtr->ReportId_valid)
      {
         dataReqId = respPtr->ReportId;
      }

      if( respPtr->Resp.sns_result_t != SNS_RESULT_SUCCESS_V01 &&
          respPtr->Resp.sns_err_t != SENSOR1_SUCCESS )
      {
        SNS_SCM_PRINTF2(ERROR, "SCM : error in SNS_SMGR_BUFFERING_RESP_V01,err:%d, acknack:%d",
            respPtr->Resp.sns_err_t, respPtr->AckNak);
        if( respPtr->Resp.sns_err_t == SENSOR1_EBAD_MSG_ID )
        {
           sns_scm_smgr_buffering_flag = false;
           if( sns_scm_switch_smgr_data_req( dataReqId ) == SNS_SUCCESS )
           {
              // Retrying with periodic data request
              return SNS_ERR_WOULDBLOCK;
           }
        }
        else if( respPtr->AckNak_valid &&
                 respPtr->AckNak == SNS_SMGR_RESPONSE_NAK_REPORT_RATE_V01 )
        {
           if( sns_scm_switch_smgr_data_req( dataReqId ) == SNS_SUCCESS )
           {
              // Retrying with periodic data request
              return SNS_ERR_WOULDBLOCK;
           }
        }
      }

      if (respPtr->AckNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 ||
          respPtr->AckNak == SNS_SMGR_RESPONSE_ACK_MODIFIED_V01)
      {
         SNS_SCM_DEBUG3(MEDIUM, DBG_SCM_SMGR_RESP_SUCCESS,
                        dataReqId, respPtr->AckNak, respPtr->ReasonPair_len);

         return SNS_SUCCESS;
      }

      if (dataReqId < SNS_SCM_MAX_DATA_REQS &&
          sns_scm_data_req_dbase[dataReqId] != NULL)
      {
         SNS_SCM_DEBUG3(HIGH, DBG_SCM_SMGR_RESP_ACK_VAL,
                        dataReqId, respPtr->AckNak, respPtr->ReasonPair_len);
      }
      else
      {
         SNS_SCM_DEBUG3(HIGH, DBG_SCM_SMGR_RESP_DATA_DROP,
                        dataReqId, respPtr->AckNak, respPtr->ReasonPair_len);
      }

      if (respPtr->ReasonPair_len < SNS_SMGR_MAX_NUM_REASONS_V01)
      {
         for (i = 0; i < respPtr->ReasonPair_len; i++)
         {
            SNS_SCM_DEBUG2(MEDIUM, DBG_SCM_SMGR_RESP_INFO,
                           respPtr->ReasonPair[i].ItemNum,
                           respPtr->ReasonPair[i].Reason);
         }
      }
      return((sns_err_code_e)(respPtr->Resp.sns_err_t));
   }
   else if (msgId == SNS_SMGR_REPORT_RESP_V01)
   {
      sns_smgr_periodic_report_resp_msg_v01* respPtr =
      (sns_smgr_periodic_report_resp_msg_v01*)smgrRespPtr;

      if (respPtr->Resp.sns_result_t != SNS_SUCCESS)
      {
         SNS_SCM_PRINTF0(ERROR, "SCM : error in SNS_SMGR_REPORT_RESP_V01");

         return((sns_err_code_e)(respPtr->Resp.sns_err_t));
      }

      dataReqId = respPtr->ReportId;

      if (respPtr->AckNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 ||
          respPtr->AckNak == SNS_SMGR_RESPONSE_ACK_MODIFIED_V01)
      {
         SNS_SCM_DEBUG3(MEDIUM, DBG_SCM_SMGR_RESP_SUCCESS,
                        dataReqId, respPtr->AckNak, respPtr->ReasonPair_len);

         return SNS_SUCCESS;
      }

      if (dataReqId < SNS_SCM_MAX_DATA_REQS &&
          sns_scm_data_req_dbase[dataReqId] != NULL)
      {
         SNS_SCM_DEBUG3(HIGH, DBG_SCM_SMGR_RESP_ACK_VAL,
                        dataReqId, respPtr->AckNak, respPtr->ReasonPair_len);
      }
      else
      {
         SNS_SCM_DEBUG3(HIGH, DBG_SCM_SMGR_RESP_DATA_DROP,
                        dataReqId, respPtr->AckNak, respPtr->ReasonPair_len);
      }

      if (respPtr->ReasonPair_len < SNS_SMGR_MAX_NUM_REASONS_V01)
      {
         for (i = 0; i < respPtr->ReasonPair_len; i++)
         {
            SNS_SCM_DEBUG2(MEDIUM, DBG_SCM_SMGR_RESP_INFO,
                           respPtr->ReasonPair[i].ItemNum,
                           respPtr->ReasonPair[i].Reason);
         }
      }
   }
   else if (msgId == SNS_SMGR_CAL_RESP_V01)
   {
      sns_smgr_sensor_cal_resp_msg_v01* respPtr =
      (sns_smgr_sensor_cal_resp_msg_v01*)smgrRespPtr;

      if (respPtr->Resp.sns_result_t != SNS_SUCCESS)
      {
         SNS_SCM_PRINTF1(ERROR, "error in SNS_SMGR_CAL_RESP_V01 = %d",
                         respPtr->Resp.sns_result_t);

         return((sns_err_code_e)(respPtr->Resp.sns_err_t));
      }
   }
   else
   {
      SNS_SCM_DEBUG1(HIGH, DBG_SCM_SMGR_RESP_DROPPED, msgId);
   }

   return SNS_SUCCESS;
}

/*=========================================================================
  FUNCTION:  sns_scm_process_smgr_int_resp
  =========================================================================*/
/*!
  @brief Processes the response received from sensors manager

  @param[i] smgrRespPtr: Pointer to sensors manager response message
  @param[i] msgId: Message ID

  @return Sensors error code
*/
/*=======================================================================*/
STATIC sns_err_code_e sns_scm_process_smgr_int_resp(
   const void *smgrRespPtr,
   int32_t msgId)
{
   if (msgId == SNS_SMGR_SENSOR_STATUS_MONITOR_RESP_V02)
   {
      uint32_t i;
      sns_smgr_sensor_status_monitor_resp_msg_v02* respPtr =
         (sns_smgr_sensor_status_monitor_resp_msg_v02*) smgrRespPtr;

      if (respPtr->resp.sns_result_t != SNS_SUCCESS)
      {
         SNS_SCM_PRINTF2(
           ERROR, "error in SENSOR_STATUS_MONITOR sensor=0x%x err=%d",
           respPtr->sensor_id, respPtr->resp.sns_err_t);

         return((sns_err_code_e)(respPtr->resp.sns_err_t));
      }
      for (i=0; i < SNS_SCM_MAX_SNS_MON &&
          sns_scm_sensor_dbase[i].sensorId != SNS_SCM_INVALID_ID; i++)
      {
         if (sns_scm_sensor_dbase[i].sensorId == respPtr->sensor_id
             && sns_scm_sensor_dbase[i].status == SNS_SCM_INVALID_ID)
         {
            sns_scm_sensor_dbase[i].status = SNS_SMGR_SENSOR_STATUS_UNKNOWN_V01;
            break;
         }
      }
   }
   else
   {
      SNS_SCM_DEBUG1(HIGH, DBG_SCM_SMGR_RESP_DROPPED, msgId);
   }

   return SNS_SUCCESS;
}

/*=========================================================================
  FUNCTION:  sns_scm_process_smgr_item
  =========================================================================*/
/*!
  @brief    Processes a sample in a SMGR buffering indication message

  @param[i] dataPtr: pointer to sample in buffering indication message
  @param[i] timestamp:  timestamp of the current sample
  @param[i] dataReqId:  index of data request entry in dbase

  @return   Sensors error code
*/
/*=======================================================================*/
STATIC sns_err_code_e sns_scm_process_smgr_item(
   const void* dataPtr,
   uint32_t timestamp,
   uint8_t dataReqId,
   uint32_t sensorId)
{
   uint32_t i;

   sns_smgr_buffering_sample_s_v01 * samplePtr =
      (sns_smgr_buffering_sample_s_v01*)dataPtr;
   uint8_t itemSize = sizeof(samplePtr->Data);

   if (dataReqId >= SNS_SCM_MAX_DATA_REQS ||
       sns_scm_data_req_dbase[dataReqId] == NULL)
   {
      return SNS_ERR_FAILED;
   }

   //execute algorithms waiting for this sensor data
   for (i = 0; i < sns_scm_data_req_dbase[dataReqId]->algoInstCount; i++)
   {
      uint8_t algoInstId = sns_scm_data_req_dbase[dataReqId]->algoInstDbase[i];
      sns_scm_algo_inst_s *algoInstPtr = sns_scm_algo_inst_dbase[algoInstId];

      if (sns_scm_algo_dbase[algoInstPtr->algoIndex]->serviceId == SNS_SCM_GYRO_CAL_SVC)
      {
         uint8_t algoInpSize, *algoInpPtr;
         gyro_cal_output_s* gyroCalResult = algoInstPtr->outputData.memPtr;
         gyro_cal_state_s* gyroCalState = algoInstPtr->stateData.memPtr;
         int32_t *gyroData = samplePtr->Data;
         bool skipGyroCal = false;
         uint8_t j;

         //Skip gyro calibration if absolute bias is exceeded on any axis
         for (j=0; j < 3; j++)
         {
            if (FX_ABS(gyroData[j]) > SNS_SCM_GYRO_CAL_MAX_ABS_BIAS)
            {
               skipGyroCal = true;
               break;
            }
         }
         if (skipGyroCal)
         {
            sns_scm_stop_sensor_data(algoInstId);
            if (sns_scm_data_req_dbase[dataReqId] == NULL)
            {
               break;
            }
         }

         //copy sensor data to algorithm input
         algoInpPtr = algoInstPtr->inputData.memPtr;
         algoInpSize = algoInstPtr->inputData.memSize;
         if (itemSize > algoInpSize)
         {
            SNS_SCM_DEBUG2(ERROR, DBG_SCM_SMGR_IND_INVALID, algoInpSize, itemSize);
            continue;
         }
         SNS_OS_MEMCOPY(algoInpPtr, samplePtr->Data, itemSize);

         //execute the algorithm
         sns_profiling_log_qdss( SNS_SCM_ALGO_UPDATE_ENTER, 1, SNS_SCM_GYRO_CAL_SVC );
         sns_scm_algo_dbase[algoInstPtr->algoIndex]->algoApi.
         sns_scm_algo_update(algoInstPtr->stateData.memPtr,
                             algoInstPtr->inputData.memPtr,
                             algoInstPtr->outputData.memPtr);
         sns_profiling_log_qdss( SNS_SCM_ALGO_UPDATE_EXIT, 1, SNS_SCM_GYRO_CAL_SVC );

         if (gyroCalResult->motionState == GYRO_CAL_MOT_STATE_REST)
         {
            sns_scm_process_algo_result(algoInstId);
         }

         sns_scm_log_algo_result(algoInstId, timestamp);

         if (gyroCalResult->motionState == GYRO_CAL_MOT_STATE_UNKNOWN)
         {
            continue;
         }

         SNS_SCM_DEBUG3(LOW, DBG_SCM_GYRO_CAL_REQ_INFO,
                        (int32_t)(gyroCalState->variance[0] >> 32),
                        (int32_t)(gyroCalState->variance[1] >> 32),
                        (int32_t)(gyroCalState->variance[2] >> 32));
         SNS_SCM_DEBUG3(LOW, DBG_SCM_GYRO_CAL_REQ_INFO,
                        (int32_t)(gyroCalState->variance[0]),
                        (int32_t)(gyroCalState->variance[1]),
                        (int32_t)(gyroCalState->variance[2]));

         //stop sensor data
         sns_scm_stop_sensor_data(algoInstId);
         if (sns_scm_data_req_dbase[dataReqId] == NULL)
         {
            break;
         }
      }
      else if (sns_scm_algo_dbase[algoInstPtr->algoIndex]->serviceId == SNS_SCM_MAG_CAL_SVC)
      {
         bool hard_reset = false;
         qmag_cal_input_s *inp = (qmag_cal_input_s *)(algoInstPtr->inputData.memPtr);
         qmag_cal_output_s *outp = (qmag_cal_output_s *)(algoInstPtr->outputData.memPtr);

         // Peak Detector
         if (outp->accuracy_HI >= QMAG_CAL_MIN_HI_GOOD_ACCURACY &&
          sensorId == SNS_SMGR_ID_MAG_V01)
         {
            // convert raw -> calibrated sample and compute its norm squared
            uint32_t col;
            float cal_mag_norm_sq = 0.0;
            void *statePtr = NULL;

            for(col=0; col<QMAG_CAL_COL; col++)
            {
               cal_mag_norm_sq +=
                  FX_FIXTOFLT_Q16_SP(samplePtr->Data[col] - outp->offset_HI[col]) *
                  FX_FIXTOFLT_Q16_SP(samplePtr->Data[col] - outp->offset_HI[col]);
            }

            if (cal_mag_norm_sq > QMAG_CAL_HARD_RESET_THRESH_GAUSS_SQ)
            {
              hard_reset = true;
              SNS_SCM_PRINTF0(ERROR, "QMag Cal Hard Reset");
              qmag_cal_config_s *state = (qmag_cal_config_s *)algoInstPtr->stateData.memPtr;
              //qmag_cal_scm_state_reset resets the state persistent state.
              statePtr = qmag_cal_scm_state_reset(algoInstPtr->configData.memPtr,
                                                  algoInstPtr->stateData.memPtr);
              //Reset the persistent state here.
              uint8_t col=0;
              for (col = 0; col < QMAG_CAL_COL; col++)
              {    
                  state->persistent_state.published_offs_HI[col] = 0;
              }
              state->persistent_state.published_accuracy_HI = QMAG_CAL_ACCURACY_FAILED;
              state->persistent_state.published_offset_valid = false;
              state->persistent_state.published_offset_time = 0;

              if (statePtr == NULL)
              {
                SNS_SCM_PRINTF0(ERROR, "QMag Cal State Reset Error");
              }

            }
         }

         //the passed timestamp already accounts for the offset.
         inp->sensor_time   = timestamp ;
         inp->sensor_data[0] = samplePtr->Data[0];
         inp->sensor_data[1] = samplePtr->Data[1];
         inp->sensor_data[2] = samplePtr->Data[2];

         if (sensorId == SNS_SMGR_ID_ACCEL_V01)
         {
             /* Update with Accel */
             inp->input_type = QMAG_CAL_INPUT_TYPE_ACCEL;
             //SNS_SCM_PRINTF3(MED, "ACCEL INPUTS : %d, %d, ts, %u ",
             //            inp->sensor_data[0],
             //             inp->sensor_data[1],
             //            inp->sensor_time);
         }
         else if (sensorId == SNS_SMGR_ID_MAG_V01)
         {
             /* Update with Mag */
             inp->input_type = QMAG_CAL_INPUT_TYPE_MAG;
             //SNS_SCM_PRINTF3(MED, "MAG INPUTS : %d, %d,ts , %u",
             //            inp->sensor_data[0],
             //            inp->sensor_data[1],
             //            inp->sensor_time);
         }
         else
         {
            SNS_SCM_PRINTF0(ERROR, "Cannot understand sensor id - bail !");
            return(SNS_ERR_FAILED);
         }
         if (inp->sensor_time > 0) 
         {
            //execute the algorithm
            sns_profiling_log_qdss( SNS_SCM_ALGO_UPDATE_ENTER, 1, SNS_SCM_MAG_CAL_SVC );
            sns_scm_algo_dbase[algoInstPtr->algoIndex]->algoApi.
            sns_scm_algo_update(algoInstPtr->stateData.memPtr,
                     algoInstPtr->inputData.memPtr,
                     algoInstPtr->outputData.memPtr);
            sns_profiling_log_qdss( SNS_SCM_ALGO_UPDATE_EXIT, 1, SNS_SCM_MAG_CAL_SVC );

            SNS_SCM_PRINTF3(MED, "OUTPUT OFFSETS: %d %d %d",
              outp->offset_HI[0],
              outp->offset_HI[1],
              outp->offset_HI[2]);

            SNS_SCM_PRINTF2(MED, "after input with sns_type %d, accuracy : %d ",
                      inp->input_type,
                      outp->accuracy_HI);
         }
         //Reset the output here as we feed the hard reset
         //input to the algo update. 
         if(hard_reset)
         {
            outp->accuracy_HI = QMAG_CAL_ACCURACY_FAILED;
            //the passed timestamp already accounts for the offset.
            outp->offset_time = timestamp;
            outp->offset_updated = true;

         }
         if (outp->offset_updated || hard_reset)
         {
            sns_scm_process_algo_result(algoInstId);
         }
         sns_scm_log_algo_result(algoInstId, timestamp);
      }
      else
      {
         SNS_SCM_PRINTF0(ERROR, "Skipping sample");

      }
   }

   return SNS_SUCCESS;
}

/*=========================================================================
  FUNCTION:  sns_scm_process_smgr_buffer
  =========================================================================*/
/*!
  @brief Processes samples in the buffering indication received from SMGR

  @param[i] indPtr: Pointer to buffering indication message
  @param[i] dataReqId: Index of entry in data request database

  @return Sensors error code
*/
/*=======================================================================*/
STATIC sns_err_code_e sns_scm_process_smgr_buffer(
   const sns_smgr_buffering_ind_msg_v01 *indPtr,
   uint8_t dataReqId)
{
   uint32_t i;
   sns_err_code_e err = SNS_SUCCESS;
   const sns_smgr_buffering_sample_s_v01 * samplePtr[SNS_SMGR_BUFFERING_REQUEST_MAX_ITEMS_V01];
   uint32_t sampleCount[SNS_SMGR_BUFFERING_REQUEST_MAX_ITEMS_V01] = { 0 };
   uint32_t timestamp[SNS_SMGR_BUFFERING_REQUEST_MAX_ITEMS_V01] = { 0 };
   uint32_t samplesProcessed = 0;

   SNS_OS_MEMSET( samplePtr, 0, sizeof(samplePtr) );

   for (i = 0; i < indPtr->Indices_len; i++)
   {
      sampleCount[i] = indPtr->Indices[i].SampleCount;
      timestamp[i] = indPtr->Indices[i].FirstSampleTimestamp;

      if ( sampleCount[i] > 0 )
      {
         samplePtr[i] = &indPtr->Samples[indPtr->Indices[i].FirstSampleIdx]; 
      }
   }

   while( samplesProcessed < indPtr->Samples_len )
   {
      uint32_t minTimestamp = 0;
      uint32_t oldestSampleIdx = 0;
      const sns_smgr_buffering_sample_s_v01 * oldestSamplePtr = NULL;

      samplesProcessed++;
      for (i = 0; i < indPtr->Indices_len; i++)
      {
         // Find first valid sample to initialize min timestamp
         if( samplePtr[i] != NULL )
         {
            minTimestamp = timestamp[i];
            oldestSampleIdx = i;
            break;
         }
      }

      for (i = oldestSampleIdx + 1; i < indPtr->Indices_len; i++)
      {
         // Find first minimum based on timestamp
         if( samplePtr[i] != NULL &&
             ((timestamp[i] < minTimestamp && (minTimestamp - timestamp[i] < UINT32_MAX / 2)) ||
              (timestamp[i] > minTimestamp && (timestamp[i] - minTimestamp > UINT32_MAX / 2)) ))
         {
            minTimestamp = timestamp[i];
            oldestSampleIdx = i;
         }
      }

      oldestSamplePtr = samplePtr[oldestSampleIdx];
      if( !oldestSamplePtr )
      {
         SNS_SCM_PRINTF1( ERROR, "SCM bad sample ptr for index %d", oldestSampleIdx );
         err = SNS_ERR_BAD_PTR;
         break;
      }

      if( sampleCount[oldestSampleIdx] > 1 )
      {
         samplePtr[oldestSampleIdx]++;
         timestamp[oldestSampleIdx] += (samplePtr[oldestSampleIdx]->TimeStampOffset);
         sampleCount[oldestSampleIdx]--;
      }
      else
      {
         samplePtr[oldestSampleIdx] =  NULL;
         timestamp[oldestSampleIdx] = UINT32_MAX;
         sampleCount[oldestSampleIdx] = 0;
      }

      if (oldestSamplePtr->Flags == SNS_SMGR_ITEM_FLAG_INVALID_V01 ||
          oldestSamplePtr->Quality ==
          SNS_SMGR_ITEM_QUALITY_INVALID_FAILED_SENSOR_V01 ||
          oldestSamplePtr->Quality ==
          SNS_SMGR_ITEM_QUALITY_INVALID_NOT_READY_V01 ||
          oldestSamplePtr->Quality ==
          SNS_SMGR_ITEM_QUALITY_INVALID_SUSPENDED_V01)
      {
         SNS_SCM_DEBUG1(MEDIUM, DBG_SCM_SMGR_IND_DROPPED, oldestSampleIdx);
         continue;
      }

      //execute algorithms waiting for this sensor data
      sns_profiling_log_qdss( SNS_SCM_PROCESS_SMGR_ITEM_ENTER, 1,
         indPtr->Indices[oldestSampleIdx].SensorId );
      err = sns_scm_process_smgr_item(oldestSamplePtr, minTimestamp,
                    dataReqId, indPtr->Indices[oldestSampleIdx].SensorId);
      sns_profiling_log_qdss( SNS_SCM_PROCESS_SMGR_ITEM_EXIT, 1,
         indPtr->Indices[oldestSampleIdx].SensorId );
   }
   SNS_SCM_PRINTF1( LOW, "SCM processed %d samples from buffering indication", samplesProcessed );
   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_process_smgr_buffering_ind
  =========================================================================*/
/*!
  @brief Processes the buffering indication received from sensors manager

  @param[i] smgrIndPtr: Pointer to sensors manager buffering indication msg

  @return Sensors error code
*/
/*=======================================================================*/
STATIC sns_err_code_e sns_scm_process_smgr_buffering_ind(const void *smgrIndPtr)
{
   sns_smgr_buffering_ind_msg_v01* indPtr;
   uint8_t dataReqId = SNS_SCM_INVALID_ID;
   sns_scm_data_req_s * dataReqPtr = NULL;
   uint32_t i, j;
   sns_err_code_e err;
   bool foundType = false;

   indPtr = (sns_smgr_buffering_ind_msg_v01*)smgrIndPtr;
   dataReqId = indPtr->ReportId;
   if (dataReqId >= SNS_SCM_MAX_DATA_REQS ||
       sns_scm_data_req_dbase[dataReqId] == NULL)
   {
      SNS_SCM_DEBUG1(ERROR, DBG_SCM_SMGR_IND_DROPPED, dataReqId);
      return SNS_ERR_FAILED;
   }

   if( indPtr->Indices_len == 0 || indPtr->Samples_len == 0 )
   {
      SNS_SCM_DEBUG1(ERROR, DBG_SCM_SMGR_IND_DROPPED, dataReqId);
      return SNS_ERR_FAILED;
   }

   //Validate sensor data
   dataReqPtr = sns_scm_data_req_dbase[dataReqId];
   for( i = 0; !foundType && i < indPtr->Indices_len; i++ )
   {
      // Check if report has at least one of the types that was requested
      for( j = 0; !foundType && j < dataReqPtr->sensorCount; j++ )
      {
         if( dataReqPtr->sensorDbase[j].sensorId == indPtr->Indices[i].SensorId &&
             dataReqPtr->sensorDbase[j].dataType == indPtr->Indices[i].DataType )
         {
            foundType = true;
         }
      }
   }
   SNS_SCM_PRINTF3(LOW, "SCM: got buffering ind with %d samples and %d types, found_type %d",
                   indPtr->Samples_len, indPtr->Indices_len, foundType);

   //TODO: If report has types that were not requested, bail
   if( !foundType )
   {
      SNS_SCM_DEBUG1(ERROR, DBG_SCM_SMGR_IND_DROPPED, dataReqId);
      return SNS_ERR_FAILED;
   }

#if defined(SNS_PLAYBACK_SKIP_SMGR)
   if(sns_scm_pb_update_next_report_time==true)
   {
      if(sns_scm_pb_next_report_time < indPtr->Item[0].TimeStamp)
      {
         //Simulate timeout
         sns_scm_timer_cb((void*)sns_scm_pb_gyro_cal_algo_svc_id);
         sns_scm_pb_next_report_time += sns_scm_pb_report_timeout;
      }
   }
#endif

   //execute algorithms waiting for this sensor data
   sns_profiling_log_qdss( SNS_SCM_PROCESS_SMGR_BUFF_ENTER, 3,
      indPtr->Samples_len, indPtr->Indices_len, foundType );
   err = sns_scm_process_smgr_buffer( indPtr, dataReqId);
   sns_profiling_log_qdss( SNS_SCM_PROCESS_SMGR_BUFF_EXIT, 0 );
   return err;
}

bool sns_scm_enable_algo_in_qmd_state(uint8_t motion_state,
                                      const sns_scm_algo_s* algo)
{
   switch (algo->motionTrigFlags)
   {
      case  MOTION_DETECT_FLAG_DONT_CARE:
      {
         return (true); // Algo does not depend on motion state, enable it
         break;
      }

      case  MOTION_DETECT_FLAG_ACTIVE_IN_REST:
      {
         return (motion_state == SNS_SAM_MOTION_REST_V01); // Algo wants to be in rest
         break;
      }

      case  MOTION_DETECT_FLAG_ACTIVE_IN_MOTION:
      {
         return (motion_state == SNS_SAM_MOTION_MOVE_V01); // Algo wants to be in rest
         break;
      }

      default:
      {
         // All possible values have been captured above - this is a
         // catch all for any future expansions
         return(true);
      }

   }
}

/*=========================================================================
  FUNCTION:  sns_scm_process_sensor_status_ind
  =========================================================================*/
/*!
  @brief Processes sensor status indication received from sensors manager

  @param[i] smgrIndPtr: Pointer to sensor status indication message

  @return Sensors error code
*/
/*=======================================================================*/
STATIC sns_err_code_e sns_scm_process_sensor_status_ind(
   const void *smgrIndPtr)
{
   uint8_t i;
   bool found = false;
   sns_smgr_sensor_status_monitor_ind_msg_v02* indPtr =
      (sns_smgr_sensor_status_monitor_ind_msg_v02*)smgrIndPtr;

   SNS_SCM_PRINTF2(
     LOW, "SCM: sensor %d, #clients %d", indPtr->sensor_id, indPtr->num_clients);

   /*Update sensor state*/
   for (i=0; i<SNS_SCM_MAX_SNS_MON &&
             sns_scm_sensor_dbase[i].sensorId != SNS_SCM_INVALID_ID; i++)
   {
      if (sns_scm_sensor_dbase[i].sensorId == indPtr->sensor_id)
      {
         found = true;

         SNS_SCM_PRINTF3(
           LOW, "sensor_status - db=%u status=%u #clients=%u",
           i, sns_scm_sensor_dbase[i].status, sns_scm_sensor_dbase[i].num_clients);

         if (indPtr->num_clients == sns_scm_sensor_dbase[i].num_clients)
         {
            break; /* ignores other changes */
         }

         if (indPtr->num_clients == 0)
         {
            sns_scm_sensor_dbase[i].status = SNS_SMGR_SENSOR_STATUS_IDLE_V01;
         }
         else if ((indPtr->num_clients == 1) &&
                  (sns_scm_sensor_dbase[i].status == SNS_SMGR_SENSOR_STATUS_ACTIVE_V01))
         {
            sns_scm_sensor_dbase[i].status = SNS_SMGR_SENSOR_STATUS_ONE_CLIENT_V01;
         }
         else
         {
            sns_scm_sensor_dbase[i].status = SNS_SMGR_SENSOR_STATUS_ACTIVE_V01;
         }
         sns_scm_sensor_dbase[i].num_clients = indPtr->num_clients;

         SNS_SCM_PRINTF2(
           LOW, "SCM: dbase=%u status=%u", i, sns_scm_sensor_dbase[i].status);

         sns_scm_reg_sensor_status_hyst_timer(i);
         break;
      }
   }

   if (!found)
   {
      SNS_SCM_PRINTF1(LOW, "SCM: Was not monitoring sensor %d, bailing",
                     indPtr->sensor_id);
      return SNS_ERR_FAILED;
   }

   return SNS_SUCCESS;
}

/*=========================================================================
  FUNCTION:  sns_scm_get_algo_handle
  =========================================================================*/
/*!
  @brief Get the handle to the specified algorithm in the algorithm database

  @param[i] algoSvcId: algorithm service id

  @return handle to the specified algorithm if found,
          NULL otherwise
*/
/*=======================================================================*/
sns_scm_algo_s* sns_scm_get_algo_handle(
   uint8_t algoSvcId)
{
   uint8_t i;

   for (i = 0; i < SNS_SCM_NUM_ALGO_SVCS && sns_scm_algo_dbase[i] != NULL; i++)
   {
      if (sns_scm_algo_dbase[i]->serviceId == algoSvcId)
      {
         return sns_scm_algo_dbase[i];
      }
   }
   SNS_SCM_DEBUG1(ERROR, DBG_SCM_GET_ALGO_INDX_ERR, algoSvcId);

   return NULL;
}

/*=========================================================================
  FUNCTION:  sns_scm_process_msg
  =========================================================================*/
/*!
  @brief Process the messages from SCM input message queue

  @return Sensors error code
*/
/*=======================================================================*/
STATIC sns_err_code_e sns_scm_process_msg(void)
{
  scm_q_item_s           *msgPtr      = NULL;
  sns_err_code_e         error_code   = SNS_SUCCESS;

  while (NULL != (msgPtr = (scm_q_item_s *)sns_scm_q_get(SCM_SAM_SMGR_MSG_SIG)))
  {
    error_code   = SNS_SUCCESS;

    SNS_SCM_PRINTF2(LOW, "SCM: got msg_id %d, scm_msg_type %d",
                   msgPtr->msg_id, msgPtr->scm_msg_type);

    /* process SCM - SMGR indications */
    if (msgPtr->scm_msg_type == SCM_SMGR_IND_MSG)
    {
      if (msgPtr->msg_id == SNS_SMGR_BUFFERING_IND_V01)
      {
        sns_profiling_log_qdss( SNS_SCM_PROCESS_SMGR_IND_ENTER, 0 );
        error_code = sns_scm_process_smgr_buffering_ind( msgPtr->body_ptr);
        sns_profiling_log_qdss( SNS_SCM_PROCESS_SMGR_IND_EXIT, 0 );
      }
      else
      {
        SNS_SCM_PRINTF0(ERROR, "data in SCM_SMGR_IND_MSG_SIG is invalid");
      }
    }
    /* process SCM - SMGR Internal indications */
    else if (msgPtr->scm_msg_type == SCM_SMGR_INT_IND_MSG)
    {
      if (msgPtr->msg_id == SNS_SMGR_SENSOR_STATUS_MONITOR_IND_V02)
      {
        error_code = sns_scm_process_sensor_status_ind( msgPtr->body_ptr);
      }
    }
    /* process SCM - SAM indications */
    else if (msgPtr->scm_msg_type == SCM_SAM_IND_MSG)
    {
      error_code = sns_scm_process_sam_ind(msgPtr->body_ptr);
    }
    /* process SCM - SMGR responses */
    else if (msgPtr->scm_msg_type == SCM_SMGR_RESP_MSG)
    {
      error_code = sns_scm_process_smgr_resp(msgPtr->body_ptr, msgPtr->msg_id);
    }
    /* process SCM - SMGR Internal responses */
    else if (msgPtr->scm_msg_type == SCM_SMGR_INT_RESP_MSG)
    {
      error_code = sns_scm_process_smgr_int_resp(msgPtr->body_ptr, msgPtr->msg_id);
    }
    /* process SCM - SAM responses */
    else if (msgPtr->scm_msg_type == SCM_SAM_RESP_MSG)
    {
      error_code = sns_scm_process_sam_resp(msgPtr->body_ptr, msgPtr->msg_id);
    }
    else
    {
      SNS_SCM_PRINTF1(ERROR, "Invalid SCM message type %d", msgPtr->scm_msg_type);
    }

    if (error_code != SNS_SUCCESS)
    {
      SNS_SCM_PRINTF3(ERROR,
                      "SCM: msg processing error %d, msg_id %d, scm_msg_type %d",
                      error_code, msgPtr->msg_id, msgPtr->scm_msg_type);
    }

    SNS_OS_FREE(msgPtr->body_ptr);
    SNS_OS_FREE(msgPtr);
    msgPtr = NULL;
  }
   return SNS_SUCCESS;
}

/*=========================================================================
  FUNCTION:  sns_scm_reg_algo_svc
  =========================================================================*/
/*!
  @brief Register the algorithm with SCM. This is expected to be done
         at SCM initialization for all algorithms to be handled by SCM

  @param[i] algoSvcId: Algorithm service id

  @return Sensors error code
*/
/*=======================================================================*/
STATIC sns_err_code_e sns_scm_reg_algo_svc(
   uint8_t algoSvcId)
{
   uint8_t algoIndex,j;
   for (algoIndex = 0;
       algoIndex < SNS_SCM_NUM_ALGO_SVCS && sns_scm_algo_dbase[algoIndex] != NULL;
       algoIndex++)
   {
      if (sns_scm_algo_dbase[algoIndex]->serviceId == algoSvcId)
      {
         SNS_SCM_DEBUG1(ERROR, DBG_SCM_REG_ALGO_ERR, algoSvcId);
         return SNS_SUCCESS;
      }
   }

   if (algoIndex >= SNS_SCM_NUM_ALGO_SVCS)
   {
      return SNS_ERR_FAILED;
   }

   sns_scm_algo_dbase[algoIndex] = SNS_OS_MALLOC(SNS_SCM_DBG_MOD,
                                                 sizeof(sns_scm_algo_s));
   SNS_ASSERT(sns_scm_algo_dbase[algoIndex] != NULL);

   sns_scm_algo_dbase[algoIndex]->serviceId = algoSvcId;
   sns_scm_algo_dbase[algoIndex]->timer = NULL;
   sns_scm_algo_dbase[algoIndex]->timeout = false;
   sns_scm_algo_dbase[algoIndex]->defSensorReportRate = 0;

   for (j=0; j<SNS_SCM_MAX_REG_ITEMS_PER_ALGO; j++)
   {
      sns_scm_algo_dbase[algoIndex]->regItemType[j]=SNS_SCM_REG_ITEM_TYPE_NONE;
   }

   return(sns_scm_reg_algo(sns_scm_algo_dbase[algoIndex]));
}

/*=========================================================================
  FUNCTION:  sns_scm_reg_algos
  =========================================================================*/
/*!
  @brief Register all algorithms

  @return None
*/
/*=======================================================================*/
STATIC void sns_scm_reg_algos(void)
{
   //initialise the sensor status database with sensors to be monitored
   int k = 0;
   uint8_t snsIdx, algoIdx, IdIdx, algoCount = 0;

   /* Monitor Accel... */
   sns_scm_sensor_dbase[k].sensorId = SNS_SMGR_ID_ACCEL_V01;
   sns_scm_sensor_dbase[k].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
   sns_scm_sensor_dbase[k].auto_cal_reg_id = SNS_REG_SCM_GROUP_ACCEL_DYN_CAL_PARAMS_V02;
   k++;

   /* and Gyro...*/
   sns_scm_sensor_dbase[k].sensorId = SNS_SMGR_ID_GYRO_V01;
   sns_scm_sensor_dbase[k].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
   sns_scm_sensor_dbase[k].auto_cal_reg_id = SNS_REG_SCM_GROUP_GYRO_DYN_CAL_PARAMS_V02;
   k++;

   sns_scm_sensor_dbase[k].sensorId = SNS_SMGR_ID_GAME_ROTATION_VECTOR_V01;
   sns_scm_sensor_dbase[k].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
   sns_scm_sensor_dbase[k].auto_cal_reg_id = SNS_REG_SCM_GROUP_GYRO_DYN_CAL_PARAMS_V02;
   k++;

   sns_scm_reg_algo_svc(SNS_SCM_GYRO_CAL_SVC);
   algoCount++;

#ifdef SNS_SCM_MAG_CAL
   /* And Mag */
   sns_scm_sensor_dbase[k].sensorId = SNS_SMGR_ID_MAG_V01;
   sns_scm_sensor_dbase[k].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
   sns_scm_sensor_dbase[k].auto_cal_reg_id = SNS_REG_SCM_GROUP_MAG_DYN_CAL_PARAMS_V02;
   k++;

   sns_scm_reg_algo_svc(SNS_SCM_MAG_CAL_SVC);
   algoCount++;
#endif

   // Cross-checks and locates all sensors that should monitor this service
   for (snsIdx = 0; snsIdx < SNS_SCM_MAX_SNS_MON && snsIdx < k; snsIdx++)
   {
      sns_scm_sensor_dbase[snsIdx].monitorAlgoIndex = SNS_SCM_INVALID_ID;
      for (algoIdx = 0; algoIdx < algoCount; algoIdx++)
      {
         for (IdIdx = 0; IdIdx < sns_scm_algo_dbase[algoIdx]->monitorSensorCount; IdIdx++)
         {
            if (sns_scm_sensor_dbase[snsIdx].sensorId ==
                sns_scm_algo_dbase[algoIdx]->monitorSensorId[IdIdx])
            {
               sns_scm_sensor_dbase[snsIdx].monitorAlgoIndex = algoIdx;
            }
         }
      }
   }
}

/*=========================================================================
  FUNCTION:  sns_scm_reg_smgr
  =========================================================================*/
/*!
  @brief Register sensors that are registered to at least one service.

  @return None
*/
/*=======================================================================*/
STATIC void sns_scm_reg_smgr(void)
{
#ifndef SNS_QDSP_SIM
   uint8_t k;

   for (k = 0;
        k < SNS_SCM_MAX_SNS_MON &&
          sns_scm_sensor_dbase[k].sensorId != SNS_SCM_INVALID_ID;
        k++)
   {
      if(sns_scm_sensor_dbase[k].monitorAlgoIndex != SNS_SCM_INVALID_ID)
      {
         sns_scm_reg_sensor_status(sns_scm_sensor_dbase[k].sensorId);
      }
   }
#endif
}

/*=========================================================================
  FUNCTION:  sns_scm_handle_sensor_status_hyst_timeout
  =========================================================================*/
/*!
  @brief Handle the timeout indicating sensor status hysteresis period is complete

  @return None
*/
/*=======================================================================*/
STATIC void sns_scm_handle_sensor_status_hyst_timeout(void)
{
   uint8_t sensorIndex,i,monitorAlgoIndex;
   sns_scm_sensor_s* sensorPtr;
   bool enable_algo;

   for (sensorIndex = 0; sensorIndex < SNS_SCM_MAX_SNS_MON; sensorIndex++)
   {
      sensorPtr = &sns_scm_sensor_dbase[sensorIndex];

      if (sensorPtr != NULL && sensorPtr->timer != NULL)
      {
         if (sensorPtr->timeout == true)
         {
            sensorPtr->timeout = false;
            monitorAlgoIndex = sensorPtr->monitorAlgoIndex;

            /* Look for the corresponding algo service */
            sns_scm_update_sensor_status(sns_scm_algo_dbase[monitorAlgoIndex]);

            /* The code below is custom designed for algos that depend on QMD state,
            we must bypass QMD specific logic for some algos */

            SNS_SCM_PRINTF2(LOW, "sensor status hysteresis timeout sensorId=%d overall_status=%d",
                            sensorPtr->sensorId, sns_scm_algo_dbase[monitorAlgoIndex]->monitorSensorStatus);

            switch (sns_scm_algo_dbase[monitorAlgoIndex]->monitorSensorStatus)
            {
               case SNS_SMGR_SENSOR_STATUS_ACTIVE_V01:
               {
                  if (!scm_qmd_state.qmd_enabled)
                  {
                     for (i = 0; (i < SNS_SCM_NUM_ALGO_SVCS) && (sns_scm_algo_dbase[i] != NULL); i++)
                     {
                        if(sns_scm_algo_dbase[i]->enableAlgo   /* algo can be enabled */
                           && sns_scm_algo_dbase[i]->motionTrigFlags != MOTION_DETECT_FLAG_DONT_CARE) /* and needs AMD information */
                        {
                           sns_scm_send_qmd_start_req();
                           break;
                        }
                     }
                  }

                  enable_algo = sns_scm_enable_algo_in_qmd_state(scm_qmd_state.qmd_state, sns_scm_algo_dbase[monitorAlgoIndex]);

                  SNS_SCM_PRINTF3(LOW, "SCM: Enable algo %d, %d, %d",
                                       sns_scm_algo_dbase[monitorAlgoIndex]->enableAlgo,
                                       sensorPtr->sensorId,
                                       enable_algo);
                  if ( sns_scm_algo_dbase[monitorAlgoIndex]->enableAlgo && enable_algo)
                  {
                     SNS_SCM_PRINTF2(LOW, "SCM: Enabling algo %d, for sensor %d", monitorAlgoIndex, sensorPtr->sensorId);
                     sns_scm_enable_algo(monitorAlgoIndex);
                  }
                  break; // end case SMGR_SENSOR_STATUS_ACTIVE
               }

               case SNS_SMGR_SENSOR_STATUS_IDLE_V01:
               {
                  uint8_t algoIndex, qmdAlgoCnt = sns_scm_algo_inst_count;

                  /* Checks all algo instances and disables algo if needed */
                  for (i=0; i < SNS_SCM_MAX_ALGO_INSTS && qmdAlgoCnt > 0; i++)
                  {
                     if (sns_scm_algo_inst_dbase[i] != NULL)
                     {
                        if (sns_scm_algo_inst_dbase[i]->algoIndex == monitorAlgoIndex)
                        {
                           qmdAlgoCnt--;
                           sns_scm_disable_algo(monitorAlgoIndex);
                           break;
                        }
                        else
                        {
                           algoIndex = sns_scm_algo_inst_dbase[i]->algoIndex;
                           if(sns_scm_algo_dbase[algoIndex]->motionTrigFlags == MOTION_DETECT_FLAG_DONT_CARE)
                           {
                              qmdAlgoCnt--;
                           }
                        }
                     }
                  }

                  if (scm_qmd_state.qmd_enabled && qmdAlgoCnt == 0 &&
                      scm_qmd_state.qmd_inst_id_valid && !sns_scm_active_qmd_client())
                  {
                     sns_scm_send_qmd_stop_req();
                  }

                  // store auto cal param, algo state in registry
                  if (sns_scm_algo_dbase[monitorAlgoIndex] != NULL && sns_scm_algo_dbase[monitorAlgoIndex]->enableAlgo)
                  {
                     sns_scm_store_auto_cal_params_registry(monitorAlgoIndex);
                     sns_scm_store_auto_cal_state_registry(monitorAlgoIndex);
                  }
                  break; // end case SMGR_SENSOR_STATUS_IDLE
               }

               case SNS_SMGR_SENSOR_STATUS_ONE_CLIENT_V01:
               {
                  if (sns_scm_algo_inst_count == 0 &&
                      scm_qmd_state.qmd_enabled &&
                      scm_qmd_state.qmd_inst_id_valid &&
                      scm_qmd_state.qmd_state != SNS_SAM_MOTION_MOVE_V01 &&
                      sensorPtr->sensorId != SNS_SMGR_ID_MAG_V01 &&
                      !sns_scm_active_qmd_client() )
                  {
                     sns_scm_send_qmd_stop_req();
                  }

                  /* Forcing the loop below only for the Mag */
                  for (i=0; i < SNS_SCM_MAX_ALGO_INSTS; i++)
                  {
                     if (sns_scm_algo_inst_dbase[i] != NULL)
                     {
                        if (sns_scm_algo_inst_dbase[i]->algoIndex == monitorAlgoIndex)
                        {
                           if(sensorPtr->sensorId == SNS_SMGR_ID_MAG_V01)
                           {
                              SNS_SCM_PRINTF2(LOW, "Disabling Mag Cal %d, %d",
                                                   0, 0);
                              sns_scm_disable_algo(monitorAlgoIndex);
                              break;
                           }
                           else if (sensorPtr->sensorId == SNS_SMGR_ID_GYRO_V01)
                           {
                              sns_scm_stop_sensor_data(i);
                              break;
                           }
                        }
                     }
                  }

                  break; // end case SMGR_SENSOR_STATUS_ONE_CLIENT
               }
               case SNS_SMGR_SENSOR_STATUS_UNKNOWN_V01:
               default:
                  break;

            }

         }
      }
   }
}

/*=========================================================================
  FUNCTION:  sns_scm_handle_report_timeout
  =========================================================================*/
/*!
  @brief Handle the timeout indicating periodic report to client is due

  @return None
*/
/*=======================================================================*/
STATIC void sns_scm_handle_report_timeout(void)
{
   uint8_t algoIndex;
   sns_scm_algo_s* algoPtr;

   for (algoIndex = 0; algoIndex < SNS_SCM_NUM_ALGO_SVCS; algoIndex++)
   {
      algoPtr = sns_scm_algo_dbase[algoIndex];

      if (algoPtr != NULL && algoPtr->timer != NULL)
      {
         if (algoPtr->timeout == true)
         {
            algoPtr->timeout = false;

            if (algoPtr->monitorSensorStatus == SNS_SMGR_SENSOR_STATUS_ACTIVE_V01 ||
                algoPtr->monitorSensorStatus == SNS_SMGR_SENSOR_STATUS_ONE_CLIENT_V01)
            {
               uint8_t algoInstId = sns_scm_find_algo_inst(
                  algoPtr->serviceId,
                  algoPtr->defConfigData.memPtr);

               if (algoInstId < SNS_SCM_MAX_ALGO_INSTS)
               {
                  //reset algorithm state
                  algoPtr->algoApi.sns_scm_algo_reset(
                     sns_scm_algo_inst_dbase[algoInstId]->configData.memPtr,
                     sns_scm_algo_inst_dbase[algoInstId]->stateData.memPtr);

                  //start sensor data
                  sns_scm_req_sensor_data(algoInstId);
               }
            }
         }
      }
   }
}

/*=========================================================================
  FUNCTION:  sns_scm_init_cal_algos
  =========================================================================*/
/*!
  @brief Initialize calibration algorithms

  @return none
*/
/*=======================================================================*/
STATIC void sns_scm_init_cal_algos(void)
{
   uint8_t i, j, numRegQuery = 0;
   sns_err_code_e err;

   /* Send registry requests for initial dynamic calibration parameters */
   for (i = 0; i < SNS_SCM_MAX_SNS_MON &&
        sns_scm_sensor_dbase[i].sensorId != SNS_SCM_INVALID_ID; i++)
   {
      if (sns_scm_sensor_dbase[i].auto_cal_reg_id != SNS_SCM_INVALID_ID &&
          ((sns_scm_sensor_dbase[i].sensorId == SNS_SMGR_ID_GYRO_V01) ||
           (sns_scm_sensor_dbase[i].sensorId == SNS_SMGR_ID_MAG_V01)))
      {
         err = sns_scm_req_reg_data(SNS_SCM_REG_ITEM_TYPE_GROUP,
                                    sns_scm_sensor_dbase[i].auto_cal_reg_id);

         if ( SNS_SUCCESS != err )
         {
            /* Could not send request message. Use default cal. Already set for
               default at start of init process */
            SNS_SCM_DEBUG3(ERROR, DBG_SCM_REG_REQ_FAIL,
                           sns_scm_sensor_dbase[i].sensorId,
                           sns_scm_sensor_dbase[i].auto_cal_reg_id,
                           err);
         }
         else
         {
            numRegQuery++;
         }
      }
   }

   /* Send registry requests for algorithm configuration parameters */
   for (i = 0;
       i < SNS_SCM_NUM_ALGO_SVCS && sns_scm_algo_dbase[i] != NULL; i++)
   {
      for ( j = 0;
            j < SNS_SCM_MAX_REG_ITEMS_PER_ALGO && sns_scm_algo_dbase[i]->regItemType[j] != SNS_SCM_REG_ITEM_TYPE_NONE;
            j++)
      {
         if (sns_scm_algo_dbase[i]->regItemType[j] != SNS_SCM_REG_ITEM_TYPE_NONE)
         {
            err = sns_scm_req_reg_data(sns_scm_algo_dbase[i]->regItemType[j],
                                       sns_scm_algo_dbase[i]->regItemId[j]);

            SNS_SCM_PRINTF2(LOW, "SCM Algo Init Registry Req Type:%d, Id:%d",
                            sns_scm_algo_dbase[i]->regItemType[j],
                            sns_scm_algo_dbase[i]->regItemId[j]);

            if ( SNS_SUCCESS != err )
            {
               /* Could not send request message. Use default configuration. */
               SNS_SCM_DEBUG3(ERROR, DBG_SCM_REG_REQ_FAIL,
                              sns_scm_algo_dbase[i]->serviceId,
                              sns_scm_algo_dbase[i]->regItemId[j],
                              err);
            }
            else
            {
               numRegQuery++;
            }
         }
      }
   }

   /* Process registry responses */
   if (numRegQuery)
   {
      if (sns_scm_reg_init_timer(sns_scm_sig_event) == SNS_SUCCESS)
      {
         sns_scm_process_init_events(numRegQuery, sns_scm_sensor_dbase);
      }

      sns_scm_dereg_init_timer();
   }
}

/*=========================================================================
  FUNCTION:  sns_scm_smgr_resp_cb
  =========================================================================*/
/*!
  @brief SCM - SMGR response callback

  @detail
  Handles the responses from SMGR to SCM.

  @param[i]   user_handle         Handle used by the infrastructure to identify different clients.
  @param[i]   msg_id                Message ID
  @param[i]   resp_buf              Pointer to the response buffer
  @param[i]   resp_buf_len        Length of the response buffer
  @param[i]   user_cb_data       Pointer to the User-data
  @param[i]   transp_err           Error code

  @return None
*/
/*=======================================================================*/
void sns_scm_smgr_resp_cb(
   qmi_client_type user_handle,
   unsigned int msg_id,
   void *resp_buf,
   unsigned int resp_buf_len,
   void *user_cb_data,
   qmi_client_error_type transp_err)
{
  sns_scm_q_put(msg_id, resp_buf, SCM_SMGR_RESP_MSG, transp_err, 0, 0);
}

/*=========================================================================
  FUNCTION:  sns_scm_smgr_int_resp_cb
  =========================================================================*/
/*!
  @brief SCM - SMGR response callback

  @detail
  Handles the responses from SMGR Internal to SCM.

  @param[i]   user_handle         Handle used by the infrastructure to identify different clients.
  @param[i]   msg_id                Message ID
  @param[i]   resp_buf              Pointer to the response buffer
  @param[i]   resp_buf_len        Length of the response buffer
  @param[i]   user_cb_data       Pointer to the User-data
  @param[i]   transp_err           Error code

  @return None
*/
/*=======================================================================*/
void sns_scm_smgr_int_resp_cb(
   qmi_client_type user_handle,
   unsigned int msg_id,
   void *resp_buf,
   unsigned int resp_buf_len,
   void *user_cb_data,
   qmi_client_error_type transp_err)
{
  sns_scm_q_put(msg_id, resp_buf, SCM_SMGR_INT_RESP_MSG, transp_err, 0, 0);
}

/*=========================================================================
  FUNCTION:  sns_scm_sam_resp_cb
  =========================================================================*/
/*!
  @brief SCM - SAM response callback

  @detail
  Handles the responses from SAM to SCM.

  @param[i]   user_handle         Handle used by the infrastructure to identify different clients.
  @param[i]   msg_id                Message ID
  @param[i]   resp_buf              Pointer to the response buffer
  @param[i]   resp_buf_len        Length of the response buffer
  @param[i]   user_cb_data       Pointer to the User-data
  @param[i]   transp_err           Error code

    @return None
*/
/*=======================================================================*/
void sns_scm_sam_resp_cb (
   qmi_client_type user_handle,
   unsigned int msg_id,
   void *resp_buf,
   unsigned int resp_buf_len,
   void *user_cb_data,
   qmi_client_error_type transp_err)
{
  sns_scm_q_put(msg_id, resp_buf, SCM_SAM_RESP_MSG, transp_err, 0, 0);
}

/*=========================================================================
  FUNCTION:  sns_scm_smgr_ind_cb
  =========================================================================*/
/*!
  @brief SCM - SMGR indication callback

  @detail
  Handles the indications from SMGR to SCM.

  @param[i]   user_handle         Handle used by the infrastructure to identify different clients.
  @param[i]   msg_id                Message ID
  @param[i]   ind_buf                Pointer to the indication buffer
  @param[i]   ind_buf_len          Length of the indication buffer
  @param[i]   user_cb_data       Pointer to the User-data

  @return None
*/
/*=======================================================================*/
void sns_scm_smgr_ind_cb(
   qmi_client_type user_handle,
   unsigned int msg_id,
   void *ind_buf,
   unsigned int ind_buf_len,
   void *user_cb_data)
{
   int32  decode_msg_len = 0;

   if (msg_id == SNS_SMGR_BUFFERING_IND_V01)
   {
      decode_msg_len = sizeof (sns_smgr_buffering_ind_msg_v01);
   }
   else if (msg_id == SNS_SMGR_REPORT_IND_V01)
   {
      decode_msg_len = sizeof (sns_smgr_periodic_report_ind_msg_v01);
   }
   else
   {
      SNS_SCM_PRINTF1(ERROR, "error in msg_id = %d", msg_id);

      return;
   }

   sns_scm_q_put(msg_id, ind_buf, SCM_SMGR_IND_MSG, QMI_NO_ERR, ind_buf_len, decode_msg_len);
}

/*=========================================================================
  FUNCTION:  sns_scm_smgr_ind_cb
  =========================================================================*/
/*!
  @brief SCM - SMGR indication callback

  @detail
  Handles the indications from SMGR to SCM.

  @param[i]   user_handle         Handle used by the infrastructure to identify different clients.
  @param[i]   msg_id                Message ID
  @param[i]   ind_buf                Pointer to the indication buffer
  @param[i]   ind_buf_len          Length of the indication buffer
  @param[i]   user_cb_data       Pointer to the User-data

  @return None
*/
/*=======================================================================*/
void sns_scm_smgr_int_ind_cb(
   qmi_client_type user_handle,
   unsigned int msg_id,
   void *ind_buf,
   unsigned int ind_buf_len,
   void *user_cb_data)
{
   int32  decode_msg_len = 0;

   if (msg_id == SNS_SMGR_SENSOR_STATUS_MONITOR_IND_V02)
   {
      decode_msg_len = sizeof (sns_smgr_sensor_status_monitor_ind_msg_v02);
   }
   else
   {
      SNS_SCM_PRINTF1(ERROR, "smgr_int_ind_cb - error in msg_id = %d", msg_id);

      return;
   }

   sns_scm_q_put(msg_id, ind_buf, SCM_SMGR_INT_IND_MSG, QMI_NO_ERR, ind_buf_len, decode_msg_len);
}

/*=========================================================================
  FUNCTION:  sns_scm_sam_ind_cb
  =========================================================================*/
/*!
  @brief SCM - SAM indication callback

  @detail
  Handles the indications from SAM to SCM.

  @param[i]   user_handle         Handle used by the infrastructure to identify different clients.
  @param[i]   msg_id                Message ID
  @param[i]   ind_buf                Pointer to the indication buffer
  @param[i]   ind_buf_len          Length of the indication buffer
  @param[i]   user_cb_data       Pointer to the User-data

  @return None
*/
/*=======================================================================*/
void sns_scm_sam_ind_cb(
  qmi_client_type user_handle,
  unsigned int msg_id,
  void *ind_buf,
  unsigned int ind_buf_len,
  void *user_cb_data)
{
   int32  decode_msg_len = 0;

   if (msg_id == SNS_SAM_AMD_REPORT_IND_V01)
   {
      decode_msg_len = sizeof(sns_sam_qmd_report_ind_msg_v01);
   }
   else
   {
      SNS_SCM_PRINTF1(ERROR, "error in msg_id = %d", msg_id);

      return;
   }

   sns_scm_q_put(msg_id, ind_buf, SCM_SAM_IND_MSG, QMI_NO_ERR, ind_buf_len, decode_msg_len);
}

/*=========================================================================
  FUNCTION:  sns_scm_reg_ind_cb
  =========================================================================*/
/*!
  @brief SCM - REG indication callback

  @detail
  Handles the indications from REG to SCM.

  @param[i]   user_handle         Handle used by the infrastructure to identify different clients.
  @param[i]   msg_id                Message ID
  @param[i]   ind_buf                Pointer to the indication buffer
  @param[i]   ind_buf_len          Length of the indication buffer
  @param[i]   user_cb_data       Pointer to the User-data

  @return None
*/
/*=======================================================================*/
void sns_scm_reg_ind_cb(
  qmi_client_type user_handle,
  unsigned int msg_id,
  void *ind_buf,
  unsigned int ind_buf_len,
  void *user_cb_data)
{
  /* Not used currently */
}

/*=========================================================================
  FUNCTION:  sns_scm_q_get
  =========================================================================*/
/*!
  @brief Get the SCM data from QUEUE

  @param[i] qflag: queue flag that is to be picked

  @return Queue data pointer
          NULL otherwise
*/
/*=======================================================================*/
void* sns_scm_q_get (OS_FLAGS qflag)
{
  void    *msg_ptr = NULL;
  uint8_t os_err   = 0;

  sns_os_mutex_pend(scm_mutex, 0, &os_err);
  if (qflag == SCM_SAM_SMGR_MSG_SIG)
  {
    msg_ptr = sns_q_get(&scm_sam_smgr_q);
  }
  else
  {
    msg_ptr = sns_q_get(&scm_reg_q);
  }
  os_err = sns_os_mutex_post(scm_mutex);

  return msg_ptr;
}

/*=========================================================================
  FUNCTION:  sns_scm_q_put
  =========================================================================*/
/*!
  @brief Put the SCM data in QUEUE

  @detail
  Puts the SCM adat in QUEUE

  @param[i]   msg_id                Message ID
  @param[i]   buffer                  Pointer to the message buffer
  @param[i]   scm_msg_type     SCM message type
  @param[i]   transp_err           Error code
  @param[i]   buf_len                Length of the message buffer
  @param[i]   decode_msg_len   Length of the message to be decoded

  @return None
*/
/*=======================================================================*/
void sns_scm_q_put(
   unsigned int msg_id,
   void *buffer,
   sns_scm_msg_type_e scm_msg_type,
   qmi_client_error_type transp_err,
   int32 buf_len,
   int32 decode_msg_len)
{
  uint8_t               os_err = 0;
  scm_q_item_s          *scm_q_ptr = NULL;
  void                  *decode_buf_ptr = NULL;
  void                  *scm_msg_buf_ptr = NULL;
  qmi_client_error_type qmi_err = 0;

  if (buffer == NULL)
  {
    SNS_SCM_PRINTF2(ERROR, "Null Buffer, scm_msg_type = %d, msg_id = %d",
                    scm_msg_type, msg_id);

    return;
  }

  switch (scm_msg_type)
  {
    case SCM_SAM_RESP_MSG:
    case SCM_SMGR_RESP_MSG:
    case SCM_SMGR_INT_RESP_MSG:
    case SCM_REG_RESP_MSG:
      if (transp_err != QMI_NO_ERR)
      {
        SNS_OS_FREE(buffer);

        SNS_SCM_PRINTF3(ERROR,
                        "Transporatation Error = %d, scm_msg_type = %d, msg_id = %d",
                        transp_err, scm_msg_type, msg_id);
        return;
      }

      scm_msg_buf_ptr = buffer;
      break;

    case SCM_SMGR_IND_MSG:
      if(scm_sam_smgr_q.cnt > SCM_MAX_IND_Q_LEN )
      {
        SNS_SCM_PRINTF0(ERROR, "SCM Queue full: Dropping SMGR indication");
         return;
      }
     //. There is no break; here. We want to fall through
     // And process this indication along with the case statements below

    case SCM_SAM_IND_MSG:
    case SCM_SMGR_INT_IND_MSG:
      decode_buf_ptr = SNS_OS_MALLOC (SNS_SCM_DBG_MOD, decode_msg_len);

      if (decode_buf_ptr ==  NULL)
      {
        SNS_SCM_PRINTF1(ERROR, "Malloc Fail for requested size = %d",
                        decode_msg_len);
        return;
      }

      if (scm_msg_type == SCM_SMGR_IND_MSG)
      {
         qmi_err = qmi_client_message_decode(scm_smgr_qmi_client_if.user_handle,
                                             QMI_IDL_INDICATION,
                                             msg_id, buffer, buf_len,
                                             decode_buf_ptr, decode_msg_len);
      }
      else if (scm_msg_type == SCM_SMGR_INT_IND_MSG)
      {
         qmi_err = qmi_client_message_decode(scm_smgr_int_qmi_client_if.user_handle,
                                             QMI_IDL_INDICATION,
                                             msg_id, buffer, buf_len,
                                             decode_buf_ptr, decode_msg_len);
      }
      else
      {
        qmi_err = qmi_client_message_decode(scm_sam_qmi_client_if.user_handle,
                                            QMI_IDL_INDICATION,
                                            msg_id, buffer, buf_len,
                                            decode_buf_ptr, decode_msg_len);
      }

      if (qmi_err != QMI_NO_ERR)
      {
         SNS_OS_FREE(decode_buf_ptr);

         SNS_SCM_PRINTF2(ERROR,
                         "qmi_client_message_decode fail error = %d, scm_msg_type = %d",
                         qmi_err, scm_msg_type);

         return;
      }

      scm_msg_buf_ptr = decode_buf_ptr;
      break;
    default:
      SNS_SCM_PRINTF1(ERROR, "Invalid scm_msg_type = %d", scm_msg_type);
      return;
  }

  scm_q_ptr = (scm_q_item_s *)
                 SNS_OS_MALLOC (SNS_SCM_DBG_MOD, sizeof(scm_q_item_s));
  if (scm_q_ptr == NULL)
  {
    SNS_OS_FREE(scm_msg_buf_ptr);

    SNS_SCM_PRINTF1(ERROR, "Malloc Fail for requested size = %d",
                    sizeof(scm_q_item_s));

    return;
  }

  scm_q_ptr->msg_id = msg_id;
  scm_q_ptr->body_ptr = scm_msg_buf_ptr;
  scm_q_ptr->scm_msg_type = scm_msg_type;
  sns_q_link(scm_q_ptr, &scm_q_ptr->q_link);

  sns_os_mutex_pend(scm_mutex, 0, &os_err);
  if (os_err != OS_ERR_NONE)
  {
    SNS_OS_FREE(scm_q_ptr->body_ptr);
    SNS_OS_FREE(scm_q_ptr);

    SNS_SCM_PRINTF1(ERROR, "Error acquiring mutex = %d", os_err);

    return ;
  }

  if (SCM_REG_RESP_MSG == scm_msg_type)
  {
    sns_q_put(&scm_reg_q, &scm_q_ptr->q_link);
  }
  else
  {
    sns_q_put(&scm_sam_smgr_q, &scm_q_ptr->q_link);
  }

  os_err = sns_os_mutex_post(scm_mutex);
  SNS_ASSERT( os_err == OS_ERR_NONE );

  if (SCM_REG_RESP_MSG == scm_msg_type)
  {
    sns_os_sigs_post (sns_scm_sig_event, SCM_REG_MSG_SIG, OS_FLAG_SET, &os_err);
  }
  else
  {
    sns_os_sigs_post (sns_scm_sig_event, SCM_SAM_SMGR_MSG_SIG, OS_FLAG_SET, &os_err);
  }
}

/*===========================================================================

  FUNCTION:   smgr_scm_qmi_client_init

===========================================================================*/
STATIC qmi_client_error_type sns_scm_qmi_client_init(
   uint8_t                     svc_num,
   uint32_t                    msg_sigs,
   uint32_t                    timer_sigs,
   qmi_idl_service_object_type svc_obj,
   qmi_client_ind_cb           ind_cb,
   scm_qmi_client_if_s*        qmi_cl_ptr
)
{
  qmi_client_error_type err;

  sns_os_sigs_add(sns_scm_sig_event, msg_sigs);
  sns_os_sigs_add(sns_scm_sig_event, timer_sigs);
  memset(&(qmi_cl_ptr->os_params),0,sizeof(qmi_cl_ptr->os_params));
  qmi_cl_ptr->os_params.ext_signal = &sns_scm_sig_event->signal;
  qmi_cl_ptr->os_params.sig        = msg_sigs;
  qmi_cl_ptr->os_params.timer_sig  = timer_sigs;
  qmi_cl_ptr->svc_num              = svc_num;
  qmi_cl_ptr->svc_obj              = svc_obj;
  qmi_cl_ptr->ind_cb               = ind_cb;

  err = qmi_client_notifier_init(svc_obj,
                                 &qmi_cl_ptr->os_params,
                                 &qmi_cl_ptr->notifier_handle);
  SNS_SCM_PRINTF3(
    HIGH, "qmi_client_notifier_init(%d) result(%d) hndl(%x)",
    svc_num, err, (unsigned)qmi_cl_ptr->notifier_handle);

  return err;
}

/*===========================================================================

  FUNCTION:   scm_qmi_client_connect_service

===========================================================================*/
STATIC void scm_qmi_client_connect_service(scm_qmi_client_if_s* qmi_cl_ptr)
{
  qmi_client_error_type err;
  qmi_service_info service_info;
  unsigned int num_entries = 1;
  unsigned int num_services;

  if ( (qmi_cl_ptr == NULL) || (qmi_cl_ptr->notifier_handle == 0) )
  {
    SNS_SCM_PRINTF2(
      MED, "client_connect_service - cl_ptr(0x%x) notifier_handle(0x%x)",
      (unsigned)qmi_cl_ptr, qmi_cl_ptr ? (unsigned)qmi_cl_ptr->notifier_handle : 0);
    return;
  }

  memset(&(qmi_cl_ptr->os_params),0,sizeof(qmi_cl_ptr->os_params));

  err = qmi_client_get_service_list(
    qmi_cl_ptr->svc_obj, &service_info, &num_entries, &num_services);

  SNS_SCM_PRINTF3(
    MED, "client_connect_service - get_service_list(%d) result(%d) #svcs(%d)",
    qmi_cl_ptr->svc_num, err, num_services);

  if ( err == QMI_NO_ERR )
  {
     err = qmi_client_init(&service_info,
                           qmi_cl_ptr->svc_obj,
                           qmi_cl_ptr->ind_cb,
                           qmi_cl_ptr,
                           &qmi_cl_ptr->os_params,
                           &qmi_cl_ptr->user_handle);
     SNS_SCM_PRINTF3(
       MED, "client_connect_service - qmi_client_init(%d) result(%d) hndl(%x)",
       qmi_cl_ptr->svc_num, err, (unsigned)qmi_cl_ptr->user_handle);
  }
}

/*===========================================================================

  FUNCTION:   scm_qmi_client_wait_for_services

===========================================================================*/
/*!
  @brief Returns only after SMGR, SAM, and REG2 services become available

  @param [i]  sigs_grp_ptr
  @param [i]  sig_mask

  @return   none
 */
/*=========================================================================*/
STATIC void scm_qmi_client_wait_for_services(
  OS_FLAG_GRP* sigs_grp_ptr,
  OS_FLAGS     sig_mask
)
{
  OS_FLAGS sig_flags = 0;
  uint8_t err;

  while ( sig_mask != 0 )
  {
    SNS_SCM_PRINTF1(MED, "wait4service - sig_mask=0x%08x ...", sig_mask);
    sig_flags = sns_os_sigs_pend(sigs_grp_ptr, sig_mask,
                                 OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME, 0, &err);
    SNS_SCM_PRINTF1(LOW, "wait4service - sig_flags=0x%x", sig_flags);
    sns_os_sigs_del(sigs_grp_ptr, sig_flags);

    if ( sig_flags & SCM_SMGR_QCSI_WAIT_SIG )
    {
      scm_qmi_client_connect_service(&scm_smgr_qmi_client_if);
    }
    if ( sig_flags & SCM_SMGR_INT_QCSI_WAIT_SIG )
    {
      scm_qmi_client_connect_service(&scm_smgr_int_qmi_client_if);
    }
    if ( sig_flags & SCM_SAM_QCSI_WAIT_SIG )
    {
      scm_qmi_client_connect_service(&scm_sam_qmi_client_if);
    }
#ifndef ADSP_STANDALONE
    if ( sig_flags & SCM_REG_QCSI_WAIT_SIG )
    {
      scm_qmi_client_connect_service(&scm_reg_qmi_client_if);
    }
#endif
    sig_mask &= ~sig_flags;
  }
}

/*=========================================================================
  FUNCTION:  sns_scm_task
  =========================================================================*/
/*!
  @brief Sensors calibration manager task

  @detail
  All algorithms are executed in this task context.
  Waits on events primarily from sensors manager or client.

  @param[i] argPtr: pointer to task argument

  @return None
*/
/*=======================================================================*/
STATIC void sns_scm_task(
   void *argPtr)
{
   int8_t                            i;
   OS_FLAGS                          sigFlags = 0;
   uint8_t                           err;
   qmi_client_error_type             qmi_err;

   /* mutex create */
   scm_mutex = sns_os_mutex_create (SNS_SCM_MODULE_PRIORITY, &err);

   /* Queue Init */
   sns_q_init(&scm_sam_smgr_q);
   sns_q_init(&scm_reg_q);

   /* SCM - SMGR */
   qmi_err = sns_scm_qmi_client_init(SNS_SMGR_SVC_ID_V01,
                                     SCM_SMGR_QCSI_WAIT_SIG,
                                     0,
                                     SNS_SMGR_SVC_get_service_object_v01(),
                                     sns_scm_smgr_ind_cb,
                                     &scm_smgr_qmi_client_if);
   if (QMI_NO_ERR != qmi_err)
   {
      return;
   }
   sigFlags |= SCM_SMGR_QCSI_WAIT_SIG;

   /* SCM - SMGR Internal */
   qmi_err = sns_scm_qmi_client_init(SNS_SMGR_INTERNAL_SVC_ID_V01,
                                     SCM_SMGR_INT_QCSI_WAIT_SIG,
                                     0,
                                     SNS_SMGR_INTERNAL_SVC_get_service_object_v02(),
                                     sns_scm_smgr_int_ind_cb,
                                     &scm_smgr_int_qmi_client_if);
   if (QMI_NO_ERR != qmi_err)
   {
      return;
   }
   sigFlags |= SCM_SMGR_INT_QCSI_WAIT_SIG;

   /* SCM - SAM */
   qmi_err = sns_scm_qmi_client_init(SNS_SAM_AMD_SVC_ID_V01,
                                     SCM_SAM_QCSI_WAIT_SIG,
                                     0,
                                     SNS_SAM_AMD_SVC_get_service_object_v01(),
                                     sns_scm_sam_ind_cb,
                                     &scm_sam_qmi_client_if);
   if (QMI_NO_ERR != qmi_err)
   {
      return;
   }
   sigFlags |= SCM_SAM_QCSI_WAIT_SIG;

#ifndef ADSP_STANDALONE
   /* SCM - REG */
   qmi_err = sns_scm_qmi_client_init(SNS_REG2_SVC_ID_V01,
                                     SCM_REG_QCSI_WAIT_SIG,
                                     0,
                                     SNS_REG2_SVC_get_service_object_v02(),
                                     sns_scm_reg_ind_cb,
                                     &scm_reg_qmi_client_if);
   if (QMI_NO_ERR != qmi_err)
   {
      return;
   }
   sigFlags |= SCM_REG_QCSI_WAIT_SIG;
#endif /* ADSP_STANDALONE */

   //initialize algorithm database
   for (i = SNS_SCM_NUM_ALGO_SVCS-1; i >= 0; i--)
   {
      sns_scm_algo_dbase[i] = NULL;
   }

   //initialize algorithm instance database
   for (i = SNS_SCM_MAX_ALGO_INSTS-1; i >= 0; i--)
   {
      sns_scm_algo_inst_dbase[i] = NULL;
   }
   sns_scm_algo_inst_count = 0;

   //initialize sensor data request database
   for (i = SNS_SCM_MAX_DATA_REQS-1; i >= 0; i--)
   {
      sns_scm_data_req_dbase[i] = NULL;
   }
   sns_scm_data_req_count = 0;

   for (i = SNS_SCM_MAX_SNS_MON-1; i >= 0; i--)
   {
      sns_scm_sensor_dbase[i].sensorId = SNS_SCM_INVALID_ID;
      sns_scm_sensor_dbase[i].dataType = SNS_SCM_INVALID_ID;
      sns_scm_sensor_dbase[i].status = SNS_SCM_INVALID_ID;
      sns_scm_sensor_dbase[i].num_clients = 0;
      sns_scm_sensor_dbase[i].auto_cal_reg_id = SNS_SCM_INVALID_ID;
      sns_scm_sensor_dbase[i].timer = NULL;
      sns_scm_sensor_dbase[i].timeout = false;
      sns_scm_sensor_dbase[i].auto_cal_bias_params_valid = false;
      sns_scm_sensor_dbase[i].auto_cal_scale_params_valid = false;
      sns_scm_sensor_dbase[i].auto_cal_accuracy = 0;
   }

   //register algorithms
   sns_scm_reg_algos();

   //allow other modules to initialize while SCM wait for needed services
   sns_init_done();
   SNS_SCM_PRINTF0(LOW, "sns_init_done done");

   scm_qmi_client_wait_for_services(sns_scm_sig_event, sigFlags);

   /* SCM - SAM & SMGR messages signal */
   sns_os_sigs_add(sns_scm_sig_event, SCM_SAM_SMGR_MSG_SIG);

   /* SCM - REG messages signal */
   sns_os_sigs_add(sns_scm_sig_event, SCM_REG_MSG_SIG);

   /* SCM - hysteresis timeout signal */
   sns_os_sigs_add(sns_scm_sig_event, SNS_SCM_SENSOR_STATUS_HYST_TIMER_SIG);

   // Detect SMGR Buffering support
   sns_scm_smgr_buffering_flag = sns_scm_detect_smgr_buffering( sns_scm_sig_event );

   if( sns_scm_smgr_buffering_flag )
   {
      // If buffering is supported, check if default sensor report rates are present in registry
      // A registry version check is sufficient to get this info.
      sns_scm_sensor_report_rate_available = sns_scm_check_sensor_report_rate( sns_scm_sig_event );
   }
   SNS_SCM_PRINTF2(LOW, "SCM: SMGR Buffering support:%d, Sensor report rates in registry:%d",
      sns_scm_smgr_buffering_flag, sns_scm_sensor_report_rate_available);

   //initialize calibration algorithms SCM to REG
   sns_scm_init_cal_algos();
   SNS_SCM_PRINTF0(LOW, "sns_scm_init_cal_algos done");

   //register SCM to SMGR for sensor status reports
   sns_scm_reg_smgr();
   SNS_SCM_PRINTF0(LOW, "sns_scm_reg_smgr done");

   SNS_SCM_PRINTF0(LOW, "SCM init success");

#ifdef SNS_SCM_TEST_ENABLE
   SNS_SCM_PRINTF0(LOW, "start SCM uinit Tests");

   sns_scm_ext_test();

   SNS_SCM_PRINTF0(LOW, "sns_scm_ext_test test done");

   sns_scm_send_qmd_start_req();

   SNS_SCM_PRINTF0(LOW, "sns_scm_send_qmd_start_req test done");

   sns_scm_send_qmd_stop_req();

   SNS_SCM_PRINTF0(LOW, "sns_scm_send_qmd_stop_req test done");
#endif

   while (1)
   {
     sns_profiling_log_qdss( SNS_SCM_TASK_EXIT, 0 );
     //wait for event
     sigFlags = sns_os_sigs_pend(sns_scm_sig_event,
                                 SNS_SCM_SAM_SMGR_FLAGS,
                                 OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME,
                                 0,
                                 &err);
    sns_profiling_log_qdss( SNS_SCM_TASK_ENTER, 0 );

     SNS_ASSERT(err == 0);

     //timer event check
     if (sigFlags & SNS_SCM_REPORT_TIMER_SIG)
     {
        SNS_SCM_PRINTF0(LOW, "SCM : got the SNS_SCM_REPORT_TIMER_SIG signal");

        sns_scm_handle_report_timeout();
     }

     if (sigFlags & SNS_SCM_SENSOR_STATUS_HYST_TIMER_SIG)
     {
        SNS_SCM_PRINTF0(LOW, "SCM : got the SNS_SCM_SENSOR_STATUS_HYST_TIMER_SIG signal");

        sns_scm_handle_sensor_status_hyst_timeout();
     }


     if (sigFlags & SCM_SAM_SMGR_MSG_SIG)
     {
       sns_scm_process_msg();
     }

     if (sigFlags & SCM_REG_MSG_SIG)
     {
       sns_scm_process_reg_msg();
     }

     sigFlags &= (~SNS_SCM_SAM_SMGR_FLAGS);

     if (sigFlags != 0)
     {
        SNS_SCM_PRINTF1(ERROR, "error signal flag %d", sigFlags);
     }
   }
}

/*---------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
/*=========================================================================
  FUNCTION:  sns_scm_init
  =========================================================================*/
/*!
  @brief Sensors calibration manager initialization.
         Creates the SCM task and internal databases.

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_scm_init(void)
{
   uint8_t err;

   //initialize events
   sns_scm_sig_event = sns_os_sigs_create(SNS_SCM_REPORT_TIMER_SIG, &err);

   SNS_ASSERT(sns_scm_sig_event != NULL);

   //create the SCM task
   err = sns_os_task_create_ext(sns_scm_task,
                                NULL,
                                &sns_scm_task_stk[SNS_SCM_MODULE_STK_SIZE-1],
                                SNS_SCM_MODULE_PRIORITY,
                                SNS_SCM_MODULE_PRIORITY,
                                &sns_scm_task_stk[0],
                                SNS_SCM_MODULE_STK_SIZE,
                                (void *)0,
                                OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR,
                                (uint8_t *)"SNS_SCM");
   SNS_ASSERT(err == 0);

   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_get_sensor_index
  =========================================================================*/
/*!
  @brief Get the index into the sensor status database for specified sensor

  @param[i] sensorId: algorithm service id

  @return index for the specified sensor in database if found,
          SNS_SCM_INVALID_ID otherwise
*/
/*=======================================================================*/
uint8_t sns_scm_get_sensor_index(
   uint8_t sensorId)
{
   uint8_t i;

   for (i=0; i < SNS_SCM_MAX_SNS_MON &&
       sns_scm_sensor_dbase[i].sensorId != SNS_SCM_INVALID_ID; i++)
   {
      if (sns_scm_sensor_dbase[i].sensorId == sensorId)
      {
         return i;
      }
   }

   return SNS_SCM_INVALID_ID;
}
