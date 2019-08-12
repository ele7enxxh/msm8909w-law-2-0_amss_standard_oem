/*
  @file
  rflm_autopin.c

  @brief
  This file contains prototypes and definitions used by RFLM AutoPin.
*/

/*=============================================================================
 
  Copyright (c) 2015 - 2016 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document are confidential 
  and proprietary information of Qualcomm Technologies Incorporated and all rights therein are 
  expressly reserved. By accepting this material the recipient agrees that this material and the 
  information contained therein are held in confidence and in trust and will not be used, copied, 
  reproduced in whole or in part, nor its contents revealed in any manner to others without the 
  express written permission of Qualcomm Technologies Incorporated.
 
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/autopin/src/rflm_autopin.c#1 $

when       who    what, where, why
--------   ---    -------------------------------------------------------------
02/01/16   whz    Log packet support
02/01/16   vs     Copy peak avg power from FW
12/22/15   vkm    Enable Autopin for LTE
12/17/15   whz    Enable TDSCDMA autopin
12/16/15   ag     Enable WCDMA autopin, hookup NV control, 
                  fix Firmware standlone error
12/09/15   kma    Update REFLOG PAR to 32 bits
12/03/15   whz    Remove unncessary check in run_decsion
11/10/15   kma    Removed TA featurization
11/05/15   whz    AutoPin on TA: online support
11/03/15   kma    Featurize for TA modem
10/30/15   kma    Added autopin FW msg
10/29/15   whz    Remove timers completely
10/28/15   whz    Stop increasing Pin accum when RGI is max
10/21/15   whz    Version 2 log packets
                  Optimize log context ID usage
10/14/15   whz    Remove timers as we schedule AutoPin every time
09/24/15   kma    Enable CDMA autopin
09/21/15   whz    Update the definition of Log Packets again
09/17/15   kma    Enabled debug msg in diag for critical error 
                  Removed TDS check for ENV scaling value
09/10/15   aa     Enable WCDMA autopin
09/09/15   aa     autopin run deecision initialization fix
09/03/15   whz    Update the definition of Log Packets
09/03/15   whz    Enable TDSCDMA AutoPin
08/25/15   cdb    Enable LTE AutoPin
08/25/15   cdb    Featurization update
08/20/15   cdb    Add env_scale backoff criteria to autopin run decision
07/16/15   cdb    Add EFS support 
07/16/15   cdb    Abstract Thor/AMAM specific processing 
06/30/15   cdb    Add override controls
06/25/15   cdb    Remove mutex, add error checking
06/23/15   whz    Do not check log_status when doing log packet allocation
06/10/15   whz    Provide buffer to store FBRx IQ
06/10/15   cdb    Update FW API
06/08/15   whz    Add new API to return the latest Pin accum
06/05/15   whz    Don't include timer.h for FW standalone build
06/04/15   whz    Initial version

===========================================================================*/

#include "rflm.h"
#include "rflm_diag_log.h"
#include "rflm_autopin.h"
#include "rflm_dm_api.h"
#include "rflm_diag.h"
#include "rflm_wcdma_msg.h"
#include "rflm_wcdma_sw_api.h"
#include "rflm_tds_msg_proc.h"
#include "rflm_autopin_log.h"
#include "rflm_cmn_msg.h"
#include <qurt.h>
#if defined(FEATURE_RF_AUTOPIN) && (RFLM_FEATURE_BUILD_MODE!=RFLM_FEATURE_BUILD_FW_LIB)
#include "rfcommon_autopin_db.h"
#endif
#include "rflm_txagc_api.h"

fw_autopin_proc rflm_autopin_fw_autopin_fn = NULL;

static rflm_autopin_dm_template_t rflm_autopin_dm_bufs[RFLM_AUTOPIN_MAX_BUF];

boolean rflm_autopin_in_progress = FALSE;

static int32 RFLM_AUTOPIN_TRK_THRESH[RFLM_MAX_TECH];

static uint8 RFLM_AUTOPIN_STEP_SIZE[RFLM_MAX_TECH];

uint32 rflm_autopin_seq_nr = 0;

static int rflm_autopin_log_context_id[2] = { -1, -1 };

rflm_handle_tx_t rflm_autopin_current_active_tx_handle = 0xFFFFFFFF;

boolean rflm_autopin_is_enabled( rflm_tech_id_t tech )
{
 #if defined(FEATURE_RF_AUTOPIN) && (RFLM_FEATURE_BUILD_MODE!=RFLM_FEATURE_BUILD_FW_LIB)
  autopin_efs_record_t* efs_record=NULL;
  efs_record = rfcommon_autopin_efs_get_record(tech);

  if (efs_record != NULL)
  {
    /* use enable setting from EFS file */
    return efs_record->autopin_enable;
  }
  else
  {
    /* no valid EFS data. use hardcoded enables per tech */
    switch (tech)
    {
      case RFLM_TECH_WCDMA:
        return TRUE;
      break;
      case RFLM_TECH_TDSCDMA:
        return TRUE;
      break;
      case RFLM_TECH_1X:
      case RFLM_TECH_HDR:
        return TRUE;
      break;
      case RFLM_TECH_LTE:
        return TRUE;
      break;
      default:
        return FALSE;
      break;
    }
  }
#else
  return FALSE;
#endif
}


/* Get run decision in RFLM
 * Used in Tech's txagc_update or like API
 */
rflm_err_t rflm_autopin_get_run_decision( rflm_handle_tx_t handle,
                                          rflm_autopin_get_run_decision_in_t  *in,
                                          rflm_autopin_get_run_decision_out_t *out )
{
  rflm_autopin_dm_template_t *autopin_data_ptr;

  out->run_autopin = FALSE;

  autopin_data_ptr = (rflm_autopin_dm_template_t *)rflm_dm_get_autopin_buf_ptr(handle);

  if (autopin_data_ptr == NULL)
  {
    RFLM_DIAG_TRACE_ERROR(RFLM_DIAG_MSG_SSID_CMN_AUTOPIN, "rflm_autopin_get_run_decision: No valid DM buf ptr with Tx handle %d",
                          handle);

    return RFLM_ERR_API_FAILED;
  }

  if (autopin_data_ptr->override_ctl.debug_active && autopin_data_ptr->override_ctl.pin_run_freeze)
  {
    return RFLM_ERR_NONE;
  }

  if (rflm_autopin_in_progress == TRUE)
  {
    RFLM_DIAG_TRACE_HIGH(RFLM_DIAG_MSG_SSID_CMN_AUTOPIN, "rflm_autopin_get_run_decision: No, in progress already");
  }
  else
  {
    if (autopin_data_ptr->autopin_ctl.init_done == TRUE)
    {
      if ((in->xpt_mode == RFLM_TXAGC_EPT)|| (in->xpt_mode == RFLM_TXAGC_ET))
      {
        /* get the env_scale backoff from tx agc */
        autopin_data_ptr->autopin_ctl.env_scale_backoff = rflm_autopin_get_env_scale_backoff(handle);
      
        if (autopin_data_ptr->autopin_ctl.env_scale_backoff < RFLM_AUTOPIN_ENV_SCALE_BACKOFF_LIMIT)
        {
          rflm_autopin_in_progress = TRUE;
      
          autopin_data_ptr->autopin_ctl.handle = handle; /* currently running caller */
          rflm_autopin_current_active_tx_handle = handle;
          autopin_data_ptr->autopin_ctl.pa_state = in->pa_state;
          out->run_autopin = TRUE;
                          
          RFLM_DIAG_TRACE_HIGH(RFLM_DIAG_MSG_SSID_CMN_AUTOPIN, "rflm_autopin_get_run_decision: Yes, Tech %d, Tx handle %d, PA state %d",
                               autopin_data_ptr->autopin_ctl.tech, handle, in->pa_state);
        }
        else
        {
          RFLM_DIAG_TRACE_HIGH(RFLM_DIAG_MSG_SSID_CMN_AUTOPIN, 
		  	                   "rflm_autopin_get_run_decision: No, Tech %d, Tx handle %d, PA state %d. Reason: env scale backoff %d",
                               autopin_data_ptr->autopin_ctl.tech, 
                               handle, 
                               in->pa_state, 
                               autopin_data_ptr->autopin_ctl.env_scale_backoff);      
        }
      }
      else
      {
        //RFLM_DIAG_MSG_HIGH("rflm_autopin_get_run_decision: No, Tech %d, Tx handle %d, PA state %d. Reason: xpt_mode %d",
        //                   autopin_data_ptr->autopin_ctl.tech, 
        //                   handle, 
        //                   in->pa_state, 
        //                   in->xpt_mode);
        RFLM_DIAG_TRACE_HIGH(RFLM_DIAG_MSG_SSID_CMN_AUTOPIN, "rflm_autopin_get_run_decision: No, Tech %d, Tx handle %d, PA state %d. Reason: xpt_mode %d",
                             autopin_data_ptr->autopin_ctl.tech, handle, in->pa_state, in->xpt_mode);
      }
    }
    else
    {
      //RFLM_DIAG_MSG_ERROR("rflm_autopin_get_run_decision: ERROR: Tech %d, Tx handle %d. Reason: Init done %d",
      //                   autopin_data_ptr->autopin_ctl.tech, 
      //                   handle, 
      //                   autopin_data_ptr->autopin_ctl.init_done);
      RFLM_DIAG_TRACE_ERROR(RFLM_DIAG_MSG_SSID_CMN_AUTOPIN, "rflm_autopin_get_run_decision: ERROR: Tech %d, Tx handle %d. Reason: Init done %d",
                            autopin_data_ptr->autopin_ctl.tech, 
                            handle, 
                            autopin_data_ptr->autopin_ctl.init_done);
    }
  }

  if (out->run_autopin == TRUE)
  {
    rflm_autopin_seq_nr++;
  }

  rflm_autopin_logging_control_info(autopin_data_ptr, out->run_autopin, rflm_autopin_seq_nr);

  return RFLM_ERR_NONE;
}


rflm_err_t rflm_autopin_get_latest_pin(rflm_handle_tx_t handle, uint8 pa_state, rflm_db10_t *pin)
{
  rflm_autopin_dm_template_t *autopin_data_ptr;

  rflm_tech_id_t tech = RFLM_MAX_TECH;

  autopin_data_ptr = (rflm_autopin_dm_template_t *)rflm_dm_get_autopin_buf_ptr(handle);

  if (autopin_data_ptr == NULL)
  {
    RFLM_DIAG_TRACE_ERROR(RFLM_DIAG_MSG_SSID_CMN_AUTOPIN, "rflm_autopin_get_latest_pin: No valid DM buf ptr with Tx handle %d",
                       handle);

    return RFLM_ERR_API_FAILED;
  }

  if (pa_state >= RFLM_CMN_RXAGC_MAX_LNA_STATES)
  {
    RFLM_DIAG_TRACE_ERROR(RFLM_DIAG_MSG_SSID_CMN_AUTOPIN, "rflm_autopin_get_latest_pin: invalid PA state %d",
                       pa_state);

    return RFLM_ERR_API_FAILED;
  }

  tech = autopin_data_ptr->autopin_ctl.tech;
  
  if (rflm_autopin_is_enabled(tech) == FALSE)
  {
    RFLM_DIAG_TRACE_ERROR(RFLM_DIAG_MSG_SSID_CMN_AUTOPIN, "rflm_autopin_get_latest_pin: AutoPin not enabled on Tech %d with Tx handle %d",
                          tech, handle);

    return RFLM_ERR_API_FAILED;
  }

  *pin = autopin_data_ptr->autopin_ctl.pin_accum[pa_state];

  return RFLM_ERR_NONE;
}


rflm_err_t rflm_autopin_set_data(void *autopin_data,
                                 rflm_autopin_cmd_e cmd,
                                 const void *payload)
{
  rflm_err_t ret_val = RFLM_ERR_API_FAILED;

  rflm_autopin_dm_template_t *autopin_ptr;
  
  if (autopin_data == NULL)
  {
    RFLM_DIAG_TRACE_ERROR(RFLM_DIAG_MSG_SSID_CMN_AUTOPIN, "rflm_autopin_set_data: Null pointer for AutoPin buffer");
    return ret_val;
  }

  autopin_ptr = (rflm_autopin_dm_template_t *)autopin_data;

  switch (cmd)
  {
  case RFLM_AUTOPIN_UPDATE_CTL_DATA:
    if (payload != NULL)
    {
      memscpy(&(autopin_ptr->autopin_ctl),
              sizeof(rflm_autopin_ctl_data_t),
              (rflm_autopin_ctl_data_t *)payload,
              sizeof(rflm_autopin_ctl_data_t));
    }

    ret_val = RFLM_ERR_NONE;
    
    break;
    
  case RFLM_AUTOPIN_UPDATE_PROC_RESULTS:
    if (payload != NULL)
    {
      memscpy(&(autopin_ptr->results_log),
              sizeof(rflm_autopin_proc_results_t),
              (rflm_autopin_proc_results_t *)payload,
              sizeof(rflm_autopin_proc_results_t));
    }

    ret_val = RFLM_ERR_NONE;
    
    break;
    
  default:
    RFLM_DIAG_TRACE_ERROR(RFLM_DIAG_MSG_SSID_CMN_AUTOPIN,
                          "rflm_autopin_set_data: unsupported command %d ", (int16)cmd); 
    ret_val = RFLM_ERR_API_FAILED;
  }

  if ( ret_val != RFLM_ERR_NONE )
  {
    RFLM_DIAG_TRACE_ERROR(RFLM_DIAG_MSG_SSID_CMN_AUTOPIN, "rflm_autopin_set_data: API failed for command %d", (int16)cmd); 
  }
    
  return ret_val;
}


rflm_err_t rflm_autopin_get_data(void *autopin_data,
                                 rflm_autopin_cmd_e cmd,
                                 void *payload)
{
  rflm_err_t ret_val = RFLM_ERR_API_FAILED;

  rflm_autopin_dm_template_t *autopin_ptr;

  if ( autopin_data == NULL ) 
  {
    RFLM_DIAG_TRACE_ERROR(RFLM_DIAG_MSG_SSID_CMN_AUTOPIN, "rflm_autopin_get_data: Null pointer for AutoPin buffer");
    return ret_val;
  }

  autopin_ptr = (rflm_autopin_dm_template_t *)autopin_data;

  switch (cmd)
  {
  case RFLM_AUTOPIN_GET_CTL_DATA:
    if (payload != NULL)
    {
      memscpy(payload,
              sizeof(rflm_autopin_ctl_data_t),
              &(autopin_ptr->autopin_ctl),
              sizeof(rflm_autopin_ctl_data_t));
    }

    ret_val = RFLM_ERR_NONE;
    
    break;
    
  case RFLM_AUTOPIN_GET_PROC_RESULTS:
    if (payload != NULL)
    {
      memscpy(payload,
              sizeof(rflm_autopin_proc_results_t),
              &(autopin_ptr->results_log),
              sizeof(rflm_autopin_proc_results_t));
    }

    ret_val = RFLM_ERR_NONE;
    
    break;

  default:
    RFLM_DIAG_TRACE_ERROR (RFLM_DIAG_MSG_SSID_CMN_AUTOPIN, "rflm_autopin_get_data: unsupported command %d ", (int16)cmd); 
    ret_val = RFLM_ERR_API_FAILED;
  }

  if ( ret_val != RFLM_ERR_NONE )
  {
    RFLM_DIAG_TRACE_ERROR(RFLM_DIAG_MSG_SSID_CMN_AUTOPIN, "rflm_autopin_get_data: API failed for command %d", (int16)cmd); 
  }

  return ret_val;
}


rflm_err_t rflm_autopin_dup_data(void *new_autopin_data,
                                 void *old_autopin_data)
{
  memscpy(new_autopin_data,
          sizeof(rflm_autopin_dm_template_t),
          old_autopin_data,
          sizeof(rflm_autopin_dm_template_t));

  return RFLM_ERR_NONE;
}


void rflm_autopin_init_buf(rflm_autopin_dm_template_t *template_ptr)
{
  rflm_autopin_cleanup_buf(template_ptr);
}


void rflm_autopin_cleanup_buf(rflm_autopin_dm_template_t *template_ptr)
{
  memset(template_ptr, 0, sizeof(rflm_autopin_dm_template_t));

  template_ptr->autopin_ctl.tech = RFLM_MAX_TECH;

  template_ptr->autopin_ctl.handle = 0xFFFFFFFF;
}


rflm_autopin_dm_template_t *rflm_autopin_assign_buf()
{
  int i;
  
  rflm_autopin_dm_template_t *autopin_template = NULL;

  for (i = 0; i < 4; i++)
  {
    if (rflm_autopin_dm_bufs[i].autopin_ctl.buf_in_use == FALSE)
    {
      autopin_template = &rflm_autopin_dm_bufs[i];
      rflm_autopin_init_buf(autopin_template);
      autopin_template->autopin_ctl.buf_in_use = TRUE;

      /* If we enable QXDM check here, we cannot enable QXDM AutoPin logpacket on the fly.
       * That is, we have to power cycle to let context ID be allocated.
       * if (log_status((uint16)RFLM_LOG_PACKET_AUTOPIN_DM_INFO_ID) == TRUE)
       * if (log_status((uint16)RFLM_LOG_PACKET_AUTOPIN_IQ_CAPTURE_ID) == TRUE)*/

      if (rflm_autopin_log_context_id[0] == -1)
      {
        autopin_template->autopin_ctl.logging_dm_info_context_id =
          rflm_diag_allocate_context(RFLM_LOG_PACKET_AUTOPIN_DM_INFO_ID,
                                     0,
                                     4);

        if (rflm_diag_validate_context(autopin_template->autopin_ctl.logging_dm_info_context_id) != TRUE)
        {
          RFLM_DIAG_TRACE_ERROR(RFLM_DIAG_MSG_SSID_CMN_AUTOPIN, "rflm_autopin_assign_buf: failed to allocate context id", RFLM_LOG_PACKET_AUTOPIN_DM_INFO_ID);
        }
        else
        {
          rflm_autopin_log_context_id[0] = autopin_template->autopin_ctl.logging_dm_info_context_id;
        }
      }
      else
      {
        autopin_template->autopin_ctl.logging_dm_info_context_id = rflm_autopin_log_context_id[0];
      }

      if (rflm_autopin_log_context_id[1] == -1)
      {
        autopin_template->autopin_ctl.logging_iq_capture_context_id =
          rflm_diag_allocate_context(RFLM_LOG_PACKET_AUTOPIN_IQ_CAPTURE_ID,
                                     0,
                                     4);

        if (rflm_diag_validate_context(autopin_template->autopin_ctl.logging_iq_capture_context_id) != TRUE)
        {
          RFLM_DIAG_TRACE_ERROR(RFLM_DIAG_MSG_SSID_CMN_AUTOPIN, "rflm_autopin_assign_buf: failed to allocate context id", RFLM_LOG_PACKET_AUTOPIN_IQ_CAPTURE_ID);
        }
        else
        {
          rflm_autopin_log_context_id[1] = autopin_template->autopin_ctl.logging_iq_capture_context_id;
        }
      }
      else
      {
        autopin_template->autopin_ctl.logging_iq_capture_context_id = rflm_autopin_log_context_id[1];
      }

      break;
    }
  }

  return autopin_template;
}


boolean rflm_autopin_release_buf(rflm_autopin_dm_template_t *autopin_template)
{
  int8 i = 0;
  boolean fully_released = FALSE;

  if (autopin_template->autopin_ctl.buf_in_use == TRUE)
  {
    autopin_template->autopin_ctl.buf_in_use = FALSE;
  }

  for (i = 0; i < 4; i++)
  {
    if (rflm_autopin_dm_bufs[i].autopin_ctl.buf_in_use == TRUE)
    {
      break;
    }
    fully_released = TRUE;
  }

  if (fully_released)
  {
    if(rflm_diag_validate_context(autopin_template->autopin_ctl.logging_dm_info_context_id) == TRUE)
    {
      rflm_diag_deallocate_context(autopin_template->autopin_ctl.logging_dm_info_context_id);
      rflm_autopin_log_context_id[0] = -1;
    }

    if (rflm_diag_validate_context(autopin_template->autopin_ctl.logging_iq_capture_context_id) == TRUE)
    {
      rflm_diag_deallocate_context(autopin_template->autopin_ctl.logging_iq_capture_context_id);
      rflm_autopin_log_context_id[1] = -1;
    }
  }

  return TRUE;
}

void rflm_autopin_set_params(rflm_tech_id_t tech, int32 trk_thresh, uint8 step_size)
{
  if (tech < RFLM_MAX_TECH)
  {
    RFLM_AUTOPIN_TRK_THRESH[tech] = trk_thresh;
    RFLM_AUTOPIN_STEP_SIZE[tech] = step_size;
  }  
}

rflm_err_t rflm_autopin_update_env_scale(rflm_handle_tx_t handle, uint32 env_scale_backoff)
{
  rflm_autopin_dm_template_t *autopin_data_ptr;
  
  autopin_data_ptr = (rflm_autopin_dm_template_t *)rflm_dm_get_autopin_buf_ptr(handle);
  
  if (autopin_data_ptr!=NULL)
  {
    autopin_data_ptr->autopin_ctl.env_scale_backoff = env_scale_backoff;
    return RFLM_ERR_NONE;
  }
  else
  {
    return RFLM_ERR_API_FAILED;
  }
}

uint32 rflm_autopin_get_env_scale_backoff(rflm_handle_tx_t tx_handle)
{
  /*! @brief  get txagc_template ptr from handle_id */
  rflm_txagc_event_template_t *txagc_template;
  rflm_txagc_params_t *txagc_param_pt;
  
  txagc_template = (rflm_txagc_event_template_t*) rflm_dm_get_tx_agc_ptr( (rflm_dm_handle_id_t) tx_handle);
  ASSERT(txagc_template != NULL );
  txagc_param_pt = &(txagc_template->params);
  
  return txagc_param_pt->txagc_meta[txagc_param_pt->current_param_idx].env_scale;
}

boolean rflm_autopin_is_rgi_max(rflm_handle_tx_t tx_handle)
{

  /*
  rflm_txagc_event_template_t *txagc_template = NULL;
  rflm_txagc_params_t *txagc_param_ptr = NULL;
  rflm_txagc_static_t *txagc_static_data_ptr = NULL;
  uint8 row_idx_const;
  rflm_txagc_pa_state_data_t pa_state;
  uint16 rgi_value_current;
  uint16 rgi_value_next;
  uint32 iq_gain;
  
  txagc_template = (rflm_txagc_event_template_t*) rflm_dm_get_tx_agc_ptr((rflm_dm_handle_id_t)tx_handle);

  if ( txagc_template == NULL ) 
  {
    RFLM_DIAG_TRACE_ERROR(RFLM_DIAG_MSG_SSID_CMN_AUTOPIN, "rflm_autopin_is_rgi_max: No valid TxAGC DM buffer ptr with Tx handle %d", tx_handle);
    return TRUE;  
  }

  txagc_param_ptr = &txagc_template->params;

  row_idx_const = txagc_param_ptr->txagc_meta[txagc_param_ptr->current_param_idx].row_idx_const;
  pa_state = txagc_param_ptr->txagc_meta[txagc_param_ptr->current_param_idx].pa_state;
  iq_gain = txagc_param_ptr->txagc_meta[txagc_param_ptr->current_param_idx].iq_gain;

  txagc_static_data_ptr = &txagc_template->txagc_static_data;

  rgi_value_current = txagc_static_data_ptr->tx_lin_static[pa_state].txagc_static_lut_script[row_idx_const].rgi_val;
  rgi_value_next = (row_idx_const == RFLM_TXAGC_TX_LIN_APT_SIZE-1) ? rgi_value_current : txagc_static_data_ptr->tx_lin_static[pa_state].txagc_static_lut_script[row_idx_const+1].rgi_val;

  RFLM_DIAG_MSG_HIGH("rflm_autopin_is_rgi_max: handle %d, RGI index %d, RGI value %d, IQ gain backoff %d", 
                     tx_handle,
                     row_idx_const,
                     rgi_value_current,
                     iq_gain);

  if (rgi_value_current == rgi_value_next
      && iq_gain == 0)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
  */
  return FALSE;
}

void rflm_autopin_update_fw_par_proc_result(fw_autopin_output_params_t *pout)
{
  errno_enum_type err;
  rflm_cmn_autopin_rsp_t msg;

  msgr_init_hdr(&msg.msg_hdr, MSGR_RFLM_CMN, RFLM_CMN_AUTOPIN_RSP);
  msg.msg_rsp.par = pout->result.par;
  msg.msg_rsp.valid_par = pout->result.valid_result;
  msg.msg_rsp.peak_avg_pwr = pout->result.peak_avg_pwr;
  
  err = msgr_send (&msg.msg_hdr, sizeof(msg));

  if( err != E_SUCCESS )
  {
    RFLM_DIAG_TRACE_ERROR(RFLM_DIAG_MSG_SSID_CMN_AUTOPIN, "msgr_send error: %d", err);
  }
}

