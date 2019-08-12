/*!
  @file
  rflm_autopin_log.c

  @brief
  RFLM AutoPin API for Logging
  
  @detail

*/

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved

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

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/autopin/src/rflm_autopin_log.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/12/16   whz     Remove unnecessary pin_ref
02/12/16   ag      Add logging of ref piin from NV 
01/11/16   whz     Version 3 log packets
10/28/15   whz     Log RGI value in V2
10/21/15   whz     Version 2 log packets
09/29/15   whz     Valide log ID after verifying DM allocation
09/23/15   whz     Always log raw IQ
09/21/15   whz     Update the definition of Log Packets again
09/03/15   whz     Update the definition of Log Packets
06/15/15   whz     Initial version
==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "rflm.h"
#include "rflm_api_autopin.h"
#include "rflm_api_autopin_fw.h"
#include "rflm_autopin_log.h"
#include "rflm_diag_log.h"
#include "log.h"
#include "rflm_txagc_api.h"
#include "rflm_dm_api.h"

#ifdef FEATURE_THOR_MODEM
#include "rflm_api_fbrx.h"


void rflm_autopin_logging_dm_info(rflm_autopin_dm_template_t *autopin_template,
                                  boolean run_decision,
                                  uint16 temp_read,
                                  uint32 seq_nr)
{
  rflm_diag_log_return_code_t rval = RFLM_SUCCESS;
  rflm_autopin_dm_info_log_packet_t *log_header = NULL;
  rflm_autopin_dm_info_log_packet_v2 *log_body = NULL;
  uint32 *p_buffer_user = NULL;
  rflm_txagc_event_template_t *txagc_template = NULL;
  rflm_txagc_params_t *txagc_params = NULL;
  rflm_fbrx_dm_template_t *fbrx_data_ptr = NULL;
  uint8 read_buf_idx = 0;
  uint8 row_idx_const = 0;

  if (autopin_template == NULL)
  {
    return;
  }

  /*! if log context hasn't been allocated or QXDM doesn't enable it, will return */  
  if (log_status((uint16)RFLM_LOG_PACKET_AUTOPIN_DM_INFO_ID) == FALSE ||
      rflm_diag_validate_context(autopin_template->autopin_ctl.logging_dm_info_context_id) == FALSE)
  {			  
    return;
  }

  txagc_template = (rflm_txagc_event_template_t *)rflm_dm_get_tx_agc_ptr((rflm_dm_handle_id_t)autopin_template->autopin_ctl.handle);

  if (txagc_template == NULL)
  {
    RFLM_DIAG_MSG_ERROR("rflm_autopin_logging_dm_info: No valid TxAGC DM buffer ptr with Tx handle %d", autopin_template->autopin_ctl.handle);
    return;
  }

  fbrx_data_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(autopin_template->autopin_ctl.handle);  

  if (fbrx_data_ptr == NULL)
  {
    RFLM_DIAG_MSG_ERROR("rflm_autopin_logging_dm_info: No valid FBRx DM buffer ptr with Tx handle %d", autopin_template->autopin_ctl.handle);
    return;
  }

  read_buf_idx = fbrx_data_ptr->exp_gain_data.read_buf_idx;
  if ( (read_buf_idx <= 0) || (read_buf_idx >= RFLM_FBRX_MAX_BUFFER_SIZE) )
  {
    read_buf_idx = 0;
  }

  txagc_params = &(txagc_template->params);

  rval = rflm_diag_log_alloc(sizeof(rflm_autopin_dm_info_log_packet_v2),
                             autopin_template->autopin_ctl.logging_dm_info_context_id,
                             &p_buffer_user);

  if (rval == RFLM_LOG_NEW_PACKET && p_buffer_user != NULL)
  {
    log_header = (rflm_autopin_dm_info_log_packet_t *)p_buffer_user;

    log_header->log_version = RFLM_AUTOPIN_LOG_VER_V2;
    log_header->num_sub_packets =
      (RFLM_DIAG_LOG_MAX_PACKET_SIZE-4)/sizeof(rflm_autopin_dm_info_log_packet_v2);
    log_header->reserved = 0;
    p_buffer_user++;
  }

  if ((rval == RFLM_LOG_NEW_PACKET || rval == RFLM_SUCCESS) && (p_buffer_user != NULL))
  {
    log_body = (rflm_autopin_dm_info_log_packet_v2 *)p_buffer_user;

    log_body->handle = autopin_template->autopin_ctl.handle;
    log_body->tech = autopin_template->autopin_ctl.tech;
    log_body->pa_state = autopin_template->autopin_ctl.pa_state;
    row_idx_const = txagc_params->txagc_meta[txagc_params->current_param_idx].row_idx_const;
    log_body->rgi = (uint8)(txagc_template->txagc_static_data.tx_lin_static[log_body->pa_state].txagc_static_lut_script[row_idx_const].rgi_val);
    log_body->run_decision = run_decision;
    log_body->env_scale_backoff = txagc_params->txagc_meta[txagc_params->current_param_idx].env_scale;
    if (log_body->tech != RFLM_TECH_WCDMA)
    {
      log_body->env_scale = fbrx_data_ptr->exp_gain_data.sf_data[read_buf_idx].slot_data[0].env_scale;
    }
    else
    {
      log_body->env_scale = fbrx_data_ptr->exp_gain_data.sf_data[read_buf_idx].slot_data[0].current_gain;
    }
    log_body->iq_gain_backoff = txagc_params->txagc_meta[txagc_params->current_param_idx].iq_gain;
    memscpy(log_body->ready, RFLM_TXAGC_PA_STATE_MAX*sizeof(boolean),
            autopin_template->autopin_ctl.ready, RFLM_TXAGC_PA_STATE_MAX*sizeof(boolean));
    memscpy(log_body->run_state, RFLM_TXAGC_PA_STATE_MAX*sizeof(rflm_autopin_state),
            autopin_template->autopin_ctl.run_state, RFLM_TXAGC_PA_STATE_MAX*sizeof(rflm_autopin_state));
    memscpy(log_body->pin_accum, RFLM_TXAGC_PA_STATE_MAX*sizeof(rflm_db10_t),
            autopin_template->autopin_ctl.pin_accum, RFLM_TXAGC_PA_STATE_MAX*sizeof(rflm_db10_t));
    log_body->temp_read = temp_read;
    log_body->seq_nr = seq_nr;
    log_body->reserved = 0;
  }

  return;
}


void rflm_autopin_logging_iq_capture(fw_psamp_autopin_result_s *fw_results, uint32 seq_nr)
{
  rflm_diag_log_return_code_t rval = RFLM_SUCCESS;
  rflm_autopin_iq_capture_log_packet_t *log_header = NULL;
  rflm_autopin_iq_capture_log_packet_v2 *log_body = NULL;
  uint32 *p_buffer_user = NULL;
  rflm_autopin_dm_template_t *autopin_template = NULL;
  
  if (fw_results == NULL)
  {
    return;
  }

  autopin_template = (rflm_autopin_dm_template_t *)rflm_dm_get_autopin_buf_ptr((rflm_handle_tx_t)fw_results->handle);

  if (autopin_template == NULL)
  {
    RFLM_DIAG_MSG_ERROR("rflm_autopin_logging_iq_capture: No valid AutoPin DM buffer ptr with Tx handle %d", fw_results->handle);
    return;
  }

  /*! if log context hasn't been allocated or QXDM doesn't enable it, will return */  
  if( (log_status((uint16)RFLM_LOG_PACKET_AUTOPIN_IQ_CAPTURE_ID) == FALSE ) ||
      (rflm_diag_validate_context(autopin_template->autopin_ctl.logging_iq_capture_context_id) == FALSE) )
  {			  
    return;
  }

  rval = rflm_diag_log_alloc(sizeof(rflm_autopin_iq_capture_log_packet_v2),
                             autopin_template->autopin_ctl.logging_iq_capture_context_id,
                             &p_buffer_user);

  if ((rval == RFLM_LOG_NEW_PACKET) && (p_buffer_user != NULL))
  {
    log_header = (rflm_autopin_iq_capture_log_packet_t *)p_buffer_user;

    log_header->log_version = RFLM_AUTOPIN_LOG_VER_V2;
    log_header->num_sub_packets =
      (RFLM_DIAG_LOG_MAX_PACKET_SIZE-4)/sizeof(rflm_autopin_iq_capture_log_packet_v2);
    log_header->reserved = 0;
    p_buffer_user++;
  }

  if ((rval == RFLM_LOG_NEW_PACKET || rval == RFLM_SUCCESS) && (p_buffer_user != NULL))
  {
    log_body = (rflm_autopin_iq_capture_log_packet_v2 *)p_buffer_user;

    //if (fw_results->status != 0 || fw_results->corr_snr < FWPSAMP_CORR_SNR_THRESHOLD)
    {
      memscpy(log_body->tx_samples,
              RFLM_AUTOPIN_LOG_TX_SAMPLE_SIZE*sizeof(uint32),
              autopin_template->autopin_ctl.txiqbuf.iq_cap_buf,
              autopin_template->autopin_ctl.txiqbuf.num_samp*sizeof(uint32));
      memscpy(log_body->rx_samples,
              RFLM_AUTOPIN_LOG_RX_SAMPLE_SIZE*sizeof(uint32),
              autopin_template->autopin_ctl.rxiqbuf.iq_cap_buf,
              autopin_template->autopin_ctl.rxiqbuf.num_samp*sizeof(uint32));
    }
#if 0
    else
    {
      memset(log_body->tx_samples,
             0,
             RFLM_AUTOPIN_LOG_TX_SAMPLE_SIZE*sizeof(uint32));
      memset(log_body->rx_samples,
             0,
             RFLM_AUTOPIN_LOG_TX_SAMPLE_SIZE*sizeof(uint32));
    }
#endif

    log_body->handle = fw_results->handle;
    log_body->tx_dc_i = fw_results->tx_dc_i;
    log_body->tx_dc_q = fw_results->tx_dc_q;
    log_body->tx_rms = fw_results->tx_rms;
    log_body->rx_dc_i = fw_results->rx_dc_i;
    log_body->rx_dc_q = fw_results->rx_dc_q;
    log_body->rx_rms = fw_results->rx_rms;
    log_body->peak_offset = fw_results->peak_offset;
    log_body->clip_tx_rms = fw_results->clip_tx_rms;
    log_body->clip_tx_mag = fw_results->clip_tx_mag;
    log_body->amam_err = fw_results->amam_err;
    log_body->corr_snr = fw_results->corr_snr;
    log_body->status = fw_results->status;
    log_body->rotator = 0;
    log_body->seq_nr = seq_nr;
    log_body->reserved = 0;
  }

  return;
}
#endif


void rflm_autopin_logging_control_info(rflm_autopin_dm_template_t *autopin_template, boolean run_decision, uint32 seq_nr)
{
  rflm_diag_log_return_code_t rval = RFLM_SUCCESS;
  rflm_autopin_dm_info_log_packet_t *log_header = NULL;
  rflm_autopin_dm_info_log_packet_v3 *log_body = NULL;
  uint32 *p_buffer_user = NULL;
  rflm_txagc_event_template_t *txagc_template = NULL;
  rflm_txagc_params_t *txagc_params = NULL;
  uint8 row_idx_const = 0;

  if (autopin_template == NULL)
  {
    return;
  }

  /*! if log context hasn't been allocated or QXDM doesn't enable it, will return */  
  if (log_status((uint16)RFLM_LOG_PACKET_AUTOPIN_DM_INFO_ID) == FALSE ||
      rflm_diag_validate_context(autopin_template->autopin_ctl.logging_dm_info_context_id) == FALSE)
  {			  
    return;
  }

  txagc_template = (rflm_txagc_event_template_t *)rflm_dm_get_tx_agc_ptr((rflm_dm_handle_id_t)autopin_template->autopin_ctl.handle);

  if (txagc_template == NULL)
  {
    RFLM_DIAG_MSG_ERROR("rflm_autopin_logging_control_info: No valid TxAGC DM buffer ptr with Tx handle %d", autopin_template->autopin_ctl.handle);
    return;
  }

  txagc_params = &(txagc_template->params);

  rval = rflm_diag_log_alloc(sizeof(rflm_autopin_dm_info_log_packet_v3),
                             autopin_template->autopin_ctl.logging_dm_info_context_id,
                             &p_buffer_user);

  if (rval == RFLM_LOG_NEW_PACKET && p_buffer_user != NULL)
  {
    log_header = (rflm_autopin_dm_info_log_packet_t *)p_buffer_user;

    log_header->log_version = RFLM_AUTOPIN_LOG_VER_V3;
    log_header->num_sub_packets =
      (RFLM_LOG_MAX_BUF_SIZE_WORDS*4-4)/sizeof(rflm_autopin_dm_info_log_packet_v3);
    log_header->reserved =  0;
    p_buffer_user++;
  }

  if ((rval == RFLM_LOG_NEW_PACKET || rval == RFLM_SUCCESS) && (p_buffer_user != NULL))
  {
    log_body = (rflm_autopin_dm_info_log_packet_v3 *)p_buffer_user;

    log_body->handle = autopin_template->autopin_ctl.handle;
    log_body->tech = autopin_template->autopin_ctl.tech;
    log_body->pa_state = autopin_template->autopin_ctl.pa_state;
    row_idx_const = txagc_params->txagc_meta[txagc_params->current_param_idx].row_idx_const;
    log_body->rgi = (uint8)(txagc_template->txagc_static_data.tx_lin_static[log_body->pa_state].txagc_static_lut_script[row_idx_const].rgi_val);
    log_body->run_decision = run_decision;
    log_body->env_scale_backoff = txagc_params->txagc_meta[txagc_params->current_param_idx].env_scale;
    memscpy(log_body->pin_accum, RFLM_TXAGC_PA_STATE_MAX*sizeof(rflm_db10_t),
            autopin_template->autopin_ctl.pin_accum, RFLM_TXAGC_PA_STATE_MAX*sizeof(rflm_db10_t));
    log_body->seq_nr = seq_nr;
    log_body->pin_init = autopin_template->autopin_ctl.pin_init;
  }

  return;
}

void rflm_autopin_logging_capture_result(rfcommon_autopin_capture_control_type *control_input, rflm_autopin_dm_template_t *autopin_template, boolean wtr_capture_valid, boolean reflog_capture_valid, uint32 seq_nr)
{
  rflm_diag_log_return_code_t rval = RFLM_SUCCESS;
  rflm_autopin_iq_capture_log_packet_t *log_header = NULL;
  rflm_autopin_iq_capture_log_packet_v3 *log_body = NULL;
  uint32 *p_buffer_user = NULL;
  
  if (control_input == NULL || autopin_template == NULL)
  {
    return;
  }

  /*! if log context hasn't been allocated or QXDM doesn't enable it, will return */  
  if( (log_status((uint16)RFLM_LOG_PACKET_AUTOPIN_IQ_CAPTURE_ID) == FALSE ) ||
      (rflm_diag_validate_context(autopin_template->autopin_ctl.logging_iq_capture_context_id) == FALSE) )
  {			  
    return;
  }

  rval = rflm_diag_log_alloc(sizeof(rflm_autopin_iq_capture_log_packet_v3),
                             autopin_template->autopin_ctl.logging_iq_capture_context_id,
                             &p_buffer_user);

  if ((rval == RFLM_LOG_NEW_PACKET) && (p_buffer_user != NULL))
  {
    log_header = (rflm_autopin_iq_capture_log_packet_t *)p_buffer_user;

    log_header->log_version = RFLM_AUTOPIN_LOG_VER_V3;
    log_header->num_sub_packets =
      (RFLM_LOG_MAX_BUF_SIZE_WORDS*4-4)/sizeof(rflm_autopin_iq_capture_log_packet_v3);
    log_header->reserved = 0;
    p_buffer_user++;
  }

  if ((rval == RFLM_LOG_NEW_PACKET || rval == RFLM_SUCCESS) && (p_buffer_user != NULL))
  {
    log_body = (rflm_autopin_iq_capture_log_packet_v3 *)p_buffer_user;

    memscpy(&log_body->sum_i_q_sqr, 3*sizeof(uint64), &control_input->wtr_capture_result.sum_i_q_sqr, 3*sizeof(uint64));
    log_body->reflog_par = control_input->reflog_result.par;
    log_body->mean_pwr = control_input->reflog_result.mean_pwr;
    log_body->peak_avg_pwr = control_input->reflog_result.peak_avg_pwr;
    log_body->capture_length = autopin_template->autopin_ctl.reflog_capture_input.capture_length;
    log_body->sum_i = control_input->wtr_capture_result.sum_i;
    log_body->sum_q = control_input->wtr_capture_result.sum_q;
    log_body->i_dc = control_input->wtr_capture_result.i_dc;
    log_body->q_dc = control_input->wtr_capture_result.q_dc;
    log_body->fbrx_accum = control_input->wtr_capture_result.fbrx_accum;
    log_body->sum_peaks = control_input->wtr_capture_result.sum_peaks;
    log_body->wtr_ccs_end_time = autopin_template->autopin_ctl.wtr_ccs_end_time;
    log_body->reflog_capture_time = autopin_template->autopin_ctl.reflog_capture_input.tx_capture_time + autopin_template->autopin_ctl.reflog_capture_input.tx_capture_delay;
    log_body->wtr_par = control_input->wtr_capture_result.par;
    log_body->wtr_valid_result = wtr_capture_valid;
    log_body->reflog_valid_result = reflog_capture_valid;
    log_body->seq_nr = seq_nr;
    log_body->num_peaks = control_input->wtr_capture_result.num_peaks;
    memset(log_body->reserved, 0, 3);
  }

  return;
}

