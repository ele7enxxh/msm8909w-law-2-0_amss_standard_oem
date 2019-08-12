/*============================================================================
  FILE: sns_scm_ext.c

  This file contains the Sensors Calibration Manager implementation

  Copyright (c) 2011-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
 ============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "sns_scm_priv.h"
#include "gyro_cal.h"
#include "qmag_cal.h"

#include "sns_init.h"
#include "sns_memmgr.h"
#include "sns_smgr_api_v01.h"
#include "sns_smgr_internal_api_v02.h"
#include "sns_sam_amd_v01.h"
#include "sns_debug_str.h"
#include "fixed_point.h"

#include "sns_reg_common.h"
#include "sns_reg_api_v02.h"

#include <qmi_client.h>

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
//Initialization timeout period of 10sec
#define SNS_SCM_INIT_TIMER_PERIOD_USEC (10000000)

//Gyro bias calibration execution period of 1 minute
#define SNS_SCM_GYRO_CAL_PERIOD_USEC   (60000000)

/*---------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 *Extern Global Data Definitions
 * -------------------------------------------------------------------------*/
extern scm_qmi_client_if_s        scm_smgr_qmi_client_if;
extern scm_qmi_client_if_s        scm_smgr_int_qmi_client_if;

#ifndef ADSP_STANDALONE
extern scm_qmi_client_if_s        scm_reg_qmi_client_if;
#endif

extern sns_q_s scm_reg_q;
extern OS_EVENT *scm_mutex;

/*---------------------------------------------------------------------------
 * Static Global Data Definitions
 * -------------------------------------------------------------------------*/
//Sensor database
static sns_scm_sensor_s* sns_scm_sensor_dbase;

/*---------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
//SCM event signal
static OS_FLAG_GRP *sns_scm_init_sig_event;

//SCM initialization timer
static sns_em_timer_obj_t sns_scm_init_timer;

/*---------------------------------------------------------------------------
 * Function Definitions
 * -------------------------------------------------------------------------*/

/*=========================================================================
  FUNCTION:  sns_scm_init_timer_cb
  =========================================================================*/
/*!
  @brief Callback registered for initialization timer expiry

  @param[i] argPtr: pointer to argument for callback function

  @return None
*/
/*=======================================================================*/
void sns_scm_init_timer_cb(
   void *argPtr)
{
   uint8_t err;
   sns_os_sigs_post(sns_scm_init_sig_event,
                    SNS_SCM_REPORT_TIMER_SIG,
                    OS_FLAG_SET,
                    &err);
   if (err != OS_ERR_NONE)
   {
      SNS_SCM_DEBUG1(ERROR, DBG_SCM_TIMER_CB_SIGNALERR, err);
   }
}

/*=========================================================================
  FUNCTION:  sns_scm_reg_init_timer
  =========================================================================*/
/*!
  @brief Register timer for for SCM initialization

  @param[i] sigEventFlags: Event signal flags

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_scm_reg_init_timer(
   OS_FLAG_GRP *sigEventFlags)
{
   sns_err_code_e err;

   sns_scm_init_sig_event = sigEventFlags;

   err = sns_em_create_timer_obj(sns_scm_init_timer_cb,
                                 NULL,
                                 SNS_EM_TIMER_TYPE_ONESHOT,
                                 &(sns_scm_init_timer));
   if (err == SNS_SUCCESS)
   {
      err = sns_em_register_timer(sns_scm_init_timer,
                                  sns_em_convert_usec_to_localtick(
                                     SNS_SCM_INIT_TIMER_PERIOD_USEC));
      if (err == SNS_SUCCESS)
      {
        SNS_SCM_DEBUG2(MEDIUM, DBG_SCM_REG_TIMER_STARTED,
                       0, SNS_SCM_INIT_TIMER_PERIOD_USEC);
      }
   }
   if (err != SNS_SUCCESS)
   {
      SNS_SCM_DEBUG1(ERROR, DBG_SCM_REG_TIMER_FAILED, err);
   }
   return err;
}


/*=========================================================================
  FUNCTION:  sns_scm_dereg_init_timer
  =========================================================================*/
/*!
  @brief Deregister timer for SCM initialization

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_scm_dereg_init_timer(void)
{
   sns_err_code_e err;

   err = sns_em_cancel_timer(sns_scm_init_timer);
   if (err != SNS_SUCCESS)
   {
      SNS_SCM_DEBUG1(ERROR, DBG_SCM_DEREG_TIMER_FAILED, err);
   }

   err = sns_em_delete_timer_obj(sns_scm_init_timer);
   if (err == SNS_SUCCESS)
   {
      sns_scm_init_timer = NULL;

      SNS_SCM_DEBUG3(MEDIUM, DBG_SCM_DEREG_TIMER_DELETED,
                     0, 0, sns_em_get_timestamp());
   }
   else
   {
     SNS_SCM_DEBUG1(ERROR, DBG_SCM_DEREG_TIMER_FAILED, err);
   }

   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_send_cal_msg
  =========================================================================*/
/*!
  @brief Process a factory calibration message and forward data to the SMGR

  @param[i] snsInfoPtr: pointer to sensor info in sensor database

  @return None
*/
/*=======================================================================*/
static void sns_scm_send_cal_msg(
   const sns_scm_sensor_s* snsInfoPtr)
{
   sns_smgr_sensor_cal_req_msg_v01 *msgPtr;
   sns_smgr_sensor_cal_resp_msg_v01 *msgRespP;
   qmi_txn_handle                   txn_handle;

   msgRespP = (sns_smgr_sensor_cal_resp_msg_v01 *)
              SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_smgr_sensor_cal_resp_msg_v01));
   SNS_ASSERT(msgRespP != NULL);

   msgPtr = (sns_smgr_sensor_cal_req_msg_v01 *)
            SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_smgr_sensor_cal_req_msg_v01));
   SNS_ASSERT(msgPtr != NULL);

   msgPtr->usage = SNS_SMGR_CAL_DYNAMIC_V01;
   msgPtr->SensorId = snsInfoPtr->sensorId;
   msgPtr->DataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;

   SNS_SCM_PRINTF1(MEDIUM, "Sending SMGR Cal Req SensorId x:%d",
                   snsInfoPtr->sensorId);

   //Send bias correction values to SMGR
   msgPtr->ZeroBias_len = 0;
   msgPtr->ScaleFactor_len = 0;
   msgPtr->CompensationMatrix_valid = false;
   msgPtr->CalibrationAccuracy_valid = true; 
   msgPtr->CalibrationAccuracy = 0;

   if ( snsInfoPtr->auto_cal_bias_params_valid )
   {
      msgPtr->ZeroBias_len = 3;
      msgPtr->ZeroBias[0] = snsInfoPtr->auto_cal_bias_params[0];
      msgPtr->ZeroBias[1] = snsInfoPtr->auto_cal_bias_params[1];
      msgPtr->ZeroBias[2] = snsInfoPtr->auto_cal_bias_params[2];

      SNS_SCM_PRINTF3(MEDIUM, "Sending SMGR Cal Req Bias x:%d, y:%d, z:%d",
                      snsInfoPtr->auto_cal_bias_params[0],
                      snsInfoPtr->auto_cal_bias_params[1],
                      snsInfoPtr->auto_cal_bias_params[2]);
   }

   if ( snsInfoPtr->auto_cal_scale_params_valid )
   {
      msgPtr->ScaleFactor_len = 3;
      msgPtr->ScaleFactor[0] = snsInfoPtr->auto_cal_scale_params[0]; 
      msgPtr->ScaleFactor[1] = snsInfoPtr->auto_cal_scale_params[1];
      msgPtr->ScaleFactor[2] = snsInfoPtr->auto_cal_scale_params[2];

      SNS_SCM_PRINTF3(MEDIUM, "Sending SMGR Cal Req Scale x:%d, y:%d, z:%d",
                      snsInfoPtr->auto_cal_scale_params[0],
                      snsInfoPtr->auto_cal_scale_params[1],
                      snsInfoPtr->auto_cal_scale_params[2]);

   }

   if ( snsInfoPtr->auto_cal_bias_params_valid || snsInfoPtr->auto_cal_scale_params_valid )
   {
      msgPtr->CalibrationAccuracy = snsInfoPtr->auto_cal_accuracy;
   }

   SNS_SCM_PRINTF1(MEDIUM, "Sending SMGR Cal Req Accuracy x:%d",
                   snsInfoPtr->auto_cal_accuracy);

   sns_scm_qmi_client_send_msg_async (scm_smgr_qmi_client_if.user_handle, 
                                    SNS_SMGR_CAL_REQ_V01,
                                    msgPtr, sizeof(sns_smgr_sensor_cal_req_msg_v01),
                                    msgRespP, sizeof(sns_smgr_sensor_cal_resp_msg_v01),
                                    sns_scm_smgr_resp_cb, NULL, &txn_handle);
}

/*=========================================================================
  FUNCTION:  sns_scm_init_sensor_cal
  =========================================================================*/
/*!
  @brief Initialize sensor calibration parameters
  This implementation works for any sensor with the same format
  of 3 int32_t values each for bias & scale params.
  any other format requires a different implementation

  @param[i] sensorId: sensor id
  @param[i] regDataPtr: pointer to the data in the registry message

  @return None
*/
/*=======================================================================*/
static bool sns_scm_init_sensor_cal(
   uint8_t sensorId,
   const int32_t* regDataPtr)
{
   uint8_t i = 0;
   bool status = false, send_cal = false;

   if(NULL != sns_scm_sensor_dbase)
   {
      for(i = 0; i < SNS_SCM_MAX_SNS_MON; i++)
      {
         if (sns_scm_sensor_dbase[i].sensorId == sensorId)
         {
            if (sensorId == SNS_SMGR_ID_GYRO_V01 || sensorId == SNS_SMGR_ID_ACCEL_V01)
            {
               if (regDataPtr[0] != 0 || regDataPtr[1] != 0 || regDataPtr[2] != 0)
               {
                  sns_scm_sensor_dbase[i].auto_cal_bias_params_valid = true; 
                  sns_scm_sensor_dbase[i].auto_cal_bias_params[0] = regDataPtr[0];
                  sns_scm_sensor_dbase[i].auto_cal_bias_params[1] = regDataPtr[1];
                  sns_scm_sensor_dbase[i].auto_cal_bias_params[2] = regDataPtr[2];
                  sns_scm_sensor_dbase[i].auto_cal_accuracy = 3;
                  send_cal = true;
               }
               status = true;
               break;
            }
            else if (sensorId == SNS_SMGR_ID_MAG_V01)
            {
               sns_reg_mag_dyn_cal_params_data_group_s *qmagRegPtr = 
                  (sns_reg_mag_dyn_cal_params_data_group_s *)(regDataPtr);

               if (qmagRegPtr->bias_valid)
               {
                  sns_scm_sensor_dbase[i].auto_cal_bias_params_valid = true; 
                  sns_scm_sensor_dbase[i].auto_cal_bias_params[0] = qmagRegPtr->x_dyn_bias;
                  sns_scm_sensor_dbase[i].auto_cal_bias_params[1] = qmagRegPtr->y_dyn_bias;
                  sns_scm_sensor_dbase[i].auto_cal_bias_params[2] = qmagRegPtr->z_dyn_bias;
                  sns_scm_sensor_dbase[i].auto_cal_accuracy = qmagRegPtr->accuracy;
                  send_cal = true;
               }
               status = true;
               break;
            }
         }
      }
   }

   if (send_cal)
   {
      sns_scm_send_cal_msg(&sns_scm_sensor_dbase[i]); 
   }

   return status;
}

/*=========================================================================
  FUNCTION:  sns_scm_init_qmag_auto_cal_state
  =========================================================================*/
/*!
  @brief Initialize algo with last known state from registry

  @param[i] algoPtr: pointer to algorithm in database
  @param[i] regDataPtr: pointer to the data in the registry message

  @return None
*/
/*=======================================================================*/
static void sns_scm_init_qmag_auto_cal_state(
   sns_scm_algo_s* algoPtr,
   const sns_reg_qmag_cal_state_data_group_s* regDataPtr)
{
   qmag_cal_config_s *defCfgPtr =
      (qmag_cal_config_s *)(algoPtr->defConfigData.memPtr);

   defCfgPtr->persistent_state.published_offs_HI[0] = regDataPtr->published_offs_HI_x;
   defCfgPtr->persistent_state.published_offs_HI[1] = regDataPtr->published_offs_HI_y;
   defCfgPtr->persistent_state.published_offs_HI[2] = regDataPtr->published_offs_HI_z;
   defCfgPtr->persistent_state.published_accuracy_HI = regDataPtr->published_accuracy_HI;
   defCfgPtr->persistent_state.published_offset_time = regDataPtr->published_offset_time;
   defCfgPtr->persistent_state.published_offset_valid = regDataPtr->published_offset_valid;

   SNS_SCM_PRINTF1(LOW, "qmag_cal init, loading state from registry to algo dbase, acc:%d",
                   defCfgPtr->persistent_state.published_accuracy_HI);

   SNS_SCM_PRINTF3(LOW, "qmag_cal offsetx: %d, offsety: %d, offsetz: %d",
                   FX_FLTTOFIX_Q16(defCfgPtr->persistent_state.published_offs_HI[0]),
                   FX_FLTTOFIX_Q16(defCfgPtr->persistent_state.published_offs_HI[1]),
                   FX_FLTTOFIX_Q16(defCfgPtr->persistent_state.published_offs_HI[2]));

   return;
}

/*=========================================================================
  FUNCTION:  sns_scm_init_qmag_auto_cal
  =========================================================================*/
/*!
  @brief Process an algorithm calibration message from the registry

  @param[i] algoPtr: pointer to algorithm in database
  @param[i] regDataPtr: pointer to the data in the registry message

  @return None
*/
/*=======================================================================*/
static void sns_scm_init_qmag_auto_cal(
   sns_scm_algo_s* algoPtr,
   const sns_reg_qmag_cal_algo_data_group_s* regDataPtr)
{
   qmag_cal_config_s *defCfgPtr =
      (qmag_cal_config_s *)(algoPtr->defConfigData.memPtr);
   float reg_report_rate = FX_FIXTOFLT_Q16_SP(regDataPtr->report_rate);
   float reg_sample_rate = FX_FIXTOFLT_Q16_SP(regDataPtr->sample_rate);

   algoPtr->enableAlgo = regDataPtr->enable_algo;
   defCfgPtr->sample_rate_accel = regDataPtr->sample_rate; // Hz Q16
   defCfgPtr->sample_rate_mag = regDataPtr->sample_rate; // Hz Q16

   if (reg_sample_rate < QMAG_CAL_MIN_MAG_SAMPLERATE)
   {
      defCfgPtr->sample_rate_accel = QMAG_CAL_MIN_MAG_SAMPLERATE_Q16; // Hz Q16
      defCfgPtr->sample_rate_mag = QMAG_CAL_MIN_MAG_SAMPLERATE_Q16; // Hz Q16
   }
   else if (reg_sample_rate > QMAG_CAL_MAX_MAG_SAMPLERATE)
   {
      defCfgPtr->sample_rate_accel = QMAG_CAL_MAX_MAG_SAMPLERATE_Q16; // Hz Q16
      defCfgPtr->sample_rate_mag = QMAG_CAL_MAX_MAG_SAMPLERATE_Q16; // Hz Q16
   }

   if(sns_scm_is_sensor_report_rate_available())
   {
      if ( reg_report_rate > 0.0f &&
           reg_report_rate <= FX_FIXTOFLT_Q16_SP(defCfgPtr->sample_rate_mag) )
   {
         algoPtr->defSensorReportRate = regDataPtr->report_rate; // Hz Q16
      }
      else
      {
         algoPtr->defSensorReportRate = defCfgPtr->sample_rate_mag;// Hz Q16
      }
   }

   if (regDataPtr->version_no >= 3 && regDataPtr->param3 > 0)
   {
      defCfgPtr->publish_HI_delay = QMAG_CAL_CALIBRATION_HISTORY < regDataPtr->param1
        ? QMAG_CAL_CALIBRATION_HISTORY : regDataPtr->param1;
      defCfgPtr->publish_max_buffered_accuracy = (0 != regDataPtr->param2);

      defCfgPtr->qmag_cal_tuning_param.qmag_cal_calibration_history = regDataPtr->param3;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_radius_history = regDataPtr->param4;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_min_radius_gauss = regDataPtr->param5;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_max_radius_gauss = regDataPtr->param6;
      defCfgPtr->qmag_cal_tuning_param.min_rel_data_span_hi_accuracy_0 = regDataPtr->param7;
      defCfgPtr->qmag_cal_tuning_param.min_rel_data_span_hi_accuracy_1 = regDataPtr->param8;
      defCfgPtr->qmag_cal_tuning_param.min_rel_data_span_hi_accuracy_2 = regDataPtr->param9;
      defCfgPtr->qmag_cal_tuning_param.min_rel_data_span_hi_accuracy_3 = regDataPtr->param10;
      defCfgPtr->qmag_cal_tuning_param.min_accel_seg_variance_hi_0 = regDataPtr->param11;
      defCfgPtr->qmag_cal_tuning_param.min_accel_seg_variance_hi_1 = regDataPtr->param12;
      defCfgPtr->qmag_cal_tuning_param.min_accel_seg_variance_hi_2 = regDataPtr->param13;
      defCfgPtr->qmag_cal_tuning_param.min_accel_seg_variance_hi_3 = regDataPtr->param14;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_min_decimation_delta_sq = regDataPtr->param15;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_max_decimation_delta_sq = regDataPtr->param16;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_min_decimation_gyro_angle = regDataPtr->param17;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_default_3d_mag_field_intensity = regDataPtr->param18;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_hi_cal_win_size_second = regDataPtr->param19;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_calibration_cycle_sec = regDataPtr->param20;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_motion_window_sec = regDataPtr->param21 * defCfgPtr->qmag_cal_tuning_param.qmag_cal_calibration_cycle_sec;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_min_mag_samples_autonomous_10hz = regDataPtr->param22;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_mag_variance_window_sec = regDataPtr->param23;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_accel_variance_window_sec = regDataPtr->param24;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_gyro_variance_window_sec = regDataPtr->param25;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_mag_variance_thresh = regDataPtr->param26;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_accel_variance_thresh = regDataPtr->param27;

      defCfgPtr->qmag_cal_tuning_param.qmag_cal_default_decimation_delta_sq = defCfgPtr->qmag_cal_tuning_param.qmag_cal_max_decimation_delta_sq;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_hi_cal_win_dize_usec = defCfgPtr->qmag_cal_tuning_param.qmag_cal_hi_cal_win_size_second * 1000000.0f;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_decim_window_sec = defCfgPtr->qmag_cal_tuning_param.qmag_cal_hi_cal_win_size_second ;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_hi_cal_win_size_ticks = sns_em_convert_usec_to_dspstick((uint32_t)(defCfgPtr->qmag_cal_tuning_param.qmag_cal_hi_cal_win_dize_usec));


   }

   SNS_SCM_PRINTF3(MEDIUM, "read qmag_cal algo config registry params,"
                           " enable:%d sample rate:%d report rate:%d",
                   algoPtr->enableAlgo,
                   defCfgPtr->sample_rate_mag,
                   algoPtr->defSensorReportRate);
   return;
}

/*=========================================================================
  FUNCTION:  sns_scm_init_gyro_auto_cal
  =========================================================================*/
/*!
  @brief Process an algorithm calibration message from the registry

  @param[i] algoPtr: pointer to algorithm in database
  @param[i] regDataPtr: pointer to the data in the registry message

  @return None
*/
/*=======================================================================*/
static void sns_scm_init_gyro_auto_cal(
   sns_scm_algo_s* algoPtr,
   const sns_reg_gyro_cal_data_group_s* regDataPtr)
{
   gyro_cal_config_s *defCfgPtr =
      (gyro_cal_config_s *)(algoPtr->defConfigData.memPtr);
   float reg_report_rate = FX_FIXTOFLT_Q16_SP(regDataPtr->def_sensor_report_rate);
   float reg_sample_rate = FX_FIXTOFLT_Q16_SP(regDataPtr->def_gyro_sample_rate);

   algoPtr->enableAlgo = regDataPtr->def_gyro_cal_algo_state;
   defCfgPtr->numSamples = regDataPtr->def_gyro_num_samples;
   defCfgPtr->varianceThold = regDataPtr->def_gyro_var_thold;
   defCfgPtr->sampleRate = regDataPtr->def_gyro_sample_rate; // Hz Q16

   if ( reg_sample_rate < GYRO_CAL_MIN_SAMPLERATE )
   {
      defCfgPtr->sampleRate = GYRO_CAL_MIN_SAMPLERATE_Q16; // Hz Q16
   }
   else if (reg_sample_rate > GYRO_CAL_MAX_SAMPLERATE)
   {
      defCfgPtr->sampleRate = GYRO_CAL_MAX_SAMPLERATE_Q16; // Hz Q16
   }

   if( sns_scm_is_sensor_report_rate_available() )
   {
      if ( reg_report_rate > 0.0f &&
           reg_report_rate <= FX_FIXTOFLT_Q16_SP(defCfgPtr->sampleRate) )
      {
         algoPtr->defSensorReportRate = regDataPtr->def_sensor_report_rate; // Hz Q16
      }
      else
      {
         algoPtr->defSensorReportRate = defCfgPtr->sampleRate; // Hz Q16
      }
   }

   SNS_SCM_PRINTF3(MEDIUM, "read gyro_cal algo config registry params,"
                        " enable:%d sample rate:%d report rate:%d",
                   algoPtr->enableAlgo,
                   defCfgPtr->sampleRate,
                   algoPtr->defSensorReportRate);

   return;
}

/*=========================================================================
  FUNCTION:  sns_scm_process_reg_data
  =========================================================================*/
/*!
  @brief Process data received from registry

  @param[i] regItemType - registry item type
  @param[i] regItemId - registry item identifier
  @param[i] regDataLen - registry data length
  @param[i] regDataPtr - registry data pointer

  @return None
*/
/*=======================================================================*/
static void sns_scm_process_reg_data(
   sns_scm_reg_item_type_e regItemType,
   uint16_t regItemId,
   uint32_t regDataLen,
   const uint8_t* regDataPtr)
{
   uint8_t i = 0;
   bool success = false;

   switch(regItemId)
   {
      case SNS_REG_SCM_GROUP_GYRO_CAL_ALGO_V02:
      {
         sns_scm_algo_s* algoPtr =
            sns_scm_get_algo_handle(SNS_SCM_GYRO_CAL_SVC);
         if (NULL != algoPtr)
         {
            sns_reg_gyro_cal_data_group_s * dataPtr =
               (sns_reg_gyro_cal_data_group_s *)regDataPtr;
            sns_scm_init_gyro_auto_cal(algoPtr,dataPtr);
            success = true;
         }
         break;
      }

      case SNS_REG_SCM_GROUP_ACCEL_DYN_CAL_PARAMS_V02:
      case SNS_REG_SCM_GROUP_GYRO_DYN_CAL_PARAMS_V02:
      case SNS_REG_SCM_GROUP_MAG_DYN_CAL_PARAMS_V02:
      {
         if(NULL != sns_scm_sensor_dbase)
         {
            for (i = 0; i < SNS_SCM_MAX_SNS_MON &&
                 sns_scm_sensor_dbase[i].auto_cal_reg_id != SNS_SCM_INVALID_ID; i++)
            {
               if (sns_scm_sensor_dbase[i].auto_cal_reg_id == regItemId)
               {
                  success = sns_scm_init_sensor_cal(
                     sns_scm_sensor_dbase[i].sensorId, (int32_t*)regDataPtr);
                  break;
               }
            }
         }
         break;
      }

      case SNS_REG_SCM_GROUP_QMAG_CAL_ALGO_V02:
      {
         sns_scm_algo_s* algoPtr =
            sns_scm_get_algo_handle(SNS_SCM_MAG_CAL_SVC);
         if (NULL != algoPtr)
         {
            sns_reg_qmag_cal_algo_data_group_s *dataPtr =
               (sns_reg_qmag_cal_algo_data_group_s *)regDataPtr;
            sns_scm_init_qmag_auto_cal(algoPtr,dataPtr);
            success = true;
         }
         break;
      }

      case SNS_REG_SCM_GROUP_QMAG_CAL_STATE_V02:
      {
         sns_scm_algo_s* algoPtr =
            sns_scm_get_algo_handle(SNS_SCM_MAG_CAL_SVC);
         if (NULL != algoPtr)
         {
            sns_reg_qmag_cal_state_data_group_s *dataPtr =
               (sns_reg_qmag_cal_state_data_group_s *)regDataPtr;
            SNS_SCM_PRINTF0(LOW, "qmag_cal: read algo state params in registry");
            sns_scm_init_qmag_auto_cal_state(algoPtr,dataPtr);
            success = true;
         }
         break;
      }



      default:
         break;
   }

   SNS_SCM_PRINTF1(LOW, "SCM Init Processed Reg Id:%d ", regItemId);

   if (!success)
   {
      SNS_SCM_DEBUG3(ERROR, DBG_SCM_REG_RESP_PROC_ERR,
                     regItemId, regItemType, regDataLen);
   }
}

/*=========================================================================
  FUNCTION:  sns_scm_process_reg_single_read_resp
  =========================================================================*/
/*!
  @brief Process registry response to single item read request

  @param[i] regRespPtr: Pointer to registry response message

  @return None
*/
/*=======================================================================*/
static void sns_scm_process_reg_single_read_resp(
   const void *regRespPtr)
{
   sns_reg_single_read_resp_msg_v02* msgPtr
      = (sns_reg_single_read_resp_msg_v02*)regRespPtr;

   if (msgPtr->resp.sns_result_t == SNS_SUCCESS)
   {
      sns_scm_process_reg_data(SNS_SCM_REG_ITEM_TYPE_SINGLE,
                               msgPtr->item_id,
                               msgPtr->data_len,
                               msgPtr->data);
   }
   else
   {
      SNS_SCM_DEBUG3(ERROR, DBG_SCM_REG_RESP_ERR,
                     msgPtr->item_id,
                     msgPtr->resp.sns_result_t,
                     msgPtr->resp.sns_err_t);
   }
}

/*=========================================================================
  FUNCTION:  sns_scm_process_reg_group_read_resp
  =========================================================================*/
/*!
  @brief Process registry response to group item read request

  @param[i] regRespPtr: Pointer to registry response message

  @return None
*/
/*=======================================================================*/
static void sns_scm_process_reg_group_read_resp(
   const void *regRespPtr)
{
   sns_reg_group_read_resp_msg_v02* msgPtr
      = (sns_reg_group_read_resp_msg_v02*)regRespPtr;

   if (msgPtr->resp.sns_result_t == SNS_SUCCESS)
   {
      sns_scm_process_reg_data(SNS_SCM_REG_ITEM_TYPE_GROUP,
                               msgPtr->group_id,
                               msgPtr->data_len,
                               msgPtr->data);
   }
   else
   {
      SNS_SCM_DEBUG3(ERROR, DBG_SCM_REG_RESP_ERR,
                     msgPtr->group_id,
                     msgPtr->resp.sns_result_t,
                     msgPtr->resp.sns_err_t);
   }
}

/*=========================================================================
  FUNCTION:  sns_scm_process_reg_msg
  =========================================================================*/
/*!
  @brief Process messages received from registry

  @return Number of messages processed
*/
/*=======================================================================*/
uint8_t sns_scm_process_reg_msg(void)
{
   uint8_t numMsgProcessed = 0;
   scm_q_item_s *msgPtr;

   while (NULL != (msgPtr = (scm_q_item_s *)sns_scm_q_get(SCM_REG_MSG_SIG)))
   {
     if (msgPtr->scm_msg_type == SCM_REG_RESP_MSG)
     {
       if (msgPtr->msg_id == SNS_REG_GROUP_READ_RESP_V02)
       {
         sns_scm_process_reg_group_read_resp(msgPtr->body_ptr);
         numMsgProcessed++;
       }
       else if (msgPtr->msg_id == SNS_REG_SINGLE_READ_RESP_V02)
       {
         sns_scm_process_reg_single_read_resp(msgPtr->body_ptr);
         numMsgProcessed++;
       }
       else if (SNS_REG_GROUP_WRITE_RESP_V02 )
       {
         sns_reg_group_write_resp_msg_v02 *reg_gp_wr_respP;
         reg_gp_wr_respP = (sns_reg_group_write_resp_msg_v02 *) msgPtr->body_ptr;

         SNS_SCM_PRINTF2(LOW, "sns_result_t = %d, sns_err_t = %d",
                         reg_gp_wr_respP->resp.sns_result_t,
                         reg_gp_wr_respP->resp.sns_err_t);
       }
       else
       {
         SNS_SCM_PRINTF1(ERROR, "Invalid response msg_id = %d", msgPtr->msg_id);
       }
     }
     else
     {
       SNS_SCM_PRINTF1(ERROR, "Invalid SCM message type, scm_msg_type = %d",
                       msgPtr->scm_msg_type);
     }

     SNS_OS_FREE(msgPtr->body_ptr);
     SNS_OS_FREE(msgPtr);
     msgPtr = NULL;
   }
   return numMsgProcessed;
}

/*=========================================================================
  FUNCTION:  sns_scm_process_init_events
  =========================================================================*/
/*!
  @brief Process initialization events

  @param[i] numRegQuery: Number of queries made to registry
  @param[i] snsDbaseHandle: Handle to the SCM internal sensor DB

  @return None
*/
/*=======================================================================*/
void sns_scm_process_init_events(
   uint8_t numRegQuery,
   sns_scm_sensor_s* snsDbaseHandle)
{
   uint8_t err;
   OS_FLAGS sigFlags;

   SNS_SCM_DEBUG1(HIGH, DBG_SCM_REG_REQ_COUNT, numRegQuery);

   sns_scm_sensor_dbase = snsDbaseHandle;

   while (numRegQuery)
   {
      //wait for event
      sigFlags = sns_os_sigs_pend(sns_scm_init_sig_event,
                                  SNS_SCM_REG_FLAGS,
                                  OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME,
                                  0,
                                  &err);
      SNS_ASSERT(err == 0);

      //message event check
      if (sigFlags & SCM_REG_MSG_SIG)
      {
         SNS_SCM_PRINTF0(LOW, "SCM EXT: got the SCM_REG_RESP_MSG_SIG signal\n");

         uint8_t numRegQueryProcessed = sns_scm_process_reg_msg();
         SNS_ASSERT(numRegQuery >= numRegQueryProcessed);
         numRegQuery -= numRegQueryProcessed;
      }
      //client report timer event check
      if (sigFlags & SNS_SCM_REPORT_TIMER_SIG)
      {
         SNS_SCM_DEBUG1(HIGH, DBG_SCM_REG_REQ_FAIL, numRegQuery);
         sigFlags &= (~SNS_SCM_REPORT_TIMER_SIG);
         return;
      }

      sigFlags &= (~SNS_SCM_REG_FLAGS);

      if (sigFlags != 0)
      {
         SNS_SCM_DEBUG1(HIGH, DBG_SCM_PROCESS_EVT_UNKWN_EVT, sigFlags);
      }
   }

   SNS_SCM_DEBUG0(HIGH, DBG_SCM_REG_REQ_SUCCESS);
}

/*=========================================================================
  FUNCTION:  sns_scm_req_reg_data
  =========================================================================*/
/*!
  @brief Request registry data

  @param[i] regItemType: Registry item type (single / group)
  @param[i] regItemId: Registry item identifier

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_scm_req_reg_data(
   sns_scm_reg_item_type_e regItemType,
   uint32_t regItemId)
{
#ifndef ADSP_STANDALONE
   void*                            reqMsgPtr = NULL;
   void*                            respMsgPtr = NULL;
   sns_reg_single_read_req_msg_v02  *msgReqSingleP = NULL;
   sns_reg_single_read_resp_msg_v02 *msgRespSingleP = NULL;
   sns_reg_group_read_req_msg_v02   *msgReqGroupP = NULL;
   sns_reg_group_read_resp_msg_v02  *msgRespGroupP = NULL;
   qmi_client_error_type            err;
   uint16_t                         reqSize;
   uint16_t                         respSize;
   uint32                           msg_id;
   qmi_txn_handle                   txn_handle;

   if (regItemType == SNS_SCM_REG_ITEM_TYPE_GROUP)
   {
     msgRespGroupP = (sns_reg_group_read_resp_msg_v02 *)
                     SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_reg_group_read_resp_msg_v02));
     SNS_ASSERT(msgRespGroupP != NULL);

     msgReqGroupP = (sns_reg_group_read_req_msg_v02 *)
                    SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_reg_group_read_req_msg_v02));
     SNS_ASSERT(msgReqGroupP != NULL);

     reqMsgPtr = msgReqGroupP;
     respMsgPtr = msgRespGroupP;
     msg_id = SNS_REG_GROUP_READ_REQ_V02;
     reqSize = sizeof(sns_reg_group_read_req_msg_v02);
     respSize = sizeof(sns_reg_group_read_resp_msg_v02);
     ((sns_reg_group_read_req_msg_v02*)reqMsgPtr)->group_id = regItemId;
   }
   else if (regItemType == SNS_SCM_REG_ITEM_TYPE_SINGLE)
   {
     msgRespSingleP = (sns_reg_single_read_resp_msg_v02 *)
                        SNS_OS_MALLOC (SNS_SCM_DBG_MOD,
                          sizeof(sns_reg_single_read_resp_msg_v02));
     SNS_ASSERT(msgRespSingleP != NULL);

     msgReqSingleP  = (sns_reg_single_read_req_msg_v02 *)
                        SNS_OS_MALLOC (SNS_SCM_DBG_MOD,
                          sizeof(sns_reg_single_read_req_msg_v02));
     SNS_ASSERT(msgReqSingleP != NULL);

     reqMsgPtr = msgReqSingleP;
     respMsgPtr = msgRespSingleP;
     msg_id = SNS_REG_SINGLE_READ_REQ_V02;
     reqSize = sizeof(sns_reg_single_read_req_msg_v02);
     respSize = sizeof(sns_reg_single_read_resp_msg_v02);

     ((sns_reg_single_read_req_msg_v02*)reqMsgPtr)->item_id = regItemId;
   }
   else
   {
     return SNS_ERR_FAILED;
   }

   err = sns_scm_qmi_client_send_msg_async (scm_reg_qmi_client_if.user_handle, msg_id,
                                    reqMsgPtr, reqSize,
                                    respMsgPtr, respSize,
                                    sns_scm_reg_resp_cb, NULL, &txn_handle);

   return err;
#else
   return SNS_ERR_FAILED;
#endif /* ADSP_STANDALONE */

}

/*=========================================================================
  FUNCTION:  sns_scm_reg_algo
  =========================================================================*/
/*!
  @brief Register specified algorithm

  @param[i] algoPtr: pointer to algorithm

  @return None
*/
/*=======================================================================*/
sns_err_code_e sns_scm_reg_algo(
   sns_scm_algo_s* algoPtr)
{
   if (algoPtr->serviceId == SNS_SCM_GYRO_CAL_SVC)
   {
      gyro_cal_config_s *defCfgPtr;
      algoPtr->defConfigData.memPtr = SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(gyro_cal_config_s));
      SNS_ASSERT(algoPtr->defConfigData.memPtr != NULL);
      defCfgPtr = (gyro_cal_config_s*)(algoPtr->defConfigData.memPtr);

      algoPtr->defConfigData.memSize = sizeof(gyro_cal_config_s);
      algoPtr->defInputDataSize = sizeof(gyro_cal_input_s);
      algoPtr->defOutputDataSize = sizeof(gyro_cal_output_s);

      algoPtr->persistAutoCalParam.memPtr = SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(gyro_cal_output_s));
      SNS_ASSERT(algoPtr->persistAutoCalParam.memPtr != NULL);
      algoPtr->persistAutoCalParam.memSize = sizeof(gyro_cal_output_s);
      SNS_OS_MEMZERO(algoPtr->persistAutoCalParam.memPtr, algoPtr->persistAutoCalParam.memSize);

      algoPtr->algoApi.sns_scm_algo_mem_req =
         (int32_t(*)(void*))gyro_cal_scm_mem_req_sns;
      algoPtr->algoApi.sns_scm_algo_reset =
         (void*(*)(void*,void*))gyro_cal_scm_state_reset_sns;
      algoPtr->algoApi.sns_scm_algo_update =
         (void(*)(void*,void*,void*))gyro_cal_scm_update_sns;

      algoPtr->regItemType[0] = SNS_SCM_REG_ITEM_TYPE_GROUP;
      algoPtr->regItemId[0] = SNS_REG_SCM_GROUP_GYRO_CAL_ALGO_V02;

      algoPtr->sensorIndex = sns_scm_get_sensor_index(SNS_SMGR_ID_GYRO_V01);

      /* Sensor types that depend on this calibration service */
      algoPtr->monitorSensorId[0] = SNS_SMGR_ID_GYRO_V01;
      algoPtr->monitorSensorId[1] = SNS_SMGR_ID_GAME_ROTATION_VECTOR_V01;
      algoPtr->monitorSensorCount = 2;

      algoPtr->period = sns_em_convert_usec_to_localtick(
                        SNS_SCM_GYRO_CAL_PERIOD_USEC);

      algoPtr->motionTrigFlags = MOTION_DETECT_FLAG_ACTIVE_IN_REST;

      algoPtr->enableAlgo = SNS_REG_GYRO_CAL_ENABLE_ALGO;
      defCfgPtr->sampleRate = SNS_REG_GYRO_CAL_DEF_SAMP_RATE_HZ_Q16;
      defCfgPtr->numSamples = SNS_REG_GYRO_CAL_DEF_NUM_SAMPLES;
      defCfgPtr->varianceThold = SNS_REG_GYRO_CAL_DEF_VARIANCE_THOLD;
//      algoPtr->defSensorReportRate = SNS_REG_GYRO_CAL_DEF_REPORT_RATE_Q16;
   }
   if (algoPtr->serviceId == SNS_SCM_MAG_CAL_SVC)
   {
      qmag_cal_config_s *defCfgPtr;
      algoPtr->defConfigData.memPtr = SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(qmag_cal_config_s));
      SNS_ASSERT(algoPtr->defConfigData.memPtr != NULL);
      defCfgPtr = (qmag_cal_config_s*)(algoPtr->defConfigData.memPtr);

      algoPtr->defConfigData.memSize = sizeof(qmag_cal_config_s);
      algoPtr->defInputDataSize = sizeof(qmag_cal_input_s);
      algoPtr->defOutputDataSize = sizeof(qmag_cal_output_s);

      algoPtr->persistAutoCalParam.memPtr = SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(qmag_cal_output_s));
      SNS_ASSERT(algoPtr->persistAutoCalParam.memPtr != NULL);
      algoPtr->persistAutoCalParam.memSize = sizeof(qmag_cal_output_s);
      SNS_OS_MEMZERO(algoPtr->persistAutoCalParam.memPtr, algoPtr->persistAutoCalParam.memSize);

      algoPtr->algoApi.sns_scm_algo_mem_req =
         (int32_t(*)(void*))qmag_cal_scm_mem_req;
      algoPtr->algoApi.sns_scm_algo_reset =
         (void*(*)(void*,void*))qmag_cal_scm_state_reset;
      algoPtr->algoApi.sns_scm_algo_update =
         (void(*)(void*,void*,void*))qmag_cal_scm_update;

      algoPtr->regItemType[0] = SNS_SCM_REG_ITEM_TYPE_GROUP;
      algoPtr->regItemId[0] = SNS_REG_SCM_GROUP_QMAG_CAL_ALGO_V02;
      algoPtr->regItemType[1] = SNS_SCM_REG_ITEM_TYPE_GROUP;
      algoPtr->regItemId[1] = SNS_REG_SCM_GROUP_QMAG_CAL_STATE_V02;


      algoPtr->sensorIndex = sns_scm_get_sensor_index(SNS_SMGR_ID_MAG_V01);

      /* Sensor types that depend on this calibration service */
      algoPtr->monitorSensorId[0] = SNS_SMGR_ID_MAG_V01;
      algoPtr->monitorSensorCount = 1;

      algoPtr->period = 0; // No duty cycling
                           // ASJ : Hook these to the registry 
      algoPtr->enableAlgo = 1;
      algoPtr->motionTrigFlags = MOTION_DETECT_FLAG_ACTIVE_IN_MOTION;
                             
      defCfgPtr->sample_rate_mag   =  SNS_REG_GROUP_QMAG_CAL_ALGO_DEF_SAMPLE_RATE_Q16;
      defCfgPtr->sample_rate_accel =  SNS_REG_GROUP_QMAG_CAL_ALGO_DEF_SAMPLE_RATE_Q16;

      //default tuning param config
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_calibration_history = SNS_REG_GROUP_QMAG_CAL_ALGO_DEF_PARAM3;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_radius_history = SNS_REG_GROUP_QMAG_CAL_ALGO_DEF_PARAM4;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_min_radius_gauss = SNS_REG_GROUP_QMAG_CAL_ALGO_DEF_PARAM5;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_max_radius_gauss = SNS_REG_GROUP_QMAG_CAL_ALGO_DEF_PARAM6;
      defCfgPtr->qmag_cal_tuning_param.min_rel_data_span_hi_accuracy_0 = SNS_REG_GROUP_QMAG_CAL_ALGO_DEF_PARAM7;
      defCfgPtr->qmag_cal_tuning_param.min_rel_data_span_hi_accuracy_1 = SNS_REG_GROUP_QMAG_CAL_ALGO_DEF_PARAM8;
      defCfgPtr->qmag_cal_tuning_param.min_rel_data_span_hi_accuracy_2 = SNS_REG_GROUP_QMAG_CAL_ALGO_DEF_PARAM9;
      defCfgPtr->qmag_cal_tuning_param.min_rel_data_span_hi_accuracy_3 = SNS_REG_GROUP_QMAG_CAL_ALGO_DEF_PARAM10;
      defCfgPtr->qmag_cal_tuning_param.min_accel_seg_variance_hi_0 = SNS_REG_GROUP_QMAG_CAL_ALGO_DEF_PARAM11;
      defCfgPtr->qmag_cal_tuning_param.min_accel_seg_variance_hi_1 = SNS_REG_GROUP_QMAG_CAL_ALGO_DEF_PARAM12;
      defCfgPtr->qmag_cal_tuning_param.min_accel_seg_variance_hi_2 = SNS_REG_GROUP_QMAG_CAL_ALGO_DEF_PARAM13;
      defCfgPtr->qmag_cal_tuning_param.min_accel_seg_variance_hi_3 = SNS_REG_GROUP_QMAG_CAL_ALGO_DEF_PARAM14;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_min_decimation_delta_sq = SNS_REG_GROUP_QMAG_CAL_ALGO_DEF_PARAM15;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_max_decimation_delta_sq = SNS_REG_GROUP_QMAG_CAL_ALGO_DEF_PARAM16;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_min_decimation_gyro_angle = SNS_REG_GROUP_QMAG_CAL_ALGO_DEF_PARAM17;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_default_3d_mag_field_intensity = SNS_REG_GROUP_QMAG_CAL_ALGO_DEF_PARAM18;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_hi_cal_win_size_second = SNS_REG_GROUP_QMAG_CAL_ALGO_DEF_PARAM19;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_calibration_cycle_sec = SNS_REG_GROUP_QMAG_CAL_ALGO_DEF_PARAM20;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_motion_window_sec = SNS_REG_GROUP_QMAG_CAL_ALGO_DEF_PARAM21;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_min_mag_samples_autonomous_10hz = SNS_REG_GROUP_QMAG_CAL_ALGO_DEF_PARAM22;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_mag_variance_window_sec = SNS_REG_GROUP_QMAG_CAL_ALGO_DEF_PARAM23;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_accel_variance_window_sec = SNS_REG_GROUP_QMAG_CAL_ALGO_DEF_PARAM24;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_gyro_variance_window_sec = SNS_REG_GROUP_QMAG_CAL_ALGO_DEF_PARAM25;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_mag_variance_thresh = SNS_REG_GROUP_QMAG_CAL_ALGO_DEF_PARAM26;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_accel_variance_thresh = SNS_REG_GROUP_QMAG_CAL_ALGO_DEF_PARAM27;

      defCfgPtr->qmag_cal_tuning_param.qmag_cal_default_decimation_delta_sq = defCfgPtr->qmag_cal_tuning_param.qmag_cal_max_decimation_delta_sq;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_hi_cal_win_dize_usec = defCfgPtr->qmag_cal_tuning_param.qmag_cal_hi_cal_win_size_second * 1000000.0f;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_decim_window_sec = defCfgPtr->qmag_cal_tuning_param.qmag_cal_hi_cal_win_size_second ;
      defCfgPtr->qmag_cal_tuning_param.qmag_cal_hi_cal_win_size_ticks = sns_em_convert_usec_to_dspstick((uint32_t)(defCfgPtr->qmag_cal_tuning_param.qmag_cal_hi_cal_win_dize_usec));

   }
   else
   {
      SNS_SCM_DEBUG1(ERROR, DBG_SCM_REG_ALGO_DFLT_ERR, algoPtr->serviceId);
      return SNS_ERR_FAILED;
   }

   return SNS_SUCCESS;
}

/*=========================================================================
  FUNCTION:  sns_scm_reg_sensor_status
  =========================================================================*/
/*!
  @brief Send a request to sensors manager for sensor status report

  @param[i] sensorId: sensor id

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_scm_reg_sensor_status(
   uint8_t sensorId)
{
   qmi_client_error_type               err;
   sns_smgr_sensor_status_monitor_req_msg_v02  *msgReqP;
   sns_smgr_sensor_status_monitor_resp_msg_v02 *msgRespP;
   size_t req_msg_size  = sizeof(sns_smgr_sensor_status_monitor_req_msg_v02);
   size_t resp_msg_size = sizeof(sns_smgr_sensor_status_monitor_resp_msg_v02);
   qmi_txn_handle                      txn_handle;

   msgReqP = (sns_smgr_sensor_status_monitor_req_msg_v02 *)
             SNS_OS_MALLOC(SNS_SCM_DBG_MOD, req_msg_size);
   SNS_ASSERT(msgReqP != NULL);

   msgRespP = (sns_smgr_sensor_status_monitor_resp_msg_v02 *)
              SNS_OS_MALLOC(SNS_SCM_DBG_MOD, resp_msg_size);
   SNS_ASSERT(msgRespP != NULL);

   msgReqP->sensor_id   = (uint64_t)sensorId;
   msgReqP->registering = true;

   err = sns_scm_qmi_client_send_msg_async (scm_smgr_int_qmi_client_if.user_handle, 
                                    SNS_SMGR_SENSOR_STATUS_MONITOR_REQ_V02,
                                    msgReqP, req_msg_size,
                                    msgRespP, resp_msg_size,
                                    sns_scm_smgr_int_resp_cb, NULL, &txn_handle);

   SNS_SCM_DEBUG3(MEDIUM, DBG_SCM_REQ_SNSR_STATUS_INFO,
                  SNS_SMGR_SENSOR_STATUS_ADD_V01, sensorId, 1);

   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_store_qmag_auto_cal_state_algodbase
  =========================================================================*/
/*!
  @brief Store qmag cal state to SCM's algo database

  @param[i] qmagCalState: pointer to qmag calibration algo state
  @param[i] qmagCalAlgoDBConfig: pointer to qmag cal algo config from sns_scm_algo_db

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_scm_store_qmag_auto_cal_state_algodbase(
   void* qmagCalState,
   void* qmagCalAlgoDBConfig)
{
   sns_err_code_e err = SNS_SUCCESS;
   qmag_cal_state_s *qmagCalStatePtr = (qmag_cal_state_s *)qmagCalState;
   qmag_cal_config_s *qmagCalAlgoDBConfigPtr = (qmag_cal_config_s *)qmagCalAlgoDBConfig;

   if (qmagCalStatePtr == NULL || qmagCalAlgoDBConfigPtr == NULL)
   {
      return SNS_ERR_FAILED;
   }

   // save persistent state parameters in algo database config
   SNS_OS_MEMCOPY(&qmagCalAlgoDBConfigPtr->persistent_state,&qmagCalStatePtr->persistent_state,sizeof(qmag_cal_persistent_state_s));

   SNS_SCM_PRINTF3(MED, "stored qmag auto cal state in algo dbase, published offsets: %d, %d, %d",
                   FX_FLTTOFIX_Q16(qmagCalAlgoDBConfigPtr->persistent_state.published_offs_HI[0]),
                   FX_FLTTOFIX_Q16(qmagCalAlgoDBConfigPtr->persistent_state.published_offs_HI[1]),
                   FX_FLTTOFIX_Q16(qmagCalAlgoDBConfigPtr->persistent_state.published_offs_HI[2]));

   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_store_qmag_cal_state_registry
  =========================================================================*/
/*!
  @brief Store qmag cal state from algo database in registry

  @param[i] qmagCalAlgoDBConfig: pointer to qmag cal algo config from sns_scm_algo_db

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_scm_store_qmag_auto_cal_state_registry(
   void* qmagCalAlgoDBConfig)
{
#ifndef ADSP_STANDALONE
   qmag_cal_config_s *qmagCalAlgoDBConfigPtr = (qmag_cal_config_s *)qmagCalAlgoDBConfig;
   sns_reg_group_write_req_msg_v02* msgPtr;
   sns_reg_qmag_cal_state_data_group_s *dataPtr;
   qmi_client_error_type            err;
   sns_reg_group_write_resp_msg_v02 *msgRespPtr;
   qmi_txn_handle                   txn_handle;
#endif /* ADSP_STANDALONE */

#ifndef ADSP_STANDALONE
   msgRespPtr = (sns_reg_group_write_resp_msg_v02 *)
              SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_reg_group_write_resp_msg_v02));
   SNS_ASSERT(msgRespPtr != NULL);

   msgPtr = (sns_reg_group_write_req_msg_v02 *)
            SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_reg_group_write_req_msg_v02));
   SNS_ASSERT(msgPtr != NULL);

   msgPtr->group_id = SNS_REG_SCM_GROUP_QMAG_CAL_STATE_V02;
   msgPtr->data_len = sizeof(sns_reg_qmag_cal_state_data_group_s);
   dataPtr = (sns_reg_qmag_cal_state_data_group_s *)msgPtr->data;

   dataPtr->published_offs_HI_x = qmagCalAlgoDBConfigPtr->persistent_state.published_offs_HI[0];
   dataPtr->published_offs_HI_y = qmagCalAlgoDBConfigPtr->persistent_state.published_offs_HI[1];
   dataPtr->published_offs_HI_z = qmagCalAlgoDBConfigPtr->persistent_state.published_offs_HI[2];
   dataPtr->published_accuracy_HI = qmagCalAlgoDBConfigPtr->persistent_state.published_accuracy_HI;
   dataPtr->published_offset_time = qmagCalAlgoDBConfigPtr->persistent_state.published_offset_time;
   dataPtr->published_offset_valid = qmagCalAlgoDBConfigPtr->persistent_state.published_offset_valid;

   err = sns_scm_qmi_client_send_msg_async (scm_reg_qmi_client_if.user_handle, 
                                    SNS_REG_GROUP_WRITE_REQ_V02,
                                    msgPtr, sizeof(sns_reg_group_write_req_msg_v02),
                                    msgRespPtr, sizeof(sns_reg_group_write_resp_msg_v02),
                                    sns_scm_reg_resp_cb, NULL, &txn_handle);

   if (QMI_NO_ERR == err)
   {
      SNS_SCM_PRINTF3(MED, "stored qmag cal state in registry, published offsets: %d, %d, %d",
                   FX_FLTTOFIX_Q16(dataPtr->published_offs_HI_x),
                   FX_FLTTOFIX_Q16(dataPtr->published_offs_HI_y),
                   FX_FLTTOFIX_Q16(dataPtr->published_offs_HI_z));
      SNS_SCM_PRINTF1(MED, "stored qmag cal state in registry, published accuracy: %d",
                   FX_FLTTOFIX_Q16(dataPtr->published_accuracy_HI));
   }
   else
   {
      SNS_SCM_PRINTF1(ERROR, "qmag cal state registry write failed with err: %d",err);
   }

   return err;
#else
   return SNS_ERR_FAILED;
#endif /* ADSP_STANDALONE */

}

/*=========================================================================
  FUNCTION:  sns_scm_store_qmag_auto_cal_params_registry
  =========================================================================*/
/*!
  @brief Store qmag auto cal parameters in registry 

  @param[i] qmagCalOutput: pointer to qmag calibration algo output

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_scm_store_qmag_auto_cal_params_registry(
   void* qmagCalOutput)
{
#ifndef ADSP_STANDALONE
   sns_reg_mag_dyn_cal_params_data_group_s *dataPtr;
   sns_reg_group_write_req_msg_v02* msgPtr;
#endif /* ADSP_STANDALONE */
   qmag_cal_output_s *qmagCalPtr = (qmag_cal_output_s *)qmagCalOutput;

   //skip storing to registry if calibration did not occur
   if (qmagCalPtr->offset_HI[0] == 0 && qmagCalPtr->offset_HI[1] == 0 && qmagCalPtr->offset_HI[2] == 0)
   {
      return SNS_SUCCESS;
   }
#ifndef ADSP_STANDALONE
   qmi_client_error_type            err;
   sns_reg_group_write_resp_msg_v02 *msgRespP;
   qmi_txn_handle                   txn_handle;

   msgRespP = (sns_reg_group_write_resp_msg_v02 *)
              SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_reg_group_write_resp_msg_v02));
   SNS_ASSERT(msgRespP != NULL);

   msgPtr = (sns_reg_group_write_req_msg_v02 *)
            SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_reg_group_write_req_msg_v02));
   SNS_ASSERT(msgPtr != NULL);

   msgPtr->group_id = SNS_REG_SCM_GROUP_MAG_DYN_CAL_PARAMS_V02;
   msgPtr->data_len = sizeof(sns_reg_mag_dyn_cal_params_data_group_s);
   dataPtr = (sns_reg_mag_dyn_cal_params_data_group_s *)msgPtr->data;

   dataPtr->version_no = 1;
   dataPtr->bias_valid = 1;
   dataPtr->cal_mat_valid = 0;

   dataPtr->x_dyn_bias = -(qmagCalPtr->offset_HI[0]);
   dataPtr->y_dyn_bias= -(qmagCalPtr->offset_HI[1]);
   dataPtr->z_dyn_bias = -(qmagCalPtr->offset_HI[2]);
   dataPtr->compensation_matrix[0][0] = FX_ONE_Q16;
   dataPtr->compensation_matrix[0][1] = 0;
   dataPtr->compensation_matrix[0][2] = 0;
   dataPtr->compensation_matrix[1][0] = 0;
   dataPtr->compensation_matrix[1][1] = FX_ONE_Q16;
   dataPtr->compensation_matrix[1][2] = 0;
   dataPtr->compensation_matrix[2][0] = 0;
   dataPtr->compensation_matrix[2][1] = 0;
   dataPtr->compensation_matrix[2][2] = FX_ONE_Q16;
   dataPtr->accuracy = qmagCalPtr->accuracy_HI;

   err = sns_scm_qmi_client_send_msg_async (scm_reg_qmi_client_if.user_handle, 
                                    SNS_REG_GROUP_WRITE_REQ_V02,
                                    msgPtr, sizeof(sns_reg_group_write_req_msg_v02),
                                    msgRespP, sizeof(sns_reg_group_write_resp_msg_v02),
                                    sns_scm_reg_resp_cb, NULL, &txn_handle);

   if (QMI_NO_ERR == err)
   {
      SNS_SCM_PRINTF3(MED, "stored qmag auto cal params in registry, offsets: %d, %d, %d",
                   dataPtr->x_dyn_bias,
                   dataPtr->y_dyn_bias,
                   dataPtr->z_dyn_bias);
      SNS_SCM_PRINTF1(MED, "stored qmag auto cal params in registry, accuracy: %d",
                      dataPtr->accuracy);
   }
   else
   {
      SNS_SCM_PRINTF1(ERROR, "qmag cal params registry write failed with err: %d",err);
   }

   return err;
#else
   return SNS_ERR_FAILED;
#endif /* ADSP_STANDALONE */

}

/*=========================================================================
  FUNCTION:  sns_scm_store_gyro_auto_cal_params_registry
  =========================================================================*/
/*!
  @brief Store gyro dynamic calibration parameters in registry

  @param[i] gyroCalOutput: pointer to gyro calibration algo output

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_scm_store_gyro_auto_cal_params_registry(
   void* gyroCalOutput)
{
   gyro_cal_output_s *gyroCalPtr = (gyro_cal_output_s *)gyroCalOutput;

   //skip storing to registry if calibration did not occur
   if (gyroCalPtr->bias[0] == 0 && gyroCalPtr->bias[1] == 0 && gyroCalPtr->bias[2] == 0)
   {
      return SNS_SUCCESS;
   }
#ifndef ADSP_STANDALONE
   int32_t                          *dataPtr;
   sns_reg_group_write_req_msg_v02  *msgPtr;
   qmi_client_error_type            err;
   sns_reg_group_write_resp_msg_v02 *msgRespP;
   qmi_txn_handle                   txn_handle;

   msgRespP = (sns_reg_group_write_resp_msg_v02 *)
              SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_reg_group_write_resp_msg_v02));
   SNS_ASSERT(msgRespP != NULL);

   msgPtr = (sns_reg_group_write_req_msg_v02 *)
            SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_reg_group_write_req_msg_v02));
   SNS_ASSERT(msgPtr != NULL);

   msgPtr->group_id = SNS_REG_SCM_GROUP_GYRO_DYN_CAL_PARAMS_V02;
   msgPtr->data_len = 6 * sizeof(int32_t);
   dataPtr = (int32_t*)msgPtr->data;

   dataPtr[0] = -(gyroCalPtr->bias[0]);
   dataPtr[1] = -(gyroCalPtr->bias[1]);
   dataPtr[2] = -(gyroCalPtr->bias[2]);
   dataPtr[3] = FX_ONE_Q16;
   dataPtr[4] = FX_ONE_Q16;
   dataPtr[5] = FX_ONE_Q16;

   err = sns_scm_qmi_client_send_msg_async (scm_reg_qmi_client_if.user_handle, 
                                    SNS_REG_GROUP_WRITE_REQ_V02,
                                    msgPtr, sizeof(sns_reg_group_write_req_msg_v02),
                                    msgRespP, sizeof(sns_reg_group_write_resp_msg_v02),
                                    sns_scm_reg_resp_cb, NULL, &txn_handle);

   if (QMI_NO_ERR == err)
   {
      SNS_SCM_PRINTF3(MED, "stored gyro auto cal params in registry, offsets: %d, %d, %d",
                      dataPtr[0],
                      dataPtr[1],
                      dataPtr[2]);
   }
   else
   {
      SNS_SCM_PRINTF1(ERROR, "gyro cal params registry write failed with err: %d",err);
   }

   return err;
#else
   return SNS_ERR_FAILED;
#endif /* ADSP_STANDALONE */

}

/*=========================================================================
  FUNCTION:  sns_scm_reg_resp_cb
  =========================================================================*/
/*!
  @brief SCM - REG response callback

  @detail
  Handles the responses from REG to SCM.

  @param[i]   user_handle         Handle used by the infrastructure to identify different clients.
  @param[i]   msg_id                Message ID
  @param[i]   resp_buf              Pointer to the response buffer
  @param[i]   resp_buf_len        Length of the response buffer
  @param[i]   user_cb_data       Pointer to the User-data
  @param[i]   transp_err           Error code

  @return None
*/
/*=======================================================================*/
void sns_scm_reg_resp_cb(
   qmi_client_type user_handle,
   unsigned int msg_id,
   void *resp_buf,
   unsigned int resp_buf_len,
   void *user_cb_data,
   qmi_client_error_type transp_err)
{
  sns_scm_q_put(msg_id, resp_buf, SCM_REG_RESP_MSG, transp_err, 0, 0);
}

/*=========================================================================
  FUNCTION:  sns_scm_send_smgr_version_req
  =========================================================================*/
/*!
  @brief  Sends version request message to Sensor Manager.

  @return  None
*/
/*=======================================================================*/
static sns_err_code_e sns_scm_send_smgr_version_req( void )
{
   qmi_client_error_type            err;
   sns_common_version_resp_msg_v01 *msgRespP;
   qmi_txn_handle                   txn_handle;

   msgRespP = (sns_common_version_resp_msg_v01 *)
              SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_common_version_resp_msg_v01));
   SNS_ASSERT(msgRespP != NULL);

   // Send SMGR API version request
   err = sns_scm_qmi_client_send_msg_async (scm_smgr_qmi_client_if.user_handle, 
                                    SNS_SMGR_VERSION_REQ_V01,
                                    NULL, 0, // no payload
                                    msgRespP, sizeof(sns_common_version_resp_msg_v01),
                                    sns_scm_smgr_resp_cb, NULL, &txn_handle);

   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_detect_smgr_buffering
  =========================================================================*/
/*!
  @brief  Detects support for Buffering in Sensor Manager.

  @return  true, if SMGR supports buffering; false, otherwise
*/
/*=======================================================================*/
bool sns_scm_detect_smgr_buffering(
   OS_FLAG_GRP *sigEventPtr)
{
   bool buffering_supported = false;
   OS_FLAGS sigFlags = 0;
   scm_q_item_s* msgPtr;
   uint8_t err;
   bool gotVersion = false;

   //request smgr version info
   if (sns_scm_send_smgr_version_req() == SNS_SUCCESS)
   {
      if (sns_scm_reg_init_timer(sigEventPtr) == SNS_SUCCESS)
      {
         while( !gotVersion )
         {
            //wait for smgr version info response
            sigFlags = sns_os_sigs_pend(sns_scm_init_sig_event,
                                        SNS_SCM_SAM_SMGR_FLAGS,
                                        OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME,
                                        0,
                                        &err);
            SNS_ASSERT(err == 0);

            //message event check
            if (sigFlags & SCM_SAM_SMGR_MSG_SIG)
            {
               while (NULL != (msgPtr = (scm_q_item_s *)sns_scm_q_get(SCM_SAM_SMGR_MSG_SIG)))
               {
                 if (msgPtr->scm_msg_type == SCM_SMGR_RESP_MSG)
                 {
                   if (msgPtr->msg_id == SNS_SMGR_VERSION_RESP_V01)
                   {
                      sns_common_version_resp_msg_v01* respMsgPtr =
                         (sns_common_version_resp_msg_v01 *)(msgPtr->body_ptr);
                      if (respMsgPtr->resp.sns_result_t == SNS_SUCCESS)
                      {
                         buffering_supported =
                          (respMsgPtr->interface_version_number >= 0x08);
                      }
                      gotVersion = true;
                      break;
                   }
                   else
                   {
                     SNS_SCM_PRINTF1(ERROR, "SCM:Invalid resp msg = %d", msgPtr->msg_id);
                   }
                 }
                 else
                 {
                   SNS_SCM_PRINTF1(ERROR, "Invalid scm_msg_type = %d", msgPtr->scm_msg_type);
                 }

                 SNS_OS_FREE(msgPtr->body_ptr);
                 SNS_OS_FREE(msgPtr);
                 msgPtr = NULL;
               }
            }

            //client report timer event check
            if (sigFlags & SNS_SCM_REPORT_TIMER_SIG)
            {
               SNS_SCM_PRINTF0(LOW, "SCM: init timer expired");
               sigFlags &= (~SNS_SCM_REPORT_TIMER_SIG);
               break;
            }

            sigFlags &= (~SNS_SCM_SAM_SMGR_FLAGS);

            if (sigFlags != 0)
            {
               SNS_SCM_DEBUG1(MEDIUM, DBG_SCM_PROCESS_EVT_UNKWN_EVT, sigFlags);
            }
         }
      }
      sns_scm_dereg_init_timer();
   }

   return buffering_supported;
}

/*=========================================================================
  FUNCTION:  sns_scm_send_registry_version_req
  =========================================================================*/
/*!
  @brief  Sends version request message to Registry service.

  @return  None
*/
/*=======================================================================*/
static sns_err_code_e sns_scm_send_registry_version_req( void )
{
   qmi_client_error_type            err = SNS_ERR_FAILED;
#ifndef ADSP_STANDALONE
   sns_common_version_resp_msg_v01 *msgRespP;
   qmi_txn_handle                   txn_handle;

   msgRespP = (sns_common_version_resp_msg_v01 *)
              SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_common_version_resp_msg_v01));
   SNS_ASSERT(msgRespP != NULL);

   // Send Registry API version request
   err = sns_scm_qmi_client_send_msg_async (scm_reg_qmi_client_if.user_handle, 
                                    SNS_REG_VERSION_REQ_V02,
                                    NULL, 0, // no payload
                                    msgRespP, sizeof(sns_common_version_resp_msg_v01),
                                    sns_scm_reg_resp_cb, NULL, &txn_handle);
#endif /* ADSP_STANDALONE */
   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_check_sensor_report_rate
  =========================================================================*/
/*!
  @brief  Checks registry version to see if default sensor report rates are
          available.

  @return  true, if expected to be available; false, otherwise
*/
/*=======================================================================*/
bool sns_scm_check_sensor_report_rate(
   OS_FLAG_GRP *sigEventPtr)
{
   bool sensor_report_rate_available = false;
   OS_FLAGS sigFlags = 0;
   scm_q_item_s* msgPtr;
   uint8_t err;
   bool gotVersion = false;

   //request sensor info
   if (sns_scm_send_registry_version_req() == SNS_SUCCESS)
   {
      if (sns_scm_reg_init_timer(sigEventPtr) == SNS_SUCCESS)
      {
         while( !gotVersion )
         {
            //wait for sensor info response
            sigFlags = sns_os_sigs_pend(sns_scm_init_sig_event,
                                        SNS_SCM_REG_FLAGS,
                                        OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME,
                                        0,
                                        &err);
            SNS_ASSERT(err == 0);

            //message event check
            if (sigFlags & SCM_REG_MSG_SIG)
            {
               while (NULL != (msgPtr = (scm_q_item_s *)sns_scm_q_get(SCM_REG_MSG_SIG)))
               {
                 if (msgPtr->scm_msg_type == SCM_REG_RESP_MSG)
                 {
                   if (msgPtr->msg_id == SNS_REG_VERSION_RESP_V02)
                   {
                      sns_common_version_resp_msg_v01* respMsgPtr =
                         (sns_common_version_resp_msg_v01 *)(msgPtr->body_ptr);
                      if (respMsgPtr->resp.sns_result_t == SNS_SUCCESS)
                      {
                         sensor_report_rate_available =
                          (respMsgPtr->interface_version_number >= 0x0B);
                      }
                      gotVersion = true;
                      break;
                   }
                   else
                   {
                     SNS_SCM_PRINTF1(ERROR, "SCM:Invalid resp msg = %d", msgPtr->msg_id);
                   }
                 }
                 else
                 {
                   SNS_SCM_PRINTF1(ERROR, "SCM:Invalid scm_msg_type = %d", msgPtr->scm_msg_type);
                 }

                 SNS_OS_FREE(msgPtr->body_ptr);
                 SNS_OS_FREE(msgPtr);
                 msgPtr = NULL;
               }
            }

            //client report timer event check
            if (sigFlags & SNS_SCM_REPORT_TIMER_SIG)
            {
               SNS_SCM_PRINTF0(LOW, "SCM: init timer expired");
               sigFlags &= (~SNS_SCM_REPORT_TIMER_SIG);
               break;
            }

            sigFlags &= (~SNS_SCM_REG_FLAGS);

            if (sigFlags != 0)
            {
               SNS_SCM_DEBUG1(MEDIUM, DBG_SCM_PROCESS_EVT_UNKWN_EVT, sigFlags);
            }
         }
      }
      sns_scm_dereg_init_timer();
   }

   return sensor_report_rate_available;
}

#ifdef SNS_SCM_TEST_ENABLE
void sns_scm_ext_test (void)
{
  gyro_cal_output_s gyroCalPtr;

  sns_scm_sensor_s snsInfoPtr =
  {
    SNS_SMGR_ID_GYRO_V01,
    SNS_SMGR_DATA_TYPE_PRIMARY_V01,
    0
  };

  sns_scm_send_cal_msg(&snsInfoPtr);
  sns_scm_req_reg_data(SNS_SCM_REG_ITEM_TYPE_GROUP, 1);
  sns_scm_req_reg_data(SNS_SCM_REG_ITEM_TYPE_SINGLE, 1);
  sns_scm_store_gyro_auto_cal_params_registry (SNS_REG_GROUP_WRITE_REQ_V02, &gyroCalPtr);
}
#endif
