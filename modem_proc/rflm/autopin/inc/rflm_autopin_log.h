#ifndef RFLM_AUTOPIN_LOG_H
#define RFLM_AUTOPIN_LOG_H

/*
  @file
  rflm_autopin_log.h

  @brief
  This file contains logpacket interface for AutoPin.
*/

/*=============================================================================
 
  Copyright (c) 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/autopin/inc/rflm_autopin_log.h#1 $

when       who    what, where, why
--------   ---    -------------------------------------------------------------
01/11/16   whz    Version 3 log packets
10/28/15   whz    Log RGI value in V2
10/21/15   whz    Version 2 log packets
09/21/15   whz    Update the definition of Log Packets again
09/03/15   whz    Update the definition of Log Packets
07/16/15   cdb    Abstract Thor/AMAM specific processing 
07/16/15   whz    Update log packet IDs
06/22/15   whz    Fix FW standalone build issue
06/15/15   whz    Initial version

===========================================================================*/

#include "rflm_api_autopin.h"
#ifdef FEATURE_THOR_MODEM
#include "fw_psamp_autopin_intf.h"
#endif
#include "fw_autopin_intf.h"


/* Since QXDM release 3.14.1037 */
#define RFLM_LOG_PACKET_AUTOPIN_DM_INFO_ID 0x18FB
#define RFLM_LOG_PACKET_AUTOPIN_IQ_CAPTURE_ID 0x18FC

/* Log packet cannot exceed 3964 bytes; truncate IQ to 490 (in unit of word) */
#define RFLM_AUTOPIN_LOG_TX_SAMPLE_SIZE 490
#define RFLM_AUTOPIN_LOG_RX_SAMPLE_SIZE 490


enum
{
  RFLM_AUTOPIN_LOG_VER_V1 = 1,
  RFLM_AUTOPIN_LOG_VER_V2 = 2,
  RFLM_AUTOPIN_LOG_VER_V3 = 3,
};


typedef struct
{
  rflm_handle_tx_t handle; // 32bits
  rflm_tech_id_t tech;  // 8bits
  uint8 pa_state;  // 8bits
  uint8 row_idx_const;  // 8bits
  boolean run_decision;  // 8bits
  uint32 env_scale_backoff;  // 32 bits
  uint32 iq_gain;  // 32 bits
  boolean ready[RFLM_TXAGC_PA_STATE_MAX];  // 8 x 8 bits
  rflm_autopin_state run_state[RFLM_TXAGC_PA_STATE_MAX];  // 8 x 8 bits
  rflm_db10_t pin_accum[RFLM_TXAGC_PA_STATE_MAX];  // 16 * 8 bits
  uint16 temp_read;  // 16 bits
  uint8 seq_nr;
  int8 reserved;
} rflm_autopin_dm_info_log_packet_v1;


typedef struct
{
  rflm_handle_tx_t handle; // 32bits
  rflm_tech_id_t tech;  // 8bits
  uint8 pa_state;  // 8bits
  uint8 rgi;  // 8bits
  boolean run_decision;  // 8bits
  uint32 env_scale_backoff;  // 32 bits
  uint32 env_scale;  // 32 bits
  uint32 iq_gain_backoff;  // 32 bits
  boolean ready[RFLM_TXAGC_PA_STATE_MAX];  // 8 x 8 bits
  rflm_autopin_state run_state[RFLM_TXAGC_PA_STATE_MAX];  // 8 x 8 bits
  rflm_db10_t pin_accum[RFLM_TXAGC_PA_STATE_MAX];  // 16 * 8 bits
  uint16 temp_read;  // 16 bits
  int16 reserved;
  uint32 seq_nr;
} rflm_autopin_dm_info_log_packet_v2;


typedef struct
{
  rflm_handle_tx_t handle; // 32bits
  rflm_tech_id_t tech;  // 8bits
  uint8 pa_state;  // 8bits
  uint8 rgi;  // 8bits
  boolean run_decision;  // 8bits
  uint32 env_scale_backoff;  // 32bits
  rflm_db10_t pin_accum[RFLM_TXAGC_PA_STATE_MAX];  // 16*8bits
  uint32 seq_nr;  // 32bits
  int16 pin_init; // 16bits
  int16 reserved; // 16bits
} rflm_autopin_dm_info_log_packet_v3;


typedef struct
{
  uint8 log_version;
  uint8 num_sub_packets;
  int16 reserved;

  union rflm_autopin_dm_info_log_packet_versions
  {
    rflm_autopin_dm_info_log_packet_v1 v1;
    rflm_autopin_dm_info_log_packet_v2 v2;
    rflm_autopin_dm_info_log_packet_v3 v3;
  } versions;
} rflm_autopin_dm_info_log_packet_t;


typedef struct
{
  uint32 tx_samples[RFLM_AUTOPIN_LOG_TX_SAMPLE_SIZE];
  uint32 rx_samples[RFLM_AUTOPIN_LOG_RX_SAMPLE_SIZE];
  rflm_handle_tx_t handle; // 32bits
  int16 tx_dc_i; // dBfs/100
  int16 tx_dc_q; // dBfs/100
  int16 tx_rms; // dBfs/100
  int16 rx_dc_i; // dBfs/100
  int16 rx_dc_q; // dBfs/100
  int16 rx_rms; // dBfs/100
  int32 peak_offset; // 1e-4
  int16 clip_tx_rms; // dBfs/100            
  int16 clip_tx_mag; // dBfs/100
  int32 amam_err; // 1e-6
  int16 corr_snr;
  uint8 status;
  int8 seq_nr;
} rflm_autopin_iq_capture_log_packet_v1;


typedef struct
{
  uint32 tx_samples[RFLM_AUTOPIN_LOG_TX_SAMPLE_SIZE];
  uint32 rx_samples[RFLM_AUTOPIN_LOG_RX_SAMPLE_SIZE];
  rflm_handle_tx_t handle; // 32bits
  int16 tx_dc_i; // dBfs/100
  int16 tx_dc_q; // dBfs/100
  int16 tx_rms; // dBfs/100
  int16 rx_dc_i; // dBfs/100
  int16 rx_dc_q; // dBfs/100
  int16 rx_rms; // dBfs/100
  int32 peak_offset; // 1e-4
  int16 clip_tx_rms; // dBfs/100            
  int16 clip_tx_mag; // dBfs/100
  int32 amam_err; // 1e-6
  int32 rotator;
  uint32 seq_nr;
  int16 corr_snr;
  uint8 status;
  int8 reserved;
} rflm_autopin_iq_capture_log_packet_v2;


typedef struct
{
  uint64 sum_i_q_sqr;
  uint64 sum_i_and_q_sqr_ave;
  uint64 p_ave;
  uint32 reflog_par;
  uint16 mean_pwr;
  uint16 peak_avg_pwr;
  uint32 capture_length;
  int32 sum_i;
  int32 sum_q;
  int32 i_dc;
  int32 q_dc;
  uint32 fbrx_accum;
  uint32 sum_peaks;
  uint32 wtr_ccs_end_time;
  uint32 reflog_capture_time;
  uint16 wtr_par;
  boolean wtr_valid_result;
  boolean reflog_valid_result;
  uint32 seq_nr;
  uint8 num_peaks;
  int8 reserved[3];
} rflm_autopin_iq_capture_log_packet_v3;


typedef struct
{
  uint8 log_version;
  uint8 num_sub_packets;
  int16 reserved;

  union rflm_autopin_iq_capture_log_packet_versions
  {
    rflm_autopin_iq_capture_log_packet_v1 v1;
    rflm_autopin_iq_capture_log_packet_v2 v2;
    rflm_autopin_iq_capture_log_packet_v3 v3;
  } version;
} rflm_autopin_iq_capture_log_packet_t;


#ifdef FEATURE_THOR_MODEM
void rflm_autopin_logging_dm_info(rflm_autopin_dm_template_t *autopin_template,
                                  boolean run_decision,
                                  uint16 temp_read,
                                  uint32 seq_nr);

void rflm_autopin_logging_iq_capture(fw_psamp_autopin_result_s *fw_results,
                                     uint32 seq_nr);
#endif


#ifndef RFCOMMON_AUTOPIN_API_H
/* The following definitions are extracted from rfcommon_autopin_api.h */
typedef enum 
{
  AUTOPIN_PAR_WTR_CAPTURE                = 0,
  AUTOPIN_PAR_REFLOG_CAPTURE             = 1,  
  AUTOPIN_MAX_SUPPORTED_ACTION
} rfcommon_autopin_capture_t;

typedef struct
{
  uint64  sum_i_q_sqr;
  uint64  sum_i_and_q_sqr_ave;
  uint64  p_ave;
  int32   sum_i;
  int32   sum_q;
  int32   i_dc;
  int32   q_dc;
  uint32  fbrx_accum;
  uint32  sum_peaks;
  uint16  par;
  uint8   num_peaks;
} rfdevice_fbrx_autopin_data_type;

typedef struct
{
  rfcommon_autopin_capture_t capture_type;
  boolean valid_capture;
  uint16 par;
  fw_autopin_psamp_result_out_t reflog_result;
  rfdevice_fbrx_autopin_data_type wtr_capture_result;
} rfcommon_autopin_capture_control_type;
#endif

void rflm_autopin_logging_control_info(rflm_autopin_dm_template_t *autopin_template, boolean run_decision, uint32 seq_nr);

void rflm_autopin_logging_capture_result(rfcommon_autopin_capture_control_type *control_input, rflm_autopin_dm_template_t *autopin_template, boolean wtr_capture_valid, boolean reflog_capture_valid, uint32 seq_nr);


#endif
