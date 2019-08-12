/*!
  @file
  rfcommon_autopin.c

  @brief
  This module contains implementations of common AutoPin.
*/

/*==============================================================================

  Copyright (c) 2015 - 2016 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/autopin/src/rfcommon_autopin.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/13/16   pkm     Make sure to Apply correction factor only once.
02/15/15   ag      Auto-pin Acq/Trk mode changes
02/5/16    vr      auto-pin correction factor check-in
02/01/16   whz     Log packet support
02/01/16   vs      Do not apply Pin if Peak avg power > 35k
12/20/15   vr      auto-pin check-in
12/14/15   whz     Set imme_trig to FALSE by default
                   Sanity check on REFLOG and WTR timings
12/11/15   whz     Add TDS API
                   Change sign of the algorithm
12/10/15   ag      Addition of F3s, Add W API to call func for apin update
12/10/15   kma     Init REFLOG capture ptr in common code
12/09/15   ag      Convert Reflog PAR returned by FW into db100 unit
12/03/15   whz     Reset status in enable_tx
11/02/15   kma     Set autopin in progress to false when process is done
11/02/15   kma     Added flag to enable WTR debug mem dump
10/29/15   whz     Remove timers completely
10/22/15   kma     Add capture control and pin update API for PAR algo 
10/20/15   whz     Move buffer cleanup from disable to enable API
10/14/15   whz     Remove timers as we schedule AutoPin every time
09/22/15   kma     Updated debug msg
09/15/15   whz     Revert CL905111 but provide another fix
09/14/15   whz     Remove unncessary FBRx AG check
                   Clear Pin accum when disabling Tx
08/25/15   cdb     Update default loop speed for LTE
07/21/15   cdb     Add EFS support 
07/16/15   cdb     Abstract Thor/AMAM specific processing 
07/15/15   cdb     Add support to override Autopin operation
07/02/15   whz     Do not clean-up timers in exit_mode
06/09/15   whz     Provide buffer to store FBRx IQ
06/04/15   whz     Initial version

==============================================================================*/


#include "rfcommon_autopin.h"
#include "msg.h"
#include "rf_dispatch.h"
#include "rf_cmd_interface.h"
#include "rf_task.h"
#include "rflm_dm_api.h"
#include "rflm_cmn_msg.h"
#include "modem_mem.h"
#include "rflm_autopin.h"
#include "rflm_api_fbrx_fw.h"
#include "rfcommon_autopin_db.h"
#include "fw_autopin_intf.h"
#include "rfcommon_mdsp_data.h"
#include "rfcommon_autopin_msm.h"
#ifdef FEATURE_WCDMA
#include "rfwcdma_core_auto_pin.h"
#endif
#ifdef FEATURE_TDSCDMA
#include "rf_tdscdma_core_txlin.h"
#endif
#include "rflm_autopin_log.h"
#include "stringl.h"
#include "rflm_api_autopin_hw.h"

extern uint32 rflm_autopin_seq_nr;
/* Absolute Value */
#ifndef ABS_VAL
#define ABS_VAL(x) (((x)<0) ? -(x) : (x))
#endif


typedef enum
{
    FTM_AUTOPIN_CMD_OVR_CTRL = 1,     /* Enable/Disable AutoPin override mode */
    FTM_AUTOPIN_CMD_RUN_FREEZE,       /* Override to freeze run decision execution */
    FTM_AUTOPIN_CMD_ACCUM_FREEZE,     /* Override to freeze pin accum updates */
    FTM_AUTOPIN_CMD_ACCUM_OVERRIDE,   /* Override to Pin accum values */
    FTM_AUTOPIN_CMD_DEBUG_CTRL,       /* Reserved for generic debug */
    FTM_AUTOPIN_CMD_MAX
}ftm_autopin_cmd_type;

extern void rf_cdma_autopin_update_pin(
  				uint32 handle,
				uint8 pa_state,
			        int16 pin_accum);

static timetick_type rfcommon_autopin_acq_time_ms[RFCOM_TXLIN_NUM_LINEARIZERS][RFLM_MAX_TECH];

static timetick_type rfcommon_autopin_trk_time_ms[RFCOM_TXLIN_NUM_LINEARIZERS][RFLM_MAX_TECH];

static boolean rfcommon_autopin_msg_register_once = FALSE;

static boolean rfcommon_autopin_debug = FALSE;

uint32 rfcommon_autopin_tx_iqbuf[RFLM_AUTOPIN_MAX_TX_SAMPLE_SIZE] = { 0 };
uint32 rfcommon_autopin_tx_proc_iqbuf[RFLM_AUTOPIN_MAX_TX_SAMPLE_SIZE] = { 0 };

uint32 rfcommon_autopin_rx_iqbuf[RFLM_AUTOPIN_MAX_RX_SAMPLE_SIZE] = { 0 };
uint32 rfcommon_autopin_rx_proc_iqbuf[RFLM_AUTOPIN_MAX_RX_SAMPLE_SIZE] = { 0 };

/*Structure to store REFLOG capture sample*/
uint32 rfcommon_autopin_reflog_sample[RFLM_AUTOPIN_REFLOG_MAX_SAMPLE_SIZE] = { 0 };

/* default algorithm parameters */
#define RFLM_AUTOPIN_DEFAULT_ACQ_TIME1 (100)  /* ms */
#define RFLM_AUTOPIN_DEFAULT_ACQ_TIME2  (10)  /* ms */
#define RFLM_AUTOPIN_DEFAULT_TRK_TIME (1000)  /* ms */
#define RFLM_AUTOPIN_DEFAULT_TRK_THRESH (0)
#define RFLM_AUTOPIN_DEFAULT_STEP_SIZE (1)    /* 0.1dB dB10 */

#define RFLM_CDMA_AUTOPIN_ACQ_LIMIT  3
#define RFLM_CDMA_AUTOPIN_TRK_LIMIT  4

#define RFLM_WCDMA_AUTOPIN_ACQ_LIMIT  3
#define RFLM_WCDMA_AUTOPIN_TRK_LIMIT  4


#define RFLM_TDSCDMA_AUTOPIN_ACQ_LIMIT  4
#define RFLM_TDSCDMA_AUTOPIN_TRK_LIMIT  5

#define RFLM_LTE_AUTOPIN_ACQ_LIMIT  3
#define RFLM_LTE_AUTOPIN_TRK_LIMIT  5

#define INVALID_MEAS_THRESHOLD_ACQ  8
#define INVALID_MEAS_THRESHOLD_TRK  4


/*PAR autopin debug flag*/
boolean wtr_debug_mem_enable = FALSE;

/*WTR debug mem dump structure*/
uint8 wtr_debug_mem[WTR_DEBUG_MEM_SIZE];

static boolean wtr_capture_done = FALSE;
static boolean reflog_capture_done = FALSE;
static boolean wtr_capture_valid = FALSE;
static boolean reflog_capture_valid = FALSE;

uint16 iCount = 0;
uint8 num_invalid_meas_in_Acq = 0;
uint8 num_invalid_meas_in_Trk = 0;



void rfcommon_autopin_get_nv_info(rfm_mode_enum_type rfm_tech)
{
  uint8 pa_state;
  uint32 acq_time;
  autopin_efs_record_t* efs_record;

  rflm_tech_id_t tech = rfcommon_autopin_mc_convert_rfm_mode_to_rflm_tech(rfm_tech);

  efs_record = rfcommon_autopin_efs_get_record(tech);

  if (efs_record==NULL)
  {
    /* no valid EFS file exist. Use default values */
    switch (tech)
    {
      case RFLM_TECH_1X:
      case RFLM_TECH_HDR:
      case RFLM_TECH_WCDMA:
      case RFLM_TECH_TDSCDMA:
        acq_time = RFLM_AUTOPIN_DEFAULT_ACQ_TIME1;
        break;
      default: 
        acq_time = RFLM_AUTOPIN_DEFAULT_ACQ_TIME2;
        break;
  }
    for (pa_state = 0; pa_state < RFCOM_TXLIN_NUM_LINEARIZERS; pa_state++)
    {
      rfcommon_autopin_acq_time_ms[pa_state][tech] = acq_time;
      rfcommon_autopin_trk_time_ms[pa_state][tech] = RFLM_AUTOPIN_DEFAULT_TRK_TIME;
    }
    rflm_autopin_set_params(tech, RFLM_AUTOPIN_DEFAULT_TRK_THRESH, RFLM_AUTOPIN_DEFAULT_STEP_SIZE);
  }
  else
  {
    /* valid EFS file exists, use efs record values for this tech */
    for (pa_state = 0; pa_state < RFCOM_TXLIN_NUM_LINEARIZERS; pa_state++)
    {
      rfcommon_autopin_acq_time_ms[pa_state][tech] = efs_record->loop_speed_acq;
      rfcommon_autopin_trk_time_ms[pa_state][tech] = efs_record->loop_speed_trk;
    }
    rflm_autopin_set_params(tech, efs_record->trk_thresh, efs_record->pin_step);
  }
}


rflm_tech_id_t rfcommon_autopin_mc_convert_rfm_mode_to_rflm_tech(rfm_mode_enum_type rfm_tech)
{
  rflm_tech_id_t rflm_tech;
  
  switch (rfm_tech)
  {
  case RFM_IMT_MODE:
  case RFM_IMT_MODE_2:
    rflm_tech = RFLM_TECH_WCDMA;
    break;

  case RFM_1X_MODE:
    rflm_tech = RFLM_TECH_1X;
    break;

  case RFM_1XEVDO_MODE:
  case RFM_SVDO_MODE:
    rflm_tech = RFLM_TECH_HDR;
    break;

  case RFM_LTE_MODE:
  case RFM_LTE_FTM_MODE:
    rflm_tech = RFLM_TECH_LTE;
    break;

  case RFM_TDSCDMA_MODE:
  case RFM_TDSCDMA_MODE_FTM:
    rflm_tech = RFLM_TECH_TDSCDMA;
    break;

  default:
    rflm_tech = RFLM_MAX_TECH;
    MSG_FATAL("AutoPin doesn't support tech %d",
              rfm_tech, 0, 0);
  }

  return rflm_tech;
}


/* Check if AutoPin feature is enabled for rfm_tech
 */
boolean rfcommon_autopin_is_enabled(rfm_mode_enum_type rfm_tech)
{
  rflm_tech_id_t tech = rfcommon_autopin_mc_convert_rfm_mode_to_rflm_tech(rfm_tech);
  return rflm_autopin_is_enabled(tech);
}


/* Do common and Tech specific AutoPin initialization
 * Need to be called in Tech's mc enter_mode
 */
rfcommon_autopin_error_type rfcommon_autopin_mc_enter_mode(rfm_mode_enum_type rfm_tech)
{
  rflm_tech_id_t tech = rfcommon_autopin_mc_convert_rfm_mode_to_rflm_tech(rfm_tech);
  rfcommon_mdsp_smem_data_type *smem_data = NULL;

  if (!rfcommon_autopin_is_enabled(rfm_tech))
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "AutoPin is not enabled for tech %d",
          rfm_tech);
    
    return RFCOMMON_AUTOPIN_ERROR;
  }

  rfcommon_autopin_get_nv_info(rfm_tech);

  if (!rfcommon_autopin_msg_register_once)
  {
    smem_data = rfcommon_mdsp_get_smem_data();

    if (smem_data == NULL)
    {
      RF_MSG( RF_ERROR, "rfcommon_autopin_mc_enter_mode: SMEM null" );
      return RFCOMMON_AUTOPIN_ERROR;
    }

    if (smem_data->initialized)
    {
      rflm_autopin_fw_autopin_fn = smem_data->smem_ptr->fw_autopin_fn;
    }
    else
    {
      RF_MSG( RF_ERROR, "rfcommon_autopin_mc_enter_mode: FW AutoPin function pointer will be null!" );
      return RFCOMMON_AUTOPIN_ERROR;
    }

    MSG(MSG_SSID_RF, MSG_LEGACY_HIGH,
        "AutoPin registers FW function pointer");

    rf_dispatch_register_command(rf_task_get_dispatch_handle(),
                                 MSGR_TECH_RFLM, 
                                 RFLM_CMN_AUTOPIN_RSP, 
                                 rfcommon_autopin_reflog_done_response,
                                 NULL,
                                 1<<RF_DISPATACH_MSGR_REG);

    rfcommon_autopin_msg_register_once = TRUE;
  }

  return RFCOMMON_AUTOPIN_SUCCESS;
}


/* Do AutoPin cleanup
 * Need to be called in Tech's mc exit_mode
 */
rfcommon_autopin_error_type rfcommon_autopin_mc_exit_mode(rfm_mode_enum_type rfm_tech)
{
  rflm_tech_id_t tech = rfcommon_autopin_mc_convert_rfm_mode_to_rflm_tech(rfm_tech);

  if (!rfcommon_autopin_is_enabled(rfm_tech))
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "AutoPin is not enabled for tech %d",
          rfm_tech);

    return RFCOMMON_AUTOPIN_ERROR;
  }

  return RFCOMMON_AUTOPIN_SUCCESS;
}


/* Do Tx specific initialization
 * Needs to be called when a new Tx handle is turned on 
 * (for example, in Tech's mc enable_tx) 
 * The associated AutoPin buffer in DM will be configured 
 */
rfcommon_autopin_error_type rfcommon_autopin_mc_enable_tx(rflm_handle_tx_t tx_handle,
                                                          rfm_mode_enum_type rfm_tech)
{
  rflm_tech_id_t tech = rfcommon_autopin_mc_convert_rfm_mode_to_rflm_tech(rfm_tech);

  rflm_autopin_dm_template_t *autopin_template = NULL;

  if (!rfcommon_autopin_is_enabled(rfm_tech))
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "AutoPin is not enabled for tech %d",
          rfm_tech);
    
    return RFCOMMON_AUTOPIN_ERROR;
  }

  autopin_template = (rflm_autopin_dm_template_t *)rflm_dm_get_autopin_buf_ptr(tx_handle);
 
   if (autopin_template == NULL)
   {
     MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "Failed to get AutoPin buffer from DM for tech %d, Tx handle %d",
            rfm_tech, tx_handle);
 
     return RFCOMMON_AUTOPIN_ERROR;
   }

  autopin_template->autopin_ctl.tech = tech;

  autopin_template->autopin_ctl.reflog_capture_input.ftm_mode = 0;

  autopin_template->autopin_ctl.reflog_capture_input.tx_iq_sample_ptr = rfcommon_autopin_reflog_sample;

  /* On TDSCDMA, if the scheduled UL slot is the first one in a subframe, we must make sure Common FW
   * programms REFLOG _before_ TFW sets OFFL_EN high.  TDSCDMA will overwite this flag to TRUE */
  autopin_template->autopin_ctl.reflog_capture_input.imme_trig = FALSE;

  memset(autopin_template->autopin_ctl.pin_accum, 0, sizeof(rflm_db10_t)*RFLM_TXAGC_PA_STATE_MAX);

  /* Initialize the run state to ACQ mode */
  memset(autopin_template->autopin_ctl.run_state, /*RFLM_AUTOPIN_DEFAULT*/RFLM_AUTOPIN_ACQ , sizeof(rflm_autopin_state)*RFLM_TXAGC_PA_STATE_MAX);

  iCount = 0;
  num_invalid_meas_in_Acq = 0;
  num_invalid_meas_in_Trk = 0;
  autopin_template->autopin_ctl.auxiliary_data = FALSE;
  
   rflm_autopin_xpt_sanity_check(tx_handle, RFLM_TXAGC_MAX_XPT_MODE);

  /* Reset flags for the next capture */
  /* This is to guarantee the status is clear if Tx is abnormally disabled */
  wtr_capture_done = FALSE;
  reflog_capture_done = FALSE;
  wtr_capture_valid = FALSE;
  reflog_capture_valid = FALSE;
  rflm_autopin_in_progress = FALSE;

  autopin_template->autopin_ctl.init_done = TRUE;
  
  /* Initialize the run period to 10msec */
  autopin_template->autopin_ctl.run_period = RFLM_AUTOPIN_ACQ_TIME_OUT;
  
  MSG_4(MSG_SSID_RF, MSG_LEGACY_HIGH,
          "AutoPin Tx is enabled for tech %d handle %d autopin run period : %d  autopin run state: %d",
          rfm_tech, tx_handle,autopin_template->autopin_ctl.run_period,autopin_template->autopin_ctl.run_state[RFLM_TXAGC_PA_STATE3]);
  
  return RFCOMMON_AUTOPIN_SUCCESS;
}


/* Do Tx specific cleanup
 * Needs to be called when the Tx handle is turned off 
 * (for example, in Tech's mc disable_tx)
 * The associated DM AutoPin buffer for the Tx handle will NOT be cleaned up
 */
rfcommon_autopin_error_type rfcommon_autopin_mc_disable_tx_preserv(rflm_handle_tx_t tx_handle,
                                                                   rfm_mode_enum_type rfm_tech)
{
  rflm_tech_id_t tech = rfcommon_autopin_mc_convert_rfm_mode_to_rflm_tech(rfm_tech);
  
  if (!rfcommon_autopin_is_enabled(rfm_tech))
  {
    return RFCOMMON_AUTOPIN_ERROR;
  }

  MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH,
        "AutoPin Tx is disabled for tech %d handle %d",
        rfm_tech, tx_handle);
  
  return RFCOMMON_AUTOPIN_SUCCESS;
}


/* Do Tx specific cleanup
 * Needs to be called when the Tx handle is turned off 
 * (for example, in Tech's mc disable_tx)
 * The associated DM AutoPin buffer for the Tx handle will be cleaned up
 */
rfcommon_autopin_error_type rfcommon_autopin_mc_disable_tx(rflm_handle_tx_t tx_handle,
                                                           rfm_mode_enum_type rfm_tech)
{
  rflm_autopin_dm_template_t *autopin_template = NULL;

  autopin_template = (rflm_autopin_dm_template_t *)rflm_dm_get_autopin_buf_ptr(tx_handle);

  if (autopin_template == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "Failed to get AutoPin buffer from DM for tech %d",
          rfm_tech);
    
    return RFCOMMON_AUTOPIN_ERROR;
  }

  autopin_template->autopin_ctl.tech = RFLM_MAX_TECH;

  if (rflm_autopin_in_progress == TRUE)
  {
    // Issue a warning but continue
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "Common FW FBRx process ongoing when disabling Tx for tech %d",
          rfm_tech);
  }
  
  autopin_template->autopin_ctl.init_done = FALSE;

  autopin_template->autopin_ctl.auxiliary_data = FALSE;
  
  return rfcommon_autopin_mc_disable_tx_preserv(tx_handle, rfm_tech);
}


/*------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Override AutoPin execution
 
  @details
  Set overrides to control AutoiPin loop and accumulator
 
  @param tx_handle
  TxLM handle to which the override applies
 
  @param ovr_cmd_id
  Override command ID
 
  @param ovr_enable
  Enable/disable the override applicable to the command ID

  @param ovr_val1
  Usage depend on command ID
  
  @return
  Status of execution
*/
rfcommon_autopin_error_type rfcommon_autopin_override_ctrl(uint8 tx_handle,
                                                           uint8 ovr_cmd_id,
                                                           uint8 ovr_enable,
                                                           uint8 ovr_val1,
                                                           uint8 ovr_val2)
{
  rflm_autopin_dm_template_t *autopin_template = NULL;
  rflm_autopin_proc_results_t meas_result;
  
  autopin_template = (rflm_autopin_dm_template_t *)rflm_dm_get_autopin_buf_ptr((rflm_handle_tx_t)tx_handle);
  if (autopin_template == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "Failed to get AutoPin buffer from DM for Tx handle %d",
          tx_handle);
    return RFCOMMON_AUTOPIN_ERROR;
  }
  
  switch (ovr_cmd_id)
  {
    case FTM_AUTOPIN_CMD_OVR_CTRL:
    case FTM_AUTOPIN_CMD_RUN_FREEZE:
    case FTM_AUTOPIN_CMD_ACCUM_FREEZE:
    case FTM_AUTOPIN_CMD_ACCUM_OVERRIDE:
        RF_MSG_3(RF_MED, "autopin override cmd %d, enable %d, handle %d", ovr_cmd_id, ovr_enable, tx_handle);
        if (ovr_cmd_id == FTM_AUTOPIN_CMD_OVR_CTRL)
        {
          /* Enable/disable debug mode for specified TX handle */
          autopin_template->override_ctl.debug_active = ovr_enable;
        } 
        else if (ovr_cmd_id == FTM_AUTOPIN_CMD_RUN_FREEZE)
        {
          /* freeze run decision execution for specified handle */
          autopin_template->override_ctl.pin_run_freeze = ovr_enable;
        }
        else if (ovr_cmd_id == FTM_AUTOPIN_CMD_ACCUM_FREEZE)
        {
          /* freeze Pin accumulator updates for specified handle */
          autopin_template->override_ctl.pin_accum_freeze = ovr_enable;
        }
        else if (ovr_cmd_id == FTM_AUTOPIN_CMD_ACCUM_OVERRIDE)
        {
          /* override Pin accumulator for specified handle */
          if (ovr_val1 >= 0x80)
          {
            autopin_template->override_ctl.pin_accum_ovr_val[ovr_val2] = (int32)(0x80-ovr_val1);
          }
          else
          {
            autopin_template->override_ctl.pin_accum_ovr_val[ovr_val2] = ovr_val1;
          }             
          autopin_template->override_ctl.pin_accum_override = ovr_enable;
          RF_MSG_2(RF_MED, "pin_accum_override value = %d, pa_state = %d", 
                       autopin_template->override_ctl.pin_accum_ovr_val[ovr_val2],
                       ovr_val2);
        }
      break;
    case FTM_AUTOPIN_CMD_DEBUG_CTRL:
        switch (ovr_val1)
        {
          case 1 :
              /* simulated FW result */ 
              meas_result.seq_nr = 0;
              meas_result.valid_result = TRUE;
              if (ovr_val2 >= 0x80)
              {
                meas_result.meas_error = (int32)(0x80-ovr_val2);
              }
              else
              {
                meas_result.meas_error = ovr_val2;
              }
              RF_MSG_2(RF_MED, "meas_error = %d, handle = %d", meas_result.meas_error, tx_handle);
              rflm_autopin_update_proc_result( (rflm_handle_tx_t)tx_handle, &meas_result);
            break;
          default:
            RF_MSG_1(RF_ERROR,"Bad debug id: %d", ovr_val1);
            break;
        }
      break;
    default:
      RF_MSG_1(RF_ERROR,"rfcommon_autopin_override_ctrl: Bad ovr_cmd_id: %d", ovr_cmd_id);
      break;
  }
  return RFCOMMON_AUTOPIN_SUCCESS;
}


void rfcommon_autopin_reflog_done_response(void *req_ptr,
                                           rf_dispatch_cid_info_type *cid_info,
                                           void *cb_data)
{
  rfcommon_autopin_capture_control_type control_input = { 0 };
  rflm_cmn_autopin_rsp *payload;
  payload = (rflm_cmn_autopin_rsp *)((rf_cmd_type *)req_ptr)->payload;

  control_input.capture_type = AUTOPIN_PAR_REFLOG_CAPTURE;
  control_input.valid_capture = payload->valid_par;
  /*FW returns reflog PAR in Q16 uint, need to right shift by 16 bits to convert into db100 as WTR PAR is in db100*/
  control_input.par = (uint16)(payload->par>>16);
  control_input.reflog_result.par = control_input.par;
  control_input.reflog_result.valid_result = control_input.valid_capture;
  control_input.reflog_result.mean_pwr = payload->mean_pwr;
  control_input.reflog_result.peak_avg_pwr = payload->peak_avg_pwr;

#if 0
  RF_MSG_3(RF_HIGH, "AutoPin REFLOG: handle %d, valid_capture %d, par %d",
           rflm_autopin_current_active_tx_handle,
           control_input.valid_capture,
           control_input.par);
#endif

  rfcommon_autopin_capture_control(rflm_autopin_current_active_tx_handle,
                                   control_input);
}


/*------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Autopin capture control API
 
  @details
  Check validity of WTR and REFLOG capture. If both captures are valid, proceed to PAR algo
 
  @param tx_handle
  TxLM handle to which the capture computation applies
 
  @param capture_input
  Contains information about capture type, par result and validity of the capture
  
  @return
  
*/
void rfcommon_autopin_capture_control(rflm_handle_tx_t handle, 
                                      rfcommon_autopin_capture_control_type capture_input)
{
  static uint16 wtr_par = 0;
  static uint16 reflog_par = 0;
  int32 timing_delta = 0;
  uint32 timing_window = 0;
  uint32 timing_tolerance = 192;
  rflm_autopin_dm_template_t *autopin_template = NULL;
  uint16 peak_avg_pwr = 0;
  static rfcommon_autopin_capture_control_type capture_input_cache = { 0 };
  uint16 wtr_correction_factor = 10; /* in 100dbm units cal result showed a correction of  0.1dBm  */
  
  autopin_template = (rflm_autopin_dm_template_t *)rflm_dm_get_autopin_buf_ptr(handle);

  if (autopin_template == NULL)
  {
    RF_MSG_1(RF_ERROR,"rfcommon_autopin_update_pin: Invalid TX handle: %d", handle);
    return;
  }

  switch (capture_input.capture_type)
  {
     case AUTOPIN_PAR_WTR_CAPTURE:
       wtr_capture_done = TRUE;
       memscpy(&capture_input_cache.wtr_capture_result, sizeof(rfdevice_fbrx_autopin_data_type), &capture_input.wtr_capture_result, sizeof(rfdevice_fbrx_autopin_data_type));
       wtr_capture_valid = capture_input.valid_capture;
       wtr_par = capture_input.par;
	   RF_MSG_2(RF_HIGH,"rfcommon_autopin_capture_control: WTR PAR %d, WTR valid capture %d", wtr_par,wtr_capture_valid);
       break;

     case AUTOPIN_PAR_REFLOG_CAPTURE:
       reflog_capture_done = TRUE;
       memscpy(&capture_input_cache.reflog_result, sizeof(fw_autopin_psamp_result_out_t), &capture_input.reflog_result, sizeof(fw_autopin_psamp_result_out_t));
       reflog_capture_valid = capture_input.valid_capture;
       reflog_par = capture_input.par;
       peak_avg_pwr = capture_input.reflog_result.peak_avg_pwr;
       RF_MSG_3(RF_HIGH,"rfcommon_autopin_capture_control: Reflog PAR %d,  peak_avg_pwr %d , reflog valid capture %d", reflog_par,peak_avg_pwr, reflog_capture_valid);
       break;

     default:
       RF_MSG_1(RF_ERROR,"rfcommon_autopin_capture_control: Invalid capture type: %d", capture_input.capture_type);
  }

  switch (autopin_template->autopin_ctl.tech)
  {
    case RFLM_TECH_WCDMA:
      timing_window = AUTOPIN_PAR_WCDMA_CAPTURE_TIME_OFFSET;
      break;

    case RFLM_TECH_TDSCDMA:
      timing_window = AUTOPIN_PAR_TDSCDMA_CAPTURE_TIME_OFFSET;
      break;

    case RFLM_TECH_1X:
    case RFLM_TECH_HDR:
      timing_window = AUTOPIN_PAR_CDMA_CAPTURE_TIME_OFFSET;
      break;

    case RFLM_TECH_LTE:
      timing_window = AUTOPIN_PAR_LTE_CAPTURE_TIME_OFFSET;
      break;

    default:
      RF_MSG_1(RF_ERROR, "rfcommon_autopin_capture_control: unexpected tech %d", autopin_template->autopin_ctl.tech);
  }

  if (wtr_capture_done & reflog_capture_done)
  {
    rflm_autopin_logging_capture_result(&capture_input_cache, autopin_template, wtr_capture_valid, reflog_capture_valid, rflm_autopin_seq_nr);

    /*Both captures are done and proceed to check capture validity*/
    if (wtr_capture_valid & reflog_capture_valid)
    {
      timing_delta = autopin_template->autopin_ctl.wtr_ccs_end_time
        - (0x3FFFFF & (autopin_template->autopin_ctl.reflog_capture_input.tx_capture_time
                       + autopin_template->autopin_ctl.reflog_capture_input.tx_capture_delay));

      if (timing_delta > timing_window + timing_tolerance || timing_delta < timing_window - timing_tolerance)
      {
        RF_MSG_4(RF_ERROR,"rfcommon_autopin_update_pin: WTR and REFLOG capture timing mismatch, WTR 0x%x, REFLOG 0x%x, tech %d, Tx handle %d",
                 autopin_template->autopin_ctl.wtr_ccs_end_time,
                 autopin_template->autopin_ctl.reflog_capture_input.tx_capture_time + autopin_template->autopin_ctl.reflog_capture_input.tx_capture_delay,
                 autopin_template->autopin_ctl.tech,
                 handle);
      }

      if (autopin_template->autopin_ctl.tech == RFLM_TECH_1X || autopin_template->autopin_ctl.tech == RFLM_TECH_HDR )
      {
        wtr_par = wtr_par - wtr_correction_factor;
      }
   
      /*Both capture are valid and proceed with Pin calculation*/
      rfcommon_autopin_update_pin(handle, wtr_par, reflog_par,peak_avg_pwr);
     

    }
    /*Reset flags for the next capture*/
    wtr_capture_done = FALSE;
    reflog_capture_done = FALSE;
    wtr_capture_valid = FALSE;
    reflog_capture_valid = FALSE;
    wtr_par = 0;
    reflog_par = 0;
    rflm_autopin_in_progress = FALSE;
  }
}

void rfcommon_autopin_update_pin(rflm_handle_tx_t handle, int16 wtr_par, int16 reflog_par, uint16 peak_avg_pwr )
{
  int16 par_error = 0;
  rflm_autopin_dm_template_t *autopin_data_ptr = NULL;
  int16 sum = 0;

  autopin_data_ptr = (rflm_autopin_dm_template_t *)rflm_dm_get_autopin_buf_ptr(handle);

  if (autopin_data_ptr == NULL)
  {
    RF_MSG_1(RF_ERROR,"rfcommon_autopin_update_pin: Invalid TX handle: %d", handle);
    return;
  }

  par_error = reflog_par - wtr_par;

  /* Algorithm for Acq/TRK mode
   When measurement is valid (Reflog peak power > 35k)
  
 1. 	  Start in acquisition mode (current run frequency is 10ms for all tech?)
 2. 	  Save Pin update signs (+1, 0, or -1) from the previous N=10 measurements
 3. 	  If abs(sum of N signs) < T1 and in acquisition mode, then go to tracking mode (trk mode frequency is 100ms)
 4. 	  If abs(sum of N signs) > T2 and in tracking mode, then go to acquisition mode


   When measurement is invalid (Reflog peak power <= 35k)
 1.     Count consecutive invalid (low peak power) measurements in both modes.
 2.     If the invalid count X1 exceeds a threshold N1 in ACQ mode then switch to TRK mode.  If a valid peak power occurs in ACQ mode, reset X1 to zero and continue in ACQ mode.
 3.     If a valid measurement is made in TRK mode and when the invalid count X2 is greater than threshold N2 then switch to ACQ mode.  If a valid peak power occurs in TRK mode when X2<N2, then reset X2 to zero and continue in TRK mode.
 
     Counts X1, X2 are reset on any mode switch. */



/*If valid measurement occurs*/
 if(peak_avg_pwr > 35000)
 {
 

  if ((num_invalid_meas_in_Acq <= INVALID_MEAS_THRESHOLD_ACQ)&&(autopin_data_ptr->autopin_ctl.run_state[autopin_data_ptr->autopin_ctl.pa_state] == RFLM_AUTOPIN_ACQ))
  {
    num_invalid_meas_in_Acq = 0;
  }

  else if((num_invalid_meas_in_Trk > INVALID_MEAS_THRESHOLD_TRK) && (autopin_data_ptr->autopin_ctl.run_state[autopin_data_ptr->autopin_ctl.pa_state] == RFLM_AUTOPIN_TRK))
  {
    num_invalid_meas_in_Trk = 0;
	iCount = 0;
	autopin_data_ptr->autopin_ctl.run_state[autopin_data_ptr->autopin_ctl.pa_state] = RFLM_AUTOPIN_ACQ;
	autopin_data_ptr->autopin_ctl.run_period = RFLM_AUTOPIN_ACQ_TIME_OUT;
	RF_MSG(RF_ERROR,"rfcommon_autopin_update_pin: Switching to Acq");
  }

  else if ((num_invalid_meas_in_Trk <= INVALID_MEAS_THRESHOLD_TRK) && (autopin_data_ptr->autopin_ctl.run_state[autopin_data_ptr->autopin_ctl.pa_state] == RFLM_AUTOPIN_TRK))
  {
    num_invalid_meas_in_Trk = 0;
  }
    

/*
   insert +1 for +ve par_error and -1 for -ve par error
   and "0" for par_error =0 
*/

  if ((par_error > AUTOPIN_PAR_ERROR_THRESHOLD) || (par_error < -AUTOPIN_PAR_ERROR_THRESHOLD))
  {
    if (par_error > 0)
    {
      if (autopin_data_ptr->autopin_ctl.pin_accum[autopin_data_ptr->autopin_ctl.pa_state] < RFLM_AUTOPIN_MAX_PIN_OFFSET)
      {
        autopin_data_ptr->autopin_ctl.pin_accum[autopin_data_ptr->autopin_ctl.pa_state] += RFLM_AUTOPIN_DEFAULT_STEP_SIZE;
	autopin_data_ptr->autopin_ctl.pin_sign[autopin_data_ptr->autopin_ctl.pa_state][iCount] = 1;
		
      }
    }
    else if (par_error < 0)
    {
      if (autopin_data_ptr->autopin_ctl.pin_accum[autopin_data_ptr->autopin_ctl.pa_state] > -RFLM_AUTOPIN_MAX_PIN_OFFSET)
      {
        autopin_data_ptr->autopin_ctl.pin_accum[autopin_data_ptr->autopin_ctl.pa_state] -= RFLM_AUTOPIN_DEFAULT_STEP_SIZE;
	autopin_data_ptr->autopin_ctl.pin_sign[autopin_data_ptr->autopin_ctl.pa_state][iCount] = -1;
      }
      }
    }
  else
  {
	 autopin_data_ptr->autopin_ctl.pin_sign[autopin_data_ptr->autopin_ctl.pa_state][iCount] = 0;
	
  }
  
  RF_MSG_2(RF_ERROR,"rfcommon_autopin_update_pin: count: %d, autopin_sign_calc: %d", 
							  iCount, autopin_data_ptr->autopin_ctl.pin_sign[autopin_data_ptr->autopin_ctl.pa_state][iCount]);

  iCount ++;

/*
   if already in ACQ mode ACQ/TRK decision time is 100 msec (10 trials each one with 10msec)
   if already in TRK mode ACQ/TRK decision time is 1 sec (10 trials each trial with 100msec)
*/

  if (iCount % 10 == 0)
  { 
    for (int8 i = 0; i < MAX_PIN_SIGN_SAMPLES; i++ )
    {
       sum = sum + autopin_data_ptr->autopin_ctl.pin_sign[autopin_data_ptr->autopin_ctl.pa_state][i];
    }

    switch (autopin_data_ptr->autopin_ctl.tech)
    {

       case RFLM_TECH_WCDMA:
#ifdef FEATURE_WCDMA
  if ( (ABS_VAL(sum) < RFLM_WCDMA_AUTOPIN_ACQ_LIMIT) && (autopin_data_ptr->autopin_ctl.run_state[autopin_data_ptr->autopin_ctl.pa_state] == RFLM_AUTOPIN_ACQ))
        {
         autopin_data_ptr->autopin_ctl.run_state[autopin_data_ptr->autopin_ctl.pa_state] = RFLM_AUTOPIN_TRK;
	     autopin_data_ptr->autopin_ctl.run_period = RFLM_AUTOPIN_TRK_TIME_OUT;
	     num_invalid_meas_in_Acq=0;
	    num_invalid_meas_in_Trk=0;
	   // RF_MSG(RF_ERROR,"rfcommon_autopin_update_pin: Switching to TRK");
	}
	if ( (ABS_VAL(sum) > RFLM_WCDMA_AUTOPIN_TRK_LIMIT) && (autopin_data_ptr->autopin_ctl.run_state[autopin_data_ptr->autopin_ctl.pa_state] == RFLM_AUTOPIN_TRK))
	{
            autopin_data_ptr->autopin_ctl.run_state[autopin_data_ptr->autopin_ctl.pa_state] = RFLM_AUTOPIN_ACQ;
	    autopin_data_ptr->autopin_ctl.run_period = RFLM_AUTOPIN_ACQ_TIME_OUT;
	    num_invalid_meas_in_Acq=0;
	    num_invalid_meas_in_Trk=0;
	  //  RF_MSG(RF_ERROR,"rfcommon_autopin_update_pin: Switching to Acq");
        }		
#endif
	break;
	
      case RFLM_TECH_TDSCDMA:
  #ifdef FEATURE_TDSCDMA

        if ( (ABS_VAL(sum) < RFLM_TDSCDMA_AUTOPIN_ACQ_LIMIT) && (autopin_data_ptr->autopin_ctl.run_state[autopin_data_ptr->autopin_ctl.pa_state] == RFLM_AUTOPIN_ACQ))
        {
            autopin_data_ptr->autopin_ctl.run_state[autopin_data_ptr->autopin_ctl.pa_state] = RFLM_AUTOPIN_TRK;
	    autopin_data_ptr->autopin_ctl.run_period = RFLM_AUTOPIN_TRK_TIME_OUT;
        num_invalid_meas_in_Acq=0;
	    num_invalid_meas_in_Trk=0;
	}
	if ( (ABS_VAL(sum) > RFLM_TDSCDMA_AUTOPIN_TRK_LIMIT) && (autopin_data_ptr->autopin_ctl.run_state[autopin_data_ptr->autopin_ctl.pa_state] == RFLM_AUTOPIN_TRK))
	{
           autopin_data_ptr->autopin_ctl.run_state[autopin_data_ptr->autopin_ctl.pa_state] = RFLM_AUTOPIN_ACQ;
	   autopin_data_ptr->autopin_ctl.run_period = RFLM_AUTOPIN_ACQ_TIME_OUT;
	   num_invalid_meas_in_Acq=0;
	   num_invalid_meas_in_Trk=0;
        }
		
#endif
      break;
	
	  case RFLM_TECH_1X:
	  case RFLM_TECH_HDR:
 #ifdef FEATURE_CDMA
	  if ( (ABS_VAL(sum) < RFLM_CDMA_AUTOPIN_ACQ_LIMIT) && (autopin_data_ptr->autopin_ctl.run_state[autopin_data_ptr->autopin_ctl.pa_state] == RFLM_AUTOPIN_ACQ))
          {
             autopin_data_ptr->autopin_ctl.run_state[autopin_data_ptr->autopin_ctl.pa_state] = RFLM_AUTOPIN_TRK;
	     autopin_data_ptr->autopin_ctl.run_period = RFLM_AUTOPIN_TRK_TIME_OUT;
	     num_invalid_meas_in_Acq=0;
	     num_invalid_meas_in_Trk=0;
         RF_MSG(RF_ERROR,"rfcommon_autopin_update_pin: Switching to TRK");
		 
	  }
	  if ( (ABS_VAL(sum) > RFLM_CDMA_AUTOPIN_TRK_LIMIT) && (autopin_data_ptr->autopin_ctl.run_state[autopin_data_ptr->autopin_ctl.pa_state] == RFLM_AUTOPIN_TRK))
	  {
 	     autopin_data_ptr->autopin_ctl.run_state[autopin_data_ptr->autopin_ctl.pa_state] = RFLM_AUTOPIN_ACQ;
	     autopin_data_ptr->autopin_ctl.run_period = RFLM_AUTOPIN_ACQ_TIME_OUT;
	     num_invalid_meas_in_Acq=0;
	     num_invalid_meas_in_Trk=0;
		 
	  }
#endif
	break;
	
	  case RFLM_TECH_LTE:
          if ( (ABS_VAL(sum) < RFLM_LTE_AUTOPIN_ACQ_LIMIT) && (autopin_data_ptr->autopin_ctl.run_state[autopin_data_ptr->autopin_ctl.pa_state] == RFLM_AUTOPIN_ACQ))
          {
             autopin_data_ptr->autopin_ctl.run_state[autopin_data_ptr->autopin_ctl.pa_state] = RFLM_AUTOPIN_TRK;
	     autopin_data_ptr->autopin_ctl.run_period = RFLM_AUTOPIN_TRK_TIME_OUT;
	     num_invalid_meas_in_Acq=0;
	     num_invalid_meas_in_Trk=0;
         RF_MSG(RF_ERROR,"rfcommon_autopin_update_pin: Switching to TRK");
		 
	  }
	  if ( (ABS_VAL(sum) > RFLM_LTE_AUTOPIN_TRK_LIMIT) && (autopin_data_ptr->autopin_ctl.run_state[autopin_data_ptr->autopin_ctl.pa_state] == RFLM_AUTOPIN_TRK))
	  {
 	     autopin_data_ptr->autopin_ctl.run_state[autopin_data_ptr->autopin_ctl.pa_state] = RFLM_AUTOPIN_ACQ;
	     autopin_data_ptr->autopin_ctl.run_period = RFLM_AUTOPIN_ACQ_TIME_OUT;
	     num_invalid_meas_in_Acq=0;
	     num_invalid_meas_in_Trk=0;
		 
	  }

		break;
	
	  default:
		RF_MSG_1(RF_ERROR,"rfcommon_autopin_update_pin: Invalid Tech: %d", autopin_data_ptr->autopin_ctl.tech);
		break;
	 
	}
   

	RF_MSG_3(RF_ERROR,"rfcommon_autopin_update_pin: sum of signs : %d run state: %d: run period: %d ms", 
		                    sum,
		                    autopin_data_ptr->autopin_ctl.run_state[autopin_data_ptr->autopin_ctl.pa_state], 
		                    autopin_data_ptr->autopin_ctl.run_period/19200);

	iCount = 0;
	
  }
 }

 else 
 {
  if( autopin_data_ptr->autopin_ctl.run_state[autopin_data_ptr->autopin_ctl.pa_state] == RFLM_AUTOPIN_ACQ)
  {
    num_invalid_meas_in_Acq++;
	
	if(num_invalid_meas_in_Acq > INVALID_MEAS_THRESHOLD_ACQ)
    {
      num_invalid_meas_in_Acq = 0;
      iCount = 0;
	  autopin_data_ptr->autopin_ctl.run_state[autopin_data_ptr->autopin_ctl.pa_state] = RFLM_AUTOPIN_TRK;
	  autopin_data_ptr->autopin_ctl.run_period = RFLM_AUTOPIN_TRK_TIME_OUT;

	  RF_MSG_2(RF_ERROR,"rfcommon_autopin_update_pin: Switching to TRK, run state: %d: run period: %d ms", 
						  autopin_data_ptr->autopin_ctl.run_state[autopin_data_ptr->autopin_ctl.pa_state], 
						  autopin_data_ptr->autopin_ctl.run_period/19200);
    }
  }

  else if (autopin_data_ptr->autopin_ctl.run_state[autopin_data_ptr->autopin_ctl.pa_state] == RFLM_AUTOPIN_TRK)
  {
    num_invalid_meas_in_Acq++;
  }

 }
   

  switch (autopin_data_ptr->autopin_ctl.tech)
  {
  case RFLM_TECH_WCDMA:
#ifdef FEATURE_WCDMA
    rfwcdma_core_autopin_update_pin(handle,
                                    autopin_data_ptr->autopin_ctl.pa_state,
                                    autopin_data_ptr->autopin_ctl.pin_accum[autopin_data_ptr->autopin_ctl.pa_state]);
#endif
    break;

  case RFLM_TECH_TDSCDMA:
#ifdef FEATURE_TDSCDMA
    rf_tdscdma_core_txlin_autopin_update(autopin_data_ptr->autopin_ctl.pin_accum[autopin_data_ptr->autopin_ctl.pa_state],
                                         autopin_data_ptr->autopin_ctl.pa_state);
#endif
    break;

  case RFLM_TECH_1X:
  case RFLM_TECH_HDR:
#ifdef FEATURE_CDMA
  rf_cdma_autopin_update_pin(handle,
                         autopin_data_ptr->autopin_ctl.pa_state,
                         autopin_data_ptr->autopin_ctl.pin_accum[autopin_data_ptr->autopin_ctl.pa_state]);
#endif
    break;

  case RFLM_TECH_LTE:
    break;

  default:
    RF_MSG_1(RF_ERROR,"rfcommon_autopin_update_pin: Invalid Tech: %d", autopin_data_ptr->autopin_ctl.tech);
    break;
  }
}
