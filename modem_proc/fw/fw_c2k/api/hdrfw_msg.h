/*!
  @file
  hdrfw_msg.h

  
  Defines the external message interface for the HDRFW module

*/

/*===========================================================================

  Copyright (c) 2011-13 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/31/11   isalman Don't use q6 headers when compiling for UMPLT.

===========================================================================*/
/** 
  @mainpage Nike-L MCDO Firmware Interface Document
  @authors Kai Ting, Vijay Ramasami
  @section intro Revision
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_c2k/api/hdrfw_msg.h#1 $
  @section whatsnew What's New
  \li hdrfw_tx_pa_on_ctl_msg_t (used to be hdrfw_tx_timed_enable_carrier_msg_t)
  \li cfw_rx_start_rsp_msg_t (used to hdrfw_rx_start_rsp_msg_t)
  \li cfw_rx_stop_rsp_msg_t (used to hdrfw_rx_stop_rsp_msg_t)
  \li cfw_tx_dac_start_msg_t  (see cfw_rf_intf.h)
  \li cfw_tx_dac_stop_msg_t (see cfw_rf_intf.h)
*/

#ifndef HDRFW_MSG_H
#define HDRFW_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "msgr.h"
#include "msgr_umid.h"

#include "fw_dpd_intf.h"
#ifdef __cplusplus
}  // extern "C"
#endif

#include "cfw_rf_intf.h"
#include "hdrfw_const.h"
#include "hdrfw_logging.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define MSGR_MODULE_FW                0x00

#define MSGR_HDR_FW                   MSGR_TECH_MODULE( MSGR_TECH_HDR, MSGR_MODULE_FW)

/*! \brief Commands the HDR FW module receives. */
// Payload input for documentation only.          
enum
{
  /*! \brief The first message in the enum */
  HDR_FW_CMD_FIRST = MSGR_UMID_BASE(MSGR_HDR_FW, MSGR_TYPE_CMD),
  MSGR_DEFINE_UMID(HDR, FW, CMD, STATE_CFG,                   0x0,    hdrfw_state_cfg_msg_t),    
  MSGR_DEFINE_UMID(HDR, FW, CMD, FING_ASSIGN,                 0x1,    hdrfw_fing_assign_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, RESET_SEQ_NUM,               0x2,    hdrfw_reset_seq_num_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, FTCMAC_CHANNEL_CFG,          0x3,    hdrfw_ftcmac_channel_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, FMAC_ACTIVATE,               0x4,    hdrfw_fmac_activate_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, TUNEAWAY_START,              0x5,    hdrfw_tuneaway_start_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, HANDOFF_CFG,                 0x6,    hdrfw_handoff_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, CC_DEMOD_CFG,                0x7,    hdrfw_cc_demod_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, REL0_RL_CFG,                 0x8,    hdrfw_rel0_rl_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, EQ_CFG,                      0x9,    hdrfw_eq_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, EQ_OVERRIDE,                 0xA,    hdrfw_eq_override_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, FLL_START_ACQ,               0xB,    hdrfw_fll_start_acq_msg_t ),
  MSGR_DEFINE_UMID(HDR, FW, CMD, SLEEP_ADJ,                   0xC,    hdrfw_sleep_adj_msg_t ),
  MSGR_DEFINE_UMID(HDR, FW, CMD, SCC,                         0xD,    hdrfw_scc_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, FPD,                         0xE,    hdrfw_fpd_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, HYPERSPACE_CFG,              0xF,   hdrfw_hyperspace_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, HSTR_OFFSET_CFG,             0x10,  hdrfw_hstr_offset_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, DECOB_UPDATE,                0x11,  hdrfw_decob_update_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, DRC_CFG,                     0x12,  hdrfw_drc_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, INDICATION_CFG,              0x13,  hdrfw_indication_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, DRC_TRANSLATION_CFG,         0x14,  hdrfw_drc_translation_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, TIMERS_CFG,                  0x15,  hdrfw_timers_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, BCC_DETECT,                  0x16,  hdrfw_bcc_detect_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, FLOW_CONTROL_CFG,            0x17,  hdrfw_flow_control_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, ASP_UPDATE,                  0x18,  hdrfw_asp_update_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, FLL_ACCUM_LOAD,              0x19,  hdrfw_fll_accum_load_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, FLL_GAIN_CFG,                0x1A,  hdrfw_fll_gain_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, FLL_SLEW_CFG,                0x1B,  hdrfw_fll_slew_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, DLL_CFG,                     0x1C,  hdrfw_dll_cfg_msg_t),     
  MSGR_DEFINE_UMID(HDR, FW, CMD, FMAC_GAUP_CFG,               0x1D,  hdrfw_fmac_gaup_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, MSTR_SLAM,                   0x1E,  hdrfw_mstr_slam_msg_t    ),
  MSGR_DEFINE_UMID(HDR, FW, CMD, DECODE_CTL,                  0x1F,  hdrfw_decode_ctl_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, PARTIAL_LD_RCL_CFG,          0x20,  hdrfw_partial_ld_rcl_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, DECOB_IND_RATE,              0x21,  hdrfw_decob_ind_rate_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, FING_LOCK_THRESH,            0x22,  hdrfw_fing_lock_thresh_msg_t),  
  MSGR_DEFINE_UMID(HDR, FW, CMD, LEGACY_FLOW_CONTROL,         0x23,  hdrfw_legacy_flow_control_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, FMAC_ACK_MODE_CFG,           0x24,  hdrfw_fmac_ack_mode_cfg_msg_t),  
  MSGR_DEFINE_UMID(HDR, FW, CMD, QPCH,                        0x25,  hdrfw_qpch_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, TUNEAWAY_STOP,               0x26,  hdrfw_tuneaway_stop_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, DRC_FILT_RESET,              0x27,  hdrfw_drc_filt_reset_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, FMAC_HANDOFF_CFG,            0x28,  hdrfw_fmac_handoff_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, MUP_DECODE_FILT_CFG,         0x29,  hdrfw_mup_decode_filt_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, MIN_PREAM_THRESH_CFG,        0x2A,  hdrfw_min_pream_thresh_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, PLL_CFG,                     0x2B,  hdrfw_pll_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, FLL_ROT_CFG,                 0x2C,  hdrfw_fll_rot_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, FLL_ROT_OVERRIDE,            0x2D,  hdrfw_fll_rot_override_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, FLL_PDM_OVERRIDE,            0x2E,  hdrfw_fll_pdm_override_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, FLL_ENABLE,                  0x2F,  hdrfw_fll_enable_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, HANDOFF_OVERRIDE,            0x30,  hdrfw_handoff_override_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, DIVERSITY_CFG,               0x31,  hdrfw_diversity_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, FORCE_COMB_MODE,             0x32,  hdrfw_force_comb_mode_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, MIN_BEST_ASP_SINR,           0x33,  hdrfw_min_best_asp_sinr_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, MAX_DRC_CFG,                 0x34,  hdrfw_max_drc_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, RMAC_SUBFRAME_CFG,           0x35,  hdrfw_rmac_subframe_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, RMAC0_GAIN_OFFSET_CFG,       0x36,  hdrfw_rmac0_gain_offset_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, RMAC_SEND_PACKET,            0x37,  hdrfw_rmac_send_packet_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, RMAC_MISC_CFG,               0x38,  hdrfw_rmac_misc_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, RMAC_ATTRIB_CFG,             0x39,  hdrfw_rmac_attrib_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, RMAC_ENABLE_CARRIER,         0x3A,  hdrfw_rmac_enable_carrier_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, RMAC_OPENLOOP_OVERRIDE,      0x3B,  hdrfw_rmac_openloop_override_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, RMAC_HARQCOEFFC2I_CFG,       0x3C,  hdrfw_rmac_harqcoeffc2i_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, RMAC_RESET_PILOTPWR_FILT,    0x3D,  hdrfw_rmac_reset_pilotpwrfilt_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, RMAC_RESET_RL_INTERLACE,     0x3E,  hdrfw_rmac_reset_rl_interlace_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, RMAC_MODULATOR_CFG,          0x3F,  hdrfw_rmac_modulator_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, RMAC_PWRMARGIN_CFG,          0x40,  hdrfw_rmac_pwrmargin_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, RMAC_THROTTLE_CFG,           0x41,  hdrfw_rmac_throttle_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, RMAC_DTXMODE_CFG,            0x42,  hdrfw_rmac_dtxmode_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, RMAC_MAXBACKOFF_CFG,         0x43,  hdrfw_rmac_maxbackoff_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, RMAC_T2PBIAS_CFG,            0x44,  hdrfw_rmac_t2pbias_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, PILOT_MEAS_CFG_REQ,          0x45,  hdrfw_pilot_meas_cfg_req_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, PILOT_MEAS_STOP_STREAM,      0x46,  hdrfw_pilot_meas_stop_stream_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, TX_PA_PARAMS_CONFIG,         0x47,  hdrfw_tx_pa_params_config_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, TX_LIMIT_CONFIG,             0x48,  cfw_tx_limit_config_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, TX_LUT_PARAMS_CONFIG,        0x49,  hdrfw_tx_lut_params_config_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, TX_FREQ_OFFSET_CONFIG,       0x4A,  hdrfw_tx_freq_offset_config_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, TX_CLOSED_LOOP_CONFIG,       0x4B,  hdrfw_tx_closed_loop_config_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, TX_OPEN_LOOP_CONFIG,         0x4C,  hdrfw_tx_open_loop_config_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, TX_OPEN_LOOP_ANT_GAIN_DELTA, 0x4D,  hdrfw_tx_open_loop_ant_gain_delta_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, RXLM_CONFIG_MASKED,          0x4E,  hdrfw_rxlm_config_masked_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, FLL_XO_CFG,                  0x4F,  hdrfw_fll_xo_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, RX_START,                    0x50,  cfw_rx_start_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, RX_START_CFG,                0x51,  cfw_rx_start_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, RX_AGC_CFG,                  0x52,  cfw_rx_agc_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, LNA_OVERRIDE,                0x53,  cfw_lna_override_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, LNA_GAIN_OFFSET,             0x54,  cfw_lna_gain_offset_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, RX_AGC_STOP,                 0x55,  cfw_rx_agc_stop_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, INTELLICEIVER_UPDATE,        0x56,  cfw_intelliceiver_update_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, RXF_DYNAMIC_UPDATE,          0x57,  cfw_rxf_dynamic_update_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, TX_PA_CTL,                   0x58,  hdrfw_tx_pa_ctl_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, RX_STOP,                     0x59,  cfw_rx_stop_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, HDET_REQUEST,                0x5A,  cfw_tx_hdet_request_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, TX_START,                    0x5B,  cfw_tx_start_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, TX_STOP,                     0x5C,  cfw_tx_stop_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, TX_AGC_CFG,                  0x5D,  cfw_tx_agc_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, TX_DAC_START,                0x5E,  cfw_tx_dac_start_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, TX_DAC_STOP,                 0x5F,  hdrfw_tx_dac_stop_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, HDET_CONFIG,                 0x60,  cfw_tx_hdet_config_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, GRICE_SET_FAC_GAIN,          0x61,  hdrfw_grice_set_fac_gain_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, GRICE_OVERRIDE,              0x62,  hdrfw_grice_override_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, VI_CTL,                      0x63,  hdrfw_vi_ctl_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, DYNAMIC_TXC_UPDATE,          0x64,  cfw_dynamic_txc_update_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, TX_PWR_FILTER_CONFIG,        0x65,  cfw_rl_pwr_est_filter_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, EPT_CONFIG,                  0x66,  cfw_ept_config_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, XPT_CONFIG,                  0x67,  cfw_xpt_config_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, DIAG_LOG_CONFIG,             0x68,  hdrfw_diag_log_config_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, JD_THRESHOLD_UPDATE,         0x69,  hdrfw_jd_threshold_update_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, JD_MODE,                     0x6A,  hdrfw_jd_mode_msg_t),
  
  MSGR_DEFINE_UMID(HDR, FW, CMD, OHC_CONFIG,                  0x6B,  hdrfw_ohc_config_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, REL0_TX_CFG,                 0x6C,  hdrfw_rel0_tx_cfg_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, XPT_DELAY_UPDATE,            0x6D,  cfw_xpt_delay_update_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, DLNA_CTL,                    0x6E,  hdrfw_dlna_ctl_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, EARLY_PREAM_CTL,             0x6F,  hdrfw_early_pream_ctl_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, FORCE_ODD_PREAM,             0x70,  hdrfw_force_odd_pream_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, FTM_FORCE_SLEEP,             0x71,  hdrfw_ftm_force_sleep_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, EXERCISE_MILLI_SLEEP,        0x72,  cfw_rx_exercise_milli_micro_sleep_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, FMAC_DETECT_CFG,             0x73,  hdrfw_fmac_detect_cfg_msg_t),
  /* L1 DSDX command */
  MSGR_DEFINE_UMID(HDR, FW, CMD, DSDX_ENABLE,                 0x74,  hdrfw_dsdx_enable_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, REGISTER_RX_TX_ACTIVITY,     0x75,  hdrfw_dsdx_priority_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, CMD, DSDX_FREQUENCY_UPDATE,       0x76,  hdrfw_dsdx_frequency_msg_t),
  /* L1 QTA command */
  MSGR_DEFINE_UMID(HDR, FW, CMD, QTA_CFG,                     0x77,  hdrfw_qta_cfg_msg_t),
  /* QXDMA IQ sample capture */
  MSGR_DEFINE_UMID(HDR, FW, CMD, RX_IQ_SAMPLES_LOGGING,       0x78,  hdrfw_rx_iq_samples_logging_msg_t),

#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY
  MSGR_DEFINE_UMID(HDR, FW, CMD, ANT_SWITCH_ASDIV,            0x79,  cfw_rf_execute_ant_switch_msg_t),
  HDR_FW_CMD_LAST = HDR_FW_CMD_ANT_SWITCH_ASDIV
#else
  HDR_FW_CMD_LAST = HDR_FW_CMD_RX_IQ_SAMPLES_LOGGING
#endif

};

/*! \brief Indications/Updates the HDR FW module sends. */       
// Payload input for documentation only.                     
enum                                                             
{                                                                
  HDR_FW_IND_FIRST = MSGR_UMID_BASE(MSGR_HDR_FW, MSGR_TYPE_IND),
  //MSGR_DEFINE_UMID(HDR, FW, IND, RMAC_SUBFRAME,   0x0, hdrfw_rmac_subframe_ind_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, IND, DECOB_STATUS,    0x1, hdrfw_decob_status_ind_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, IND, BEST_ASP_CHANGE, 0x2, hdrfw_best_asp_change_ind_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, IND, RMAC0_FRAME,     0x3 , hdrfw_rmac0_frame_ind_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, IND, SCC_PREAMBLE,    0x4 , hdrfw_scc_preamble_ind_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, IND, DRC_SUPERVISION_TIMEOUT, 0x5, hdrfw_drc_supervision_timeout_ind_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, IND, DRC_SUPERVISION_RESTART, 0x6, hdrfw_drc_supervision_restart_ind_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, IND, LOG_BUFFER,      0x7, hdrfw_log_buffer_ind_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, IND, TIMERS,          0x8, hdrfw_timers_ind_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, IND, VI_CTL,          0x9, hdrfw_vi_ctl_ind_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, IND, RXAGC_TRACK,     0xA, hdrfw_rxagc_track_ind_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, IND, RESET_REQ,       0xB, hdrfw_reset_req_ind_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, IND, SCC_DECODE,      0xC, hdrfw_scc_decode_ind_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, IND, FLEXCONN_REPOINT, 0xD, hdrfw_tx_flexconn_ind_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, IND, TX_RESET_REQ,    0xE, hdrfw_reset_tx_seq_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, IND, RXAGC_TRACK_QTA, 0xF, hdrfw_rxagc_track_ind_msg_t),

  HDR_FW_IND_LAST = HDR_FW_IND_RXAGC_TRACK_QTA
};

/*! \brief Response the HDR FW module sends. */
// Payload input for documentation only.          
enum
{
  HDR_FW_RSP_FIRST = MSGR_UMID_BASE(MSGR_HDR_FW, MSGR_TYPE_RSP),
  MSGR_DEFINE_UMID(HDR, FW, RSP, STATE_CFG,           0x0, hdrfw_state_cfg_rsp_msg_t),         
  MSGR_DEFINE_UMID(HDR, FW, RSP, SRCH_FING_ASSIGN,    0x1, hdrfw_srch_fing_assign_rsp_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, RSP, SRCH_FLL_ACQ,        0x2, hdrfw_srch_fll_acq_rsp_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, RSP, SRCH_SLAM,           0x3, hdrfw_srch_slam_rsp_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, RSP, SRCH_RESET_DRC_FILT, 0x4, hdrfw_srch_reset_drc_filt_rsp_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, RSP, SRCH_FLL_ACCUM_LD,   0x5, hdrfw_srch_fll_accum_ld_rsp_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, RSP, SRCH_EQ_CONFIG,      0x6, hdrfw_srch_eq_config_rsp_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, RSP, CANCEL_ACTIVE_PACKET,0x7, hdrfw_cancel_active_packet_rsp_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, RSP, RX_START,            0x8, cfw_rx_start_rsp_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, RSP, RX_STOP,             0x9, cfw_rx_stop_rsp_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, RSP, INTELLICEIVER_UPDATE,0xA, cfw_intelliceiver_update_rsp_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, RSP, HDET_RESPONSE,       0xB, cfw_tx_hdet_response_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, RSP, PILOT_MEAS_CFG,      0xC, hdrfw_pilot_meas_cfg_rsp_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, RSP, PILOT_MEAS_STOP_STREAM, 0xD, hdrfw_pilot_meas_stop_stream_rsp_t),

  MSGR_DEFINE_UMID(HDR, FW, RSP, TX_START,            0xE, cfw_tx_start_rsp_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, RSP, TX_STOP,             0xF, cfw_tx_stop_rsp_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, RSP, TX_DAC_START,        0x10, cfw_tx_dac_start_rsp_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, RSP, TX_DAC_STOP,         0x11, cfw_tx_dac_stop_rsp_msg_t),
  
  MSGR_DEFINE_UMID(HDR, FW, RSP, ASP_UPDATE,          0x12, hdrfw_asp_update_rsp_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, RSP, TX_PA_CTL,           0x13, hdrfw_tx_pa_ctl_rsp_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, RSP, IRAT_RX_START,       0x14, cfw_irat_rx_start_rsp_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, RSP, PA_PARAMS_CONFIG,    0x15, cfw_pa_params_rsp_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, RSP, XPT_TRANS_COMPL,     0x16, cfw_xpt_trans_compl_rsp_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, RSP, XPT_CONFIG,          0x17, cfw_xpt_config_rsp_msg_t),
  MSGR_DEFINE_UMID(HDR, FW, RSP, SRCH_FING_ASSIGN_SLAM,    0x18, hdrfw_srch_fing_assign_slam_rsp_msg_t),
#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY
  MSGR_DEFINE_UMID(HDR, FW, RSP, ASDIV_PRI_CHANGE,    0x19, hdrfw_asdiv_pri_change_rsp_msg_t),

  HDR_FW_RSP_LAST = HDR_FW_RSP_ASDIV_PRI_CHANGE
#else

  HDR_FW_RSP_LAST = HDR_FW_RSP_SRCH_FING_ASSIGN_SLAM
#endif
};

  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR INPUTS RECEIVED FROM Search                   */
  /*-----------------------------------------------------------------------*/

/**
    \defgroup fmsg Finger Assignment       
    Finger Assignment Message Structures. */
/*\{*/
/** Bit-fields of finger assignment config word. */
typedef union {

  struct {
    /** Enable/Disable Finger. 1=enable finger, 0=disable finger */
    uint32 enable:1;
    uint32 :7;
    /** Maps the finger to an ASP. The ASP structure must be configured before
        finger assignment. Valid range is 0..HDRFW_NUM_ASP-1. No more than
        HDRFW_NUM_FINGERS_PER_ASP fingers per carrier may be assigned to the same ASP. */
    uint32 asp:3;
    uint32 :5;
    uint32 :16;
  };
  uint32 intval;
} hdrfw_fing_config_field_t;

/*! \brief Finger Assigment Entry - An entry in the Carrier Finger Assigment
           Message */
typedef struct {
  uint32 Config;
  uint32 RTCOfs;
  int16 InitSNR;
} hdrfw_finger_assign_intf_t;

typedef struct {
  /**   Finger configuration.
      \verbatim
        15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
      |    |    |    |    |    |    ASP       |    |    |    |    |    |    |    |  E |
      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
      
      E   : Enable. 1=enable finger, 0=disable finger

      ASP : Maps the finger to an ASP. The ASP structure must be configured
      before finger assignment. Valid range is 0 ..NUM_ASP-1. No more than
      FINGERS_PER_ASP fingers per carrier may be assigned to the same ASP.
      \endverbatim

      */
  union {
    // XXX Must match hdrfw_finger_assign_intf_t
    struct {

      hdrfw_fing_config_field_t config;

      /*! \brief Initial finger RTC offset in units of 1/8 chip (unsigned). */
      uint32 rtcOfs;

      /*! \brief Starting value for the finger's filtered RSSI (both antennas if 
        diversity mode). Linear scale, range is 0..0x7FFF, see 1.8 for units. */
      uint16 initSnr;

    };
    hdrfw_finger_assign_intf_t assign_intf;
  };

} hdrfw_fing_assign_t;

/** Multi-Carrier Multi-Finger Assignment Write Interface - Assign fingers to specified carriers */
typedef struct  ALIGN(32) {

  /*! \brief The requested configuration for the given finger. */
  hdrfw_fing_assign_t       fingC[HDRFW_DEMOD_CARRIERS][HDRFW_NUM_FINGERS];

} hdrfw_fing_assign_interface_t;


/** Finger Assignment Message. 
    After configuring the assignment interface structure, Software sends
    this message to firmware to initiate the finger assignment. assignMask indicates
    valid finger assignment entries in the shared memory.

    Repsonse: hdrfw_srch_fing_assign_rsp_msg_t.
    (or)
              hdrfw_srch_fing_assign_slam_rsp_msg_t (In case of slam with finger)
*/
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  /*! \brief The mask of which fingers should be actually updated. */
  uint16                    assignMask[HDRFW_DEMOD_CARRIERS];

  /*! \indicate if "slam" is needed in same HS as fing assign or not */
  /* NOTE: set by SW in SYNC state and FW need to send response to SW*/
  boolean                   slamWithFinger;

} hdrfw_fing_assign_msg_t;

/** Finger Lock Threshold Config Message. */
typedef struct {
  
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /** If antenna RSSIFilt or finger PathRSSIFilt rises above this threshold,
      the antenna or finger is in lock. Default=32 (-12dB).  */
  uint16                    fingLockThreshHi;
  /** If antenna RSSIFilt or finger PathRSSIFilt drops below this threshold,
      the antenna or finger is out of lock. Default=15 (-15dB).  */
  uint16                    fingLockThreshLo;
} hdrfw_fing_lock_thresh_msg_t;

/*\}*/
  
  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR ASP                                           */
  /*-----------------------------------------------------------------------*/
/** Asp Assignment Init Control. Controls how FRABSoft and QRABSoft are
    initialized when ASP is configured. */
typedef union {
  struct {
    uint32 :14;
    /** F=1: FRABSoft = max of FRAB over all in lock sectors (hdrmdsp_mac_t::FRAB), 
        QRABSoft = 0 */
    uint32 F:1;
    /* Z=1: FRABSoft/QRABSoft = 0 */
    uint32 Z:1;
    uint32  :16;
  };
  uint32 intval;
} hdrfw_asp_init_ctl_field_t;

/**
    \defgroup aspu Active Set Pilot
    Active Set Pilot Structure update and various other configuration params 
    at 
    - TCA ASET Update
    - ASET Idle Handoff
    - ASET Enter Idle
    - ASP Update at non-TC */
/*\{*/
/* Bit-fields for ASP ID field config word. */
typedef union {
  struct {
    /** Link identifier for this ASP. Copied to the decoder packet header. */
    uint32 linkID:8;
    /** Scheduler group for this ASP. Copied to the decoder packet header. */
    uint32 groupID:8;
    uint32  :16;
  };
  uint32 intval;
}hdrfw_asp_id_field_t;

typedef struct {
  /** ASP Configuration. 
   *  Bit 15: if set as 1, ASP is active. The ASP must be activated before fingers may be assigned
   *  to it. This bit shall not be set for C-set pilot that gets included in ASP structure only
   *  for the purpose of gRICE tracking.
   *  Bit 14: if set as 1, the associated eqRTCOfsCx2 is valid when GRICE is enabled.
   *  Bit 0: if this bit is set as 1, bit 15 shall be set as 0, this pilot belongs to C-set and but is
   *  included in ASP structure only for the purpose of gRICE tracking and finger assignment.
  */
  uint16                  config;
  /** Carrier index for this ASP, from 0..HDRFW_TOTAL_CARRIERS-1. */
  uint16                  carrier; 
  /** Link and Group IDs. */
  hdrfw_asp_id_field_t    ID;     
  /** PN index for the ASP, in units of 64 chips (0..511). */
  uint16                  pnIndex;
  /** The MAC index assigned for forward traffic channel on this ASP. Maximum value is 383. */
  uint16                  flMACIndex;
  /** The MAC index used for the reverse link associated with this ASP. Maximum value is 383. */
  uint16                  rlMACIndex;
  /** The MAC index used to monitor RAB channel for the reverse link associated with this ASP. */
  uint16                  rabMACIndex;
  /** Walsh cover index for the DRC channel. */
  uint16                  drcCover;
  /** Walsh cover index for the DRC channel when requesting Rel B DRC 16-27
      rates. Predictor does not select Rel B rates if AUXDRCCover is not
     assigned (default = 0). */
  uint16                  auxDrcCover;
  /** RAB length, in slots. Must be a power of two. */
  uint16                  rabLength;  
  /** RAB offset, in slots. Aligned with the roll-over of the MSTR. */
  uint16                  rabOffset;  
  /** Filter coefficient for RAB LLR, a function of the RAChannelGain attribute.
      RABChannelGain = ROUND(4*sqrt(RAChannelGain)*4096) Q12 signed. */
  uint16                  rabChannelGain;
  /** Data Source Control value of the cell to which this ASP belongs. */
  uint16                  dsc;        
  /** Identifies the cell associated with this ASP. Range is 0..HDRFW_NUM_CELLS-1. */
  uint16                  cellMap;
  /** Equalizer RTC offset for this ASP, based on SW center of mass algorithm. 1/2 chip units. */
  uint16                  eqRTCOfsCx2;
  /** Backoff per asp per carrier for NLB in dB Q7. */
  int16                   NlbBackOffSinr;
} hdrfw_asp_config_intf_params_t;

typedef union {
  struct {
    /** ASP Configuration. 
     *  Bit 15: if set as 1, ASP is active. The ASP must be activated before fingers may be assigned
     *  to it. This bit shall not be set for C-set pilot that gets included in ASP structure only
     *  for the purpose of gRICE tracking.
     *  Bit 0: if this bit is set as 1, bit 15 shall be set as 0, this pilot belongs to C-set and  but is
     *  included in ASP structure only for the purpose of gRICE tracking and finger assignment.
    */
    uint16                  config;
    /** Carrier index for this ASP, from 0..HDRFW_TOTAL_CARRIERS-1. */
    uint16                  carrier; 
    /** Link and Group IDs. */
    hdrfw_asp_id_field_t    ID;     
    /** PN index for the ASP, in units of 64 chips (0..511). */
    uint16                  pnIndex;
    /** The MAC index assigned for forward traffic channel on this ASP. Maximum value is 383. */
    uint16                  flMACIndex;
    /** The MAC index used for the reverse link associated with this ASP. Maximum value is 383. */
    uint16                  rlMACIndex;
    /** The MAC index used to monitor RAB channel for the reverse link associated with this ASP. */
    uint16                  rabMACIndex;
    /** Walsh cover index for the DRC channel. */
    uint16                  drcCover;
    /** Walsh cover index for the DRC channel when requesting Rel B DRC 16-27
        rates. Predictor does not select Rel B rates if AUXDRCCover is not
       assigned (default = 0). */
    uint16                  auxDrcCover;
    /** RAB length, in slots. Must be a power of two. */
    uint16                  rabLength;  
    /** RAB offset, in slots. Aligned with the roll-over of the MSTR. */
    uint16                  rabOffset;  
    /** Filter coefficient for RAB LLR, a function of the RAChannelGain attribute.
        RABChannelGain = ROUND(4*sqrt(RAChannelGain)*4096) Q12 signed. */
    uint16                  rabChannelGain;
    /** Data Source Control value of the cell to which this ASP belongs. */
    uint16                  dsc;        
    /** Identifies the cell associated with this ASP. Range is 0..HDRFW_NUM_CELLS-1. */
    uint16                  cellMap;
    /** Equalizer RTC offset for this ASP, based on SW center of mass algorithm. 1/2 chip units. */
    uint16                  eqRTCOfsCx2;
    /** Backoff per asp per carrier for NLB in dB Q7. */
    int16                   NlbBackOffSinr;
  };
  hdrfw_asp_config_intf_params_t intf_params;

} hdrfw_asp_config_t;

/** ASP Config Interface. Shared memory structure definition of ASP update interface. */
typedef struct  ALIGN(32) {
  /*! \brief ASP configuration sturcture */
  hdrfw_asp_config_t        aspC[HDRFW_DEMOD_CARRIERS][HDRFW_NUM_ASP];

  /** Rank of ASP in terms of filtered SINR.
   *  snrRank[c][0..6] would be read by FW if bit C carrierMask is set.
   *  
   *  0 for strongest to HDR_NUM_ASP-1 for weakest; 7 for inactive ASP.
   *  DSP does not check Config word in ASP structure to determine active
   *  status of ASP and solely rely on this field for G-set selection.
   *  Rank needs to be continuous for the set of valid ASPs.
   *  */
  uint8                   snrRank[HDRFW_DEMOD_CARRIERS][HDRFW_NUM_ASP]; 

} hdrfw_asp_cfg_interface_t;

/** ASP Update Message. Firmware copies information in ASP config interface upon receiving this message */
typedef struct {
  
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  uint16                    carrierMask;
  /** ASP Update Mask.
      Mask indicating updated ASP information available in hdrfw_asp_cfg_interface_t. 
      aspUpdateMask[c], bit 'a' indicates that aspC[c][a] is available for update.
  */
  uint16                      updateMask[HDRFW_DEMOD_CARRIERS];

  /** FRABSoft/QRABSoft inititialization control. */
  hdrfw_asp_init_ctl_field_t  initCtl[HDRFW_DEMOD_CARRIERS][HDRFW_NUM_ASP]; 

} hdrfw_asp_update_msg_t;

/** Best ASP change indication Message.*/
typedef struct {
  
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
 
  /** Bit(c) indicates carrier c has a change of best ASP. New ASP can be found in bestAsp[c]. */
  uint16                    bestAspChangeMask;
  /*! \brief New ASP index after handoff */
  uint16                    bestAsp[HDRFW_DEMOD_CARRIERS];

} hdrfw_best_asp_change_ind_msg_t;
/*\}*/

/**
    \defgroup hoctl Handoff Configuration
    Messages configuring handoff override, reference carrier and cc demod carrier.
    */
/*\{*/

/** NLB configuration substructure */
typedef struct {
  /** ccDemodOnAllCarrier==1 => NLB enabled otherwise not */
  int16                     ccDemodOnAllCarriers; 
  /** NlbK is in Q6 format */
  int16                     NlbK[HDRFW_DEMOD_CARRIERS];
  /** NlbTh is in Q7 format */
  int16                     NlbTh;
} hdrfw_handoff_nlb_t;

/** Handoff Configuration Message */
typedef struct {
  msgr_hdr_struct_type      hdr;
  uint32                    handoffControl_Valid:1;
  /** 1 = Locked handoff mode (ServingSectorLockAcrossSubActiveSets=1) */
  uint32                    handoffControl:1;
  uint32                    handoffRefCarrier_Valid:1;
  /** Demod carrier which is the reference pilot sub-active set.. Relevant only in 
      Locked mode. */
  uint32                    handoffRefCarrier:3;
  uint32                    ccDemodCarrier_Valid:1;   
  /** Specifies the demod carrier which contains control channel
      [0..HDRFW_DEMOD_CARRIERS-1]. Control channel will not be detected on other
      carriers. */
  uint32                    ccDemodCarrier:3;   

  /** NLB configuration parameters */
  hdrfw_handoff_nlb_t       NLBCfg;

} hdrfw_handoff_cfg_msg_t;

/** Bit-fields of Handoff Override config word. */
typedef union {
  struct {
    uint32  :13;
    /** Reset predC[c].rclPredAdjHi/Lo to 0 in trigger mode
        handoff. Note that this bit is ignored in normal handoff override. */
    uint32  resetRCL:1;
    /** Trigger mode. Set to allow firmware to resume normal operation  
        after switching to BestASPDict. Firmware clears HandoffOverride
        flag upon completion of the handoff. */
    uint32  trigger:1;
    /** Software controlled handoff to ASP specified in bestASPDict */
    uint32  override:1;
    uint32  :16;
  };
  uint32 intval;
} hdrfw_ho_override_field_t;

typedef struct {
  
  /** Configuration in this message is valid for this carrier. */
  uint32                    carrierValid:1;
  /** Software dictated ASP for idle mode override or immediate handoff trigger. */
  uint32                    bestASPDict:3;
  hdrfw_ho_override_field_t handoffOverride;
} hdrfw_ho_override_t;

/** Handoff Override Message.  */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  hdrfw_ho_override_t       handoffOverride[HDRFW_DEMOD_CARRIERS];
} hdrfw_handoff_override_msg_t;
/*\}*/

  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR RTCMAC SW to FW Config Messages               */
  /*-----------------------------------------------------------------------*/

/**
    \defgroup rmacconfig RTCMAC Rev A/B Configuration
     Rev A/B RTCMAC Configuration.
    */
/*\{*/

/** Bit-fields for ModulatorConfig word */
typedef union {
  struct {
    /** Bit(c) indicates Overhead channels from Demod Carrier c are associated
        with this pilot. */
    uint32 carrierMask:3;
    uint32 :12;
    /** Pilot for this channel is enabled */
    uint32 enable:1;
    uint32 :16;
  };
  uint32 intval;
} hdrfw_modulator_config_field_t;

/** Bit-fields of RAB decision control word */
typedef union {

  struct {
    /** Anytime RAB is disabled, set asp[].QRABSoft = 0, asp[].FRABSoft = max(0,
        asp[].FRABSoft), macC[].FRAB = max(0, macC[].FRAB). */
    uint32 M:1;
    /** Anytime RAB is disabled, clear asp[].QRABSoft, asp[].FRABSoft, and macC[].FRAB.*/
    uint32 R:1;
    uint32  :13;
    /** Disable update of macC[].FRAB, asp[].QRABSoft, and asp[].FRABSoft. */
    uint32 D:1;
    uint32  :16;
  };
  int32 intval;
} hdrfw_rab_decision_ctl_field_t;

/** Bit-fields of Gain Adjust Override config word */
typedef union {
  struct {
    /** Source carrier. */
    uint32 carrier:3;
    uint32 :9;
    /** Initialize to minimum transmit pilot power.  
        GainAdjValue = MinTxP-OpenLoop
        where MinTxP is minimum instantaneous pilot power among enabled carriers.
        OpenLoop is open loop power of the carrier being initialized.
        source carrier field is don't care when this bit is set. */
    uint32 initToMinPwr:1;
    /** Override with GainAdjVal of source carrier */
    uint32 useGainAdjVal:1;
    /** Trigger Mode. Can be paired with either Bit 15 or 14. Firmware copies
        the corresponding source value to GainAdjVal and resumes normal operation. */
    uint32 triggerMode:1;
    /** Override with GainAdjWrite of source carrier */
    uint32 useGainAdjWrite:1;
    uint32 :16;
  };
  int32 intval;
} hdrfw_gain_adj_override_field_t;

/** Bit-fields of Force Early Terminate word.
   \li 0x0001: terminate after 1 subpacket transmitted
   \li 0x0002: terminate after 2 subpackets transmitted  earlier termination possible
   \li 0x0003: terminate after 3 subpackets transmitted  earlier termination possible
   \li 0x8001: terminate exactly after 1 subpacket transmitted
   \li 0x8002: terminate exactly after 2 subpackets transmitted
   \li 0x8003: terminate exactly after 3 subpackets transmitted
 */
typedef union {
  struct {
    /** n: <1,2,3>, subpacket termination target. */
    uint32 n:2;
    uint32 :13;
    /** Force exact termination at specified target. */
    uint32 force:1;
    uint32 :16;
  };
  uint32 intval;
} hdrfw_rl_force_early_term_field_t;

/** Per carrier RMAC Misc Config - DRC, Closed-loop gain adjust and RAB config */
typedef struct {
   
  /** EnableDRC */
  uint16                           enableDrc;

  /** GainAdjOverride interface. */
  hdrfw_gain_adj_override_field_t  gainAdjOverride;
  uint16                           gainAdjWrite;

  /** Control behavior of Rel-A RAB when the RAB channel is disabled. 
      Whenever RAB is disabled, default behavior unless otherwise specifed:
	    macC[].FRAB: no change
	    asp[].QRABSoft: 0
	    asp[].FRABSoft: no change */
  hdrfw_rab_decision_ctl_field_t   rabDecisionCtl;

  /** EnableDRCSupervision */
  uint16                           enableDrcSupervision;

  /** UnfreezeDRCSupervision */
  uint16                           unfreezeDrcSupervision;

} hdrfw_rmac_per_carrier_misc_cfg_t;

/** DRC, Closed-loop gain adjust and RAB field mask */
typedef struct {
  uint16                        enableDRC_Valid:1;
  uint16                        gainAdjOverride_Valid:1;
  uint16                        rabDecisionCtl_Valid:1;
  uint16                        enableDrcSupervision_Valid:1;
  uint16                        unfreezeDrcSupervision_Valid:1;
} hdrfw_rmac_misc_cfg_field_mask_t;

/*! \brief RTCMAC Misc Config Entry - DRC enable, closed loop adjust and RAB decision control message.*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type               hdr;

  /** Field mask to indicate which field is valid
      within per carrier config structure C[c]
    */
  hdrfw_rmac_misc_cfg_field_mask_t   fieldMask;

  /** Software sets bit c to indicate rmacCfg[c] is valid */
  uint16                             carrierMask;

  /*! \brief Per carrier RTCMAC misc config entry - DRC enable, close loop adjust, RAB decision ctl */
  hdrfw_rmac_per_carrier_misc_cfg_t  C[HDRFW_DEMOD_CARRIERS];

} hdrfw_rmac_misc_cfg_msg_t;


/*! \brief Per-carrier RMAC attribute config*/
typedef struct {

  /** macC[c].hArqCoeffC2i */
  uint16                           hArqCoeffC2i;
  uint16                           auxPilotToData;
  uint16                           rriToPilot;
  
} hdrfw_rmac_attrib_cfg_t;

/*! \brief RTCMAC Attribute Config*/
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type              hdr;

  /** Software sets bit c to indicate attribCfg[c] is valid */
  uint16                            carrierMask;
  
  /*! \brief Per carrier RTCMAC config entry - DRC enable, close loop adjust, RAB decision ctl */
  hdrfw_rmac_attrib_cfg_t           attribCfg[HDRFW_DEMOD_CARRIERS];

  /** (1-FRAB coefficient).
      Coefficient for Filtered RAB filter based on the IIR filter time constant, FRABFilterTC:
      = ROUND((FRABFilterTC-1)/FRABFilterTC * 65536)
      Q16 unsigned, range: [0x8000, 0xFFFF] = [0.5, 1.0).
      Default: 0xFF55 = 383/384 (for FRABFilterTC=384 slots).
  */
  uint16                            oneMinusFrabCoeff;
  /** (1-QRAB coefficient).
      Coefficient for Quick RAB filter based on the IIR filter time constant, QRABFilterTC:
      = ROUND((QRABFilterTC-1)/QRABFilterTC * 65536)
      Q16 unsigned, range: [0x8000,0xFFFF] = [0.5,1.0).
      Default: 0xC000 =  (for QRABFilterTC = 4 slots)
  */
  uint16                            oneMinusQrabCoeff;
  
  uint16                            auxPilotMinPayload;
  uint16                            txTrafficToPilotMin;

} hdrfw_rmac_attrib_cfg_msg_t;


/*! \brief RTCMAC Enable Carrier */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type              hdr;

  /** Software sets bit c to indicate modulatorConfig[c] is valid */
  uint16                            carrierMask;

  /*! \brief Per carrier modulator config entry */
  hdrfw_modulator_config_field_t    modulatorConfig[HDRFW_DEMOD_CARRIERS];

} hdrfw_rmac_enable_carrier_msg_t;

/*! \brief This message turn on and off PA/TX based on the provided 
          timing information */
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type              hdr;

  /** The targeted Tx Half-Slot index at which the PA/TX needs to be on or
   *  off. When turning on PA/TX, usually, a PA warm up period is needed;
   *  thus FW schedule to turn on PA/TX at txHalfSlotIndex-1. This field is
   *  only useful is immediate is not set. */
  uint16                            txHalfSlotIndex;

  /*! \brief Immediate async PA/TX_ON scheduling. If set as 1, PA/TX will be scheduled
             in the upcoming tx HalfSlot.*/
  uint16                            immediate;
  
  /** 1=Turn PA/TX ON, 0=Turn PA/TX OFF. */
  uint16                            txPaState;

} hdrfw_tx_pa_ctl_msg_t;

typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type hdr;
 
  /*! indicates traffic mode. 0/1/2 for Rev-0/A/B */
  int16 hdrTrafficMode;

} hdrfw_tx_dac_stop_msg_t;

/** Bit-fields of Openloop Override config word */
typedef union {
  struct {
    /** Source carrier. */
    uint32 carrier:3;

    /** Field valid bit - used as carrier mask */
    uint32 fieldValid:1;
    
    uint32 :10;
    /** Trigger Mode. Can be paired with either Bit 15. Firmware copies
        the corresponding source value to OpenLoop and resumes normal operation. */
    uint32 triggerMode:1;
    /** Override with txC[c].OpenLoop of source carrier. */
    uint32 enableOverride:1;
    uint32 :16;
  };
  uint32 intval;
} hdrfw_openloop_override_field_t;


/*! \brief RTCMAC Openloop Override Config */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type              hdr;

  /*! \brief Per carrier open loop override entry */
  hdrfw_openloop_override_field_t  openLoopOverride[HDRFW_DEMOD_CARRIERS];

} hdrfw_rmac_openloop_override_msg_t;


/*! \brief RTCMAC hArqCoeffC2i Config */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type              hdr;

  /** Software sets bit c to indicate hArqCoeffC2i[c] is valid */
  uint16                            carrierMask;

  /*! \brief Per carrier open loop override entry */
  uint16                            hArqCoeffC2i[HDRFW_DEMOD_CARRIERS];

} hdrfw_rmac_harqcoeffc2i_cfg_msg_t;


/*! \brief RTCMAC Reset Filtered Pilot Power*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type              hdr;

  /** Software sets bit c to reset pilotpowerfilt for the carrier */
  uint16                            carrierMask;

} hdrfw_rmac_reset_pilotpwrfilt_msg_t;


/** RTCMAC Reset RL Interlace. Originally rlPacketC[].SyncRefresh. 
    SyncIndex/SyncHalfSlotTrigger no longer needed */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type              hdr;

  /** Software sets bit c to clear the carrier's to-be-queued data to encoder */
  uint16                            carrierMask;

} hdrfw_rmac_reset_rl_interlace_msg_t;


/*! \brief RTCMAC Modulator Config*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type              hdr;

  /*! \brief Modulator mode. 0 for Rev0. 1 for RevA. 2 for RevB*/
  uint16                            hdrModulatorModeConfig;

  /** Step size for closed-loop power control adjustment, Q9 unsigned in dB.
       1.5 dB: 768  1dB: 512  (Default) 0.5dB: 256 */
  uint16                            txGainAdjStep;

  uint16                            txPilotPowerFiltCoeff;
  
  hdrfw_rl_force_early_term_field_t rlForceEarlyTerminate;

} hdrfw_rmac_modulator_cfg_msg_t;

/*! \brief Payload of RTCMAC Power Margin Config Message*/
typedef struct {
  uint16                            txPwrMarginMax;
  uint16                            txPwrMarginMin;
  uint16                            txPwrMarginUpStep;
  uint16                            txPwrMarginDownStep;
  uint16                            txPwrMarginUseCL;
} hdrfw_rmac_pwrmargin_cfg_params_t; 

/*! \brief RTCMAC Power Margin Config*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type              hdr;

  union {
    struct {    
      uint16                            txPwrMarginMax;
      uint16                            txPwrMarginMin;
      uint16                            txPwrMarginUpStep;
      uint16                            txPwrMarginDownStep;
      uint16                            txPwrMarginUseCL;
    };
    hdrfw_rmac_pwrmargin_cfg_params_t   txPwrMarginCfgParams; 
  };
} hdrfw_rmac_pwrmargin_cfg_msg_t;


/*! \brief RTCMAC Configure Throttling*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type              hdr;

  uint16                            txThrottleDisable;

} hdrfw_rmac_throttle_cfg_msg_t;


/*! \brief RTCMAC DTXmode Config*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type              hdr;

  uint16                            dtxConfig;
  uint16                            dtxFrameOffset;
  uint16                            drcGainBoostCfg;
  uint16                            flexConnEnable;
  int16                             dtxTxPwrThreshold;

} hdrfw_rmac_dtxmode_cfg_msg_t;


/*! \brief RTCMAC Multi-carrier Max Backoff Config*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type              hdr;

  uint16                            txTotalMaxBackoff;

} hdrfw_rmac_maxbackoff_cfg_msg_t;


/*! \brief RTCMAC configure T2P bias message.*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type              hdr;

  /** T2PBias for RRI = 1 to 12. */
  uint16                            rlT2PBiasRri[12];

} hdrfw_rmac_t2pbias_cfg_msg_t;


/*\}*/

/**
    \defgroup rel0rl Release 0 Reverse Link
    Release 0 Reverse Link Configuration.
   */

/*\{*/

typedef union {
  struct {
    uint32                        enableDRC_Valid:1;
    uint32                        gainAdjOverride_Valid:1;
    uint32                        txGainAdjStep_Valid:1;
    /** modulatorConfig_Valid is unused in DIME **/
    uint32                        modulatorConfig_Valid:1;
    uint32                        txAgcAccessMode_Valid:1;
    uint32                        txThrottleDisable_Valid:1;
    uint32                        enableDrcSupervision_Valid:1;
    uint32 :25;
  };
  uint32 intval;
} hdrfw_rel0_rl_cfg_field_mask_t;

/*! \brief MDSP Rel 0 RL Config Message.                                */
typedef struct {
  /*! \brief  Message header */
  msgr_hdr_struct_type            hdr;

  hdrfw_rel0_rl_cfg_field_mask_t  fieldMask;

  uint16                          enableDRC;
  
  hdrfw_gain_adj_override_field_t gainAdjOverride;
  uint16                          gainAdjWrite;
  uint16                          txGainAdjStep;

  /** Indication of transmission of Access Probes on modulator 0 only. */
  uint16                         txAgcAccessMode;
  
  uint16                         txThrottleDisable;
  /*! \brief  txRel0DataToPilot[] */
  uint16                         enableDrcSupervision;
} hdrfw_rel0_rl_cfg_msg_t;

typedef struct {
   /*! \brief  Message header */
  msgr_hdr_struct_type            hdr;
  uint16                          dataToPilot[6];
} hdrfw_rmac0_gain_offset_cfg_msg_t;

/*! \brief RTCMAC0 Frame Status Update Message from Firmware.      */
typedef struct {
  
  /*! \brief  Message header */
  msgr_hdr_struct_type      hdr;
  /** 1: At least one sector in the active set had the RA  bit set.
      0: No RA bits received from any sector */
  uint16                    rabDecision;
  uint16                    txMaxDataRateIndex;

} hdrfw_rmac0_frame_ind_msg_t;

/**
    \defgroup rmacsub RTCMAC Subframe Configuration    */
/*\{*/

/*! \brief RTCMAC Funnel Mode Config Entry.                         */
typedef struct {
 
  uint32    funnelStateValid;
  uint32    funnelState;
  uint32    dataActiveCarrierIndex;
 
} hdrfw_funnel_mode_cfg_t;

/*! \brief RTCMAC Subframe Config Message.                          */
typedef struct {
  
  /*! \brief  Message header */
  msgr_hdr_struct_type      hdr;
 
  uint16                    txTotalMaxAllocated[HDRFW_DEMOD_CARRIERS];
  uint16                    txCarriersOrderedByRank[HDRFW_DEMOD_CARRIERS];
  hdrfw_funnel_mode_cfg_t   txFunnelModeCfg;
} hdrfw_rmac_subframe_cfg_msg_t;

/*! \brief MDSP RL Packet Config Entry.                                  */
typedef struct {

  /*! \brief Bit C,S (SW completed/ SW starting update)  are removed   */
  uint16                    request;    
  uint16                    rri;
  uint16                    terminationTarget;
  uint16                    dataToPilot[4];
  uint16                    rriToPilot[4];
  uint16                    maxTxBackOff[4];

} hdrfw_rl_packet_t;

/*! \brief RTCMAC Send Packet Message.                          */
typedef struct {
  
  /*! \brief  Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief  Carrier Mask. Set Bit (c) to indicate rlPacketCfg[c] valid. */
  uint16                    carrierMask;
  hdrfw_rl_packet_t         rlPacketCfg[HDRFW_DEMOD_CARRIERS];
} hdrfw_rmac_send_packet_msg_t;

/*\}*/
  
  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR RTCMAC FW to SW Subframe Status Messages      */
  /*-----------------------------------------------------------------------*/
/**
    \defgroup rmacsubstatus RTCMAC Subframe Status Indication    */
/*\{*/

/*! \brief RTCMAC Subframe RL Packet Read Entry.                     */
typedef struct {

  uint16                    arqStatus;
  uint16                    status;
  uint16                    request;
  /*! \brief This is the eramIndex corresponding to a vacant packet location
   * for the next encode on the current carrier. */
  uint16                    eramIndex;

} hdrfw_rl_packet_rd_t;

/*! \brief RTCMAC Subframe TxAGC Read Entry.                         */
typedef struct {

  uint16                    pwrMargin;
  uint16                    pilotPower;
  uint32                    pilotPowerFilt;
  uint16                    pilotPowerPeak;

} hdrfw_txagc_rd_t;

/*! \brief RTCMAC Subframe MAC Read Entry.                           */
typedef struct {

  uint16                    drcLockUpdate;
  /*! \brief aspC[c].A[].Lock   */
  uint16                    lockFlags[HDRFW_NUM_ASP];
  uint16                    frabSoft[HDRFW_NUM_ASP];
  uint16                    qrabSoft[HDRFW_NUM_ASP];

} hdrfw_mac_rd_t;

/*! \brief Per-carrier RTCMAC Subframe Indication Entry.             */
typedef struct {
  hdrfw_rl_packet_rd_t      rlPacket;
  hdrfw_txagc_rd_t          txAgc;
  hdrfw_mac_rd_t            mac;
  /*! \brief handoffC[c].BestASP */
  uint16                    bestASP;
  uint16                    fingLockedFlag;
} hdrfw_rmac_subframe_ind_c_t;

/*! \brief RTCMAC Subframe Indication Shared Memory Read Interface.  */
typedef struct {

  hdrfw_rmac_subframe_ind_c_t rmacInfo[HDRFW_DEMOD_CARRIERS];

  uint16                      txTrafficToPilotPeak;
  uint16                      txTotalOverallMaxLimit;
  uint16                      txTotalOverallMaxLimitNeq;

} hdrfw_rmac_subframe_ind_interface_t;

///*! \brief RTCMAC Subframe Status Update Message from Firmware.      */
//typedef struct {
//  
//  /*! \brief  Message header */
//  msgr_hdr_struct_type      hdr;
//  uint16                    txPwrMarginMax;
//  uint16                    txLimitingMin;
//  uint16                    txLimitingMax;
//  uint16                    txTrafficToPilotPeak;
//} hdrfw_rmac_subframe_ind_msg_t;

/*\}*/

  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR FTCMAC SW to FW Config Messages               */
  /*-----------------------------------------------------------------------*/
/**
    \defgroup fmac  FTCMAC Configuration*/
/*\{*/

/*! \brief MDSP FTCMAC TxAGC Gains Config Entry.                 */
typedef struct {
  
  /*! \brief txAgcNewParamsAvailable trigger is removed. The following gains are latched
      internally in DSP half-slot processing after message processing.  */
  /** The ratio of DRC channel to pilot. Q2 signed in dB. Default 24, i.e 6dB. Range [-48dB, 24dB]*/
  uint16                    drcToPilot;
  /** The ratio of ACK channel to pilot. Q2 signed in dB. Default 22, i.e 5dB. Range [-48dB, 24dB]*/
  uint16                    ackToPilot;
  /** The ratio of Multiuser ACK channel to pilot. Q2 signed in dB. Default 22, i.e 5dB. Range [-48dB, 24dB]*/
  uint16                    mackToPilot;
  /** The ratio of DSC channel to pilot. Q2 signed in dB. Default -36, i.e -9dB. Range [-48dB, 24dB]*/
  uint16                    dscToPilot;
  /** The ratio of the power level of the DRC Channel to pilot when the DRC channel is boosted. Q2 signed in dB
      Default 24, i.e 6dB */
  uint16                    drcBoostToPilot;
  /** The ratio of the power level of the DSC Channel to pilot when the DSC channel is boosted. Q2 signed in dB
      Default -36, i.e -9dB */
  uint16                    dscBoostToPilot;
  uint16                    drcLength;
} hdrfw_fmac_gains_t;

/*! \brief MDSP FTCMAC Channel Config Message.                     */
typedef struct {
  
  /*! \brief  Message header */
  msgr_hdr_struct_type      hdr;
  uint16                    carrierMask;
  hdrfw_fmac_gains_t        fmacGains[HDRFW_DEMOD_CARRIERS];
  uint16                    dscLength;
  /** mod dscLength count value used to track the DSC boundary. 
      SW must compute dscHalfSlotCount. Specifically, the computation is:
      dscHalfSlotCount  = 2*((T + 1 + 15*frameOffset + trigger_delta)%dscLength)
      where T is the absolute slot counter based on system time. 
      Note this value is in half-slots.  trigger_delta: amount of time in slots
      between when SW computes dscHalfSlotCount and when it expects FW to latch
      it. Recommended value: 3 slots.  
      As a minimum requirement to properly synchronize the firmware-maintained
      count with system time, SW must compute this value when frameOffset
      and/or dscLength changes and when coming out of a sleep cycle.  For the
      purpose of synchronizing this update, SW must specify dscHalfSlotTrigger
      (see below) along with dscHalfSlotCount, before setting dscTimingRefresh.
      Note: SW should compute dscHalfSlotCount and dscHalfSlotTrigger in a
      non-interruptible state.
  */
  uint16                    dscHalfSlotCount;
  /** Trigger value in half-slots. 
   This register is used for synchronizing the update of dscHalfSlotCount. The
   mDSP keeps track of system time in a 32 half-slot period (period of PN
   sequence) in txHalfSlotIndex. SW computes this trigger based on system time
   T (in slots) modulo 16, and provides a trigger in half-slots
   dscHalfSlotTrigger = 2*((T + trigger_delta) % 16)
   Note: SW should compute dscHalfSlotCount and dscHalfSlotTrigger in a
   non-interruptible state.
   */
  uint16                    dscHalfSlotTrigger;
  uint16                    frameOffset;
  uint16                    drcGating;
} hdrfw_ftcmac_channel_cfg_msg_t;

/*! \brief FMAC GAUP attribute configuration message.  */
typedef struct {
  
  /*! \brief  Message header */
  msgr_hdr_struct_type      hdr;
  uint16                    hdrShortPacketMode;
  uint16                    preamMUPEnable; 
  uint16                    mAckToPilot[HDRFW_DEMOD_CARRIERS];
  uint16                    drcBoostToPilot[HDRFW_DEMOD_CARRIERS];
  uint16                    dscBoostToPilot[HDRFW_DEMOD_CARRIERS];
  uint16                    drcBoostLength;     
  uint16                    dscBoostLength; 
  uint16                    nFTCMDRCSupervision;
} hdrfw_fmac_gaup_cfg_msg_t;

/*! \brief Set maxDRC */
typedef struct {
  /*! \brief  Message header */
  msgr_hdr_struct_type      hdr;
  /** The DSP attempts to decode the packet after each slot if hdrShortPacketMode is set. */
  uint16                    hdrShortPacketMode;
  uint16                    maxDRC; 
} hdrfw_max_drc_cfg_msg_t;

/*! \brief MDSP FTCMAC Activate Message.              */

/*! \brief For internal firmware use */
typedef struct {
  /** DRC supervision reset timer. */
  uint16                    nFTCMPRestartTx; // c
  /** Timeout for the DRC supervision timer, in slots. Default=144 slots. */
  uint16                    nFTCMDRCSupervision; // c
  uint16                    SoftHandoffDelayTimer; // c
  uint16                    SofterHandoffDelayTimer; // c
  /** Maximum number of credits allowed by the handoff algorithm. Default 128.
      Note: it must be greater than or equal to:
      max(aspnSoftHandoffDelay,aspnSofterHandoffDelay) */
  uint16                    MaxCredit; // c
  uint16                    SoftHandoffCreditThresh; // c
  uint16                    SofterHandoffCreditThresh; // c
  uint16                    drcLockBackOffLowSinr; // c
  uint16                    drcLockBackOffNormalSinr; // c
  uint16                    hdrShortPacketMode;
  uint16                    minContinuationSpan;
  uint16                    disableMUPDecodeFilter;
  uint16                    maxDRCRL; // base
  uint16                    drc_translation_table[HDRFW_NUM_DRC];
  uint16                    drcBoostLength;
  uint16                    dscBoostLength;
  uint16                    drcLockPeriod; // rl0
  uint16                    drcLockLength; // c
} hdrfw_fmac_activate_intf_params_t;

typedef struct {
  
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /** Commmon fields used in all subtypes. */
  /** System mode.
   \li 0: Release 0
   \li 1: Release A
   \li 2: Release B
   */
  uint16                    hdrSystemModeConfig; // c
  union {
    // MUST MATCH hdrfw_fmac_handoff_params_t
    struct {
      /** DRC supervision reset timer. */
      uint16                    nFTCMPRestartTx; // c
      /** Timeout for the DRC supervision timer, in slots. Default=144 slots. */
      uint16                    nFTCMDRCSupervision; // c
      uint16                    softHandoffDelayTimer; // c
      uint16                    softerHandoffDelayTimer; // c
      /** Maximum number of credits allowed by the handoff algorithm. Default 128.
          Note: it must be greater than or equal to:
          max(aspnSoftHandoffDelay,aspnSofterHandoffDelay) */
      uint16                    maxCredit; // c
      uint16                    softHandoffCreditThresh; // c
      uint16                    softerHandoffCreditThresh; // c
      uint16                    drcLockBackOffLowSinr; // c
      uint16                    drcLockBackOffNormalSinr; // c
      uint16                    hdrShortPacketMode;
      uint16                    minContinuationSpan;
      uint16                    disableMUPDecodeFilter;
      uint16                    maxDRC; // base
      uint16                    drcTranslation[HDRFW_NUM_DRC];
      uint16                    drcBoostLength;
      uint16                    dscBoostLength;
      uint16                    drcLockPeriod; // rl0
      /** The number of times a DRCLock bit is repeated. Must be a power of 2. */ 
      uint16                    drcLockLength; // c
    };
    hdrfw_fmac_activate_intf_params_t intf_params;
  };

  uint16                    enableRCL; // base
  /** Fields used in Rel A/B. */
  uint16                    preamMUPEnable;
  uint16                    maxOptionalDataRate;
  uint16                    maxOptionalPayload;
  /** Fields used in Rel 0.   */
  uint16                    nullRateDRC384; // rl0
} hdrfw_fmac_activate_msg_t;

/** FMAC ACK Mode Configuration Message. This is a per carrier message, set carrerIdx
    to specify the carrier to configure. */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief carrier to configure - c          */
  uint16                    carrierIdx:3;
  /** DSP attempts decode only on the first slot of a packet, and discards the rest. */
  uint16                    singleSlotDecode:1;
  /** The DSP only attempts a decode on the last slot of a packet. */
  uint16                    disableEarlyDecode:1;

  /*! \brief txC[c].AckOverride                */
  uint16                    ackOverride;
  /*! \brief txC[c].AckToPilot                 */
  uint16                    ackToPilot; 

} hdrfw_fmac_ack_mode_cfg_msg_t;

/** Handoff configuration done in FMAC module. */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  uint16                    maxCredit; // c
  uint16                    softerHandoffCreditThresh; // c
} hdrfw_fmac_handoff_cfg_msg_t;

/** Disable/Enable MUP decode filter.          */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /** Multi-user packets not directed at this AT are normally cancelled. D=1
   disables this filtering operation; all multi-user packets will be written to
   the output buffer. */
  uint16                    disableMUPDecodeFilter;
} hdrfw_mup_decode_filt_cfg_msg_t;

/*! \brief DRC Translation Table update for GAUP */
typedef struct {
  
  /*! \brief Message header */
  msgr_hdr_struct_type       hdr;
  uint16                     drcTranslation[HDRFW_NUM_DRC];

} hdrfw_drc_translation_cfg_msg_t;


/*! \brief Partial Loading Configuration */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  /*! \brief Enable RCL step-size and ceiling adjustment partial loading enhancement. */
  uint16                    enablePartialLoadRCL;

} hdrfw_partial_ld_rcl_cfg_msg_t;

/*\}*/

/**
    \defgroup ccmaccfg CCMAC Configuration */
/*\{*/
/** Control Channel Demod Config. */
typedef struct {
  /*! \brief  Message header */
  msgr_hdr_struct_type      hdr;
  uint16                    hdrSystemModeConfig_Valid:1;
  uint16                    hdrSystemModeConfig:2;
  uint16                    hdrShortPacketMode_Valid:1;
  uint16                    hdrShortPacketMode:1;
  /** When non-zero, DSP will detect preamble for control channel 3 packets
    with the specified MAC index. This should not be set in release 0 mode, and
    should not be set in connection state.  When enabled, recommended value is
    71. */
  uint16                    ctrl3MACIndex;
  /** When non-zero, DSP will detect preamble for control channel 4 packets
     with the specified MAC index. This should be set only if a user-specific
     control channel is assigned. */
  uint16                    ctrl4MACIndex;
} hdrfw_cc_demod_cfg_msg_t;

/** SCC Control Message.          */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  uint32                    sccOffset_Valid:1;
  /** SCC Offset. Synchronous control channel offset in slots (0-3). Set to 0xFF to indicate
      all 4 possible offsets to be checked. */
  uint32                    sccOffset:8;
  uint32                    ccStart_Valid:1;
  uint32                    ccStart:8;
  /*! \brief SCC Preamble Indication Enable. Duplicated here. \sa hdrfw_indication_cfg_msg_t */
  uint32                    enableSccPreamInd_Valid:1;
  uint32                    enableSccPreamInd:1;
  
  /*! \brief SCC decode indication. Enables sending an indication if SCC is not decoded by slot M. 
      M = sccDecodeIndThreshold. Set the _Valid bit to indicate that firmware should latch the
      threshold and enable specified below.*/
  uint32                    enableSccDecodeInd_Valid:1;


  /*! \brief L1 must enable this explicitly on standby->active transition. Firmware disables this as 
      part of active->standby transition. To stop receiving indications within a wake up, L1 must
      send this message to explicilty disable the feature. Firmware also disables this feature when
      the indication is actually sent. To arm this again, L1 needs to re-enable the feature with this
      message. */
  uint32                    enableSccDecodeInd:1;
      
  /*! Valid threshold is an integer ranging from 0 to 14. 
      15 is the last slot of the longest packet. If packet is not decoded at slot 15, 
      no indication is sent if the threshold is set to 15. */
  uint32                    sccDecodeIndThreshold:4;

} hdrfw_scc_msg_t;

typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
 
  /** SCC Preamble Detected Offset. 
      detectSccOffset = -1 indicates a missed control channel packet. 
      If sccOffset (configured with hdrfw_scc_msg_t) is unknown (-1),
      this indication is sent if a cc preamble is detected, with detectedSccOffset
      set to the offset at which the preamble is detected.
      If sccOffset is known and a cc preamble is detected, this indication is not
      sent from firmware. */
  uint16                    detectedSccOffset;

} hdrfw_scc_preamble_ind_msg_t;

/*! Indication that SCC packet is not decoded until the threshold slot index. \sa 
     hdrfw_scc_msg_t */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

} hdrfw_scc_decode_ind_msg_t;


#define HDRFW_FLEX_CONN_DSP_DSC_REPOINT 1
#define HDRFW_FLEX_CONN_DSP_DRC_REPOINT 0
/*! Indication that flexconn is disabled due to DSC/DRC repointing */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  /*! \brief Status for reason for handoff, BIT(0)->DRC repointing
  BIT(1) indicates DSC repointing */
  int32                     flexConnRepoint; 
} hdrfw_tx_flexconn_ind_msg_t;


/*\}*/

/**
    \defgroup srchd Search Dump Indication
    Per-carrier HDR update information  attached to the Search Dump Message. */
/*\{*/
typedef struct {
  uint32                    status;
  uint32                    rtcOfs;
  /*! \brief fingC[c].F[f].A[].OnTime */
  uint32                    onTime[HDRFW_NUM_ANTENNAS];
  /*! \brief fingC[c].F[f].A[].RSSIFilt */
  uint32                    antRssiFilt[HDRFW_NUM_ANTENNAS];
  /*! \brief fingC[c].F[f].PathRSSIFilt */
  uint32                    pathRSSIFilt;
} hdrfw_srch_dump_fing_t;

typedef struct {
  uint32                    CI;
  uint32                    CIAvr;
  uint32                    CIFilt;
} hdrfw_eqci_t;
typedef struct {
  
/*! Bit 0: EQ enable or not.
       Bit 19: if set, channel is flat.
       Bit 20: if set, channel is single-tap.
       Bit 21: if set, DSA choose 4MMSE
       Bit 22: if set, gset ASP1 detects FAB
       Bit 23: if set, gset ASP1 detects FAB
       Bit 24: if set, channel is dispersive
       Bit 31: EQ selected.
       */
  uint32                    status;

  uint32                    demodDecision;
  hdrfw_eqci_t              CIInfo[HDRFW_NUM_EQ_PASSESS];
} hdrfw_srch_dump_eq_t;

typedef struct {

  /*! \brief  Finger Search Dump Info */
  hdrfw_srch_dump_fing_t    fing[HDRFW_NUM_FINGERS];
  /*! \brief  EQ Search Dump Info */
  hdrfw_srch_dump_eq_t      eq;

} hdrfw_srch_dump_c_t;

/** Search Dump Indication Shared Memory Read Interface. */
typedef struct {

  hdrfw_srch_dump_c_t       srchDumpInfo[HDRFW_DEMOD_CARRIERS];

} hdrfw_srch_dump_ind_interface_t;

/*\}*/

/**
    \defgroup tuneaway Tune-Away      */ 
/*\{*/

typedef union {
  struct {
  /** (S) When S=0, predictor will use input from both antennas. When S=1,
      predictor will use input only from the primary antenna (as set in
      diversityConfig \sa hdrfw_diversity_cfg_msg_t). */
    uint32    singleAntenna:1;
    uint32    :14;
    /** (M) When M=0, DSP will transmit NULL cover by the tuneaway time. When M=1,
        NULL cover will not be transmitted, tuneAwayTime from the message is also
        ignored. */
    uint32    noNullCover:1;
    uint32               :16;
  };
  uint32 intval;

} hdrfw_tuneaway_mode_field_t;

/*! \brief Tune Away Start Message */
typedef struct {
  
  /*! \brief Message header */
  msgr_hdr_struct_type            hdr;
  /*! \brief See hdrfw_tuneaway_mode_field_t for bit definition. */
  uint16                          tuneAwayMode;
  /** The HSTR index when the software wants to tune away. The DSP will not request
      any packets that take longer than the remaining active time.
      This index must be at least (64 slots + 2*DRClength) later than the current
      HSTR count, to avoid missing any received data. */
  uint16                          tuneAwayTime;
} hdrfw_tuneaway_start_msg_t;

/** Tune Away Stop Message. 
  This is equivalent to software clearing the legacy tuneAwayActive interface variable. */
typedef struct {
  
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
} hdrfw_tuneaway_stop_msg_t;

/*\}*/

/**
    \defgroup eqconfig Equalizer Configuration */ 
/*\{*/
/** Equalizer configuration message.
    Response: hdrfw_srch_eq_config_rsp_msg_t. */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief No carrier mask is provided here. eqConfig[c] = 0 indicates that carrier c
      is not configured in this message. 
      \li 0x8000: disable equalizer. On NikeL, this disable grice. 
      \li 0x8001: enable equalizer. On NikeL, this enable grice. 
      \li 0     : no configuration */
  uint16                    eqConfig[HDRFW_DEMOD_CARRIERS];
  int16 aci_comp_config;
  int16 reg_scale;
} hdrfw_eq_cfg_msg_t;

/*! \brief Equalizer Override Message.                */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  uint16                    eqOverride;
  /** Debug field, leave as 0 in normal operation */
  uint16                    disableInterp;
} hdrfw_eq_override_msg_t;    


/*\}*/

/**
    \defgroup afcconfig AFC Configuration      */ 
/*\{*/

/**   Firmware begins FLL acquisition mode on the next half-slot, set
      fllAcqMode. The starting values of fllVCOAccum and fllRotAccum are loaded
      from fllVCOAccumInit and rotAccumInit[].
      Physical Carrier to run acquisition on always comes from
      aspC[0].A[handoffC[0].BestASP].Carrier

      Repsonse: hdrfw_srch_fll_acq_rsp_msg_t.
*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
} hdrfw_fll_start_acq_msg_t;

// Starting bit position in hdrfw_fllaccum_load_field_t
// for indicating FLL dynamic state needs to be resotred
#define HDRFW_FLL_RESTORE_DYN_STATE_CARRIER_MASK_BIT_POS   12

typedef union {
  struct {
    /** Bit(c) : rotAccumInit[c] is loaded into fllC[c].RotAccum (c=0,1,2).  */
    uint32 carrierMask:3;
    uint32 :9;
    /** If set, resotre fllDynXXXX variables into carriers indicated in carrierMask.*/
    uint32 restoreDynStateCarrierMask:3; 
    /** fllVCOAccumInit is latched into fllVCOAccum. */
    uint32 init:1;
    uint32 :16;
  };
  uint32 intval;
} hdrfw_fllaccum_load_field_t;

/** FLL Accumulator Load Message. 
    Response: hdrfw_srch_fll_accum_ld_rsp_msg_t.
*/
typedef struct {
  
  /*! \brief Message header */
  msgr_hdr_struct_type         hdr;
  /** fllVCOAccum, fllRotAccum initialization control */
  hdrfw_fllaccum_load_field_t  fllAccumLoad;
  uint32                       fllVCOAccumInit;
  /** fllRotC[Rc].RotAccumInit is loaded into fllRotC[Rc].RotAccum. See fllAccumLoad settings.*/
  uint32                       rotAccumInit[HDRFW_TOTAL_CARRIERS];

  /** If bit C of restoreDynStateCarrierMask in fllAccumLoad is set,
   *  dynamic state is restored into FW for specified carriers. See
   *  fllAccumLoad settings */
  uint16 	               fllDynCPDMode[HDRFW_TOTAL_CARRIERS]; 
  uint32                       fllDynFreqAccum[HDRFW_TOTAL_CARRIERS]; 

  /** ACQ to TRK timer duration in unit of half slots. The timer is used whenever ACQ to TRK 
    * transition happens. */
  uint16                    fllAcq2TrkTimerLenHS; 
} hdrfw_fll_accum_load_msg_t;

/** DLL Configuration Message.   */
typedef struct  {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /** The gain of the first-order time tracking loop accumulator. Default=256. */
  uint16                    dllAlpha1;                        
  /** Minimum limit of a time tracking accumulator adjustment. Default=-2048.
      Must be negative. */
  uint16                    dllDriftMax;                        
  /** Maximum limit of a time tracking accumulator adjustment. Default=2048.
      Must be positive. */
  uint16                    dllDriftMin;       
} hdrfw_dll_cfg_msg_t;

/** FLL Gain Configuration Message. */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /** Frequency tracking loop gain for acquisition mode. Recommended values are
      160 for PCS band and 320 for cell band. Default=160.
      If rotator is enabled, recommended value is 160  */
  uint16                    fllAcqGain;                       
  /** Frequency tracking loop gain for tracking mode. Recommended values are 16
      for PCS band and 32 for cell band. Default=16. 
      If rotator is enabled, recommended value is 16. */
  uint16                    fllTrkGain;        
} hdrfw_fll_gain_cfg_msg_t;

typedef struct {
  /** Limits the PDM adjustment per half-slot. 32-bit signed value;
     default=0x7FFFFFFF, i.e. no slew rate limiting
     Set to 0x7FFFFFFF to disable slew rate limiting.
     Set to 0 to disable PDM adjustment in RVAFC (i.e. rotator-only FLL). */
  int32                    fllSlewRateLimit;
  /** Frequency error below which slew rate limiting is applied. Default = 0 */
  int32                    fllSlewThreshLow;
  /** Frequency error above which slew rate limiting is applied. Default = 0 */
  int32                    fllSlewThreshHigh;
  /** Activate slew rate limiting. */
  uint16                    fllSlewLimitFlag;
} hdrfw_fll_slew_config_intf_params_t;
/** FLL Slew Configuration Message. */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  union {
    struct {
      /** Limits the PDM adjustment per half-slot. 32-bit signed value;
         default=0x7FFFFFFF, i.e. no slew rate limiting
         Set to 0x7FFFFFFF to disable slew rate limiting.
         Set to 0 to disable PDM adjustment in RVAFC (i.e. rotator-only FLL). */
      int32                    fllSlewRateLimit;
      /** Frequency error below which slew rate limiting is applied. Default = 0 */
      int32                    fllSlewThreshLow;
      /** Frequency error above which slew rate limiting is applied. Default = 0 */
      int32                    fllSlewThreshHigh;
      /** Activate slew rate limiting. */
      uint16                    fllSlewLimitFlag;
    };
    hdrfw_fll_slew_config_intf_params_t intf_params;
  };
} hdrfw_fll_slew_cfg_msg_t;

/** PLL Configuration Message. */
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  uint16                    pllEnable;  
} hdrfw_pll_cfg_msg_t;

/** FLL Rotator Configuration Message. */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /** Enable 2nd order Rotator-VCO control loop. */
  uint16                    fllRVAfcEnable;
  /** Gain of the (outer) VCO loop. Default: 2048. */
  uint16                    fllRVBetaOL;
} hdrfw_fll_rot_cfg_msg_t;

/** FLL XO Configuration Message. */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief  This is obosolete for NikeL. 
   *Enable/Disable FW writes to XO MND Counter 
   * 1 => enable
   * 0 => disable
   */
  uint16                    fllMndWrEnable;
  /*! \brief This is obsolete for NikeL.  
   *Conversion factor from Hz/bit to PPM. Q12 signed.
   * 
   *  freqGain = (((1.5625*1.2288e7)/Rx_center_freq_MHz)+1)>>1;
   *  (from Systems SDD)
   *
   */
  uint16                    fllRxFreqGain;
  /** Ratio of TX to RX carrier frequency to translate accumulated
   * RX frequency error to TX frequency. Q14 signed.
   *
   * ratio = Tx_freq/Rx_freq * 2^14
   */
  uint16                    fllTxRxFreqRatio;

   /** 
   *  2^33/rx_frequency(Hz) in Q19  
   */
  uint32                    fllInvRxFreq;

  /** rx_frequency in kHz. This can be used for converting PLL from ppm unit to error in Hz.
      Note that this is specified in kHz while the inverse version is in 1/Hz.*/
  uint32                    fllRxFreq;

} hdrfw_fll_xo_cfg_msg_t;


/** FLL Rotator Override Message. */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  /** Bit i, if set as 1, overrides rotAccum with rotatorForce[i]. */
  uint32 carrierMask;

  /** 
   *  override values for rotAccum. This is not in the hardware unit. 
   *  This is in the same unit as rotAccumInit in hdrfw_fll_accum_load_msg_t
   *  (1228800 * 2^(-34) Hz/LSB). 
   */
  uint32                    rotatorForce[HDRFW_TOTAL_CARRIERS];
} hdrfw_fll_rot_override_msg_t;

/** FLL PDM Override Message. */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /** PDM output */
  uint16                    fllPDMForce;
  /** Set enable=1 to force the PDM output, 0 to use the value calculated by the FLL.
     When enable=1, the DSP writes fllPDMForce to the PDM instead of the calculated
     FLL output. 
     This value must be set before disabling the FLL. */
  uint16                    enable;
} hdrfw_fll_pdm_override_msg_t;

/** FLL Enable Message. */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /** (E) E=1 (default): Frequency tracking loop is enabled.
      E=0: Frequency tracking loop is disabled. PDM output will be written if override is set. */
  uint16                    fllEnable;
} hdrfw_fll_enable_msg_t;

/*\}*/

/**
    \defgroup drcconfig DRC Configuration */
/*\{*/
typedef struct {
  
  uint32                    drcOverride_Valid:1;
  /*! \brief predC[c].Override                 */
  uint32                    drcOverride:1;
  /*! \brief predC[c].DictDRC                  */
  uint32                    dictDRC:8;
  uint32                    enableDRC_Valid:1;
  /*! \brief txC[c].EnableDrc                  */
  uint32                    enableDRC:1;

} hdrfw_drc_cfg_t;

/** DRC Configuration Message. */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  hdrfw_drc_cfg_t           drcCfg[HDRFW_DEMOD_CARRIERS];
} hdrfw_drc_cfg_msg_t;

/** DRC Filter Reset Message.  
    Response: hdrfw_srch_reset_drc_filt_rsp_msg_t.
*/
typedef struct {
  
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /** Carrier Valid Mask. Software sets bit c to indicate values in drcFiltResetValue[c]
      are valid for configuration.                                       */
  uint16                         carrierMask;
  uint16                         drcFiltResetValue[HDRFW_DEMOD_CARRIERS];

} hdrfw_drc_filt_reset_msg_t;

/*\}*/

/**
    \defgroup sleepconfig Sleep Configuration      */ 
/*\{*/

/** MPC Configuration Message. */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /** RTC in Cx8, 18bits */
  uint32                    mpcRtcCnt;
  /** RX RTC compare count in Cx2 */
  uint16                    mpcTxCompareCnt;
  /** TX RTC compare count in Cx2 */
  uint16                    mpcRxCompareCnt;
  uint16                    hstrCount;
} hdrfw_mpc_cfg_msg_t;

/** Sleep Adjust Message - Updated for Dime
 
    On Dime this message must be sent after the following events

    \li HDR RTC out of power collapse and running
    \li Firmware in Active state
    \li Online pulse programmed with target RTC
    
    Software also need to guarantee that this message is received well ahead
    of the online pulse.

    Upon receiving this message. Firmware will adjust the HDR RTC to 1 half-slot
    beyond the intended wake up time and then program the compare count. This
    guarantees that no bogus interrupt can be generated.

    At the online puse, the onlineRtcCx8 will be loaded and firmware will wake up
    at onlineRtcCx8 + 624(chips) and set HSTR count according to the cdma system
    time provided.
 
 */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /** Nike-L - Sleep adjustment, in slots. When this value is non-zero, the DSP will add
      it to its half-slot count and real-time counter, then clear sleepAdjust. 
      This value should be set, after Cx16 has been re-enabled, to the number of
      slots Cx16 was disabled during sleep. */
  uint16                    sleepAdjust __attribute__((deprecated)); 

  /*! \Absolute system time at the online pulse. Must be aligned to slot boundary*/
  uint64                    onlineCdmaSystemTime;
  
  /*! \Corresponding Cx8 HDR RTC count to be loaded at the online pulse.*/
  uint32                    onlineRtcCx8;

  /*! \Corresponding USTMR value programmed for online puse*/
  uint32                    onlineUstmr;

} hdrfw_sleep_adj_msg_t;

typedef struct {

  /** Packet Type:
      Set to indicate the type of packet. Constants are exported from DSP: 
      \li 1: 38.4 control channel
      \li 2: 76.8 control channel
      \li 4: sub-synchronous control channel
      \li 5: user-assigned control channel
      Note: Packet Type definition is different from DORA/SC2x firmware! */
  uint16                    forcePreamType;
  /** HSTR count corresponding to the start of the packet. */
  uint16                    forcePreamHSTR;
  /** Set this flag to enable/disable FPD 
   * 1 -> enable
   * 0 -> disable
   */
  uint16 forcePreamDetect;
} hdrfw_fpd_params_t; 

/** Force Preamble Detection Message.         */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  union {
    struct {
      /** Packet Type:
          Set to indicate the type of packet. Constants are exported from DSP: 
          \li 1: 38.4 control channel
          \li 2: 76.8 control channel
          \li 4: sub-synchronous control channel
          \li 5: user-assigned control channel
          Note: Packet Type definition is different from DORA/SC2x firmware! */
      uint16                    forcePreamType;
      /** HSTR count corresponding to the start of the packet. */
      uint16                    forcePreamHSTR;
      /** Set this flag to enable/disable FPD 
       * 1 -> enable
       * 0 -> disable
       */
      uint16 forcePreamDetect;
    }; 
    hdrfw_fpd_params_t fpdParams;
  }; 
  //XXX - set in FW uint32                    forcePreamResolveThresh;
} hdrfw_fpd_msg_t;

/** QPCH Preamble Detection Message.         */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /** HSTR count corresponding to the QPCH packet. */
  uint16                    qForcePreamHSTR;
  /** This parameter can be used to indicate QFPD (quick force
      preambe detection) flag is enabled or not (0 disables QFPD
      and non-zero value enables QFPD) */
  int16                     qForcePreamDetect;
  /** This parameter can be used to selectively enable (set to 71) or disable
      (set to 0) QPCH preamble detection. */
  uint32                    qpchMACIndex;
} hdrfw_qpch_msg_t;

/*\}*/

/**
    \defgroup acqt Acquisition and Timing */ 
/*\{*/

/** Set hstrOffset.                    */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  uint16                    hstrOffset;
} hdrfw_hstr_offset_cfg_msg_t;

/** Set mstrSlam. 
    Synchronize MSTR with earliest locked finger. To force MSTR to a particular
    finger, that finger must be the only one active.  After synchronization,
    the DSP sends software a hdrfw_srch_slam_rsp_msg_t message. If there are no
    suitable fingers active, no response is sent until such condition is met 
    and the slam is successful. In other words, receiving a slam response message
    is equivalent to polling the original mstrSlam variable until it is cleared
    by firmware.

    Response: hdrfw_srch_slam_rsp_msg_t.
 */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief For fast ACQ */
  uint16                    setMstr;
  uint16                    hstr;
  uint32                    mstrCx8;
  /*! \Absolute system time */
  uint64                    capturedCdmaSystemTime;
  /*! \Corresponding RTC */
  uint32                    capturedRtcCx8;
} hdrfw_mstr_slam_msg_t;

/** Searcher shutdown message.         */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  
  /*! \brief rxfA[].AntennaMode */
  uint16                    antennaMode[HDRFW_NUM_ANTENNAS];

} hdrfw_srch_shutdown_msg_t __attribute__((deprecated));

/** Set hyperspaceConfig.              */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /** Set to non-zero when system has been acquired */
  uint16                    hyperspaceConfig;
} hdrfw_hyperspace_cfg_msg_t;

/*\}*/

/**
    \defgroup decob Decode and Decoder Output Buffer */
/*\{*/

/** Decoder Output Buffer Status Update. Rate of this message is controled by 
    hdrfw_decob_ind_rate_msg_t. */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  /** Packet Pending Bitmask. 
      Bitmask indicating which packets are available in the decoder output
      buffer. Bit[n] corresponds to a packet at block index "n" */
  uint32                    decobPending;
} hdrfw_decob_status_ind_msg_t;

/** Software Decob Packet Read Update */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  /** Packet Done Bitmask.
      Software sets bits in this variable after the corresponding packets have been
      read from the decoder output buffer. DSP clears the corresponding bits in
      internal decobPending bitmask.
      Software can set all of the bits to clear everything pending.*/
  uint32                    decobClear;

} hdrfw_decob_update_msg_t;

/** Decode Control Message.
    Response (only if cancelActivePackets bit set): hdrfw_cancel_active_packet_rsp_msg_t. */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  uint32                    singleSlotDecode_Valid:1;  
  /** (S) S=0: The DSP attempts decode up to the full packet length.
       S=1: The DSP attempts decode only on the first slot of a packet, and discards the rest. */
  uint32                    singleSlotDecode:1;
  uint32                    disableEarlyDecode_Valid:1;
  uint32                    disableEarlyDecode:1;
  /** If cancelActivePackets is set, firmware sents a response to software when 
   decoder is no longer active. \sa hdrfw_cancel_active_packet_rsp_msg_t */
  uint32                    cancelActivePackets:1;

  /** Reserve and configure demback in EVDO mode immediately */
  uint32                    initDemback:1;

  /** Release demback mode after cancelActivePackets response is sent. Only valid if
      cancelActivePackets flag is set in the same message*/
  uint32                    releaseDemback:1;

} hdrfw_decode_ctl_msg_t;  

/** Decoder Output Buffer Indication Rate Config Message. */
typedef struct {
  
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /** The DSP will not send indication messages to the software faster than once every Rate slots. 
      When set to 1, DSP will generate a message immediately after every packet. Default is 4 slots. 
      Values larger than 4 are not recommended for multiple carriers, as it may result in dropped packets.
      Value is stored as 2*(n)-1, where n is the period in slots. Hence the default value is 2*4-1 = 7 */
  uint16                    decobIndMsgRate;
} hdrfw_decob_ind_rate_msg_t;

/** Set minPreambleThresh */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  uint16                    minPreambleThresh;    
} hdrfw_min_pream_thresh_cfg_msg_t;

/** Reset decode sequence number */
typedef struct {
  
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
 
} hdrfw_reset_seq_num_msg_t;

/*\}*/
/**
    \defgroup bcc Broadcast */
/*\{*/

/*! \brief Gold Broadcast */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  /** Update this interface to take advantage of messaging?.
      bccDetect contains 32 items per carrier, corresponding to 32 slots. bccDetect[c][0] 
      is synchronized with hstrCount=0. The ARM must update this buffer at least once 
      every 16 slots, based on the DSP's current hstrCount: at slot time S=(hstrCount/2), 
      the ARM shall write data corresponding to slots (S+8) through (S+31) to 
      bccDetect[c][(S+8) % 32)]...bccDetect[c][(S+31) % 32]. 
      The DSP clears bccDetect[c][S], including CombineMask, after processing slot S. 
      The ARM may disable BCC processing by writing all bccDetect locations as zero.
      */
  uint16                    carrierIdx;
  uint16                    bccDetect[32];

} hdrfw_bcc_detect_msg_t;


/*\}*/

/**
    \defgroup drcsind Drc Supervision Indications */
/*\{*/

/** DRC Supervision. Indicates DrcSupervisionTimeOut reaches 0 */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /** Timeout Mask. Bit c is set if predC[c].drcSupervsionTimeOut <= 0 */
  uint16                    drcSupervsionTimeOutMask;

} hdrfw_drc_supervision_timeout_ind_msg_t;

/** DRC Supervision Restart Tx. Indicates DrcSupervisionTimeOut reset */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  
  /** Restart Tx Mask. Bit c is set if predC[c].drcSupervsionTimeOut is 
      restored to nFTCMPRestartTx when non null threshold is reached. */
  uint16                    drcSupervsionRestartTxMask;

} hdrfw_drc_supervision_restart_ind_msg_t;

/*\}*/

/**
    \defgroup fc Flow Control */
/*\{*/

typedef struct {

  uint16                     DRCNullPatternLen;
  uint16                     DRCNullPattern;
  uint16                     BlkSizeInDRCLenM1;

} hdrfw_flow_control_t;

/** Flow Control Config Message */
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type       hdr;
  uint16                     carrierMask;
  hdrfw_flow_control_t       fcCfg[HDRFW_DEMOD_CARRIERS];

} hdrfw_flow_control_cfg_msg_t;

/** Legacy FLow Control Config Message */
typedef struct {
  
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  
  /*! \brief legacy flow control flag          */
  uint16 flowControlOn;

} hdrfw_legacy_flow_control_msg_t;


/*\}*/

/**
    \defgroup timer General Purpose Timers  */
/*\{*/

/*! \brief Timer Config */
typedef struct {
  
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  
  /** Timer index. Index of the timer to configure < HDRFW_NUM_TIMERS */
  uint16                    sysTimerIdx;
  /** Terminal count, in half-slots, when the timer should expire. This value 
      is compared with hstrCount */
  uint16                    sysTimerCount;              

} hdrfw_timers_cfg_msg_t;

/*! \brief Timer Expiration Indication */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief Expiration Mask. BIT(i) set indicates timer i has expired. */
  uint16                    sysTimerExpiredMask;
} hdrfw_timers_ind_msg_t;

/*\}*/

/**
    \defgroup diversity Diversity Configuration  */
/*\{*/

typedef struct {
  /** 0: Antenna 0 is primary, 1: Antenna 1 is primary. */
  uint16                    antennaSelect:1;
  uint16                    :14;
  /** 0: Non-diversity, use the antenna specified in antennaSelect. 
      1: Diversity, antennaSelect determines the primary antenna. */
  uint16                    diversity:1;
} hdrfw_diversity_cfg_field_t;

/** Set Diversity Mode. */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type        hdr;
  /*! \brief See hdrfw_diversity_cfg_field_t for bit definition. */
  uint16                       diversityConfig;
} hdrfw_diversity_cfg_msg_t;

typedef struct {
  
  /** Selects the diversity combining mode for all fingers. Default=0x0000.
      Mode: 
      \li 0: Combine mode will be selected automatically based on finger RSSI.
      \li 1: Force selection combining. Antenna selection determined by Select field.
      \li 2: Force MMSE combining.
      \li 4: Force MRC combining. */
  uint16                    combineMode:3;
  uint16                    :5;
  /** Select: Choose antenna when in selection combining mode, both automatic and forced. Ignored in other modes.
      \li 0: Select the antenna with the best RSSI.
      \li 2: Select antenna 0 always.
      \li 3: Select antenna 1 always. */
  uint16                    selectionCombine:2;

} hdrfw_force_comb_mode_field_t;
/** Set Force Combine Mode. */
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type          hdr;
  /*! \brief See hdrfw_force_comb_mode_field_t for bit definition */
  uint16                        forceCombineMode;
} hdrfw_force_comb_mode_msg_t;

/*\}*/
/**
    \defgroup misc Miscellaneous */      
/*\{*/

/** HDR State Defintion. */
typedef enum {
  /*! Initial state before HDR is requested by FWS */
  HDRFW_STATE_INACTIVE = 0,
  
  /*! Initial state after HDR is requested by FWS. Half-slot process not active. */
  HDRFW_STATE_STANDBY,
  
  /*! Traffic state for HDR firmware. Half-slot process active */
  HDRFW_STATE_ACTIVE,

  /*! Deprecated - DO NOT USE */
  HDRFW_STATE_MEAS,
  
  /*! Pilot measurement state for both Idle and Connected states */
  HDRFW_STATE_MEAS_CONNECTED,

  /*! Supported only Dime .*/
  HDRFW_STATE_SLEEP,
  
  /*! Used during Hybrid tuneaway. No real difference between Timing and Active  
      except modulator reset when transition back to Active from Timing. */
  HDRFW_STATE_TIMING
} hdrfw_state_t;


/** HDR State Config message. Firmware sents a response
 * (hdrfw_state_cfg_rsp_msg_t) after processing the message if HDR App is
 * enabled through the message. */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief Set new state of HDRFW. */
  hdrfw_state_t             state;

  /*! \brief This flag is only valid for standby to active transition message. 
       If true, FW should clear RX/TX compare count when
       going into ACTIVE state. This flag is set as true when in previous sleep,
       L1 put FW into standby instead of using FW sleep state. */
  boolean                   sleepWakeUpOverride;                   
} hdrfw_state_cfg_msg_t; 

/** DSDX Priority Config Parameters.
 *  This is the message definition for the HDR DSDX priority Configuration
 *  Command.  */
typedef struct {
  /*!< MSGR header */
  msgr_hdr_struct_type hdr;
  /*!< Priority of activity. Higher value indicates higher priority.There is no
       concept of Macro or Micro priority in FW. L1 resolves Macro/Micro priority
       and passes one single priority value to Tech FW.*/
  /*!< rxPriority and txPriority are respectively used by L1 to set the Rx and Tx
       priority. A value of 0xFFFFFFFF indicate priority not valid.*/
  uint32 rxPriority;
  uint32 txPriority;
  /*!< Priority for ASDIV, sent by L1. FW uses same priority to register Rx and TX
       activity. FW registers for both Rx and Tx everytime the switch needs to be done
       except, when TX is off. */
  uint32 asdivPriority;
} hdrfw_dsdx_priority_config_msg_t;

/** DSDA frequency Config Parameters.
 *  This is the message definition for the HDR DSDX frequency Configuration
 *  Command.  */
typedef struct {
  /*!< MSGR header */
  msgr_hdr_struct_type hdr;
  /*!< number of enabled RX carriers.*/
  uint32 rxNumcarriers;
  /*!< freq ID mapping to a physical RX carrier.*/
  uint32 rxfrequencyId[HDRFW_TOTAL_CARRIERS];
  /*!< number of enabled TX carriers.*/
  uint32 txNumcarriers;
  /*!< freq ID mapping to a physical TX carrier.*/
  uint32 txfrequencyId[HDRFW_TOTAL_CARRIERS];
  /*!< per antenna channel ID mapping to RF device.*/
  uint16 rxChannelId[HDRFW_NUM_ANTENNAS];
  /*!< channel ID mapping to RF device.*/
  uint16 txChannelId;

} hdrfw_dsdx_frequency_msg_t;

/** DSDX State Definition. */
typedef enum {
  /*! CxM based DSDX is inactive.Single sim without CxM solution */
  HDR_DSDX_STATE_INACTIVE = 0,

  /*! CxM based DSDA is active */
  HDR_DSDA_STATE_ACTIVE,

  /*! CxM based DSDS is active */
  HDR_DSDS_STATE_ACTIVE,

  /*! CxM based DR_DSDS is active */
  HDR_DR_DSDS_STATE_ACTIVE,

  /*! CxM based DSDA is active */
  HDR_STX_DSDA_STATE_ACTIVE,

} hdrfw_dsdx_state_t;

/** DSDX enable command parameters.
 *  This command is sent only once to select between DSDA/DSDS/single SIM mode. */
typedef struct {
  /*!< MSGR header */
  msgr_hdr_struct_type hdr;
  /*!< flag to select DSDx states*/
  hdrfw_dsdx_state_t dsdxMode;

} hdrfw_dsdx_enable_msg_t;

/** QTA CMD Definition. */
typedef enum {
  /*! No measurement (debug only) */
  HDRFW_QTA_NO_MEAS_CMD = 0,
  /*! QTA START command */
  HDRFW_QTA_START_CMD,
  /*! QTA END COMMAND */
  HDRFW_QTA_END_CMD,

} hdrfw_qta_cfg_t;

/** QTA CFG command parameters.
 *  This command is sent to inform about QTA START/END. */
typedef struct {
  /*!< MSGR header */
  msgr_hdr_struct_type hdr;
  /*!< flag to select QTA state*/
  hdrfw_qta_cfg_t qtaCmd;

} hdrfw_qta_cfg_msg_t;

/** HDR QXDM Diag Log Packet Control  */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  /*! \brief QXDM Log packet ID */
  uint32                    diagLogID;

  /*! \brief 0=>disable, 1=>enable */
  boolean                   enable;

} hdrfw_diag_log_config_msg_t;

/* HDR QXDM Rx IQ samples logging */
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type hdr;
  
  /*! \brief Antenna: Primiary = 0; Diversity = 1 */
  uint16 ant;

  /*! \brief Carrier Idx: c0 = 0; c1 = 1; c2 = 2 */
  uint16 carrier;

} hdrfw_rx_iq_samples_logging_msg_t;

typedef struct {
   /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  /*! \brief Threshold in linear scale*/
  uint32 jamDetThres[CFW_NUM_LNA_STATES-1];

  /*! \brief Threshold - Hysteresis in linear scale*/
  uint32 jamDetThresMinusHyst[CFW_NUM_LNA_STATES-1];

} hdrfw_jd_threshold_update_msg_t;

typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  /*! \brief Mode config for JD: 0 for enable/disable, 1 for freeze/unfreeze */
  uint16  jamFreezeMode;
  uint16  jdEnable;
} hdrfw_jd_mode_msg_t;


/** EnhancedFeedbackMultiplexing and OHC control message. FW will latch
 * hdrfw_tx_ohc_cfg_t from SMEM at the nearest Half-slot when this message is
 * recived. EFM, DRC, DSC and ACK channels can be configured with this */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

} hdrfw_ohc_config_msg_t;

/** Fimrware Indication Config Types             */
#define HDR_FW_IND__RMAC_SUBFRAME    0   /**<  RMAC_SUBFRAME*/
#define HDR_FW_IND__RMAC0_FRAME      1   /**<  RMAC0_FRAME*/
#define HDR_FW_IND__SRCH_DUMP        2   /**<  SRCH_DUMP*/
#define HDR_FW_IND__DECOB_STATUS     3   /**<  DECOB_STATUS*/
#define HDR_FW_IND__DRC_SUPERVISION  4   /**<  DRC_SUPERVISION*/
#define HDR_FW_IND__SCC_PREAM        5   /**<  SCC_PREAM*/
#define HDR_FW_IND__LOG_BUFFER       6   /**<  LOG_BUFFER*/
#define HDR_FW_IND__SAT_DETECT       7   /**<  SAT_DETECT*/
#define HDR_FW_IND__BEST_ASP_CHANGE  8   /**<  BEST_ASP_CHANGE*/

/*! \brief Firmware Indication Message Config */
typedef struct {
  
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  uint32                    type:4;
  uint32                    enable:1;
} hdrfw_indication_cfg_msg_t;

/*! \brief Log Buffer Indication */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /** Read index for the ready log buffer. 
      \sa  hdrfw_shared_memory_interface_t */
  int16                     bufferIndex;
} hdrfw_log_buffer_ind_msg_t;

/*! \brief VI mode control indication  */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  uint32                    halfSlotDone:1;
} hdrfw_vi_ctl_ind_msg_t;

/*! \brief RxAGC ACQ to TRACK State Transition Indication. Sent when AGC switches 
    from ACQ gain to TRACK gain on a particular antenna.*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  
  /*! \brief Antenna index */
  uint16                    antenna;

  /*! \brief AGC accumulation value for each carrier */
  uint16                    agcAccum[HDRFW_TOTAL_CARRIERS];

  /* ! \brief Sequence number to be piped from the hdrfw_rxagc_ant_cfg_t */
  uint32                    sequence_number;
  /*! \AGC acqusition status, to indicate success or failure conditions detected 
   * if the status is non-zero, it indicates one of many possible error/warning
   * conditions were seen during acq */
  uint32                    agcAcqStatus;

} hdrfw_rxagc_track_ind_msg_t;

/** Indication to software that a Radio Link Failure is needed.
 *  \sa hdrfw_reset_req_ind_msg_t.  */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
} hdrfw_reset_req_ind_msg_t;

/** Indication to software that TX reset is needed.
 *  \sa hdrfw_reset_tx_seq_msg_t.  */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
} hdrfw_reset_tx_seq_msg_t;

typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  uint16                    rxAgcUnlockedMode;
  uint16                    rxAgcNumUnlocked;
  uint32                    rxAgcRTCOffset;
} hdrfw_rxagc_unlocked_mode_msg_t;

typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  uint16                    predMinBestAspSinr;
} hdrfw_min_best_asp_sinr_msg_t;

typedef struct {
  uint16                    carrierMode[HDRFW_TOTAL_CARRIERS];
  uint16                    agcGain;
  uint16                    agcMax;
  uint16                    agcMin[CFW_NUM_LNA_STATES];
} hdrfw_rxagc_ant_cfg_t;

/** Simple RxAGC configuration message. This is intended mainly for firmware VI. */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  hdrfw_rxagc_ant_cfg_t     antCfg[HDRFW_NUM_ANTENNAS];
  uint16                    setPoint;
} hdrfw_rxagc_cfg_msg_t;

/** Generic Verification and Integration mode control message. */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  uint32                    startHalfSlot:1;
  uint32                    sliceIdx:2;
  uint32                    pLoadEnable:1;
  uint32                    fingMergePrevent:1;
  uint16                    demodDrc[HDRFW_DEMOD_CARRIERS];
} hdrfw_vi_ctl_msg_t;   

/*\}*/

/** \defgroup GRICE configuration messages. 
  This section lists messages for grice configuration. Messages are only
  applicable when gRICE is enabled unless otherwise specified.
  */

/** Set FAC gain */
typedef struct {

  msgr_hdr_struct_type hdr;
  /*!< MGSR header */

  uint16    facGain;
  /*!< configures the FAC gain in linear domain.
  Default is 324 in Q10, which corresponds to -10dB. Other commonly used values are:
  257 for -12dB;182 for -15dB; 129 for -18dB;
  Based on current algorithm settings, the value would be lower bounded by
  61 (-24.5dB) and upper bounded by 1024 (0dB).**/

} hdrfw_grice_set_fac_gain_msg_t;

/** GRICE override field definition*/
typedef union {
  struct {

    /** When DSA override is enabled, choose rake
     *  or EQ: 1 = always choose rake; 0 = always
     *  choose EQ*/
    uint32 dsaOverrideVal:1;
    /** Enable/Disable DSA override. 1=enable override, 0=disable override */
    uint32 dsaOverrideEnable:1;

    /** When mmse override is enabled, choose 2MMSE
     *  or 4MMSE: 1 = always choose 2MMSE; 0 =
     *  always choose 4MMSE*/
    uint32 mmseOverrideVal:1;
    /** Enable/Disable mmse override. 1=enable override, 0=disable override */
    uint32 mmseOverrideEnable:1;

    /** reserved */
    uint32 :28;

  };
  uint32 intval;
} mcdo_grice_override_field_t; 


/** Override GRICE algorithm's output.  */
typedef struct {

  msgr_hdr_struct_type hdr;
  /*!< MGSR header */

  mcdo_grice_override_field_t    griceOverride;
  /*!< overriding MMSE selections. */

} hdrfw_grice_override_msg_t;

/*\{*/

/**
    \defgroup genrsp Firmware Responses 
    This section lists messages sent from Firmware in response to 
    Software configuration command messages. Unlike indication messages, a 
    response message always corresponds to a configuration message received
    earlier.
    */
/*\{*/

/** Response to HDR Firmware App Config message.   
    \sa hdrfw_state_cfg_msg_t.  
 */
typedef struct {
  
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  hdrfw_state_t             state;
  uint16                    hdrVersion;
  /** Physical address of the shared memory interface. \sa hdrfw_shared_memory_interface_t */
  uint32                    sharedMemoryAddr;           
} hdrfw_state_cfg_rsp_msg_t;

/** Response to finger assignment message. 
    \sa hdrfw_fing_assign_msg_t.  */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
} hdrfw_srch_fing_assign_rsp_msg_t;

/** Response to finger assignment message with slam.
    \sa hdrfw_fing_assign_msg_t.  */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
} hdrfw_srch_fing_assign_slam_rsp_msg_t;

#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY
/** Response Message indicating Priority has been
 *  changed after ASDIV
 *  \sa hdrfw_asdiv_pri_change_msg_t    */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
} hdrfw_asdiv_pri_change_rsp_msg_t;
#endif

/** Response to fll start ACQ message.   
    \sa hdrfw_fll_start_acq_msg_t */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  uint16                    fllLocked;
} hdrfw_srch_fll_acq_rsp_msg_t; 

/** Response to mstrSlam message.         
    \sa hdrfw_mstr_slam_msg_t */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  
  /*! \brief new hstr if message is used for fast acq.
   * This hstrCount corresponds to the slamCdmaSystemTime
   * returned in this message and will be +1 of the actual
   * FW hstrCount at slam time, as FW HS count trails real
   * system time by 1 HS*/
  uint16                    hstrCount;

  /*! \Updated system time when FW latched the slam message*/
  uint64                    slamCdmaSystemTime;
  
} hdrfw_srch_slam_rsp_msg_t;

/** Response to drc filter reset message. 
    \sa hdrfw_drc_filt_reset_msg_t */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /** DRC Filt Reset Done Mask. Bit c is set if predC[c].drcfiltReset is cleared*/
  uint16                    drcFiltResetDoneMask;
} hdrfw_srch_reset_drc_filt_rsp_msg_t;

/** Response to fll accum load message.  
    \sa hdrfw_fll_accum_load_msg_t */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
} hdrfw_srch_fll_accum_ld_rsp_msg_t;

/** Equalizer Configuration Done Response Message.
    \sa hdrfw_eq_cfg_msg_t        */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
} hdrfw_srch_eq_config_rsp_msg_t;

/** Response Message indicating ASP update message has been
 *  processed
 *  \sa hdrfw_asp_update_msg_t    */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
} hdrfw_asp_update_rsp_msg_t;

/** Response to Cancel Active Packet command.
    \sa hdrfw_decode_ctl_msg_t    */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
} hdrfw_cancel_active_packet_rsp_msg_t;

/** Response to RX_START command.
    \sa cfw_rx_start_msg_t    */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  cfw_rx_config_rsp_info_t rsp_info;

} hdrfw_rx_start_rsp_msg_t;

/** Response to RX_STOP command.
    \sa cfw_rx_stop_msg_t    */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  
  cfw_rx_config_rsp_info_t rsp_info;

} hdrfw_rx_stop_rsp_msg_t;

/** Response Message indicating TX_PA_CTL message has been
 *  processed and return the current PA status. 
 *  \sa hdrfw_tx_pa_ctl_rsp_msg_t    */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  /*! \brief Status of PA: 0 for OFF, 1 for ON */
  int32                     Status; 
} hdrfw_tx_pa_ctl_rsp_msg_t;

/*\}*/
/**
    \defgroup trfi RF Shared Memory Interface */
/*\{*/

/*!< * Dynamic information of the linearizer table. Packed into a 64 bit word
 * so that updates to all fields of a row look atomic to the TX-ISR (the
 * reading thread)
 */
typedef struct ALIGN(32) {

  cfw_tx_lin_row_t T[CFW_NUM_PA_STATES][CFW_RFA_LUT_SIZE];

} hdrfw_rfa_lut_tables_packed_t;

/*! @brief PA Backoff LUT Tables
 *
 * SMEM interface definition for PA Backoff LUT tables provided by RF SW. These will
 * be latched whenever PA backoff is enabled via the TX_LIMIT_CONFIG  message.
 *  
 * The PA Backoff LUT table entries are in -db640 unit
 * HiBackoffLutXY[1][i] signify PA Backoff ( in -db640 unit) that need to be applied
 * corresponding to txTotal contained in HiBackoffLutXY[0][i].Similarly for LoBackoffLutXY.
 *
 * @note MUST be cache-aligned.
 *
 * \sa cfw_tx_limit_config_msg_t
 *
 */
typedef struct ALIGN(32) {

  int16 HiBackoffLutXY[2][HDRFW_PA_BACKOFF_LUT_SIZE];
  int16 LoBackoffLutXY[2][HDRFW_PA_BACKOFF_LUT_SIZE];    
} hdrfw_pa_backoff_lut_tables_t;

typedef struct {
  uint16 AGCCommonAccum;
  uint16 CarrierMode[HDRFW_TOTAL_CARRIERS];
  uint16 AGCAccum[HDRFW_TOTAL_CARRIERS];
  uint16 AGCAccumMin;
  uint16 AGCAccumMax;
  uint16 LNAState;
} hdrfw_rxf_antenna_rd_t;

typedef struct {
  
  uint16 txTotal;     /* per-carrier total power */
  uint16 pilotPower;  /* per-carrier pilot power */
  uint16 GainAdjVal;  /* gain adj value          */
  
  /* Open-Loop Value (-1/640 dB units). 
   * ----------------------------------
   * Open Loop = Filtered RxAGC + OpenLoopAntGainDelta, if no override
   * Open Loop = Open Loop Write + OpenLoopAntGainDelta, if override set
   */
  uint16 OpenLoop;    
  
  uint16 TotalLinearPwr;

} hdrfw_txmod_carrier_rd_t;
  
/** RF Read Interface*/
typedef struct {
  hdrfw_rxf_antenna_rd_t A[HDRFW_NUM_ANTENNAS];
  hdrfw_txmod_carrier_rd_t txC[HDRFW_DEMOD_CARRIERS];
  uint16 paRangeMapped;    /* PA Range */
  uint16 txTrafficToPilot; /* XXX does not correspond to any existing I/F */
  uint16 txBetaPGainCal;   /* BETAP Gain Value (-1/640 dB units) */
  uint16 txLutIndex;       /* TX LUT index */
  uint32 txAgcAdj;         /* AVGA gain word */
  int32  txAptValue;       /* APT SMPS gain word */
  int16  AnalogPwr;        /* TX linearizer power entry (1/640 dB units) */
  int16  txTotalGainVal;   /* TX total power (1/640 dB units) */
  /* Below two vars is used by RF for HDET when enhanced PA backoff is enabled.
     TxTotalOverallMaxLimitInternal is the max tx_power used by FW. It equals
     RF provided max tx_pwr when not in funnel mode. In funnel mode,it equals
     _neq value computed by FW.*/
  int16  TxTotalOverallMaxLimitInternal;/* TX total MAX power (-1/640 dB units)*/
  int16  isFunnelingEnabled; /* Flag to indicate that funneling is enabled */

  /*!< Filtered TX total for SV headroom estimation (-1/640 dB units). 
   *  See cfw_rl_pwr_est_filter_cfg_msg_t to configure this filter. See the same
   *  for FW's default filter time constant.
   */
  int16 HEFilterOutput;

  /*!< Filtered TX total for C+G Coex TX filter (-1/640 dB units). 
   *  See cfw_rl_pwr_est_filter_cfg_msg_t to configure this filter. See the same
   *  for FW's default filter time constant.
   */
  int16 CoexTxFiltOutput;
  /*!< Filtered RX AGC (1/640 dBm units).
   *  Configure the filter gain in cfw_rx_agc_cfg_msg_t::agc_filt_gain.
   */
  int16 rxFiltPwr[HDRFW_NUM_ANTENNAS][HDRFW_DEMOD_CARRIERS];

  int16 fwRfJDAlgoStatus;   /* Jammer Detection HW status */
  int16 jamDetectedFlag;    /* Jammer Dtected Flag to be read by RFSW */
} hdrfw_rf_interface_rd_t;

/*\}*/

/**
    \defgroup smem Shared Memory Interfaces */
/*\{*/

/** General Firmware Status Interface.  Content in this shared memory buffer is 
    updated every half-slot. */

typedef struct {
  
  /*! \brief fingC[].F[].Status */
  uint32                    fingStatus[HDRFW_NUM_FINGERS];
  /*! \brief txC[].OpenLoop     */
  int16                     openLoop;
  /*! \brief predC[].FiltBestASPSNR */
  uint32                    filtBestASPSNR;
  /*! \brief handoffC[].BestASP */
  uint32                    bestASP;
  /*! \brief  predC[carrierIdx].drcFilt */
  uint16                    drcFilt;
  /*! \brief  aspCC[carrierIdx][*].SNRFilt */
  uint16                    snrFilt[HDRFW_NUM_ASP];
  uint32                    rclPredAdj;
  uint16                    actualDRC;

} hdrfw_general_status_carrier_t;

typedef struct {

  uint16                         hstrCount;
  uint16                         mstrRTCError;
  uint16                         mstrTracking;
  /*! \brief Value written to HDR_MPTIME_REF, basically MSTR finger's RTCOfs, cx8, 18-bit */
  uint32                         mstrRTCOfs;
  /*! \brief Value written to HDR_MODTIME_REF, basically MSTR finger's RTCOfs minus BTF, cx8, 18-bit */
  uint32                         txRTCOfs;
  uint16                         fllLocked; 
  uint32                         fllVCOAccum;
  /*! \brief fllRotC[].RotAccum */
  uint32                         rotAccum[HDRFW_TOTAL_CARRIERS];
  /*!\ brief fllRotC[].DynXXXX. */
  uint16                         fllDynCPDMode[HDRFW_TOTAL_CARRIERS];
  uint32                         fllDynFreqAccum[HDRFW_TOTAL_CARRIERS];
  /*! \brief fllRotC[].Acq2TrkTimer: if 0, hysteresis timer after ACQ to TRK transition
             has already expired and RotAccum value is considered as settled.  */
  uint32                         fllAcq2TrkTimer[HDRFW_TOTAL_CARRIERS]; 
  uint16                         tuneAwayActive;
  /*! \brief Overall txTotal */
  int16                          txTotalTransmitPower;
  /*! \brief Previous half-slot's txTotal */
  int16                          prevTxTotalTransmitPower;
  
  uint16                         txLimitingFlags;
  
  /** RMAC0 rabDecision  */
  uint16                         rabDecision;
  /** RMAC0 txMaxDataRateIndex */
  uint16                         txMaxDataRateIndex;

  uint16                         txLogBufferIndex;

  /*! \brief Per carrier status */
  hdrfw_general_status_carrier_t C[HDRFW_DEMOD_CARRIERS];

  /*! \brief Tx sys time register for debugging*/
  uint32 rtcRevModTxSysTime;

  uint32 revModTxSysTime;

  /* RTC position of the second TRK pilot in Cx2
   * to be used by SW to start search accurately
   * in ReAcq case*/
  uint32 reAcqTrkPilotPosCx2;

} hdrfw_general_status_interface_t;

/*! \brief Specifies PN long code state. Used for all carriers. This is
 * loaded into VPE on DAC_START. Thereafter, it is loaded every frame. */
typedef union 
{ 
  struct 
  {
     uint32 bits_31to0;/* Word 0 */

     uint32 bits_41to32 : 10; /* Word 1 */

     uint32 reserved0 : 22; /* Word 1 */

  } ; 

  /* Union of all the above */
  uint32 word_arr[2];

} hdrfw_tx_pn_long_code_t;

typedef struct {

  hdrfw_tx_pn_long_code_t   pn_code;

  /*! \brief Specifies which PN group to use for each carrier. LSB is for C0.
   * MSB is for C2. This is loaded into VPE on DAC_START. Thereafter, it is
   * loaded every frame.  All carriers can be configured to use the PN group
   * 0 by programming 0b000 in this field . This field is currently unused in
   * FW. This is intended for Feedback Multiplexing feature.*/
  uint32 traffic_pn_mask_ctl : 3; /* Word 3 */

  /*! \brief This is assumed to be valid when FW recieves DAC_START command.
   * It specifies which carriers need to be physically enabled on DAC_START.
   * An enable here means WAV micro kernel runs PN generator for that carrier
   * and starts to produces samples after DAC_START. But the samples will
   * be zero until modulator on that carrier is enabled via the
   * hdrfw_rmac_enable_carrier_msg_t message. Modulators can be turned on/off at anytime 
   * between DAC_START and DAC_STOP within the subset of enabled carriers
   * specified by this mask (pn_enabled_car_mask). 
   * Setting BIT(0) to 1 enables C0  
   * Setting BIT(1) to 1 enables C1  
   * Setting BIT(2) to 1 enables C2  
   .  */
  uint32 pn_enabled_car_mask : 3; /* Word 3 */

  uint32 reserved1 : 26; /* Word 3 */

} hdrfw_tx_pn_long_state_cfg_t;

/*! \brief Specifies PN mask for I and Q paths for a carrier. These are
 * latched at DAC_START. Thereafter, they are latched every frame.
 * Frame-offset does not apply to PN_MASK loading. FW will set
 * PN_LONG_CODE_LD at the processing half-slot for the frame boundary.  */
typedef union 
{ 
  struct 
  {
     uint32 pn_i_mask_31to0;/* Word 0 */

     uint32 pn_i_mask_41to32 : 10; /* Word 1 */

     uint32 reserved0 : 22; /* Word 1 */

     uint32 pn_q_mask_31to0;/* Word 2 */

     uint32 pn_q_mask_41to32 : 10; /* Word 3 */

     uint32 reserved1 : 22; /* Word 3 */

  } ; 

  /* Union of all the above */
  uint32 word_arr[4];

} hdrfw_tx_pn_mask_cfg_t;

/*! \brief 
 * OHC (overhead) channels carrier mapping, and their PN group mapping.
 */
typedef union 
{ 
  struct 
  {
    /* Physical Carrier index to be used for the ohc channels of a logical
     * carrier. Valid values are 0 to 2. */
    uint32 ohc_carr_map : 3; /* Word 0 */

    uint32 reserved0 : 29; /* Word 0 */

    /* PN group to be used for a logical carrier's OHC channels
     * Valid values are 0 to 2. */
    uint32 ohc_pn_mask_ctl : 3; /* Word 1 */

    uint32 reserved1 : 29; /* Word 1 */

  } ; 

  /* Union of all the above */
  uint32 word_arr[2];

} hdrfw_tx_ohc_ctl_t;

/*! \brief 
 * Config for the three OHC channels - Ack, Drc, Dsc. 
 */
typedef struct 
{
  struct {
    /* 1 => transmit on Q. 0 => transmit on I */
    uint32 transmit_on_q : 1;

    /* Walsh code to be used. Valid values or 0 to 3. 0 represents W(0,4) and
     * 3 represents W(3,4)*/
    uint32 walsh_code : 2;

    uint32 reserved : 29;
  } ack;

  struct {
    /* 1 => transmit on Q. 0 => transmit on I */
    uint32 transmit_on_q : 1;

    /* Transmit pattern for the DSC channel. 
     * Bit(1) specifies for Even slots.
     * Bit (0) specifies for Odd slots. A set bit means 
     *
     * 0b00 => Transmit on Even. Transmit on Odd;
     * 0b01 => Transmit on Even. Do not transmit on Odd;
     * 0b10 => Do not transmit on Even. Transmit on Odd;
     * 0b11 => Do not transmit on Even.Do not transmit on Odd;
     * */
    uint32 transmit_pattern : 2;

    uint32 reserved : 29;
  } dsc;

  struct {
    /* 1 => transmit on I. 0 => transmit on Q */
    uint32 transmit_on_i : 1;

    /* Walsh code to be used. Valid values or 0 or 1. 0 represents W(0,2) and
     * 1 represents W(1,2)*/
    uint32 walsh_code : 1;

    uint32 reserved : 30;
  } drc;

} hdrfw_tx_ohc_chan_cfg_t;

/*! \brief Specifies configuration for OHC (overhead) channels.
 * This includes,
 * 1) FOC carrier mapping, PN group mapping.
 * 2) Config for ACK, DRC, DSC channels.
 */
typedef struct
{
  hdrfw_tx_ohc_ctl_t ohc_ctl; 

  hdrfw_tx_ohc_chan_cfg_t ohc_chan_cfg;

} hdrfw_tx_ohc_cfg_t; 

/** Software Writable Shared Memory Interface Structure. Firmware only reads from this structure. */
typedef struct ALIGN(32) {
  
  /** Finger assigment */
  hdrfw_fing_assign_interface_t        fing_assign;

  /** ASP configuration */
  hdrfw_asp_cfg_interface_t            asp_cfg;

  /** LNA interface */
  cfw_rf_LNA_interface_t               cfw_lna_cfg[HDRFW_NUM_ANTENNAS];

  /* NEW shared memory defination for PA Backoff LUT*/
  hdrfw_pa_backoff_lut_tables_t         paBackoffLUTTables;

  /* ******************** */
  // BOLT additions
  /* ******************** */

  /** PN initial state and mapping */
  hdrfw_tx_pn_long_state_cfg_t            pn_state_cfg;

  /** Traffic channel PN masks */
  hdrfw_tx_pn_mask_cfg_t                  pn_mask_cfg[HDRFW_TOTAL_CARRIERS];

  hdrfw_tx_ohc_cfg_t                      ohc_cfg[HDRFW_TOTAL_CARRIERS];

} hdrfw_smem_write_intf_t;

typedef union {
  struct {
    uint32 samp[384];                            /* W[0]..W[383] */
  };

  uint32 word_arr[384];

} hdrfw_enc_evdo_ib_packet_t;


typedef union {
  struct {
    hdrfw_enc_evdo_ib_packet_t                                   packet[4];                     /* W[0]..W[1535] */
  };

  uint32 word_arr[1536];

} hdrfw_enc_evdo_ib_car_t;


typedef union {
  struct {
    hdrfw_enc_evdo_ib_car_t                                      car[3];                        /* W[0]..W[4607] */
  };

  uint32 word_arr[4608];

} hdrfw_enc_evdo_ib_t;

/*Structure that contains byte offset and line number of antenna smaples in LMEM*/
typedef struct {
  uint32 byteOffset;
  uint32 lmLine;
} hdrfw_sample_srv_offset_t;

/*Structure that contains offset of antenna samples in LMEM. */
typedef struct {
  hdrfw_sample_srv_offset_t sampleSrv[HDRFW_TOTAL_CARRIERS][HDRFW_NUM_ANTENNAS];
  uint32 valid; /*!< If set as 1, the above address is valid. 
                     If set as 0, the above address is invalid. */
} hdrfw_fl_samp_srv_info_t; 

/** General Forward Link Related Interface. */
typedef struct {
  uint32 decobOffsetInWord; /*!< The starting offset of decob buffer in mempool. */ 
  uint32 decobOffsetValid;  /*!<  If set as 0, the above decobOffsetInWord is invalid.  
                             If set as 1, the above decobOffsetInWord is valid.
                             HDR FW set this flag to 1 after the transition to active
                             state is complete and set it as zero after transition 
                             to standby state. */

  hdrfw_fl_samp_srv_info_t sampSrvInfo; /*!< Sample Server location */

} hdrfw_fl_interface_t; 

/** Structure that contains sample server info (SRAM start AXI address). */
typedef struct {

  /* '0' represents an invalid address - i.e. SRAM isn't setup yet */
  uint32 l1sAddr[HDRFW_TOTAL_CARRIERS][HDRFW_NUM_ANTENNAS];

} hdrfw_fl_l1s_sram_info_t;

/** Software Readable Shared Memory Interface Structure. Firmware only writes to this structure. */
typedef struct ALIGN(32) {

  /** General firmware status */
  hdrfw_general_status_interface_t    general_status;

  /** RTCMAC subframe indication */
  hdrfw_rmac_subframe_ind_interface_t rmac_subframe_ind;

  /** Srch dump indication */
  hdrfw_srch_dump_ind_interface_t     srch_dump_ind;

  // RX log ping pong buffer
  hdrfw_log_buffer_t                  log_buffer[HDRFW_LOG_BUFFERS];

  // TX log ping pong buffer
  hdrfw_tx_log_buffer_t               tx_log_buffer[HDRFW_LOG_BUFFERS];
  
  // Modem power log buffer
  hdrfw_power_logging_t               pwr_log_buffer;
  
  /** Decode Header */
  uint32                              decode_header[HDRFW_DECOB_MAX_PACKETS][HDRFW_PKT_HEADER_SIZE];

  /** RF read interface */
  hdrfw_rf_interface_rd_t             rf_rd;

  /** Difference in Cx8k between STMR & VSRC */
  uint32                              stmrVsrcDiffCx8k[HDRFW_NUM_ANTENNAS];

  /*! \brief Number of HDET requests successfully serviced by FW from the
   * start of time. Will wrap-around to 0 after 2^31 -1 . SW should read this
   * value before sending a HDET request to FW and expect the incremented
   * value when FW finishes servicing the request. FW never resets this. So
   * the value is non-volatile between TX reinits. The only time this is
   * cleared is when the shared mem is cleared.*/
  uint32                              numHdetServiced;

/*! @brief The "frozen" tx total power value during the HDET measurement
 * process.
 */
  int16                               txHdetTotalPwr;

  /*! @brief Indicates if a HDET request is sucessfully serviced. SW should
   * first wait for the numHdetServiced to be incremented and then read this
   * variable. TRUE => Success, FALSE => failed. 
   */
  boolean                             hdetSuccess;

  uint32                              encIbPtr;

  /*!@brief Provides the status of forward link. This includes the  */ 
  hdrfw_fl_interface_t                   flInterface; 
  
  /*!@brief Provides the DECOB AHB start adress  */  
  uint32 decobStartAddr;  

  /*!@brief Provides Sample RAM Information (AXI address) */
  hdrfw_fl_l1s_sram_info_t           sramInfo;
  
} hdrfw_smem_read_intf_t; 

/** Shared Memory Interface Structure. The shared memory should be accessed using 
    two pointers to the read and write sections of  this structure type. 
    \sa hdrfw_state_cfg_msg_t::sharedMemoryAddr */

typedef struct {

  hdrfw_smem_read_intf_t              read;
  hdrfw_smem_write_intf_t             write;

} hdrfw_shared_memory_interface_t; 


/*\}*/

/**
   \defgroup pmeas Pilot Measurement / IRAT
   HDR Pilot Measurement Command Message Structures */
/*\{*/

/** Pilot Measurement Configuration Request Message.
    Software sends this message to configure for EVDO pilot measurements.

    Pilot measurement configuration can be initiated immediately or based on 
    start time in terms of DO RTC (Cx1). The duration of the pilot measurement
    may be infinite or given in terms of DO RTC (Cx1).

    Response: hdrfw_pilot_meas_cfg_rsp_msg_t.
    The response is generated based on either desired number of samples captured
    or end of the measurement window, whichever is earlier.
*/
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /**   Bit(0) in half-word 0
      \verbatim
        15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
      |                            startRtcCx1                                   |  I |
      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
      
      I : startImmediate. 1=Immediate - startRtcCx1 is ignored. 0=Start according
      to startRtcCx1.
      \endverbatim
  */
  uint16                    startImmediate:1;
  /**   Bit(15:1) in half-word 0
      \verbatim
        15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
      |                            StartRtcCx1                                   |  I |
      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
      
      startRtcCx1 : Start time of pilot measurement configuration in terms of RTC Cx1.
      \endverbatim
  */
  uint16                    startRtcCx1:15;

  /**   Bit(0) in half-word 1
      \verbatim
        15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
      |               durationRtcCx1                                             |  N |
      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
      
	N : durationInfinite. "Infinite" duration; durationRtcCx1 is ignored. FW will not
      release or disable any resources; FW will perform "tune" but not "tune back/away".
	Pilot measurement configuration response will be triggered solely based on 
	desired numSamplesCaptured.

      durationRtcCx1 : Duration of pilot measurement gap in terms of RTC Cx1
	\endverbatim
  */
  uint16                    durationInfinite:1;

  /**   Bit(15:0) in half-word 1
      \verbatim
        15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
      |               durationRtcCx1                                             |  N |
      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
      
      durationRtcCx1 : Duration of pilot measurement gap in terms of RTC Cx1
	\endverbatim
  */
  uint16                    durationRtcCx1:15;


  /**   Idle measurement search mode. Online search = 0, Offline search = 1. This flag is only used
   *    when durationInfinite is set. Regardless of search mode, rsp is sent after 4 half-slots worth
   *    of data is captured. In offline mode, SSI is stopped.
   */
  uint16                    idleOfflineSearch:1;


  uint16 :11;

  /**   Number of desired valid samples in sample RAM. Not used in idle mode.
   */
  uint16                    numSamplesDesired;

  /**   Initial value of RX rotator in FW units(same as rotAccum).
   */
  uint32                    rotError;

  
  /**   RF Tune Back Time Constant. Time before end of gap when RF tune back is started.
   */ 
  uint16                    rfTuneBackTimeConstant;


} hdrfw_pilot_meas_cfg_req_msg_t;

/** Pilot Measurement Configuration Response Message

    Firmware generates this response to Pilot Measurement Configuration Request message:
    hdrfw_pilot_meas_cfg_req_msg_t.
 */
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  /**   Number of half-slots that RX AGC has run.
  */
  uint16                    numRxAgcHalfSlots;

  /**   Number of samples captured after RX AGC acquisition.
  */
  uint16                    numSamplesCaptured;

  /**   Final value of RX AGC accumulator
   */
  uint32                    rxAgcAccum;

  /**   Status/Error code (TBD)
   */
  uint16                    status;

  /**   Bit(15:0) in half-word 4
      \verbatim
        15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
      |                        timeStampReqRtcCx2                                     |
      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
      
      timeStampReqRtcCx2 : DO RTC timestamp when the Pilot Measurement Configureation
      request was received.
	\endverbatim
   */
  uint16                    timeStampReqRtcCx2;

  /**   Bit(15:0) in half-word 5
      \verbatim
        15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
      |                        timeStampRspRtcCx2                                     |
      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
      
      timeStampRspRtcCx2 : DO RTC timestamp when this Pilot Measurement Configuration
      response is sent.
	\endverbatim
   */
  uint16                    timeStampRspRtcCx2;

  /**   Bit(15:0) in half-word 6
      \verbatim
        15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
      |                         streamingStopRtcCx2                                   | 
      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
      
      streamingStopRtcCx2 : RTC position in cx2 when streaming is stopped for
      connected mode and idle offline mode, to be passed on to
      SRCH FW for offline search.
	\endverbatim
   */
  uint16                    streamingStopRtcCx2;
} hdrfw_pilot_meas_cfg_rsp_msg_t;


/** Pilot Measurement Stop Stream Command
    
    At end of the idle mode measurement (in MEAS_CONNECTED state), to stop the sample
    streaming L1 must send this message before sending the state transition message
    to put firmware back to STANDBY state. This also results in firmware releasing
    the WB/NB chain allocated from FW CRM.

    Firmware assumes the chain to be stopped is the one started from PILOT_MEAS_CFG_REQ/RX_START_CFG
    sent eariler when the measurement was started.

    This message is not needed in connected mode measurement where stream is stopped automatically
    at gap end.

    Response: hdrfw_pilot_meas_stop_stream_rsp_t
    This is sent when the sample stream is stopped and RxFE chain released.
*/
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

} hdrfw_pilot_meas_stop_stream_msg_t;

/** Pilot Measurement Stop Stream Response
    This is the response to the stop stream message. After receiving this message,
    software can put firmware to STANDBY state.
*/
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

} hdrfw_pilot_meas_stop_stream_rsp_msg_t;

/*\}*/

/**
   \defgroup crfa RFA Commands
   Definitions for RFA Command Messages  */
/*@{*/


typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  // Per antenna
  uint16 antenna;
  uint16 AGCMin[CFW_NUM_LNA_STATES];
  uint16 AGCMax;
  uint16 AGCGain;

  // Shared
  uint32 RTCOffset;
  uint16 numUnlocked;
  uint16 forcedUnlockedMode;

} hdrfw_rxagc_acq_mode_msg_t;

typedef struct{

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  uint16 antenna;
  uint16 minMaxReset;
  uint16 numUnlocked;
  uint16 forcedUnlockedMode;
  uint16 AGCGain;

} hdrfw_rxagc_trk_mode_msg_t;

typedef struct{

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  uint16 rriRequest;
  uint16 txFrameOffset;

} hdrfw_rel0_tx_cfg_msg_t;

/*! \brief This message is only intended to start carrier in override mode. To 
 * set up carrier in enabled mode, use rxagc_enable message. \sa hdrfw_rxagc_enable_msg_t */
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  uint16 antenna;
  uint16 enableOverrideMask;
  uint16 agcAccumOverrideValue[HDRFW_TOTAL_CARRIERS];

} hdrfw_rxagc_override_msg_t;

typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  uint16 antenna;
  uint16 antennaMode;
  uint16 carrierMode[HDRFW_TOTAL_CARRIERS];

} hdrfw_rxagc_enable_msg_t;

typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  // Per antenna
  uint16 antenna;
  uint16 AGCGain;
} hdrfw_rxagc_gain_msg_t;

typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  
  /*! antenna - the antenna index for which the following configuration 
   * applies.
   */
  uint16 antenna;
  
  struct {

    /* This bit-field indicates whether the following carrier configuration
     * is valid or not.
     */
    uint16 carrierValid:1;
    
    /* This the RX frequency offset specified in the units of HW rotator 
     * resolution.
     */
    uint32 freqOffset;

  } C[HDRFW_DEMOD_CARRIERS];

} hdrfw_rxagc_freq_offset_msg_t;

typedef struct {
  
  msgr_hdr_struct_type      hdr;
  uint16 cxnADCClockRatio;

} hdrfw_rxagc_cxn_adc_clock_ratio_msg_t;

/*! @brief PA Parameters Message
 * 
 */
typedef struct {

  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  uint16 paRangeMap;
  /*!< PA Range Map. 
   *
   * Holds PA range mapping (see below):
   *
   \verbatim
     15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
    +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
    |\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\|  PAR0   |  PAR1   |  PAR2   |  PAR3   |
    +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
    PARi -> PA range for PA state i
   \endverbatim
   *
   * Latched immediately.
   */

  uint16 paHysteresisTimerValue;
  /*!< PA Hysteresis Timer Value 
   *
   * Hysteresis is disabled when timer = 0 (Latched immediately).
   *
   * */

} hdrfw_tx_pa_params_config_msg_t;


/*! @brief TX Limit Parameters Message
 *
 * Defines TX limit parameters. Latched at 2 places:
 * - Each frame boundary.
 * - On TX init.
 */
typedef struct {

  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  int16 txTotalMax;
  /*!< MAX txTotal (FW units: -1/640 dB) */

  int16 txTotalMin;
  /*!< MIN txTotal (FW units: -1/640 dB) */
  
} hdrfw_tx_limit_config_msg_t;

/*! @brief Set RFA LUT Parameters Message
 *
 * Define RFA LUT parameters. Latched immediately from SMEM.
 *
 */
typedef struct {

  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

} hdrfw_tx_lut_params_config_msg_t;

typedef struct {
  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  struct {

    uint16 carrierValid:1;
    /* This field indicates whether the following per-carrier information 
     * is valid or not
     */
    
    uint32 freqOffset;
    /* TX frequency offset specified in HW rotator resolution */
    
  } C[HDRFW_DEMOD_CARRIERS];

} hdrfw_tx_freq_offset_config_msg_t;

typedef struct {

  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  /*! L1 has interface message to enable/disable RPC \sa hdrfw_rmac_misc_cfg_msg_t 
   * Check how override flag is used in RFA interface. 
   * RF usage: for carrier c, set C[c].gainAdjOverride.useGainAdjWrite = 1 , 
   * C[c].gainAdjOverride.carrier = c, C[c].gainAdjOverride.gainAdjWrite = override value */
  struct {
    hdrfw_gain_adj_override_field_t gainAdjOverride;
    uint16                          gainAdjWrite;
  } C[HDRFW_DEMOD_CARRIERS];
} hdrfw_tx_closed_loop_config_msg_t;


typedef struct {

  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  /*! L1 has interface message to enable/disable openloop override  \sa hdrfw_rmac_openloop_override_msg_t
   * Check how override flag is used in RFA interface. 
   * RF usage: for carrier c, set C[c].openLoopOverride.enableOverride = 1 , 
   * C[c].openLoopWrite = override value (-1/640 dB units).
   *
   * NOTE: Open Loop Antenna Gain Offset is applied to openLoopWrite to determine
   * the final open loop value.
   */
  struct {
    hdrfw_openloop_override_field_t openLoopOverride;
    uint16 openLoopWrite;
  } C[HDRFW_DEMOD_CARRIERS];

} hdrfw_tx_open_loop_config_msg_t;

typedef struct {

  msgr_hdr_struct_type hdr;
  /*!< MSGR Header */

  uint16 OpenLoopAntGainDelta;
  /*!< Open Loop Gain Offset Delta 
   *
   * This is the offset applied to the TX open-loop gain computed based on RxAGC 
   * information. The value must be in -1/640 dB units.
   *
   * NOTE: This gain will be applied even when OpenLoopOverride is set !!
   */
} hdrfw_tx_open_loop_ant_gain_delta_msg_t;

typedef struct {

  msgr_hdr_struct_type hdr;

  /*! This is mainly for error checking */
  uint32 antenna; 
  uint32 rxlm_buffer_idx;
  uint32 rxlm_config_mask;

} hdrfw_rxlm_config_masked_msg_t;


/* Dynamic LNA Feature Control
   This message will enable or disable the Dynamic LNA feature. 
   This message will not enable/disable early preamble detection */
 
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type hdr;
 
  /*! \brief BIT(0): 0=>disable D-LNA
                     1=>enable D-LNA 
             BIT(1): 0=>disable Enhanced D-LNA
                     1=>enable Enhanced D-LNA
      Enhanced D-LNA cannot be enabled when D-LNA is disabled,
      and will automatically be disabled when D-LNA is disabled */
  uint32 enable;
} hdrfw_dlna_ctl_msg_t;

/* Early Preamble Detection Control
   This message will immediately enable or disable whether early preamble detection is attempted or not.
   This message will not enable or disable D-LNA, that can be done with the D-LNA feature ctl message */
 
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type  hdr;
 
  /*! \brief 0=>disable, 1=>enable */
  boolean enable;
} hdrfw_early_pream_ctl_msg_t;
 
/* Timed Force Odd Preamble Detection
   This message schedules a timed disable of early preamble detection.  FW needs the starting slot time,
   and the duration, in slots, of this disable.  These events will not be queued in FW, so the
   current event can be updated but not cancelled.  Also, another event cannot be scheduled before
   the previous one is executed. */
 
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type  hdr;
 
  /*! \brief Slot to start forcing odd preamble detection only 
       SlotTime = HSTR>>1 */
  uint16 startSlotTime;
 
  /*! \brief Number of slots to force odd preamble detection only */
  uint16 slotDuration;
} hdrfw_force_odd_pream_msg_t;

/*----------------------------------------------------------------------------*/
/*! 
  @brief   HDRFW sleep modes 

  @details
  For features like DLNA , FW can call the rx_on_off_ctrl API, with different
  sleep modes. Each sleep mode is responsible for a different set of register 
  writes i.e. micro sleep mode will not disable the PLL on the WTR and will
  be a lower scale sleep, while milli sleep will put the PLL to sleep as well
  each sleep mode has its own use-case. This enum matches the FED API sleep
  mode enum.

*/
typedef enum 
{
  /*! Micro Sleep mode, micro sleep is used in features like DLNA or enhanced 
  DLNA where the Rx path is shut off for shorter durations and PLL is still 
  locked */ 
  HDRFW_MICRO_SLEEP_MODE =  0     ,
  /*----------------------------------*/
  
  /*! Milli sleep mode on/off both (not supported right now) */
  HDRFW_MILLI_SLEEP_ON_OFF_MODE  , 

  /*----------------------------------*/
  /*! Milli sleep mode with off only mode , RFLM will schdule ccs events 
  for enabling Rx chain only and not disabling it (not 
  supported right now) */ 
  HDRFW_MILLI_SLEEP_ON_ONLY_MODE  , 

  /*----------------------------------*/
  /*! Milli sleep mode with off only mode , RFLM will schdule ccs events 
  for disabling Rx chain only and not enabling it (not 
  supported right now) */ 
  HDRFW_MILLI_SLEEP_OFF_ONLY_MODE  , 
   
} hdrfw_sleep_mode_e;

typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type  hdr;

  /*! \brief
  1: Enable the debug feature to force microsleep every half slot
  0: Disable the feature
  This flag is checked every half slot and will be latched at
  the next half slot. */
  uint32 forceSleepEnableDisable;
       
  /*! \brief
  The delta from the USTMR execution time where the RF will be scheduled.
  A value of 0 results in an immediate RF off. */
  uint32 rfOffUstmrDelta;
       
  /*! \brief
  The delta from the USTMR execution time where RF should be settled. */
  uint32 rfOnUstmrDelta;

 /*! \brief
  The number of half slots for force microsleep to run. */
  uint32 durationHalfSlots;
  
  /*! @brief 
  Micro/milli sleep, enum internal to c2k to determine whether 
  the sleep is micro or milli type.*/
  hdrfw_sleep_mode_e sleepMode;
  
} hdrfw_ftm_force_sleep_msg_t;

/*! @brief Set Detection mode for FMAC Channels
 *
 * Enable/Disable MMSE Detection method for FMAC Channels.
 *
 */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  struct mmseDetect_chan{
    /*! \brief 
      Enable MMSE & Selection switch detection for RAB channel.
      Setting this to '0' will fall back to legacy MRC MAC demodulation.
      */
    uint32 rab:1;
    /*! \brief
      Enable MMSE & Selection switch detection for all following channels.
      -DRC Lock
      -RPC 
      -HARQ/PARQ
      Setting this to '0' will fall back to legacy MRC MAC demodulation.
      */
    uint32 arq:1;
    /*! \brief
      Reserved bits
      */
    uint32 rsvd:30;
  } mmseDetect;

} hdrfw_fmac_detect_cfg_msg_t;


/*\}*/
      
/**
    \defgroup unions Message Unions
    Unions of message structures for MSGR receive buffer declaration. */
/*\{*/


/*! \brief Union of all message received by firmware */
typedef union {

  /*! \brief Message header */
  msgr_hdr_struct_type                    hdr;
  hdrfw_state_cfg_msg_t                   state_cfg;
  hdrfw_fing_assign_msg_t                 fing_assign;
  hdrfw_handoff_cfg_msg_t                 handoff_cfg;
  hdrfw_handoff_override_msg_t            handoff_override;
  hdrfw_rel0_rl_cfg_msg_t                 rel0_rl_cfg;
  hdrfw_rmac0_gain_offset_cfg_msg_t       rmac0_gain_offset_cfg;
  hdrfw_rmac_subframe_cfg_msg_t           rmac_subframe_cfg;
  hdrfw_rmac_send_packet_msg_t            rmac_send_packet;
  hdrfw_rmac_misc_cfg_msg_t               rmac_misc_cfg;
  hdrfw_rmac_attrib_cfg_msg_t             rmac_attrib_cfg;
  hdrfw_rmac_enable_carrier_msg_t         rmac_enable_carrier;
  hdrfw_rmac_openloop_override_msg_t      rmac_openloop_override;
  hdrfw_rmac_harqcoeffc2i_cfg_msg_t       rmac_harqcoeffc2i_cfg;
  hdrfw_rmac_reset_pilotpwrfilt_msg_t     rmac_reset_pilotpwrfilt;
  hdrfw_rmac_reset_rl_interlace_msg_t     rmac_reset_rl_interlace;
  hdrfw_rmac_modulator_cfg_msg_t          rmac_modulator_cfg;
  hdrfw_rmac_pwrmargin_cfg_msg_t          rmac_pwrmargin_cfg;
  hdrfw_rmac_throttle_cfg_msg_t           rmac_throttle_cfg;
  hdrfw_rmac_dtxmode_cfg_msg_t            rmac_dtxmode_cfg;
  hdrfw_rmac_maxbackoff_cfg_msg_t         rmac_maxbackoff_cfg;
  hdrfw_rmac_t2pbias_cfg_msg_t            rmac_t2pbias_cfg;
  hdrfw_ftcmac_channel_cfg_msg_t          ftcmac_channel_cfg;
  hdrfw_fmac_activate_msg_t               fmac_activate;
  hdrfw_cc_demod_cfg_msg_t                cc_demod_cfg;
  hdrfw_scc_msg_t                         scc;
  hdrfw_tuneaway_start_msg_t              tuneaway_start;
  hdrfw_tuneaway_stop_msg_t               tuneaway_stop;
  hdrfw_eq_cfg_msg_t                      eq_cfg;
  hdrfw_eq_override_msg_t                 eq_override;
  hdrfw_drc_cfg_msg_t                     drc_cfg;
  hdrfw_drc_filt_reset_msg_t              drc_filt_reset;
  hdrfw_sleep_adj_msg_t                   sleep_adj;
  hdrfw_fpd_msg_t                         fpd;
  hdrfw_qpch_msg_t                        qpch;
  hdrfw_hyperspace_cfg_msg_t              hyperspace_cfg;
  hdrfw_hstr_offset_cfg_msg_t             hstr_offset_cfg;
  hdrfw_decob_update_msg_t                decob_update;
  hdrfw_indication_cfg_msg_t              indication_cfg;
  hdrfw_drc_translation_cfg_msg_t         drc_translation;
  hdrfw_timers_cfg_msg_t                  timers_cfg;
  hdrfw_bcc_detect_msg_t                  bcc_detect;
  hdrfw_flow_control_cfg_msg_t            flow_control_cfg;
  hdrfw_fll_start_acq_msg_t               fll_start_acq;
  hdrfw_fll_rot_cfg_msg_t                 fll_rot_cfg;
  hdrfw_fll_xo_cfg_msg_t                  fll_xo_cfg;
  hdrfw_fll_rot_override_msg_t            fll_rot_override;
  hdrfw_fll_pdm_override_msg_t            fll_pdm_override;
  hdrfw_fll_enable_msg_t                  fll_enable;
  hdrfw_fll_accum_load_msg_t              fll_accum_load;
  hdrfw_fll_gain_cfg_msg_t                fll_gain_cfg;
  hdrfw_fll_slew_cfg_msg_t                fll_slew_cfg;
  hdrfw_dll_cfg_msg_t                     dll_cfg;
  hdrfw_pll_cfg_msg_t                     pll_cfg;
  hdrfw_asp_update_msg_t                  asp_update;
  hdrfw_fmac_gaup_cfg_msg_t               fmac_gaup_cfg;
  hdrfw_mstr_slam_msg_t                   mstr_slam;
  hdrfw_partial_ld_rcl_cfg_msg_t          partial_ld_rcl_cfg;
  hdrfw_decode_ctl_msg_t                  decode_ctl;    
  hdrfw_fing_lock_thresh_msg_t            fing_lock_thresh;
  hdrfw_decob_ind_rate_msg_t              decob_ind_rate;
  hdrfw_legacy_flow_control_msg_t         legacy_flow_control;
  hdrfw_fmac_ack_mode_cfg_msg_t           fmac_ack_mode_cfg;
  hdrfw_min_pream_thresh_cfg_msg_t        min_pream_thresh_cfg;
  hdrfw_fmac_handoff_cfg_msg_t            fmac_handoff_cfg;
  hdrfw_mup_decode_filt_cfg_msg_t         mup_decode_filt_cfg;
  hdrfw_diversity_cfg_msg_t               diversity_cfg;   
  hdrfw_force_comb_mode_msg_t             force_comb_mode;
  hdrfw_min_best_asp_sinr_msg_t           min_best_asp_sinr;
  hdrfw_max_drc_cfg_msg_t                 max_drc_cfg;
  hdrfw_reset_seq_num_msg_t               reset_seq_num;
  hdrfw_pilot_meas_cfg_req_msg_t          pilot_meas_cfg_req;
  hdrfw_pilot_meas_stop_stream_msg_t      pilot_meas_stop_stream;
  hdrfw_tx_pa_params_config_msg_t         tx_pa_params_config;
  cfw_tx_limit_config_msg_t               tx_limit_config;
  hdrfw_tx_lut_params_config_msg_t        tx_lut_params_config;
  hdrfw_tx_freq_offset_config_msg_t       tx_freq_offset_config;
  hdrfw_tx_closed_loop_config_msg_t       tx_closed_loop_config;
  hdrfw_tx_open_loop_config_msg_t         tx_open_loop_config;
  hdrfw_tx_open_loop_ant_gain_delta_msg_t tx_open_loop_ant_gain_delta;
  hdrfw_rxlm_config_masked_msg_t          rxlm_config_masked;
  cfw_rx_agc_cfg_msg_t                    rx_agc_cfg;
  cfw_lna_override_msg_t                  lna_override;
  cfw_lna_gain_offset_msg_t               lna_gain_offset; 
  cfw_rx_start_msg_t                      rx_start;
  cfw_rx_start_cfg_msg_t                  rx_start_cfg;
  cfw_rx_stop_msg_t                       rx_stop;
  hdrfw_tx_pa_ctl_msg_t                   tx_pa_ctl;
  cfw_tx_agc_cfg_msg_t                    tx_agc_cfg; 
  cfw_tx_start_msg_t                      tx_start;
  cfw_tx_stop_msg_t                       tx_stop;
  cfw_tx_dac_start_msg_t                  tx_dac_start;
  hdrfw_tx_dac_stop_msg_t                 tx_dac_stop;
  cfw_tx_hdet_request_msg_t               hdet_req;
  hdrfw_vi_ctl_msg_t                      vi_ctl;  
  cfw_tx_hdet_config_msg_t                hdet_config;
  cfw_rx_agc_stop_msg_t                   rx_agc_stop;
  cfw_intelliceiver_update_msg_t          intelliceiver_update;
  hdrfw_grice_set_fac_gain_msg_t          grice_set_fac_gain;
  hdrfw_grice_override_msg_t              grice_override;
  cfw_dynamic_txc_update_msg_t            dynamic_txc_update;
  cfw_ept_config_msg_t                    ept_config;
  cfw_rl_pwr_est_filter_cfg_msg_t         tx_pwr_filter_config;
  cfw_xpt_config_msg_t                    xpt_config;
  hdrfw_diag_log_config_msg_t             diag_log_config;
  hdrfw_jd_threshold_update_msg_t         jd_threshold_update;
  hdrfw_jd_mode_msg_t                     jd_mode;
  cfw_xpt_delay_update_msg_t              xpt_delay_update;
  hdrfw_ohc_config_msg_t                  ohc_config;
  hdrfw_rel0_tx_cfg_msg_t                 rel0_tx_cfg;
  hdrfw_dlna_ctl_msg_t                    dlna_ctl;
  hdrfw_early_pream_ctl_msg_t             early_pream_ctl;
  hdrfw_force_odd_pream_msg_t             force_odd_pream;
  hdrfw_ftm_force_sleep_msg_t             ftm_force_sleep;
  cfw_rx_exercise_milli_micro_sleep_msg_t exercise_milli_sleep;
  hdrfw_fmac_detect_cfg_msg_t             fmac_detect_config;

 /* DSDX structures */
  hdrfw_dsdx_enable_msg_t                 dsdx_state_cfg;
  hdrfw_dsdx_priority_config_msg_t        dsdx_priority_config;
  hdrfw_dsdx_frequency_msg_t              dsdx_frequency_config;
  hdrfw_qta_cfg_msg_t                     qta_cfg;

  hdrfw_rx_iq_samples_logging_msg_t       rx_iq_samples_logging;
#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY
  cfw_rf_execute_ant_switch_msg_t         rf_execute_ant_switch_msg;
#endif
  } hdrfw_msg_u;

/*! \brief Union of all indication messages recevied by software  */

typedef union {
  /*! \brief Message header */
  msgr_hdr_struct_type                    hdr;
  hdrfw_best_asp_change_ind_msg_t         best_asp_change;  
  hdrfw_rmac0_frame_ind_msg_t             rmac0_frame;        
  hdrfw_scc_preamble_ind_msg_t            scc_preamble;        
  hdrfw_scc_decode_ind_msg_t              scc_decode;
  hdrfw_decob_status_ind_msg_t            decob_status;        
  hdrfw_drc_supervision_timeout_ind_msg_t drc_supervision_timeout;
  hdrfw_drc_supervision_restart_ind_msg_t drc_supervision_restart;
  hdrfw_timers_ind_msg_t                  timers;
  hdrfw_vi_ctl_ind_msg_t                  vi_ctl;
  hdrfw_rxagc_track_ind_msg_t             track_ind;
  hdrfw_reset_req_ind_msg_t               reset_req;
  hdrfw_reset_tx_seq_msg_t                reset_tx_req;
} hdrfw_msg_ind_u;

/*! \brief Union of all response messages recevied by software  */

typedef union {
  /*! \brief Message header */
  msgr_hdr_struct_type                    hdr;
  hdrfw_state_cfg_rsp_msg_t               state_cfg;
  hdrfw_srch_fing_assign_rsp_msg_t        fing_assign;
  hdrfw_srch_eq_config_rsp_msg_t          eq_config;
  hdrfw_asp_update_rsp_msg_t              asp_update;
  hdrfw_srch_fll_acq_rsp_msg_t            fll_acq;
  hdrfw_srch_slam_rsp_msg_t               srch_slam;
  hdrfw_srch_reset_drc_filt_rsp_msg_t     reset_drc_filt;
  hdrfw_srch_fll_accum_ld_rsp_msg_t       fll_accum_ld;
  hdrfw_cancel_active_packet_rsp_msg_t    cancel_active_packet;
  hdrfw_pilot_meas_cfg_rsp_msg_t          pilot_meas_cfg;
  hdrfw_pilot_meas_stop_stream_rsp_msg_t  pilot_meas_stop_stream;
  hdrfw_tx_pa_ctl_rsp_msg_t               tx_pa_ctl;
  cfw_pa_params_rsp_msg_t                 pa_params_rsp;
  cfw_xpt_trans_compl_rsp_msg_t           xpt_trans_compl_rsp;
  cfw_xpt_config_rsp_msg_t                xpt_rsp;
  hdrfw_srch_fing_assign_slam_rsp_msg_t   fing_assign_slam_rsp;
#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY
  hdrfw_asdiv_pri_change_rsp_msg_t        asdiv_pri_change_rsp;
#endif
} hdrfw_msg_rsp_u;

/*! \brief Union of all messages recevied by software  */
typedef union {

  /*! \brief Message header */
  msgr_hdr_struct_type                    hdr;
  hdrfw_msg_ind_u                         ind;
  hdrfw_msg_rsp_u                         rsp;
} hdrfw_sw_rcv_msg_u;

/*\}*/

#define HDRFW_MAX_MSG_SIZE     sizeof(hdrfw_msg_u)
#define HDRFW_MAX_IND_MSG_SIZE sizeof(hdrfw_msg_ind_u)
#define HDRFW_MAX_RSP_MSG_SIZE sizeof(hdrfw_msg_rsp_u)

#define HDRFW_NUM_CMD_MSG       (HDR_FW_CMD_LAST - HDR_FW_CMD_FIRST + 1)
#define HDRFW_NUM_IND_MSG       (HDR_FW_IND_LAST - HDR_FW_IND_FIRST + 1)
#define HDRFW_NUM_RSP_MSG       (HDR_FW_RSP_LAST - HDR_FW_RSP_FIRST + 1)

// Internal Firmware message
enum
{
  MSGR_DEFINE_UMID(HDR, FW, TMR, RX_RTC,                 0x0,    NULL),    
  MSGR_DEFINE_UMID(HDR, FW, TMR, TX_ISR_DONE,            0x1,    NULL)
}; 

#endif /* HDRFW_MSG_H */
