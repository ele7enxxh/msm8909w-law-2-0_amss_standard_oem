/*!
  @file cdmafw_msg.h

  @brief External message interface for the CDMA1x FW module

*/

/*===========================================================================

  Copyright (c) 2010-13 Qualcomm Technologies Incorporated. All Rights Reserved

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
  @mainpage Nike-L CDMA1x Firmware Interface Document
  @authors Haobing Zhu, Vijay Ramasami
  @section intro Revision
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_c2k/api/cdmafw_msg.h#1 $
  @section whatsnew What's New

*/


#ifndef CDMAFW_MSG_H
#define CDMAFW_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "msgr.h"
#include "vstmr_1x.h"
#include "msgr_umid.h"
#include "cdmafw_txagc.h"
#include "cdmafw_logging.h"
#include "cfw_rf_intf.h"
#include "cdmafw_const.h"
#include "fw_rf_common_intf.h"
#include "cdmafw_rl_defs.h"
#include "cfw_version_defs.h"

#ifdef __cplusplus
} // extern "C"
#endif



/*============================================================================
 *
 * DOXYGEN GROUPINGS
 *
 * ==========================================================================*/
/** 
 * \defgroup capp App Configuration Messages
 * \defgroup csrch SRCH Finger SW Commands
 * \defgroup csrchafc SRCH AFC SW Commands
 * \defgroup cmux MUX SW Commands
 * \defgroup clrfa Legacy RFA SW Commands
 * \defgroup cirat IRAT Command and Response
 * \defgroup cresp FW Response Messages
 * \defgroup cind FW Indication Messages
 * \defgroup cunion FW Message Unions
 * \defgroup cshared Shared Memory Interface
 * \defgroup clog CDMA FW Logging Messages.
 */

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief The text string for the external CDMAFW Task Queue. */
#define CDMAFW_MSG_Q_NAME           "CDMAFW_Q"

/*! \brief This is the maximum number of fingers that can be assigned. */
#define CDMAFW_NUM_FINGERS             8


/*! \brief This is the walsh power log buffer length in QLIC. */
#define WALSH_LOG_BUF_LEN (66*4) //bytes

/*! \brief This is the maximum number of cells. */
#define CDMAFW_NUM_PWR_CTL_CELLS       6

/*! \brief Number of variables for FW to peek, poke (debug only) */
#define CFW_POKE_VAR_SIZE                    10
#define CFW_PEEK_VAR_SIZE                    10

/*-----------------------------------------------------------------------*/
/*     CDMAFW COMMAND & RESPONSE ID'S                                    */
/*-----------------------------------------------------------------------*/

#define MSGR_MODULE_FW              0x00
#define MSGR_CDMA_FW                MSGR_TECH_MODULE( MSGR_TECH_CDMA, MSGR_MODULE_FW)



/*-----------------------------------------------------------------------*/
/*     CDMAFW Supervisory                                                */
/*-----------------------------------------------------------------------*/

/*! @brief Enumeration of CDMA FW Supervisory commands
 */
enum
{
  /*! \brief Not used for now, sent from APPMGR */
  MSGR_DEFINE_UMID(CDMA, FW, SPR, LOOPBACK, MSGR_ID_LOOPBACK,
                   msgr_spr_loopback_struct_type),
  /*! \brief Not used for now, send to APPMGR */
  MSGR_DEFINE_UMID(CDMA, FW, SPR, LOOPBACK_REPLY, MSGR_ID_LOOPBACK_REPLY,
                   msgr_spr_loopback_reply_struct_type),

};


/*-----------------------------------------------------------------------*/

/*! @brief Supervisory Loopback  - This message is sent by the message router
           and is to be sent back to it in a cdmafw_spr_loopback_reply_s to
           confirm that the  message queues are operating correctly. */
typedef msgr_spr_loopback_struct_type       cdmafw_spr_loopback_s;

/*! @brief Supervisory Loopback Reply  - This reply is sent back to the
           message router in response to receiving a umbsrch_spr_loopback_s
           message to confirm that the message queues are operating
           correctly. */
typedef msgr_spr_loopback_reply_struct_type cdmafw_spr_loopback_reply_s;



/*-----------------------------------------------------------------------*/
/*     CDMAFW COMMAND, INDICATION and RESPONSE UMIDs                     */
/*-----------------------------------------------------------------------*/

/*! @brief Enumeration of CDMA FW Command UMIDs
 */
// Payload input for documentation only.          
enum
{
  /*! \brief The first message in the enum */
  CDMA_FW_FL_CMD_FIRST = MSGR_UMID_BASE(MSGR_CDMA_FW, MSGR_TYPE_CMD),
  
  /* APP and STATE configuration */
  MSGR_DEFINE_UMID(CDMA, FW, CMD, STATE_CFG,                     0x00, cdmafw_state_cfg_msg_t), 

  /* Finger Assignment and Configuration */
  MSGR_DEFINE_UMID(CDMA, FW, CMD, MULTIPLE_FINGERS_ASSIGN,       0x01, cdmafw_multiple_fingers_assign_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, MULTIPLE_FINGERS_CONFIG,       0x02, cdmafw_multiple_fingers_config_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, FINGER_COMMON_CONFIG,          0x03, cdmafw_finger_common_config_msg_t),

  /* AFC Configuration */
  MSGR_DEFINE_UMID(CDMA, FW, CMD, AFC_MODE,                      0x04, cdmafw_afc_mode_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, XO_AFC_STATIC_CONFIG,          0x05, cdmafw_xo_afc_static_config_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, XO_AFC_DYNAMIC_CONFIG,         0x06, cdmafw_xo_afc_dynamic_config_msg_t),

  /* L1 Forward Link Configuration Commands */
  MSGR_DEFINE_UMID(CDMA, FW, CMD, FL_SIGNALING,                  0x07, cdmafw_fl_signaling_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, APF_ENABLE,                    0x08, cdmafw_apf_enable_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, FPC_FREEZE_CONFIG,             0x09, cdmafw_fpc_freeze_cfg_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, FPC_OVERRIDE,                  0x0A, cdmafw_fpc_override_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, SCH_ACC_SCALE,                 0x0B, cdmafw_sch_acc_scale_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, FCH_TARGET_SETPOINT,           0x0C, cdmafw_fch_target_setpoint_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, SCH_TARGET_SETPOINT,           0x0D, cdmafw_sch_target_setpoint_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, RDA_CONFIG,                    0x0E, cdmafw_rda_config_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, QOF_NOISE_EST_FPC,             0x0F, cdmafw_qof_noise_est_fpc_config_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, DEMBACK_MODE_CONFIG,           0x10, cdmafw_demback_mode_config_msg_t), 

  /* Demod Configuration Commands (new on BOLT) */
  MSGR_DEFINE_UMID(CDMA, FW, CMD, DEMOD_CHANNEL0_CFG,            0x11, cdmafw_demod_channel0_cfg_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, DEMOD_CHANNEL1_CFG,            0x12, cdmafw_demod_channel1_cfg_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, DEMOD_CHANNEL2_CFG,            0x13, cdmafw_demod_channel2_cfg_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, DEMOD_RPC_CFG,                 0x14, cdmafw_demod_rpc_cfg_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, DEMOD_FPC_CFG,                 0x15, cdmafw_demod_fpc_cfg_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, DEMOD_TRK_FING_CFG,            0x16, cdmafw_demod_trk_fing_cfg_msg_t), 
  MSGR_DEFINE_UMID(CDMA, FW, CMD, DEMOD_SYS_TIME_SLAM,           0x17, cdmafw_demod_sys_time_slam_msg_t), 
  MSGR_DEFINE_UMID(CDMA, FW, CMD, DEMOD_SYS_TIME_SLEW_ROLL,      0x18, cdmafw_demod_sys_time_slew_roll_msg_t), 
  MSGR_DEFINE_UMID(CDMA, FW, CMD, DEMOD_SYS_TIME_SLEW_TIME,      0x19, cdmafw_demod_sys_time_slew_time_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, DEMOD_FRAME_OFFSET,            0x1A, cdmafw_demod_frame_offset_msg_t), 
  MSGR_DEFINE_UMID(CDMA, FW, CMD, DEMOD_MERGE_CFG,               0x1B, cdmafw_demod_merge_cfg_msg_t), 
  MSGR_DEFINE_UMID(CDMA, FW, CMD, DEMOD_LC_STATE,                0x1C, cdmafw_demod_lc_state_msg_t), 
  MSGR_DEFINE_UMID(CDMA, FW, CMD, DEMOD_LC_MASK,                 0x1D, cdmafw_demod_lc_mask_msg_t), 
 
  /* Logging Configuration Commands */
  MSGR_DEFINE_UMID(CDMA, FW, CMD, QLIC_WALSH_PWR_LOGGING,        0x1E, cdmafw_qlic_walsh_pwr_logging_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, ADV1X_LOG_CONFIG,              0x1F, cdmafw_adv1x_log_config_msg_t),

  /* RF SW TX Configuration Messages */
  MSGR_DEFINE_UMID(CDMA, FW, CMD, TX_AGC_CFG,                    0x20, cfw_tx_agc_cfg_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, TX_PA_PARAMS_CONFIG,           0x21, cdmafw_tx_pa_params_config_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, TX_LIMIT_CONFIG,               0x22, cfw_tx_limit_config_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, TX_HDET_CONFIG,                0x23, cfw_tx_hdet_config_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, DAC_CAL_CONFIG,                0x24, cfw_tx_dac_cal_config_msg_t),
  
  /* RF SW TX Override Commands */
  MSGR_DEFINE_UMID(CDMA, FW, CMD, TX_OVERRIDE,                   0x25, cdmafw_tx_override_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, TX_CLOSED_LOOP_OVERRIDE,       0x26, cdmafw_tx_closed_loop_override_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, TX_OPEN_LOOP_OVERRIDE,         0x27, cdmafw_tx_open_loop_override_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, TX_PA_STATE_OVERRIDE,          0x28, cdmafw_tx_pa_state_override_msg_t),

  /* RF SW TX Miscellanous Commands */
  MSGR_DEFINE_UMID(CDMA, FW, CMD, TX_HDET_REQUEST,               0x29, cfw_tx_hdet_request_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, RXLM_CONFIG_MASKED,            0x2A, cdmafw_rxlm_config_masked_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, DYNAMIC_TXC_UPDATE,            0x2B, cfw_dynamic_txc_update_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, TX_HDET_RESET,                 0x2C, cfw_tx_hdet_reset_msg_t),
  
  /* RX and TX Tuning Commands */
  MSGR_DEFINE_UMID(CDMA, FW, CMD, RX_START_CFG,                  0x2D, cfw_rx_start_cfg_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, RX_START,                      0x2E, cfw_rx_start_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, RX_STOP,                       0x2F, cfw_rx_stop_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, INTELLICEIVER_UPDATE,          0x30, cfw_intelliceiver_update_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, OQPCH_SEARCH_DONE,             0x31, cdmafw_oqpch_search_done_msg_t),

  /* IRAT Commands */
  MSGR_DEFINE_UMID(CDMA, FW, CMD, PILOT_MEAS_CFG_REQ,            0x32, cdmafw_pilot_meas_cfg_req_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, PILOT_MEAS_STOP_STREAM,        0x33, cdmafw_pilot_meas_stop_stream_msg_t),
  
  /* RF SW RX Configuration Messages */
  MSGR_DEFINE_UMID(CDMA, FW, CMD, RX_AGC_CFG,                    0x34, cfw_rx_agc_cfg_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, LNA_OVERRIDE,                  0x35, cfw_lna_override_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, LNA_GAIN_OFFSET,               0x36, cfw_lna_gain_offset_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, RX_AGC_STOP,                   0x37, cfw_rx_agc_stop_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, RX_SET_FREQ_OFFSET,            0x38, cdmafw_rx_set_freq_offset_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, RXAGC_ACQ_MODE_CONFIG,         0x39, cdmafw_rxagc_acq_mode_config_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, XPT_CONFIG,                    0x3A, cfw_xpt_config_msg_t),

  /*  Tx Sample Capture command */
  MSGR_DEFINE_UMID(CDMA, FW, CMD, EPT_CAPTURE,                   0x3B, cdmafw_ept_capture_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, EPT_OVERRIDE,                  0x3C, cdmafw_ept_override_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, EPT_CONFIG,                    0x3D, cfw_ept_config_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, EPT_SAMPLE_BUFFER_CFG,         0x3E, cfw_ept_sample_buffer_cfg_msg_t),

  /*  QXDM Logging Messages */
  MSGR_DEFINE_UMID(CDMA, FW, CMD, LOG_RX_IQ_SAMPLES,             0x3F, cdmafw_rx_iq_samples_logging_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, DIAG_LOG_CONFIG,               0x40, cdmafw_diag_log_config_msg_t),

  /* To be deprecated */
  MSGR_DEFINE_UMID(CDMA, FW, CMD, TDEC_BRDG_CONFIG,              0x41, cdmafw_tdec_brdg_config_msg_t),
  /* To be deprecated */
  MSGR_DEFINE_UMID(CDMA, FW, CMD, FINGER_ASSIGN,                 0x42, cdmafw_finger_assign_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, FINGER_CONFIG,                 0x43, cdmafw_finger_config_msg_t),
    
  MSGR_DEFINE_UMID(CDMA, FW, CMD, XPT_CAPTURE,                   0x44, cdmafw_xpt_capture_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, STOP_TXAGC_UPDATE,             0x45, cdmafw_stop_txagc_update_msg_t),

  MSGR_DEFINE_UMID(CDMA, FW, CMD, TX_PA_RANGE_OVERRIDE,          0x46, cdmafw_tx_pa_range_override_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, RXAGC_LNA_CAL_OVERRIDE,        0x47, cdmafw_rxagc_lna_cal_override_msg_t),

  MSGR_DEFINE_UMID(CDMA, FW, CMD, JD_THRESHOLD_UPDATE,           0x48, cdmafw_jd_threshold_update_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, JD_MODE,                       0x49, cdmafw_jd_mode_msg_t),

  MSGR_DEFINE_UMID(CDMA, FW, CMD, XPT_DELAY_UPDATE,              0x4A, cfw_xpt_delay_update_msg_t),

  MSGR_DEFINE_UMID(CDMA, FW, CMD, QXDM_DIAG_DATA,                0x4B, cdmafw_qxdm_diag_data_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, DEMBACK_SW_CONFIG,             0x4C, cdmafw_demback_sw_config_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, IQ_CAPTURE,                    0x4D, cfw_iq_capture_msg_t),
  
/* L1 DSDx priority Commands */
  MSGR_DEFINE_UMID(CDMA, FW, CMD, REGISTER_RX_TX_ACTIVITY,       0x4E, cdmafw_dsdx_config_msg_t),

  MSGR_DEFINE_UMID(CDMA, FW, CMD, EXERCISE_MILLI_SLEEP,          0x4F, cfw_rx_exercise_milli_micro_sleep_msg_t),

  MSGR_DEFINE_UMID(CDMA, FW, CMD, 1X2X_QTA_START,                0x50, cdmafw_1x2x_qta_start_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, 1X2X_QTA_STOP,                 0x51, cdmafw_1x2x_qta_stop_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, CMD, 1X2X_QTA_STATUS_QUERY,         0x52, cdmafw_1x2x_qta_status_query_msg_t),
#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY  
  MSGR_DEFINE_UMID(CDMA, FW, CMD, ANT_SWITCH_ASDIV,              0x53, cfw_rf_execute_ant_switch_msg_t),
  /*! \brief The last message in the enum */
  CDMA_FW_FL_CMD_LAST = CDMA_FW_CMD_ANT_SWITCH_ASDIV 
#else
  /*! \brief The last message in the enum */
  CDMA_FW_FL_CMD_LAST = CDMA_FW_CMD_1X2X_QTA_STATUS_QUERY
#endif
};


/*! @brief Enumeration of CDMA FW Indication UMIDs
 */
// Payload input for documentation only.          
enum
{
  CDMA_FW_IND_FIRST = MSGR_UMID_BASE(MSGR_CDMA_FW, MSGR_TYPE_IND),
  MSGR_DEFINE_UMID(CDMA, FW, IND, RXTX_FRAME_STATS,              0x00, cdmafw_rxtx_frame_stats_ind_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, IND, QLIC_FRAME_STATS,              0x01, cdmafw_qlic_frame_stats_ind_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, IND, RC11_FCH_DECODER_DONE,         0x02, cdmafw_rc11_fch_decoder_done_ind_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, IND, ADV1X_LOG_BUFFER_RDY,          0x03, cdmafw_adv1x_log_buffer_rdy_ind_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, IND, VI_CTL,                        0x04, cdmafw_vi_ctl_ind_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, IND, QP_DONE,                       0x05, cdmafw_qp_done_ind_msg_t), 
  MSGR_DEFINE_UMID(CDMA, FW, IND, RESET_REQ,                     0x06, cdmafw_reset_req_ind_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, IND, EPT_PROC_DONE,                 0x07, cdmafw_ept_proc_done_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, IND, EPT_EXPANSION_DONE,            0x08, cdmafw_ept_expansion_done_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, IND, QTA_RXAGC_ACQ_DONE,            0x09, cdmafw_qta_rxagc_acq_done_ind_msg_t),
  CDMA_FW_IND_LAST = CDMA_FW_IND_QTA_RXAGC_ACQ_DONE
};


/*! @brief Enumeration of CDMA FW Response UMIDs
 */
// Payload input for documentation only.          
enum
{
  CDMA_FW_RSP_FIRST = MSGR_UMID_BASE(MSGR_CDMA_FW, MSGR_TYPE_RSP),
  MSGR_DEFINE_UMID(CDMA, FW, RSP, QLIC_WALSH_PWR_LOGGING_READY,  0x00, cdmafw_qlic_walsh_pwr_logging_rdy_rsp_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, RSP, STATE_CFG,                     0x01, cdmafw_state_cfg_rsp_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, RSP, TX_HIGH_PWR_DET,               0x02, cdmafw_tx_high_pwr_det_rsp_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, RSP, TX_START,                      0x03, cfw_tx_start_rsp_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, RSP, TX_STOP,                       0x04, cfw_tx_stop_rsp_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, RSP, RX_START,                      0x05, cfw_rx_start_rsp_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, RSP, RX_STOP,                       0x06, cfw_rx_stop_rsp_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, RSP, TX_HDET_RESPONSE,              0x07, cfw_tx_hdet_response_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, RSP, PILOT_MEAS_CFG,                0x08, cdmafw_pilot_meas_cfg_rsp_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, RSP, PILOT_MEAS_STOP_STREAM,        0x09, cdmafw_pilot_meas_stop_stream_rsp_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, RSP, INTELLICEIVER_UPDATE,          0x0A, cfw_intelliceiver_update_rsp_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, RSP, DAC_CAL_CONFIG,                0x0B, cfw_dac_cal_config_rsp_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, RSP, IRAT_RX_START,                 0x0C, cfw_irat_rx_start_rsp_msg_t),

  MSGR_DEFINE_UMID(CDMA, FW, RSP, EPT_CAPTURE,                   0x0D, cdmafw_ept_capture_rsp_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, RSP, XPT_CAPTURE,                   0x0E, cdmafw_xpt_capture_rsp_msg_t),
//RK TODO PA_PARAMS_CONFIG to be removed after RFSW has no references to it
  MSGR_DEFINE_UMID(CDMA, FW, RSP, PA_PARAMS_CONFIG,              0x0F, cfw_pa_params_rsp_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, RSP, XPT_TRANS_COMPL,               0x10, cfw_xpt_trans_compl_rsp_msg_t),

  MSGR_DEFINE_UMID(CDMA, FW, RSP, XPT_CONFIG,                    0x11, cfw_xpt_config_rsp_msg_t),
  
  MSGR_DEFINE_UMID(CDMA, FW, RSP, IQ_CAPTURE,                    0x12, cfw_iq_capture_rsp_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, RSP, COMBINER_SLAM,                 0x13, cdmafw_rx_combiner_slam_done_rsp_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, RSP, COMBINER_SLEW_ROLL,            0x14, cdmafw_rx_combiner_slew_roll_done_rsp_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, RSP, DEMBACK_SW_CONFIG,             0x15, cdmafw_demback_sw_config_rsp_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, RSP, COMBINER_SLEW,                 0x16, cdmafw_rx_combiner_slew_done_rsp_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, RSP, 1X2X_QTA_START,                0x17, cdmafw_1x2x_qta_start_rsp_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, RSP, 1X2X_QTA_STOP,                 0x18, cdmafw_1x2x_qta_stop_rsp_msg_t),
  MSGR_DEFINE_UMID(CDMA, FW, RSP, 1X2X_QTA_STATUS_QUERY,         0x19, cdmafw_1x2x_qta_status_query_rsp_msg_t),
#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY  
  MSGR_DEFINE_UMID(CDMA, FW, RSP, ASDIV_PRI_CHANGE,              0x20, cdmafw_asdiv_pri_change_rsp_msg_t),

  CDMA_FW_RSP_LAST = CDMA_FW_RSP_ASDIV_PRI_CHANGE
#else

  CDMA_FW_RSP_LAST = CDMA_FW_RSP_1X2X_QTA_STATUS_QUERY
#endif
};

/*! @brief Enumeration of CDMA FW Internal Message UMIDs
 */
enum {
  MSGR_DEFINE_UMID(CDMA, FW, CMDI, MSGR_SHUT_DOWN,               0x00, NULL),
  /* internal RX_START message for VI use */  
  MSGR_DEFINE_UMID(CDMA, FW, CMDI, RX_START,                     0x01, NULL)
};

/*-----------------------------------------------------------------------*/
/*     DATA STRUCTURES FOR CDMAFW COMMANDS                               */
/*-----------------------------------------------------------------------*/

/**
*   \addtogroup capp
*   @{
*/

/*! \brief  CDMA State Definitions */
typedef enum {
  CDMAFW_STATE_INACTIVE=0,
  CDMAFW_STATE_STANDBY,
  CDMAFW_STATE_ACTIVE,
  CDMAFW_STATE_MEAS,
  CDMAFW_STATE_SLEEP,
  CDMAFW_STATE_MEAS_IDLE
} cdmafw_state_t;

/*! \brief CDMA STATE Config message
 *
 * Change CDMA APP state to the requested state.
 *
 */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type hdr;
  /*! \brief New state of CDMA1x APP */
  cdmafw_state_t state;
} cdmafw_state_cfg_msg_t;

/** QXDM Diag Command ("Send Data") Received Notification.

  This message is used to notify FW that a new QXDM diag command has been received
  and that the received data has been populated into the SMEM buffer.

  \sa qxdmDiagDataBuffer
  
*/  
typedef struct {

  /** MSGR Header */
  msgr_hdr_struct_type hdr;

  /** Length of the data received in bytes */
  uint32 lengthBytes;

} cdmafw_qxdm_diag_data_msg_t;

/**@}*/

/**
   \addtogroup csrch
    */
/*@{*/

/*! @brief Single Finger Assignment Command
 *
 *   This is the message definition for the cdmafw Single Finger Assignment
 *   Command
 */
typedef struct {
  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  uint32 fingerIndex:3;
  /*!< Index of the finger to configure. 
  */
  uint32 FingCfgWr_Update:1;
  /*!< Set to 1 to indicate FingCfgEnable, FingCfgPosition and 
       FingPilotFilterState update.
  */
  uint32 FingCfgWr;
  /*!< Finger Configuration
    @verbatim
    Bit 31: Request to keep pilot filter state (0=clear pilot filter state, 1=keep pilot filter state)
    Bit 30-19: Sequence number (Increase the sequence number for each finger assignment)
    Bit 18-1: FingCfgPosition (PN position in cx8)
    Bit 0: Enable flag (0=Disable finger, 1=Enable finger)
    @endverbatim
    Note: SW shall set the ARM register DEM1X_RTF_CTL_SEL as 1 so that MDSP controls fingers and 
          HW finger state machine is disabled.
  */
  uint32 PwrCtlCell_Update:1;
  /*!< Set to 1 to indicate PwrCtlCell update. \n
  	Note: When PwrCtlCell is updated, FingCfg is required to be updated. 
  */    
  uint16 PwrCtlCell;
  /*!< This indicates which Cell the finger is associated with.
    @verbatim
    Null Cell: 000
    Cell A:    001
    Cell B:    010
    Cell C:    011
    Cell D:    100
    Cell E:    101
    Cell F:    110
    @endverbatim
  */
  uint32 QLICSectorId_Update:1;
  /*!< Set to 1 to indicate QLICEnable and QLICSectorId update. \n 
    Note: When QLICEnable is updated, FingCfg is required to be updated. 
  */    
  uint16 QLICSectorId;
  /*!< QLIC Sector ID for QOF set 0.
    @verbatim
    Valid ID: 0~9
    Not in any QLIC sector: 0xFFFF
    @endverbatim
	Note: Due to hardware limitation, the maximum delay spread supported in one 
	      QLIC sector is 40 chips. 
  */
  uint32 SectorId_Update:1;
  /*!< Set to 1 to indicate SectorId update. 
  */
  uint16 SectorId;
  /*!< This indicates the fingers sector as configured in the 
       DEM1X_COMMON_REV_PWR_CTL/CTL2 registers used for 
	   F-CPCCH or F-CACKCH (1xAdv).
    @verbatim
    000: sector1 (offset specified by INIT_OFFSET1)
    001: sector2 (offset specified by INIT_OFFSET2)
    010: sector3 (offset specified by INIT_OFFSET3)
    011: sector4 (offset specified by INIT_OFFSET4)
    100: sector5 (offset specified by INIT_OFFSET5)
    101: sector6 (offset specified by INIT_OFFSET6)
	@endverbatim
  */
  
  uint32 QofSectorInfo_Update:1;
  /*!< Set to 1 to indicate QofSectorInfo Update
   */
  
  uint16 QofSectorInfo;
   /*!< QOF Sector Information

       @verbatim
       |15|                          |5   4|3         0|
       |-----------------------------------------------|
       |D | NOT USED                 |setId| sector Id |
       |-----------------------------------------------|
       3:0 - QOF sector Id.range is 0-9
       5:4 - Set Id.range is 0-3
        15 - QOF disable flag. set to 1 for disable.
       @endverbatim
  */
  uint32 fchQofUseQlicSectorERam_Update:1;
  /*!< Set to 1 to indicate fchQofUseQlicSectorERam Update
   */
    
  uint16 fchQofUseQlicSectorERam;
   /*!< Set to 0x10 for using QOF QLIC sector ERAM and 0 for not
     using it. 
  */

  uint16 qlicCarrierSelect_Update;
  /*!< Set to 1 to indicate qlicCarrierSelect Update
   */

  uint16 qlicCarrierSelect;
  /*! QLIC Carrier Input Select.
   *
   * This field indicates the carrier from which QLIC fetches its finger data. Use the 
   * following values:
   * 
   * 0 => fetch from primary chain.
   * 3 => fetch from SV chain.
   * 
   */
} cdmafw_finger_assign_msg_t;

/** 
 * @brief SRCH SW Multiple Fingers Assignment Command 
 * CDMA FW finger assignment command message structure
 */
typedef struct {
    uint32 FingCfgWr;
    /*!< Finger Configuration
      @verbatim
      Bit 31: Request to keep pilot filter state (0=clear pilot filter state, 1=keep pilot filter state)
      Bit 30-19: Sequence number (Increase the sequence number for each finger assignment)
      Bit 18-1: FingCfgPosition (PN position in cx8)
      Bit 0: Enable flag (0=Disable finger, 1=Enable finger)
      @endverbatim
      Note: SW shall set the ARM register DEM1X_RTF_CTL_SEL as 1 so that MDSP controls fingers and 
            HW finger state machine is disabled.
    */
    uint16 PwrCtlCell;
    /*!< This indicates which Cell the finger is associated with.
      @verbatim
      Null Cell: 000
      Cell A:    001
      Cell B:    010
      Cell C:    011
      Cell D:    100
      Cell E:    101
      Cell F:    110
      @endverbatim
    */
    uint16 QLICSectorId;
    /*!< QLIC Sector ID for QOF set 0.
      @verbatim
      Valid ID: 0~9
      Not in any QLIC sector: 0xFFFF
      @endverbatim
      Note: Due to hardware limitation, the maximum delay spread supported in one 
            QLIC sector is 40 chips. 
    */
    uint16 SectorId;
    /*!< This indicates the fingers sector as configured in the 
         DEM1X_COMMON_REV_PWR_CTL/CTL2 registers used for 
         F-CPCCH or F-CACKCH (1xAdv).
      @verbatim
      000: sector1 (offset specified by INIT_OFFSET1)
      001: sector2 (offset specified by INIT_OFFSET2)
      010: sector3 (offset specified by INIT_OFFSET3)
      011: sector4 (offset specified by INIT_OFFSET4)
      100: sector5 (offset specified by INIT_OFFSET5)
      101: sector6 (offset specified by INIT_OFFSET6)
      @endverbatim
    */
    uint16 QofSectorInfo;
     /*!< 
     |15|                          |5   4|3         0|
     |-----------------------------------------------|
     |D | NOT USED                 |setId| sector Id |
     |-----------------------------------------------|
     3:0 - QOF sector Id.range is 0-9
     5:4 - Set Id.range is 0-3
      15 - QOF disable flag. set to 1 for disable.
                                                 
    */
    uint16 fchQofUseQlicSectorERam;
     /*!< Set to 0x10 for using QOF QLIC sector ERAM and 0 for not
       using it. 
    */
    uint16 reserved0;
    /*!< padding for word alignment */
    uint32 FingCfgWr_Update:1;
    /*!< Set to 1 to indicate FingCfgEnable, FingCfgPosition and 
         FingPilotFilterState update.
    */
    uint32 PwrCtlCell_Update:1;
    /*!< Set to 1 to indicate PwrCtlCell update. \n
      Note: When PwrCtlCell is updated, FingCfg is required to be updated. 
    */    
    uint32 QLICSectorId_Update:1;
    /*!< Set to 1 to indicate QLICEnable and QLICSectorId update. \n 
      Note: When QLICEnable is updated, FingCfg is required to be updated. 
    */    
    uint32 SectorId_Update:1;
    /*!< Set to 1 to indicate SectorId update. 
    */
    uint32 QofSectorInfo_Update:1;
    /*!< Set to 1 to indicate QofSectorInfo Update
     */
    uint32 fchQofUseQlicSectorERam_Update:1;
    /*!< Set to 1 to indicate fchQofUseQlicSectorERam Update
     */
    uint32 reserved1:26;
    /*!< padding for word alignment */

  uint16 qlicCarrierSelect_Update;
  /*!< Set to 1 to indicate qlicCarrierSelect Update
   *
   * \deprecated { No longer needed in Dime. Shall be ignored }
   */

  uint16 qlicCarrierSelect;
  /*! QLIC Carrier Input Select.
   *
   * \deprecated { No longer needed in Dime. Shall be ignored }
   *
   * This field indicates the carrier from which QLIC fetches its finger data. Use the 
   * following values:
   * 
   * 0 => fetch from primary chain.
   * 3 => fetch from SV chain.
   * 
   */
} cdmafw_finger_assign_t;

/** @brief Multiple Fingers Assignment Command
 *
 *   This is the message definition for the cdmafw Multiple Fingers
 *   Assignment Command
 */
typedef struct {
  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  uint32 fingerMask;
  /*!< Finger Mask
    @verbatim
    Bit 0: set to 1 to indicate fingerAssign[0] update
    Bit 1: set to 1 to indicate fingerAssign[1] update
    Bit 2: set to 1 to indicate fingerAssign[2] update
    Bit 3: set to 1 to indicate fingerAssign[3] update
    Bit 4: set to 1 to indicate fingerAssign[4] update
    Bit 5: set to 1 to indicate fingerAssign[5] update
    Bit 6: set to 1 to indicate fingerAssign[6] update
    Bit 7: set to 1 to indicate fingerAssign[7] update
	@endverbatim
  */   
  cdmafw_finger_assign_t fingerAssign[CDMAFW_NUM_FINGERS];
  /*!< finger assign array  
  */
} cdmafw_multiple_fingers_assign_msg_t;

/** @brief SRCH SW Finger Configuration Command 
 * CDMA FW finger configuration command message structure 
 */

typedef struct {
  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  uint32 fingerIndex:3;
  /*!< Index of the finger to configure. 
  */
  uint32 TTEnable:1;
  /*!< When it's 1 and the finger is in lock, the Time Tracking loop is enabled. 
    When it's 0, advances or retards are not issued. 
  */
  uint16 TTComELGain;
  /*!< TTComELGain is the scale factor used for normalizing the common
    early/late pilot symbols. This value is a function of the diversity
    mode. 
  */
  uint16 TTDivELGain;
  /*!< TTDivELGain is the scale factor used for normalizing the diversity 
    early/late pilot symbols. This value is a function of the diversity 
    mode. 
  */
  uint16 ComPilotGain;
  /*!< ComPilotGain is the scale factor used for normalizing the common pilot
    symbols. This value is a function of the diversity mode. 
  */
  uint16 DivPilotGain;
  /*!< DivPilotGain is the scale factor used for normalizing the diversity 
    pilot symbols. This value is a function of the diversity mode. 
  */
  uint32 CombEnable:1;
  /*!< This register is used to mask the combiner lock state for the finger.
    @verbatim
    0: finger combining is disabled 
    1: finger combining is enabled
    @endverbatim
	Note: If the finger is in the active set, this should be set to 1.
  */
  uint16 RSSIInit;
  /*!< This value is used to initialize the MSW of the finger's RSSI value
    during finger initialization/assignment. The LSW of finger's RSSI value
    is initialized to 0. This is applied to both antennas. \n
	The unit is (Ec/Io/18/2^11).
  */
  uint16 PowerLockThresh;
  /*!< This parameter is compared to the finger RSSI value (MSW) to determine
    whether a finger is in power lock. \n
    The unit is (Ec/Io/18/2^11).
  */
  uint32 ForceLock:1;
  /*!< This bit overrides Com_Finger_Lock and Div_Finger_Lock states of the finger. 
    When it is 1, Com_Finger_Lock state and Div_Finger_Lock state are forced to the 
	state specified in ForceLockState. When Lock_State_Override is 0, ForceLock is 
    disabled and the finger lock state mechanism behaves normally for 
    primary and diversity paths. 
  */
  uint32 ForceLockState:1;
  /*!< This bit is used in conjunction with ForceLock to override Finger_Lock state. 
    When ForceLock is 1, Finger_Lock state takes on ForceLockState value. 
	A value of 1 forces finger in lock, a value of 0 forces finger out of lock. 
	This is applied to both antennas. 
  */
} cdmafw_finger_config_msg_t;

/** @brief SRCH SW Walsh, QOF Configuration Command
 */
typedef struct {
  uint8 qofFn;
  /*!< new field on BOLT, same as DEM1X_Fn_WALSH_ACCm_IMMED__QOF_SEL for CH0/1/2,
       0, 1, 2, or 3 */
  uint8 walshCode;
  /*!< new field on BOLT, same as DEM1X_Fn_WALSH_ACCm_IMMED__WALSH_CODE for CH0/1/2;
       WL is up to 128,  so walsh code is up to 127;
       if CH0/1/2 is not enabled, set walshCode to 0 */
  uint8 walshLengthMask;  
  /*!< new field on BOLT, new param, supported CH0 WL = 64,128;
       supported CH1 WL = 64,128; 
       supported CH2 WL = 4,8,16,32,64,128;
       replace DEM1X_Fn_HW_CH1/2_IMMED__SPR_MASK; 
       Walsh mask configuration:
          0 - WL=4
          1 - WL=8
          2 - WL=16
          3 - WL=32
          4 - WL=64
          5 - WL=128  */

  uint8 reserved0;
  /*!< padding for word alignment */

} cdmafw_finger_walsh_qof_t;

/** @brief SRCH SW Multiple Fingers Configuration Command 
 * CDMA FW multiple finger configuration command message structure 
 */
typedef struct {
  uint16 TTComELGain;
  /*!< TTComELGain is the scale factor used for normalizing the common
    early/late pilot symbols. This value is a function of the diversity
    mode. 
  */
  uint16 TTDivELGain;
  /*!< TTDivELGain is the scale factor used for normalizing the diversity 
    early/late pilot symbols. This value is a function of the diversity 
    mode. 
  */
  uint16 ComPilotGain;
  /*!< ComPilotGain is the scale factor used for normalizing the common pilot
    symbols. This value is a function of the diversity mode. 
  */
  uint16 DivPilotGain;
  /*!< DivPilotGain is the scale factor used for normalizing the diversity 
    pilot symbols. This value is a function of the diversity mode. 
  */
  uint16 RSSIInit;
  /*!< This value is used to initialize the MSW of the finger's RSSI value
    during finger initialization/assignment. The LSW of finger's RSSI value
    is initialized to 0. This is applied to both antennas. \n
	The unit is (Ec/Io/18/2^11).
  */
  uint16 PowerLockThresh;
  /*!< This parameter is compared to the finger RSSI value (MSW) to determine
    whether a finger is in power lock. \n
    The unit is (Ec/Io/18/2^11).
  */
  uint32 TTEnable:1;
  /*!< When it's 1 and the finger is in lock, the Time Tracking loop is enabled. 
    When it's 0, advances or retards are not issued. 
  */
  uint32 CombEnable:1;
  /*!< This register is used to mask the combiner lock state for the finger.
    @verbatim
    0: finger combining is disabled 
    1: finger combining is enabled
    @endverbatim
	Note: If the finger is in the active set, this should be set to 1.
  */
  uint32 ForceLock:1;
  /*!< This bit overrides Com_Finger_Lock and Div_Finger_Lock states of the finger. 
    When it is 1, Com_Finger_Lock state and Div_Finger_Lock state are forced to the 
	state specified in ForceLockState. When Lock_State_Override is 0, ForceLock is 
    disabled and the finger lock state mechanism behaves normally for 
    primary and diversity paths. 
  */
  uint32 ForceLockState:1;
  /*!< This bit is used in conjunction with ForceLock to override Finger_Lock state. 
    When ForceLock is 1, Finger_Lock state takes on ForceLockState value. 
	A value of 1 forces finger in lock, a value of 0 forces finger out of lock. 
	This is applied to both antennas. 
  */
  uint32 ch1Enable : 1;
  /*!< new field on BOLT, same as DEM1X_Fn_HW_CH1_IMMED__ENABLE */
  uint32 ch2Enable : 1;
  /*!< new field on BOLT, same as DEM1X_Fn_HW_CH2_IMMED__ENABLE */
  uint32 chQofPilotEnable : 1;
  /*!< new field on BOLT, same as DEM1X_Fn_WALSH_ACCm_IMMED__ENABLE for qof pilot */

  uint32 pilotPnOffset : 9;
  /*!< new field on BOLT, same as DEM1X_Fn_PILOT_IMMED__PILOT_PN_OFFSET */

  uint32 chQofPilotQofFn : 2;
  /*!< new field on BOLT, QOF Function for QOF_PILOT Channel, 
       same as DEM1X_Fn_WALSH_ACCm_IMMED__QOF_SEL for QOF noise est */

  uint32 reserved0: 14;
  /*!< padding for word alignment */

  cdmafw_finger_walsh_qof_t walshCfg[3];
  /*!< new field on BOLT, Walsh, QOF Functions for Channel0/1/2 */

} cdmafw_finger_config_t;

/*! @brief Multiple Fingers Configuration Command
 *
 *   This is the message definition for the cdmafw Multiple Fingers
 *   Configuration Command
 */
typedef struct {
  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  uint32 fingerMask;
  /*!< Finger Mask
    @verbatim
    Bit 0: set to 1 to indicate fingerConfig[0] update
    Bit 1: set to 1 to indicate fingerConfig[1] update
    Bit 2: set to 1 to indicate fingerConfig[2] update
    Bit 3: set to 1 to indicate fingerConfig[3] update
    Bit 4: set to 1 to indicate fingerConfig[4] update
    Bit 5: set to 1 to indicate fingerConfig[5] update
    Bit 6: set to 1 to indicate fingerConfig[6] update
    Bit 7: set to 1 to indicate fingerConfig[7] update
	@endverbatim
  */   
  cdmafw_finger_config_t fingerConfig[CDMAFW_NUM_FINGERS];
  /*!< finger assign array  
  */
} cdmafw_multiple_fingers_config_msg_t;

/*! @brief Finger Common Parameters Configuration Command
 *
 *   This is the message definition for the CDMA FW Finger Common 
 *   Config Command.
 */
typedef struct {
  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

    /*!< new on BOLT, same as DEM1X_Fn_DIVERSITY_IMMED__MMSE_EN */
  uint16 ttK1Gain;
  /*!< This is the K1 Gain value used in the computation of the ttFreq and 
    ttPhase parameters in the time tracking loop. \n This value is typically set
    to 2048 (0x0800).
  */
  uint16 ttK2Gain;
  /*!< This is the K2 Gain value used in the computation of the ttPhase 
    parameter in the time tracking loop. \n This value is typically set to
    16384 (0x4000).
  */
  uint16 pilotFiltGain;
  /*!< This is the filter coefficient for the Pilot filter. \n The filter coefficient
    should be set to 4096 (0x1000) for Cellular, set it to 8192 (0x2000) for PCS.
    This value is in Q15.
  */
  uint16 fingLowerlockThresh;
  /*!< This parameter is compared to the finger com/div RSSI values to determine
    whether a finger is out of com/div lock. \n
	The unit is (Ec/Io/18/2^11). The default value is 74 (=> Ec/Io = -26.97 dB). 
  */

  boolean qlicEnable;
  /*!< new on BOLT, enable/disable interference cancellation */
  boolean primaryAntIdx;
  /*!< new on BOLT, same as DEM1X_Fn_DIVERSITY_IMMED__PRIMARY_ANT */
  boolean rxd;
  /*!< new on BOLT, replace DEM1X_Fn_DIVERSITY_IMMED__DIV_MODE, reduced to 1 bit */
  boolean mmseEnable;

} cdmafw_finger_common_config_msg_t;

/** @brief OQPCH Search Done 
 *
 * This message must be sent after a OQPCH-based RX_STOP message, and it indicates that 
 * OQPCH searches are done. On receiving this message, FW will free mempool lines.
 */
typedef struct {

  msgr_hdr_struct_type hdr;
  /*!< MSGR header */
  
} cdmafw_oqpch_search_done_msg_t;

/**@}*/

/** 
 * \addtogroup csrchafc
 * @{
 */

/** @brief AFC Mode Parameters
 *
 *   This is the message definition for the cdmafw AFC Mode Command
 */
typedef struct {
  msgr_hdr_struct_type hdr;
  /*!< MSGR header */
  
  uint32 afcMode_Update:1 __attribute__((deprecated));
  /*!< Set to 1 to indicate afcMode update.
  */
  uint32 afcMode:1 __attribute__((deprecated)); 
  /*!< AFC mode configuration (Deprecated as XO AFC is used by default).
	@verbatim
    0: VCTCXO AFC
    1: XO AFC
    @endverbatim
  */
  uint32 afcCrossMode_Update:1;
  /*!< Set to 1 to indicate afcCrossMode update.
  */
  uint32 afcCrossMode:1; 
  /*!< Cross mode configuration in VCTCXO and XO AFC
	@verbatim
    0: crossmode 0 (CM0), use scaled-filtered pilot to compute frequency error.
    1: crossmode 1 (CM1), use raw pilot to compute frequency error.
    @endverbatim
    Note : Typically, CM1 is used in acquisition and CM0 is used in tracking. 
  */
  } cdmafw_afc_mode_msg_t;

/** @brief XO AFC Static Parameters
 *
 *   This is the message definition for the cdmafw XO AFC Static
 *   Configuration Command.
 */
typedef struct {
  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  uint16 afcTxRxFreqRatio;
  /*!< Ratio of Tx frequency / Rx frequency
    The formula is (Tx Freq/Rx Freq) x (2^14).
  */
  uint16 afcRxFreqGain;
  /*!< Gain used for MND update.
    The formula for SC2x is (0.125 x 1.2288 x 10^8) / (Rx Freq in MHz).
    The formula is different for different PLL setting used for CHIPXN.
  */
  uint32 afcXoInvRxFreq;
  /*!< Gain used for PPM computation.
   * It is defined as 2^33/f_DL(Hz) in Q19 format, where f_DL(Hz) is the
   * downlink frequency in Hz.
   */
  uint32 afcXoMndMBase;
  /*!< MND M base.
  */
  uint32 afcXoMndNBase;
  /*!< MND N base.
  */
  uint16 afcXoMndRndFactor;
  /*!< MND Rounding factor.
  */
  uint16 afcXoMndRndBits;
  /*!< MND number of rounding bits.
  */
  uint32 rpushParamsUpdate:1;
  /*!< Enable latching for GPS RPUSH filter parameters */
  int32 rpushEcIoFiltTcShift;
  /*!< Time constant shift value for Ecp/Io filter. Time constant would be
   * 1/2^shift in (64*4) chip time units */
} cdmafw_xo_afc_static_config_msg_t;

/** @brief XO AFC Dynamic Parameters
 *
 *   This is the message definition for the cdmafw XO AFC Dynamic
 *   Configuration Command.
 */
typedef struct {
  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  uint32 afcXoGain_Update:1;
  /*!< Set to 1 to indicate afcXoGain update.
  */
  uint16 afcXoGain;
  /*!< Rotator loop gain.
  */
  uint32 afcXoLoadStb_Update:1;
  /*!< Set to 1 to indicate afcXoLoadStb and afcXoLoadFreqAcc update.
  */
  uint32 afcXoLoadStb:1;
  /*!< Set to 1 to load afcLoadFreqAccVco and afcLoadFreqAccRot. 
  */
  uint16 afcXoLoadFreqAcc;
  /*!< Initialization value, used in conjunction with afcXoLoadStb to initialize the 
    state of the RX rotator frequency accumulator. If afcXoLoadStb is 1, then this
    value is loaded into the upper 16 bits of rotator accumulator. 
    The unit is (4.6875/2^16) Hz.
  */
} cdmafw_xo_afc_dynamic_config_msg_t;
/**@}*/

/**
 * \addtogroup cmux
 * @{
 */
/** @brief FL Signaling Parameters
 *
 *   This is the message definition for the cdmafw FL Signaling 
 *   Command.
 */
typedef struct {
  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  uint32 fliRC11Enable_Update:1;
  /*!< Set to 1 to indicate fliRC11Enable update.
  */
  uint32 fliRC11Enable:1;
  /*!< FL RC11 enable flag
	@verbatim
    0: RC11 disabled
    1: RC11 enabled
	@endverbatim
  */
  uint32 fliFchAckMask_Update:1;
  /*!< Set to 1 to indicate fliFchAckMask update.
  */
  uint16 fliFchAckMask;
  /*!< ACK Mask for R-FCH early termination
    @verbatim
    Bit(n) is for PCG (15-n), n=0..15
    Bit(n)=0: ACK for R-FCH is not punctured in PCG (15-n) of F-FCH
    Bit(n)=1: ACK for R-FCH is punctured in PCG (15-n) of F-FCH
	@endverbatim
    Latched by FW at the frame boundary. The configuration of this variable should 
	be from REV_FCH_ACK_MASK field of RCPM.
  */
  uint32 fliSchAckMask_Update:1;
  /*!< Set to 1 to indicate fliSchAckMask update.
  */
  uint16 fliSchAckMask;
  /*!< ACK Mask for R-SCH early termination
    @verbatim
    Bit(n) is for PCG (15-n), n=0..15
    Bit(n)=0: ACK for R-SCH is not punctured in PCG (15-n) of F-CACKCH
    Bit(n)=1: ACK for R-SCH is punctured in PCG (15-n) of F-CACKCH
	@endverbatim
    Latched by FW at the frame boundary. The configuration of this variable should 
	be from REV_SCH_ACK_MASK field of RCPM.
  */
  uint32 fliBlankingPeriod_Update:1;
  /*!< Set to 1 to indicate fliBlankingPeriod update.
  */
  uint32 fliBlankingPeriod:3;
  /*!< FL blanking period
	@verbatim
    0: blanking period = 1 (blanking disabled)
    3: blanking period = 4 20-ms frames
    7: blanking period = 8 20-ms frames
	@endverbatim
    Latched by FW at the frame boundary. The configuration of this variable should
	be based on FOR_N field of RCPM.
  */
  uint32 fliFchEarlyDecodePCGMask_Update:1;
  /*!< Set to 1 to indicate fliFchEarlyDecodePCGMask update.
   */
  uint16 fliFchEarlyDecodePCGMask;
  /*!< FL FCH Early Decode PCG Mask 

    @verbatim
    Bit(n) is for PCG (15-n), n=0..15
    Bit(n) = 0: no F-FCH early decoding attempt at PCG (15-n)
    Bit(n) = 1: F-FCH early decoding attempt at PCG (15-n)
    @endverbatim
    
    FW latches the configuration from SW at the frame boundary.
    
    The configuration of this variable should be derived from the value programmed in 
    rliFchAckMask. If R-ACKCH can be transmitted at PCG n, FL early decoding can be 
    triggered at PCG n-1. Since bit 15 (RL PCG0) and bit 0 (RL PCG15) of rliFchAckMask 
    should always be 0, bit 0 (FL PCG15) and bit 1 (FL PCG14) for this variable should 
    always be 0 too.  There is no early decoding for PCG0 and PCG1, so bit 15 (FL PCG0) 
    and bit 14 (FL PCG1) should always be 0 too.
  */
  
} cdmafw_fl_signaling_msg_t;
/**@}*/

/**
 * \addtogroup csrch
 * @{
 */
/*! @brief APF Configuration Message
 *
 * This message configures the Adaptive Pilot Filter 
 */
typedef struct {

  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  uint32 apfEnable:1;
  /*!< APF Enable.
   * Set to 1 to enable APF, 0 otherwise (legacy pilot filter will be used).
   */

  uint32 apfStep;
  /*!< APF Step Parameter.
   * Should be in Q30 format. The default setting should be 16. This value should be eventually
   * determined by Modem Systems.
   */
} cdmafw_apf_enable_msg_t;
/**@}*/

/**
 * \addtogroup cmux
 * @{
 */
/*! @brief FPC Freeze limit 
 *
 * This is the message definition for the FPC freeze command.
 */
typedef struct {

  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  uint32 fliFchFpcFreezeEnable:1;
  /*!< FPC freeze enable */

  uint32 : 15;
  uint32  fliFchFpcFreezeLimit:16;
  /*!< FPC Freeze limit*/

} cdmafw_fpc_freeze_cfg_msg_t; 

/*! @brief FPC override test pattern */
typedef struct { 

  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  uint32 fliFPCOverride:1;
  /*!< FPC Override mode */
  
  uint32 : 15;

  uint32 fliFPCTestPattern:16;
  /*!< FPC Test pattern Mask if override mode is enabled*/

} cdmafw_fpc_override_msg_t;


/*! @brief XPT capture message. Carries info specific to xpt sample capture.
*/
typedef struct {
  /*! @brief Message header */
  msgr_hdr_struct_type hdr;

  /*! @ brief Specify sample capture parameters, including buffer ID's,
   * response sequence ID */
  fw_rf_capture_param_t captureParam;

}cdmafw_xpt_capture_msg_t;

/*! @brief XPT_CAPTURE_RSP Message Response 
 *
 * \sa cdmafw_xpt_capture_rsp_msg_t
 */
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type hdr;

  /*! \brief DPDindex for which this capture was done */
  uint16  DPDindex;

  /*! \brief Sequence number for tracking request and response messages */
  uint32  sequenceNumber;

}cdmafw_xpt_capture_rsp_msg_t;

/**@}*/

/**
 * \addtogroup csrch
 * @{
 */

/*! @brief QLIC Walsh Power Logging
 *
 *   This is the message definition for the cdmafw QLIC Walsh Power 
 *   Logging Command.
 */
typedef struct {
  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  uint16 walshPwrSector:4;
  /*!< QLIC Sector ID for logging
	@verbatim
    Valid ID: 0~9
	@endverbatim
  */
} cdmafw_qlic_walsh_pwr_logging_msg_t;
/**@}*/

/**
 * \addtogroup cmux
 * @{
 */

/*! @brief SCH IQ Accumulation Scale
 *
 *   This is the message definition for the cdmafw SCH IQ Acc Scale 
 *   Command.
 */
typedef struct {
  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  uint16 schIqAccScale;
  /*!< Scaling applied to the IQ_ACC (Eb estimator) for the SCH FPC and DTX detection.
  */
} cdmafw_sch_acc_scale_msg_t;

/*! @brief FCH Target Setpoint
 *
 *   This is the message definition for the cdmafw FCH Target Setpoint 
 *   Configuration Command.
 */
typedef struct {
  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  uint16 ebNtMagTargetFch;
  /*!< (Q10) This is the target threshold used in the fundamental channel forward power
    control decision. This is the scaled magnitude ratio (not energy), which is set to
    @verbatim
    sqrt(pi/2).sqrt((Eb(PCB)/Eb(FCH)).Eb(FCH)/Nt).2^10
    @endverbatim
    where "Eb(PCB)/Eb(FCH)" is the PCB to fundamental channel ratio derived from
    signaling
  */
} cdmafw_fch_target_setpoint_msg_t;

/*! @brief SCH Target Setpoint
 *
 *   This is the message definition for the cdmafw SCH Target Setpoint 
 *   Configuration Command.
 */
typedef struct {
  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  uint16 ebntMagTargetSch;
  /*!< (Q10) This is the target threshold used in the supplemental channel forward
    power control decision. This is the scaled magnitude ratio (not energy), which is
    set to
    @verbatim
    sqrt(pi/2).sqrt(Eb(SCH)/Nt).2^10
    @endverbatim
    where Eb(SCH)/Nt is the desired SNR target.
 */
} cdmafw_sch_target_setpoint_msg_t;

/*! @brief TDEC Bridge Configuration
 *
 * This message configures (enables/disables) the TDEC bridge and SW mempool AHB access
 * to DEMBACK output data. It also registers mempool lines with CRM FW (required to keep
 * the mempool clock alive).
 */
typedef struct {

  /*!< MSGR header */
  msgr_hdr_struct_type hdr;

  /*!< Enable flag
   *
   * 1 => enable.
   * 0 => disable.
   */
  boolean tdecEnable;

} cdmafw_tdec_brdg_config_msg_t;

/*! @brief DEMBACK SW Configuration.

    This message allows SW to configure FW to either use VOICE mode in HW demback or buffer CH0
    symbols for SW decoding. If swDembackEn is set to TRUE, then FW does not touch demback HW, and
    instead buffers CH0 symbols in SMEM for SW decode usage. Otherwise, FW enables VOICE mode in HW
    demback. Enabling one mode will implicitly disable the other mode.

    Once FW is configured to use "SW mode" it is not expected for SW to enable "DATA" or "TDEC" 
    modes using DEMBACK_MODE_CONFIG message. "SW mode" must be disabled prior to enabling these 
    modes.

    To mimic legacy HW usage, it is best to send this message as early as possible (ex: right after
    FW goes to ACTIVE state).

*/
typedef struct {

  /** MSGR Header */
  msgr_hdr_struct_type hdr;

  /** Backdoor debug flag that enables both SW and HW demback modes (for debug).

      For DEBUG use only. Never enable on target. Only latched if swDembackEn
      is set to TRUE.

    */
  boolean debugModeEn;

  /** Set this to TRUE to update swDembackEn */
  boolean swDembackEn_Update;
  
  /** Demback SW Configuration.

      0 => Use VOICE mode in HW demback.
      1 => Use SW demback and enable CH0 SMEM buffer.

    */
  boolean swDembackEn;

  /** Set this to TRUE to update syncState */
  boolean syncStateEn_Update;

  /** Setup SYNC state (80ms or 20ms).

    0 => Ping Pong for CH0 SMEM happens every 20 ms.
    1 => Ping Pong for CH0 SMEM happens every 80 ms.

    Please note that this flag is checked at every 20ms frame boundary.

 */
  boolean syncStateEn;

} cdmafw_demback_sw_config_msg_t;

/*! @brief DEMBACK_MODE_SEL and TDEC Bridge Configuration
 * This message is used to configure 1x Demback in 1X_VOICE/1X_DATA/1X_VOICE+1X_DATA modes.
 * Additionally, this message configures (enables/disables) the TDEC bridge and SW mempool AHB access
 * to DEMBACK output data. It also registers mempool lines with CRM FW (required to keep
 * the mempool clock alive).
 */
typedef struct {

  /*!< MSGR header */
  msgr_hdr_struct_type hdr;

  /*!< Enable flag
   *
   * 1 => enable (configure 1x data mode in DEMBACK_MODE_SEL)
   * 0 => disable (disable 1x data mode in DEMBACK_MODE_SEL)
   */
  uint16 dataEnable;

  /*!< Set to 1 for tdecEnable update */
  uint16 tdecEnable_Update;
  /*!< Enable flag
   *
   * 1 => enable and config TDEC bridge
   * 0 => disable and free TDEC bridge
   */
  uint16 tdecEnable;

} cdmafw_demback_mode_config_msg_t;
/**@}*/

/** DSDX State Definition. */
typedef enum {
  /*! CxM based DSDX is inactive.Single sim without CxM solution */
  DSDX_STATE_INACTIVE = 0,

  /*! CxM based DSDA is active */
  DSDA_STATE_ACTIVE,

  /*! CxM based Dual Receive DSDS is active */
  DR_DSDS_STATE_ACTIVE,
  /* CxM based 1x->X QTA for DSDS is active */
  DSDS_1X2X_QTA_STATE_ACTIVE

} cdmafw_dsdx_state_t;

/*! @brief DSDA Priority Config Parameters
 *
 *   This is the message definition for the cdmafw
 *   dsda priority Configuration Command.
 */
typedef struct {
  uint32 rxPriority[CDMAFW_NUM_ANTENNAS];
  uint32 rxTunePriority[CDMAFW_NUM_ANTENNAS];
  /*!< Priority of activity. Higher value indicates higher priority.There is no
   * concept of Macro or Micro priority in FW. L1 resolves Macro/Micro priority
   * and passes one single priority value to Tech FW.
  */
  /*!< rxPriority is used by L1 to set the Rx
   * priority. rxTunePriority is used to set RX_START/STOP priority per antenna
     A value of 0xFFFFFFFF indicate priority not valid.
  */
  uint32  rxFrequencyId;
  /*!< RX Frequency ID.
  */
  uint32 rxChannelId[CDMAFW_NUM_ANTENNAS];
  /*!< Per antenna RX channel ID.
  */
  uint32 registerRxTuneActivity[CDMAFW_NUM_ANTENNAS];
  /*!< = 0 --> Do not register Rx tune activity
       = 1 --> Register Rx tune activity
  */
} cdmafw_rx_dsdx_config_t; 

typedef struct {
  uint32 txPriority;
  uint32 txTunePriority; 
  /*!< Priority of activity. Higher value indicates higher priority.There is no
   * concept of Macro or Micro priority in FW. L1 resolves Macro/Micro priority
   * and passes one single priority value to Tech FW.
  */
  /*!< txPriority is used by L1 to set the Tx
   * priority. txTunePriority is used to set TX_START priority. 
     A value of 0xFFFFFFFF indicate priority not valid.
  */
  uint32  txFrequencyId;
  /*!< TX Frequency ID.
  */
  uint32 txChannelId;
  /*!< TX channel ID.
  */
  uint32 registerTxTuneActivity;
  /*!< = 0 --> Do not register Tx tune activity
       = 1 --> Register Tx tune activity
  */
  uint32 asdivPriority;
  /*!< asdiv priority used by L1 to set the priority during ASDIV event */ 

} cdmafw_tx_dsdx_config_t;

typedef struct {
  cdmafw_dsdx_state_t dsdxMode;
  /*!< flag to select DSDx states */
  cdmafw_rx_dsdx_config_t rx;
  /*!< DSDx Rx config params */
  cdmafw_tx_dsdx_config_t tx;
  /*!< DSDx Tx config params */
} cdmafw_dsdx_config_t; 

typedef struct {
  msgr_hdr_struct_type hdr;
  /*!< MSGR header */
  cdmafw_dsdx_config_t config;
  /*!< DSDx config */
} cdmafw_dsdx_config_msg_t;

/**
 * \addtogroup clrfa
 * @{
 */
/*! @brief 1x->X QTA Start message */
typedef struct {
  msgr_hdr_struct_type hdr;
  /*!< MSGR header */
} cdmafw_1x2x_qta_start_msg_t;

/*! @brief 1x->X QTA Stop message */
typedef struct {
  msgr_hdr_struct_type hdr;
  /*!< MSGR header */
} cdmafw_1x2x_qta_stop_msg_t;

/*! @brief Timing logging struct for QTA RF Start tune per antenna */
typedef struct {
  uint32 rf_tune_start_ustmr;
  /*!< RTC time stamp in cx8 when rf tune scripts are called for this antenna */
  uint32 settling_time_ustmr;
  /*!< RF tune settling time for this antenna in cx8 units */
  uint32 rf_tune_done_ustmr;
  /*!< RF tune done time stamp for this antenna in RTC cx8 units */
  uint32 agc_acq_done_time_ustmr;
  /*!< Rx AGC Acq done time stamp for this antenna in RTC cx8 units */
} cdmafw_qta_rf_tune_timing_t;

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

} cdmafw_tx_pa_params_config_msg_t;
/*! @brief Timing logging struct for 1x->X QTA Start */
typedef struct {
  uint32 start_rcvd_time_ustmr;
  /*!< RTC time stamp in cx8 when QTA START msg is received */
  uint32 cxm_yield_time_ustmr;
  /*!< RTC time stamp in cx8 when CXM Yield was received */
  uint32 status_query_rcvd_time_ustmr;
  /*!< RTC time stamp in cx8 when QTA STATUS QUERY msg is received and rsp sent to L1 */
  uint32 rx_chains_free_time_ustmr;
  /*!< RTC time stamp in cx8 when WB/NB chains are freed */
  uint32 start_rsp_time_ustmr;
  /*!< RTC time stamp in cx8 when QTA START rsp send for convenience of 1x L1 */
} cdmafw_1x2x_qta_start_timing_t;

/*! @brief 1x->X QTA Start response message */
typedef struct {
  msgr_hdr_struct_type hdr;
  /*!< MSGR header */
  cdmafw_1x2x_qta_start_timing_t timing;
  /*!< QTA start related timing info for logging*/
} cdmafw_1x2x_qta_start_rsp_msg_t;

/*! @brief Timing logging struct for 1x->X QTA Stop */
typedef struct {
  uint32 cxm_proceed_time_ustmr;
  /*!< RTC time stamp in cx8 when CXM Proceed was received */
  uint32 rf_tune_start_ustmr;
  /*!< RTC time stamp in cx8 when rf tune scripts are called for this antenna */
  uint32 rf_tune_done_ustmr;
  /*!< RF tune done time stamp for this antenna in RTC cx8 units */
  uint32 qta_rxagc_acq_done_ind_time_ustmr;
  /*!< RTC time stamp in cx8 when RXAGC ACQ Done Ind is send */
  uint32 stop_rcvd_time_ustmr;
  /*!< RTC time stamp in cx8 when QTA_STOP message is received */
  uint32 stop_rsp_time_ustmr;
  /*!< RTC time stamp in cx8 when qta stop rsp send */
} cdmafw_1x2x_qta_stop_timing_t;

/*! @brief 1x->X QTA Stop response message */
typedef struct {
  msgr_hdr_struct_type hdr;
  /*!< MSGR header */
  cdmafw_1x2x_qta_stop_timing_t timing;
  /*!< QTA stop related timing info for logging*/
} cdmafw_1x2x_qta_stop_rsp_msg_t;

/** QTA Status query message */
typedef struct {
  /*!< MSGR header */
  msgr_hdr_struct_type hdr;
} cdmafw_1x2x_qta_status_query_msg_t;

/** QTA Status query response type */
typedef enum {
  /*! Default status if QTA */
  CDMAFW_1X2X_QTA_START_NOT_RCVD = 0,
  /*! QTA gap is empty as no YIELD received
   * since last QTA START */
  CDMAFW_1X2X_QTA_EMPTY_GAP,
  /*! QTA gap is not empty */
  CDMAFW_1X2X_QTA_YIELD_RCVD,
} cdmafw_1x2x_qta_status_rsp_t;

typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  cdmafw_1x2x_qta_status_rsp_t qta_status;
} cdmafw_1x2x_qta_status_query_rsp_msg_t;

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
  
} cdmafw_tx_limit_config_msg_t;

/*! @brief Set PA Range Message
 *
 * Overrides PA range (works both in normal and override TxAGC mode). SW can use 
 * this to bypass the PA state machine.
 * 
 */
typedef struct {
  
  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  uint16 paRangeOverride:1;
  /*!< PA Range Override flag */
  
  uint16 paRangeWr:2;
  /*!< PA Range Value (0-3) */

} cdmafw_tx_pa_range_override_msg_t;

/*! @brief TX Override Message
 *
 * Override TxAGC.
 */
typedef struct {

  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  uint16 txOverride:1;
  /*!< Override flag 
   *
   * When this flag is set, FW enters override mode, where intial TxAGC
   * computations (OL, CL, RA) are skipped and FW will use RF SW provided 
   * values for writing HW registers
   */
  
  uint16 paStateWr;
  /*!< PA State (for override) 
   *
   * PA State (0-3) for override. FW will apply PA range map programmed by
   * SW.
   * 
   */

  uint32 txAgcAdjWr;
  /*!< AVGA Gain Word (for override)
   *
   * 32-bit AVGA gain word (for override).
   */

  int16 txBetapGainWr;
  /*!< BETAP Gain (in dB units).
   *
   * BETAP Gain (-1/640 dB units) for override. FW will convert this into 
   * linear units before writing to BETAP gain register.
   */

} cdmafw_tx_override_msg_t;

/*! @brief Override TX Closed Loop 
 *
 * Override TX closed loop value. Both Normal and Trigger-Mode
 * overrides supported.
 * 
 */
typedef struct {

  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  uint16 txGainAdjOverride:1;
  /*!< Tx closed-loop power accumulator override request
	@verbatim
    0: normal mode, accumulator is updated by FW
    1: Override with the value specified in cTxGainAdjWr
	@endverbatim
  */
  
  uint16 txGainAdjClrOverride:1;
  /*!< Tx closed-loop power accumulator override clear request
	@verbatim
    0: stay in the override mode until cTxGainAdjOverride is set to 0
    1: go back to normal mode after taking one overridden value
	@endverbatim
  */

  int16 txGainAdjWr;
  /*!< Tx closed-loop power accumulator override value.
    The unit is -1/512 dB.
  */

} cdmafw_tx_closed_loop_override_msg_t;

/*! @brief Override TX Open Loop Message
 *
 *  Overrides TX open-loop value.
 *
 */
typedef struct {

  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  uint16 txOpenLoopOverride:1;
  /*!< Open Loop Override flag.
   *
   * 0 -> no override (normal mode).
   * 1 -> override.
   */

  int16 txOpenLoopWr;
  /*!< Open Loop Override Value.
   *
   * 1/640 dB units.
   */

} cdmafw_tx_open_loop_override_msg_t;

/*! @brief Override PA state.
 *
 * Overrides PA state (in normal mode TXAGC operation) - in override mode
 * PA state is already overrriden.
 *
 */
typedef struct {

  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  uint16 paStateOverride:1;
  /*!< PA State Override flag 
   *
   * 0 -> no override.
   * 1 -> override.
   */

  uint16 paStateWr:2;
  /*!< PA State Override Value */

} cdmafw_tx_pa_state_override_msg_t;

/*! @brief Set Frequency Offset
 *
 * This commands is used to set a frequency offset for 1X. Pre-SVDO, the purpose
 * of this message is to implement the ULIF scheme required for RTR8600.
 */
typedef struct {

  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  uint32 rxFrequencyOffset;
  /*!< Specify RX rotator frequency offset for 1X*/
  
} cdmafw_rx_set_freq_offset_msg_t;

/*! @brief Stop txagc updates
 *
 * This command stops all FW TXAGC updates. (typically used for XPT setup)
 * ************************************************** 
 * NOTE: READ THE NOTE * ABOUT stop_txagc_gain_updates below.
 * **************************************************
 * */
typedef struct {

  /*!< MSGR header */
  msgr_hdr_struct_type hdr;

  /*!< Sequence number to track command response messages */
  uint32 sequence_number;

  /*!< Boolean flag that indicates FW to stop ALL TXAGC updates.  If set to
   * TRUE, FW will bypass all TX code execution. If this command is sent to
   * FW with this set to TRUE, and at a later point RFSW wants to send any TX
   * commands to FW, this command needs to be resent to FW with flag set to
   * FALSE. Basically, if you disable FW TXAGC updates, you have to re-enable
   * them by sending this command again before sending any other TX commands.
   * */
  boolean stop_txagc_gain_updates;

} cdmafw_stop_txagc_update_msg_t;

/*! @brief RF Notch Filters Configuration Message
 *
 * This message configures the RF Notch Filters. FW will always write to 
 * notch registers as long as L1 gives us the corresponding RF chain. RF
 * SW can enable/disable notch filters at a higher level in HW.
 */
typedef struct {

  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  uint32 antenna;
  uint32 rxlm_buffer_idx;
  uint32 rxlm_config_mask;


} cdmafw_rxlm_config_masked_msg_t;

/** DEMBACK_SW_CONFIG done response.

  FW sends this response once a DEMBACK_SW_CONFIG command has been processed.

  \sa cdmafw_demback_sw_config_msg_t

*/
typedef struct {

  /** MSGR header */
  msgr_hdr_struct_type hdr;

} cdmafw_demback_sw_config_rsp_msg_t;

/*! @brief Tx High Power Detection
 *
 *   This is the message definition for the cdmafw Tx High Power 
 *   Detection Command.
 */
typedef struct {
  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  uint32 cTxPowerDetEnable:1;
  /*!< TxAGC Override Flag
    @verbatim
    0: Disable 
    1: Enable
    @endverbatim
  */
  uint16 cTxPowerDetThresh;
  /*!< The unit is in -1/640 dB resolution
  */
} cdmafw_tx_high_pwr_det_msg_t;
/*@}*/

/**@}*/

/**
 * \addtogroup cmux
 * @{
 */
/*! @brief 1xAdvance Half-PCG Logging Configuration 
 *
 * Configure 1xAdvance half-PCG logging.
 */
typedef struct {

  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  uint16 logEnable:1;
  /*!< Enable logging features (1 = enable, 0 = disable) */

  uint16 logMsgEnable:1;
  /*!< When enabled (=1), FW will send log indication messages (every 8 
   * PCGs). When disabled (=0), no log indication messages will be sent.
   */
} cdmafw_adv1x_log_config_msg_t;
/**@}*/


/*============================================================================
 *
 * INDICATION MESSAGES
 *
 * ==========================================================================*/
/**
 * \addtogroup cind
 * @{
 */

/** 
 * @brief SRCH SW RxTx Frame Stats Indication 
 * CDMA1x FW RxTx frame stats indication message structure 
 */
typedef struct
{
  uint16 ComPilotRawI; 
  /*!< Raw Pilot */
  uint16 ComPilotRawQ;
  /*!< Raw Pilot */
  uint16 DivPilotRawI;
  /*!< Raw Pilot */
  uint16 DivPilotRawQ;
  /*!< Raw Pilot */
  uint16 ComPilotFIltI;
  /*!< Filtered Pilot */
  uint16 ComPilotFiltQ;
  /*!< Filtered Pilot */
  uint16 DivPilotFiltI;
  /*!< Filtered Pilot */
  uint16 DivPilotFiltQ;
  /*!< Filtered Pilot */
  uint16 ComFingLock;
  /*!< Finger Lock */
  uint16 DivFingLock;
  /*!< Finger Lock */
  uint16 ComFiltRSSI;
  /*!< Finger estimated Ecp/Io */
  uint16 DivFiltRSSI;
  /*!< Finger estimated Ecp/Io */
  uint16 MmseFiltA11I;
  /*!< MMSE Correlation coefficients */
  uint16 MmseFiltA01I;
  /*!< MMSE Correlation coefficients */
  uint16 MmseFiltA01Q;
  /*!< MMSE Correlation coefficients */
  uint16 MmseFiltA00I;
  /*!< MMSE Correlation coefficients */
} cdmafw_fing_stats_t;

/*! @brief RxTx Frame Stats
 *
 *   This is the message definition for the cdmafw RxTx frame 
 *   Stats Indication.
 */
typedef struct {
  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  cdmafw_fing_stats_t fingerStats[CDMAFW_NUM_FINGERS];
  /*!< finger stats */
  uint16 rpcHistory;
  /*!< This parameter contains the reverse power control decisions for the most recent frame. 
    The MSB of rpcHistory is the oldest or first rcp decision of the frame, the LSB is the most 
	recent or last rpc decision of the frame. rpcHistory is valid at the beginning of the next frame, 
	however, software should wait for 1 symbol into the next frame before reading rpcHistory due to 
	potential processing latencies of the combiner request. 
  */
  uint16 rpcHistoryValid;
  /*!< This parameter indicates whether a particular RPC decision in rpcHistory was a valid decision. 
    A one in the corresponding bit position of rpcHistory would indicate that the RPC decision was valid 
	for that PC symbol. A zero would indicate that no fingers were in power lock for that PC symbol, and 
	therefore no RPC decision was output to the AGC block. rpcHistoryValid is valid when rpcHistory is 
	valid.
  */
  uint16 fpcHistory;
  /*!< This parameter contains the forward power control decisions for the most recent frame. The MSB 
    of fpcHistory is the oldest or first FPC decision of the frame, the LSB is the most recent or last 
	FPC decision of the frame. fpcHistory is valid at the beginning of the next frame, however, software 
	should wait for 1 symbol into the next frame before reading fpcHistory due to potential processing 
	latencies of the combiner request.
  */
  uint16 pcBitAccFrame;    
  /*!< Power control sub-channel energy estimate accumulated over a frame
  */
  uint16 ch0AccFrame;  
  /*!< Accumulation of symbol magnitudes (abs(I) + abs(Q)) over the frame for channel 0 (FCH/DCCH) 
  */
  uint16 ch2AccFrame;
  /*!< Accumulation of symbol magnitudes (abs(I)+abs(Q)) over the frame for channel 2 (SCH) 
  */
  uint16 fundWeightedNtMagAccFrame;  
  /*!< This is the fundamental channel weighted Nt magnitude estimate accumulated over a frame 
  */ 
  uint16 suppWeightedNtMagAccFrame;
  /*!< This is the supplemental channel weighted Nt magnitude estimate accumulated over a frame 
  */
  uint16 fundWeightedPilotAccFrame;   
  /*!< Accumulation of the weighted pilot magnitude for the FCH over a frame 
  */
  uint16 suppWeightedPilotAccFrame;
  /*!<  Accumulation of the weighted pilot magnitude for the SCH over a frame 
  */
  uint16 cTxGainAdjVal;
  /*!< Closed loop power accumulator in -1/512 dB resolution 
  */
  uint16 cTxRateAdjVal;
  /*!< TX_RATE_ADJ in -1/640 dB resolution 
  */
  uint16 cTxTotalGainVal;
  /*!< Tx Total in -1/640 dB resolution 
  */
  uint16 rlMaxTxTotal;
  /*!< Max Tx limit in -1/640 dB resolution 
  */
  uint16 rlMinTxTotal;
  /*!< Min Tx limit in -1/640 dB resolution 
  */
} cdmafw_rxtx_frame_stats_ind_msg_t;

/*! @brief QLIC Frame Stats
 *
 *   This is the message definition for the cdmafw QLIC Frame 
 *   Stats Indication.
 */
typedef struct {
  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

} cdmafw_qlic_frame_stats_ind_msg_t;

/*! @brief RC11 FCH Decoder Done
 *
 *   This is the message definition for the cdmafw RC11 FCH
 *   Decoder Done Indication.
 */
typedef struct {
  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  uint32 fliRC11FchRateInfo:5;
  /*!< RC 11 F-FCH rate determination result
    @verbatim
    0: Erasure
    1: Eighth rate
    2: Quarter rate
    3: Half rate
    4: Full rate
    @endverbatim
  */
  uint32 fliValidFPCFrame:1;
  /*!< Valid FPC frame flag
    @verbatim
    0: SW should not use this frame for channel supervision or adjusting FPC set point.
    1: SW should use this frame for channel supervision and adjusting FPC set point. 
    @endverbatim
	FW does not determine whether its erasure or blank frame. Based on the frame counter, 
	frame offset and the configuration of fliBlankingPeriod, FW determines whether its a 
	guaranteed non-null-rate frame or not.
  */
  
  uint32 fliFrameIndex:3;
  /*!< Frame Counter
   * A 3-bit frame count for the decoded frame.
   */
  
  uint32 fliFchDecodePCGIndexMinusOne:4;
  /*!< Decoded PCG Indicator
     SW shall use (fliFchDecodePCGIndexMinusOne+1) for PCG index in MAC counter.

     If (fliFchDecodePCGIndexMinusOne != 14),  this means:
     - Early decoding happens at symbol 16 of Rx PCG (fliFchDecodePCGIndexMinusOne +1) for the current frame. 
       There's no early (partial) decoding at symbol 16 of Rx PCG0, PCG1, PCG14, PCG15.
     - The number of symbols for decoding = 24*( fliFchDecodePCGIndexMinusOne +1)+16
     If (fliFchDecodePCGIndexMinusOne == 14), this means:
     - Full frame decoding happens at symbol 0 of Rx PCG 0 for the previous frame;
     - The number of symbols for decoding = 24*( fliFchDecodePCGIndexMinusOne +1)+24=384.
  */
  
} cdmafw_rc11_fch_decoder_done_ind_msg_t;

/*! @brief Adv1X Logging Buffer Ready
 *
 *   This is the message definition for the cdmafw ADV1X Logging 
 *   Buffer Ready Indication.
 */
typedef struct {
  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  int16 logBufferReadIndex;
  /*<! Log Buffer Read Index
   * 
   * SW must use this read index to read from the log buffer. The log buffer
   * is defined the SW readable SMEM region.
   * 
   * \sa cdmafw_smem_read_intf_t.
   */

} cdmafw_adv1x_log_buffer_rdy_ind_msg_t;

/** @brief Indicator message from 1x FW to 1x L1 to indicate AGC ACQ is complete
 */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
} cdmafw_qta_rxagc_acq_done_ind_msg_t;

/*! @brief VI mode control indication
 *
 * This message is used by CDMA FW to send indication messages
 * to VI thread. 
 */
typedef struct {
  msgr_hdr_struct_type hdr;
} cdmafw_vi_ctl_ind_msg_t;

/** @brief SSBI TQ Stuck Message
 * 
 * \sa cdmafw_reset_req_ind_msg_t.  
 */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
} cdmafw_reset_req_ind_msg_t;

/** @brief DEPRECATED. NOT USED ANYMORE.
 * Indicator message from FW after all the processing on all the
 * captures is done. At this point firmware has the compressed DPD tables in
 * its memory. Note the keyword "compressed". This means the DPD tables are
 * not in usable state by TxAGC code yet. Another indicator message will be
 * sent when the expanded tables are ready in firmware. * 
 * \sa .  
 *
 */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
} cdmafw_ept_proc_done_msg_t;

/** @brief Indicator message from FW after expanded DPD tables are available.
 * SW can start steps for sweep 3 on recieving this indicator because TxAGC
 * firmware has the DPD tables ready. In online mode, this might be used as
 * an indicator to firmware expansion finished within realtime deadline.
 * \sa .  
 */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  uint32                    sequenceNumber;
} cdmafw_ept_expansion_done_msg_t;

/**@}*/


/*============================================================================
 *
 * RESPONSE MESSAGES
 *
 * ==========================================================================*/
/**
 * \addtogroup cresp
 * @{
 */

/*! 
 * @brief app_enable Search Response
 * 
 * This is a message definition for the app_cgr response to be sent to SW to 
 * indicate to SW that cdma APP has been enabled.
 *
 * @deprecated This message is deprecated in Nike-L.
 *
 */
typedef struct {
  
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  /** Current status of the CDMA app. */
  uint16                    statusEnabled:1;
  
  /*! \detailed If statusIgnored == 1, the previous app_cfg command is redundant.
      i.e cdma is already running (disabled) when firmware receives the enable (disable)
      spr message. */
  uint16                    statusIgnored:1;
  uint16                    cdmaVersion;
  
  /*! \brief CDMA shared mem start address and size */
  uint32                    cdmaSmemStartAddr;
  uint16                    cdmaSmemSize;
} cdmafw_app_cfg_rsp_msg_t;

/*! @brief Response to state config message. 
 *
 * This message is sent in reponse to the STATE_CONFIG command.
 *
 * \sa cdmafw_state_cfg_msg_t
 */
typedef struct {
  /*! \brief Message Header */
  msgr_hdr_struct_type      hdr;
  /*! \brief present state of CDMA FW. */
  cdmafw_state_t             state;
  /*! \brief CDMA FW version. */
  uint16                    cdmaVersion;
  /*! \brief Physical address of the shared memory interface */
  uint32                    sharedMemoryAddr;
} cdmafw_state_cfg_rsp_msg_t;

/** @brief Quick Page results ready
 *
 *   This is the message definition for the cdmafw Quick Page ready
 *   response message.
 *
 *   \sa cdmafw_qp_done_msg_t
 */

typedef struct {
  msgr_hdr_struct_type hdr;

  /*! \brief Rx System time corresponding to 
  when the QPCH symbols are latched */
  vstmr_1xscomb_t systemTime;

  /*! \brief QPCH I/Q symbols and pilot thresholds */
  int16 qpIndI;
  int16 qpIndQ;
  int16 qpThresh1;
  int16 qpThresh2;

} cdmafw_qp_done_ind_msg_t;

/** @brief QLIC Walsh Power Logging Ready
 *
 *   This is the message definition for the cdmafw QLIC Walsh Power 
 *   Logging Ready Response.
 *
 *   \sa cdmafw_qlic_walsh_pwr_logging_msg_t
 */
typedef struct {
  msgr_hdr_struct_type hdr;
  /*!< MSGR header */
  
  uint32 sharedMemoryAddr; 

} cdmafw_qlic_walsh_pwr_logging_rdy_rsp_msg_t;

/** @brief Combiner Slam Done
 *
 *   This is the message definition for the cdmafw combiner slam
 *   done indication message
 *
 *   \sa cdmafw_rx_combiner_slam_done_rsp_msg_t
 */
typedef struct {
  msgr_hdr_struct_type hdr;

} cdmafw_rx_combiner_slam_done_rsp_msg_t;

/** @brief Slew Roll Done
 *
 *   This is the message definition for the cdmafw combiner slew
 *   roll done indication done message
 *
 *   \sa cdmafw_rx_slew_roll_done_rsp_msg_t
 */
typedef struct {
  msgr_hdr_struct_type hdr;

} cdmafw_rx_combiner_slew_roll_done_rsp_msg_t;

/** @brief Slew Done
 *
 *   This is the message definition for the cdmafw combiner slew
 *   done indication done message
 *
 *   \sa cdmafw_rx_slew_done_rsp_msg_t
 */
typedef struct {
  msgr_hdr_struct_type hdr;

} cdmafw_rx_combiner_slew_done_rsp_msg_t;

/*! @brief Tx High Power Detection
 *
 *   This is the message definition for the cdmafw Tx High Power Detection
 *   Response.
 */
typedef struct {
  msgr_hdr_struct_type hdr;
  /*!< MSGR header */
  
  uint16 cTxPowerHighFlag;
  /*!< High Tx power flag */
} cdmafw_tx_high_pwr_det_rsp_msg_t;

#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY  
/*! @brief 1x ASDIV Priority Change Response
 *
 *   This is the message definition for the cdmafw Priority Change
 *   after switch Response.
 */
typedef struct {

  /** MSGR header */
  msgr_hdr_struct_type hdr;

} cdmafw_asdiv_pri_change_rsp_msg_t;
#endif

/**@}*/

/*============================================================================
 *
 * IRAT COMMAND/RESPONSE
 *
 * ==========================================================================*/
/** 
 * \addtogroup cirat
 * @{
 */

/** 
    @brief Pilot Measurement Configuration Request Message.

    Software sends this message to configure for 1X pilot measurements.

    Pilot measurement configuration can be initiated immediately or based on 
    start time in terms of 1X RTC (Cx1). The duration of the pilot measurement
    may be infinite or given in terms of 1X RTC (Cx1).

    Response: cdmafw_pilot_meas_cfg_rsp_msg_t.
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


  uint16 :15;

  /**   Number of desired valid samples in sample RAM. Not used in idle mode.
   */
  uint16                    numSamplesDesired;

  /**   Initial value of RX rotator in FW units(same as rotAccum).
   */
  uint32                    rotError;

 
  /**   RF Tune Back Time Constant. Time before end of gap when RF tune back is started.
   */ 
  uint16                    rfTuneBackTimeConstant;


} cdmafw_pilot_meas_cfg_req_msg_t;

/** @brief Pilot Measurement Configuration Response Message

    Firmware generates this response to Pilot Measurement Configuration Request message:
    cdmafw_pilot_meas_cfg_rsp_msg_t.
 */
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  /**   Number of samples captured after RX AGC acquisition.
  */
  uint16                    numSamplesCaptured;

  /**   Status/Error code (TBD)
   */
  uint16                    status;

  /**   Bit(15:0) in half-word 4
      \verbatim
        15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
      |                        timeStampReqRtcCx2                                     |
      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
      
      timeStampReqRtcCx2 : 1X RTC timestamp when the Pilot Measurement Configureation
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
      
      timeStampRspRtcCx2 : 1X RTC timestamp when this Pilot Measurement Configuration
      response is sent.
	\endverbatim
   */
  uint16                    timeStampRspRtcCx2;

  /** RTC position in cx2 when streaming is stopped for connected mode
   * and idle offline mode, to be passed on to SRCH FW for offline
   * search*/
  uint16                    streamingStopRtcCx2;

  /**   Final value of RX AGC accumulator
   */
  uint32                    rxAgcAccum;

} cdmafw_pilot_meas_cfg_rsp_msg_t;

/** @brief Pilot Measurement Stop Stream Command
    
    At end of the idle mode measurement (in MEAS_CONNECTED state), to stop the sample
    streaming L1 must send this message before sending the state transition message
    to put firmware back to STANDBY state. This also results in firmware releasing
    the WB/NB chain allocated from FW CRM.

    Firmware assumes the chain to be stopped is the one started from PILOT_MEAS_CFG_REQ/RX_START_CFG
    sent eariler when the measurement was started.

    This message is not needed in connected mode measurement where stream is stopped automatically
    at gap end.

    Response: cdmafw_pilot_meas_stop_stream_rsp_t
    This is sent when the sample stream is stopped and RxFE chain released.
*/
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

} cdmafw_pilot_meas_stop_stream_msg_t;

/** @brief Pilot Measurement Stop Stream Response
 
    This is the response to the stop stream message. After receiving this message,
    software can put firmware to STANDBY state.
*/
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

} cdmafw_pilot_meas_stop_stream_rsp_msg_t;

/** @brief Message sent to RFSW after capturing a set of samples for EPT in
 * CAL mode.
 * \sa .  */
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  /*! \brief DPDindex for which this capture was done */
  uint16  DPDindex;

  /*! \brief Sequence number for tracking request and response messages */
  uint32  sequenceNumber;

} cdmafw_ept_capture_rsp_msg_t;

/**@}*/

/** 
 * \addtogroup cmux
 * @{
 */

/** @brief TX_DAC_START Message Definition.
 *
 */
typedef struct {

  /*!< Message Header */
  msgr_hdr_struct_type hdr;


  /*! STMR trigger time. FW will use this to configure the STMR trigger
   * that starts the TX chain. Set this according to the contents of the
   * prefill buffer. If prefill buffer data starts at the frame boundary,
   * then set this to the upcoming frame boundary 
   *
   * This field allows TX trigger time specfication in Cx8 resolution.
   *
   * TX_DAC_START for 1X will ERR_FATAL if the future action time is less than
   * < 64 chips into the future.
   * 
   */
  uint32 startRtcCx8;

  /*!< Initial PN state */
  uint32 initialPNState;

  /*!< Initial Long Code State */
  uint64 longCodeState;

  /*!< Initial Long Code Mask */
  uint64 longCodeMask;

  /*!< Initial ENC channel configuration */
  cdmafw_mod_channel_cfg_t initEncConfig[CDMAFW_NUM_ENCODER_CHANNELS];

} cdmafw_tx_dac_start_msg_t;

/** @brief HW configuration for enabling MTVA viterbi decoder. Use this to
 * override FW default value for Yamamoto threshold and the number of trace
 * back paths in MTVA viterbi decoder HW block.  MTVA viterbi decoder.
 */
typedef struct {

  /* Yamamoto Threshold. Takes values 0 to 15 */
  uint8 mtva_ym_thresh:4;

  uint8 mtva_max_tracebacks:2;

} cdmafw_mtva_hw_config_t;

/** @brief Rate determination algorithm configuration message.
 
    Configures the RDA data tables and also enables MTVA and the
    corresponding MTVA RDA.
*/
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  /*! \brief Enables Multi-trace viterbi algorithm and uses MTVA RDA for rate
   * determination. */
  boolean mtvaEnable;

  /*! \brief Loads the new RDA data tables from SW-writable shared mem . */
  boolean loadRdaDataTables;

  /*! \brief Loads the new HW config from mtvaHwConfig if TRUE. */
  boolean loadMtvaHwConfig;

  /*! \brief MTVA viterbi decoder HW configuration. see cdmafw_mtva_hw_config_t */
  cdmafw_mtva_hw_config_t mtvaHwConfig;

} cdmafw_rda_config_msg_t;

/** @brief Command to enable/disable QOF-based noise estimation for FPC.
*/
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  /*! \brief Enables QOF-based noise estimation for FPC decision if this is
   * true.  Disables if false. */
  boolean qofFchNoiseEstimationEnable;

  /*! \brief Noise bias in Q15. Default is zero in FW. */
  int16 qlicNoiseBiasUint;

} cdmafw_qof_noise_est_fpc_config_msg_t;

/**@}*/

/** 
 * \addtogroup cfwrxagc
 * @{
 */


/*!< LNA CAL Override 
 *
 * This message is basically a combination of the legacy LNA_OVERRIDE, LNA_GAIN_OFFSET and RXAGC_ACQ_
 * MODE_CONFIG messages (to reduce messaging overhead). After a while, the legacy messages shall be
 * deprecated.
 *
 */
typedef struct {

  /*!< Message Header */
  msgr_hdr_struct_type hdr;

  /*!< Antenna to which this message applies */
  int32 antenna;

  /*!< Override Mode (1 => override, 0 => normal) 
   *
   * LNA gain ofs and ACQ duration configs are ignored in normal mode.
   * 
   */
  boolean overrideFlag;

  /*!< Override LNA state value. This state shall be applied at the next
   * RxAGC update */
  int32 overrideState;

  /*!< LNA CAL gain ofs override value in dB for the overriden state */
  int16 gainOfs;

  /*!< ACQ duration in 64-chip units. AGC shall switch to ACQ mode for this 
   * duration starting from the next AGC update */
  uint32 acqDuration;

} cdmafw_rxagc_lna_cal_override_msg_t;


/*!
 * @brief RxAGC Acquisition Mode Configuration.
 *
 * This message is intended to be used in CAL to reduce the CAL segment duration. This 
 * allows SW to configure AGC in ACQ mode briefly to allow a faster settling of AGC before
 * a measurement is taken.
 */
typedef struct {

  /*! @brief Message header */
  msgr_hdr_struct_type hdr;

  /*! @brief Antenna index to configure */
  uint32 antenna;

  /*! @brief ACQ duration to use */
  uint32 acqDuration;

} cdmafw_rxagc_acq_mode_config_msg_t;

/*!
 * @brief TxEPT Capture Command 
 * 
 * This message is intended to be issued by the Software with the
 * configurations of Tx/Rx Sample Capture process. One message per capture is
 * required. FW auto increments the capture index and returns it in
 * cdmafw_ept_capture_rsp_msg_t
 *
 */
typedef struct {
  /*! @brief Message header */
  msgr_hdr_struct_type hdr;

  /*! @ brief RxLM buffer index */
  uint16 rxlmBufferId;

  /*! @ brief WB chain index. Should be between [0,2] inclusive */
  uint16 rxChain;

  /*! @ brief TXC chain index. Should be 0 or 1*/
  uint16 txChain;

  /*! @ brief number of TXC/RX samples to capture.Must be a multiple of 1024(1K) */
  uint16 numSamples;

  /*! @ brief flag to do initialization.This must be set in 1st trigger*/
  boolean firstTrig;

  /*! @ brief flag to do de-initialization.This must be set in last trigger*/
  boolean lastTrig;

  /*! @ brief Indicate if firmware should do DPD estimation on the captured
   * samples.*/
  boolean doProcessing;

  /*! @ brief Sequence number to track request and response messages */
  uint32  sequenceNumber;
} cdmafw_ept_capture_msg_t;

/*!
 * @brief TxEPT Capture Command 
 * 
 * This message is intended to be issued by the Software for third sweep when
 * DPD should be enabled. 
 * 
 *
 */
typedef struct {
  /*! @brief Message header */
  msgr_hdr_struct_type hdr;

  /*! @ brief DPD row index corresponding to the current capture*/
  uint16  DPDindex;

  /*! @ brief Enable DPD override. */
  boolean dpdOverrideEnable;

} cdmafw_ept_override_msg_t;

/**@}**/

/** 
 * \addtogroup clog
 * @{
 */

/*!
 * 
 * @brief QXDM Logging Configuration.
 *
 * This message enables/disables a specified FW diag log packet.
 *
 */
typedef struct {


  /*!< Message header */
  msgr_hdr_struct_type hdr;


  /*!< QXDM Log packet ID */
  uint32 diagLogID;

  /*!< (0 => disable, 1 => enable) */
  boolean enable;
  
} cdmafw_diag_log_config_msg_t;

/*!
 * @brief RX mempool samples capture
 * 
 * This message is intended to be issued by the Software 
 * for capturing RX IQ samples
 */
typedef struct {

  /*! @brief Message header */
  msgr_hdr_struct_type hdr;
  
  /*! @ brief Antenna: Primary = 0, Diversity = 1 */
  uint16 ant;

} cdmafw_rx_iq_samples_logging_msg_t;

typedef struct {
   /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  /*! \brief Threshold in linear scale*/
  uint32 jamDetThres[CFW_NUM_LNA_STATES-1];

  /*! \brief Threshold - Hysteresis in linear scale*/
  uint32 jamDetThresMinusHyst[CFW_NUM_LNA_STATES-1];

} cdmafw_jd_threshold_update_msg_t;

typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  /*! \brief Mode config for JD: 0 for enable/disable, 1 for freeze/unfreeze */
  uint16  jamFreezeMode;
  uint16  jdEnable;
} cdmafw_jd_mode_msg_t;


/**@}**/

/*-----------------------------------------------------------------------*/
/*     DEMOD CCONFIG DATA STRUCTURES (new on BOLT)                       */
/*-----------------------------------------------------------------------*/
typedef union
{
  struct 
  {  
    /*!< replace DEM1X_CHANNEL0_IMMED__CH0_DISABLE,
         0 - disable, 1 - enable (note: different config as CH0_DISABLE) */
    boolean  combEnable;  

    /*!< same as DEM1X_CHANNEL0_IMMED__RATE_SEL */
    uint8    rateSel;           

    /*!< same as DEM1X_COMBINER_CTL__Q_CHAN_OFF */
    boolean  bpsk;

    /*!< new param, 1 for sync channel, 0 for other channels */
    boolean  syncEnable;  
    
    /*!< new param, 13 - for WL64, 14 - for WL128 */
    uint8    ffeBias;

    /*!< same as DEM1X_CHANNEL0_IMMED__QPCH_EN */  
    boolean  qpchEnable;                
    /*!< same as DEM1X_CHANNEL0_IMMED__OOK_POSITION */  
    uint16   qpchOokPosition;    
  };
  uint64 value;
} cdmafw_demod_ch0_ctrl_t;

typedef union
{
  struct 
  {  
    /*!< same as DEM1X_CHANNEL1_IMMED__HW_EN */
    boolean  combEnable; 

    /*!< same as DEM1X_Fn_HW_CH1_IMMED__RND */ 
    uint8    ffeBias;

    /*!< new, number of QPSK symbols every frame, 
         for demback widget control */
    uint16   numSymbols;
  };
  uint32 value;
} cdmafw_demod_ch1_ctrl_t;

typedef union
{
  struct 
  {  
    /*!< same as DEM1X_CHANNEL2_IMMED__HW_EN */
    boolean  combEnable;

    /*!< same as DEM1X_Fn_HW_CH2_IMMED__RND */ 
    uint8    ffeBias;
    /*!< same as DEM1X_CHANNEL2_IMMED__SOFTDEC_RND */
    uint8    softdecRnd;
    /*!< same as DEM1X_CHANNEL2_IMMED__SOFTDEC_ACCUM_RND */
    uint8    softdecAccumRnd;
       
    /*!< new, number of QPSK symbols every frame, 
         for demback widget control */
    uint16   numSymbols;

    /*!< same as schIqAccScale in SCH_ACC_SCALE CMD, SCH_ACC_SCALE CMD is
         deprecated on BOLT, DEM1X_FWD_PWR_CTL_IMMED__WALSH_GAIN not needed*/
    uint16   fpcSchIqAccScale;
  };
  uint64 value;
} cdmafw_demod_ch2_ctrl_t;

typedef union
{
  struct 
  {
    /*!< same as DEM1X_TRAFFIC_REV_PWR_CTL__ERASE_EN */
    boolean  rpcEraseEn;  
    /*!< same as DEM1X_TRAFFIC_REV_PWR_CTL__PC_EVEN */
    boolean  rpcEven;              
    /*!< same as DEM1X_TRAFFIC_REV_PWR_CTL__PUNC_POS_SEL */
    boolean  rpcPuncPosSel; 
    /*!< same as DEM1X_TRAFFIC_REV_PWR_CTL__POWER_CTL_PCT */
    boolean  rpcPct;            
  };
  uint32  value;
}cdmafw_demod_rev_pwr_ctrl_cfg_t;

typedef union
{
  struct 
  {
    /*!< same as DEM1X_COMBINER_CTL__FPC_EN */
    boolean  fpcEnable;

    /*!< same as DEM1X_FWD_PWR_CTL_IMMED__FPC_SEL */
    uint16   fpcSel;           
  };
  uint32  value;
}cdmafw_demod_fwd_pwr_ctrl_cfg_t;

typedef enum {
  CDMAFW_CH0 = 0,
  CDMAFW_CH1,
  CDMAFW_CH2,
  CDMAFW_CH_QOF_PILOT,
  CDMAFW_NUM_CHANS
} cdmafw_demod_channel_e;

typedef struct {
  /*!< CH0: new param, supported WL = 64,128;
       CH1: replace DEM1X_CHANNEL1_IMMED__SPR_MASK, 
            supported WL = 64,128 
       CH2: replace DEM1X_CHANNEL2_IMMED__SPR_MASK, 
            supported WL = 4,8,16,32,64,128;
       Walsh mask configuration:
            0 - WL=4
            1 - WL=8
            2 - WL=16
            3 - WL=32
            4 - WL=64
            5 - WL=128  */
  uint8 walshLengthMask;  

  /*!< same as DEM1X_CHANNEL0/1/2_IMMED__SW_BETA */
  uint16 swBeta;
} cdmafw_demod_walsh_beta_cfg_t;

typedef struct {
  /*!< MSGR header */
  msgr_hdr_struct_type          hdr;

  /*!< update mode: 
         TRUE  -  update immediate
         FALSE - update at 20ms frame boundary */
  boolean                       updateImmed;

  /*!< channel 0 ctrl */
  cdmafw_demod_ch0_ctrl_t       cfg;

  cdmafw_demod_walsh_beta_cfg_t walshBeta;
}cdmafw_demod_channel0_cfg_msg_t;

typedef struct {
  /*!< MSGR header */
  msgr_hdr_struct_type          hdr;

  /*!< update mode: 
         TRUE  -  update immediate
         FALSE - update at 20ms frame boundary */
  boolean                       updateImmed;

  /*!< channel 1 ctrl  */
  cdmafw_demod_ch1_ctrl_t       cfg;

  cdmafw_demod_walsh_beta_cfg_t walshBeta;
}cdmafw_demod_channel1_cfg_msg_t;

typedef struct {
  /*!< MSGR header */
  msgr_hdr_struct_type          hdr;

  /*!< update mode: 
         TRUE  -  update immediate
         FALSE - update at 20ms frame boundary */
  boolean                       updateImmed;

  /*!< channel 2 ctrl  */
  cdmafw_demod_ch2_ctrl_t       cfg;

  /*!< walsh code and beta cfg */
  cdmafw_demod_walsh_beta_cfg_t walshBeta;
}cdmafw_demod_channel2_cfg_msg_t;

typedef struct {
  /*!< MSGR header */
  msgr_hdr_struct_type            hdr;

  /*!< update mode: 
         TRUE  -  update immediate
         FALSE - update at 20ms frame boundary */
  boolean                         updateImmed;

  /*!< RPC cfg */             
  cdmafw_demod_rev_pwr_ctrl_cfg_t rpcCfg;

}cdmafw_demod_rpc_cfg_msg_t;

typedef struct {
  /*!< MSGR header */
  msgr_hdr_struct_type            hdr;

  /*!< update mode: 
         TRUE  -  update immediate
         FALSE - update at 20ms frame boundary */
  boolean                         updateImmed;

  /*!< FPC cfg */             
  cdmafw_demod_fwd_pwr_ctrl_cfg_t fpcCfg;

}cdmafw_demod_fpc_cfg_msg_t;

typedef union {
  struct {
    /*!< DEM1X_TRACKING__ENABLE */
    boolean  enable;
    /*!< DEM1X_TRACKING__FINGER */
    uint8    fing;
  };
  uint16  value;
}cdmafw_demod_trk_fing_cfg_t;

typedef struct {
  /*!< MSGR header */
  msgr_hdr_struct_type hdr;

  /*!< trk fing cfg */
  cdmafw_demod_trk_fing_cfg_t  trkFingCfg;
}cdmafw_demod_trk_fing_cfg_msg_t;

typedef struct {
  /*!< MSGR header */
  msgr_hdr_struct_type     hdr;
  
  /*!< CX8 delay based on the tracked finger timing, 
       BOLT value = 8*(327+64+384[qlic delay]+320[bolt delay])
                  = 8760
       to replace DEM1X_ARM_COMBINER_SLAM and DEM1X_COMBINER_TIME_LOAD */
  uint32                   combDelay;
}cdmafw_demod_sys_time_slam_msg_t;

typedef struct {
  /*!< MSGR header */
  msgr_hdr_struct_type     hdr;

  /*!< slew time in unit of 26.66 ms, 
       to replace DEM1X_SLEW_COMBINER_TIME
    @verbatim
    1:  slew 26.66 ms;
    2:  slew 2*26.66 ms
    else: illegal 
	  @endverbatim
  */   
  uint32                   pnRoll;
}cdmafw_demod_sys_time_slew_roll_msg_t;

typedef struct {
  /*!< MSGR header */
  msgr_hdr_struct_type     hdr;
  
  /*!< in CX8 resolution */
  uint32                   time;

  /*!< flag to control VPE initialization for demod 
       vpeInit = 1 will slew the comb time and init VPE
       vpeInit = 0 will only slew the comb time */
  boolean                  vpeInit;                   
}cdmafw_demod_sys_time_slew_time_msg_t;


typedef struct {
  /*!< MSGR header */
  msgr_hdr_struct_type     hdr;
  
  /*!< same as DEM1X_FRAME_OFFSET__DATA */
  uint16                   frameOffset;
}cdmafw_demod_frame_offset_msg_t;

typedef union {
  struct {
    /*!< same as DEM1X_FINGER_MERGE_F8__THRESH */
    uint32 thresh : 4;
    /*!< same as DEM1X_FINGER_MERGE_F8__CHECK_EN */
    uint32 checkEn : 28;
  };
  uint32  value;
}cdmafw_demod_merge_cfg_t;

typedef struct {
  /*!< MSGR header */
  msgr_hdr_struct_type hdr;

  /*!< merge protection cfg  */
  cdmafw_demod_merge_cfg_t  mergeCfg;
}cdmafw_demod_merge_cfg_msg_t;

typedef struct {
  /*!< MSGR header */
  msgr_hdr_struct_type     hdr;
  
  /*!< long code state, 
       same as DEM1X_LC_STATE_LOAD_LO and DEM1X_LC_STATE_LOAD_HI
       latched at combiner PN roll (26.66 ms), to match legacy HW
       implementation.
       This message is lateched at combiner PN roll only.  */
  uint64                   lcState;
}cdmafw_demod_lc_state_msg_t; 
 
typedef struct {
  /*!< MSGR header */
  msgr_hdr_struct_type     hdr;

  /*!< latched at combiner PN roll (26.66 ms),
       same as DEM1X_CHn_LC_MASK_LO_IMMED and
       DEM1X_CHn_HI_MASK_LO_IMMED.
       XXX: check SW doesn't use DEM1X_CHn_LC_MASK_HI/LO_SPN_ROLL.
       This message is lateched immediately.*/
  /*!< CH0 long code mask */ 
  uint64                   ch0;
  /*!< CH1 long code mask */ 
  uint64                   ch1;
  /*!< CH2 long code mask */ 
  uint64                   ch2;
  /*!< CH0 RPC (CH7 on legacy targets) long code mask */ 
  uint64                   ch0Rpc;
}cdmafw_demod_lc_mask_msg_t;


/*============================================================================
 *
 * MESSAGE UNIONS
 *
 * ==========================================================================*/
/**
 * \addtogroup cunion
 * @{
 */

/*! \brief Union of all message received by firmware */
typedef union {
  /*! \brief Message header */
  msgr_hdr_struct_type                         hdr;
  cdmafw_finger_assign_msg_t                   finger_assign;              
  cdmafw_multiple_fingers_assign_msg_t         multiple_fingers_assign;               
  cdmafw_finger_config_msg_t                   finger_config;                  
  cdmafw_multiple_fingers_config_msg_t         multiple_fingers_config;           
  cdmafw_finger_common_config_msg_t            finger_common_config;                         
  cdmafw_oqpch_search_done_msg_t               oqpch_search_done;
  cdmafw_afc_mode_msg_t                        afc_mode;                     
  cdmafw_xo_afc_static_config_msg_t            xo_afc_static_config;                
  cdmafw_xo_afc_dynamic_config_msg_t           xo_afc_dynamic_config;                
  cdmafw_fl_signaling_msg_t                    fl_signaling;                       
  cdmafw_apf_enable_msg_t                      apf_enable;
  cdmafw_fpc_freeze_cfg_msg_t                  fpc_freeze;
  cdmafw_fpc_override_msg_t                    fpc_override;
  cdmafw_qlic_walsh_pwr_logging_msg_t          qlic_walsh_pwr_logging;            
  cdmafw_sch_acc_scale_msg_t                   sch_acc_scale;                       
  cdmafw_fch_target_setpoint_msg_t             fch_target_setpoint;                     
  cdmafw_sch_target_setpoint_msg_t             sch_target_setpoint;                    
  cdmafw_rl_mpp_cfg_msg_t                      rl_mpp_config;
  cfw_rl_pwr_est_filter_cfg_msg_t              rl_pwr_filter_config;
  cdmafw_rl_ack_test_mode_cfg_msg_t            rl_ack_test_mode_config;
  cdmafw_rl_signaling_msg_t                    rl_signaling;                  
  cdmafw_rl_legacy_rc_frame_config_msg_t       rl_legacy_rc_frame_config;              
  cdmafw_rl_frame_config_msg_t                 rl_frame_config;
  cdmafw_rl_txfe_trigger_msg_t                 rl_txfe_trigger;
  cdmafw_rl_txfe_stop_msg_t                    rl_txfe_stop;
  cdmafw_rl_fpc_override_msg_t                 rl_fpc_override;
  cdmafw_rl_rc8_frame_config_msg_t             rl_rc8_frame_config;                
  cdmafw_state_cfg_msg_t                       state_cfg;
  cdmafw_adv1x_log_config_msg_t                adv1x_log_config;
  cdmafw_pilot_meas_cfg_req_msg_t              pilot_meas_cfg_req;
  cdmafw_pilot_meas_stop_stream_msg_t          pilot_meas_stop_stream;
  cdmafw_tdec_brdg_config_msg_t                tdec_brdg_config;
  cdmafw_demback_mode_config_msg_t             demback_mode_config;
  cdmafw_demback_sw_config_msg_t               demback_sw_config;

  /* Demod config (new on BOLT) */
  cdmafw_demod_channel0_cfg_msg_t              demod_channel0_cfg;
  cdmafw_demod_channel1_cfg_msg_t              demod_channel1_cfg;
  cdmafw_demod_channel2_cfg_msg_t              demod_channel2_cfg;
  cdmafw_demod_rpc_cfg_msg_t                   demod_rpc_cfg;
  cdmafw_demod_fpc_cfg_msg_t                   demod_fpc_cfg;
  cdmafw_demod_trk_fing_cfg_msg_t              demod_trk_fing_cfg;
  cdmafw_demod_sys_time_slam_msg_t             demod_sys_time_slam;
  cdmafw_demod_sys_time_slew_roll_msg_t        demod_sys_time_slew_roll;
  cdmafw_demod_sys_time_slew_time_msg_t        demod_sys_time_slew_time;
  cdmafw_demod_frame_offset_msg_t              demod_frame_offset;
  cdmafw_demod_merge_cfg_msg_t                 demod_merge_cfg;
  cdmafw_demod_lc_state_msg_t                  demod_lc_state;
  cdmafw_demod_lc_mask_msg_t                   demod_lc_mask;

  /*Rx AGC*/
  cfw_rx_agc_cfg_msg_t                         rx_agc_cfg;
  cfw_lna_override_msg_t                       lna_override;
  cfw_lna_gain_offset_msg_t                    lna_gain_offset;
  cfw_rx_agc_stop_msg_t                        rx_agc_stop_cfg;

  /* RFA commands */
  cdmafw_tx_pa_params_config_msg_t             tx_pa_params_config;
  cfw_tx_limit_config_msg_t                    tx_limit_config;
  cdmafw_tx_pa_range_override_msg_t            tx_pa_range_override;
  cdmafw_tx_override_msg_t                     tx_override;
  cdmafw_tx_closed_loop_override_msg_t         tx_closed_loop_override;
  cdmafw_tx_open_loop_override_msg_t           tx_open_loop_override;
  cdmafw_tx_pa_state_override_msg_t            tx_pa_state_override;
  cdmafw_rx_set_freq_offset_msg_t              rx_set_freq_offset;
  cdmafw_rxlm_config_masked_msg_t              rxlm_config_masked;
  /* RFA commands */

  /* RX/TX config commands */
  cfw_rx_start_cfg_msg_t                       rx_start_cfg;
  cfw_rx_start_msg_t                           rx_start;
  cfw_rx_stop_msg_t                            rx_stop;
  cfw_tx_start_msg_t                           tx_start;
  cfw_tx_stop_msg_t                            tx_stop;
  cfw_tx_dac_start_msg_t                    tx_dac_start;
  cfw_tx_dac_stop_msg_t                        tx_dac_stop;
  /* RX/TX config commands */

  /* TX AGC CFG*/
  cfw_tx_agc_cfg_msg_t                         tx_agc_cfg;

  /* HDET measurement request */
  cfw_tx_hdet_request_msg_t                    hdet_req;

  /* HDET configuration msg */
  cfw_tx_hdet_config_msg_t                     hdet_config;

  /* Intelliceiver update msg */
  cfw_intelliceiver_update_msg_t               intelliceiver_update;

  /* TXC dynamic update msg */
  cfw_dynamic_txc_update_msg_t                 dynamic_txc_update;

  /* RDA and MTVA config message */
  cdmafw_rda_config_msg_t                      rda_config;

  cdmafw_qof_noise_est_fpc_config_msg_t        qof_noise_est_fpc_cfg;
  
  cdmafw_rxagc_acq_mode_config_msg_t           rxagc_acq_mode_config;

  cdmafw_rxagc_lna_cal_override_msg_t          rxagc_lna_cal_override;

  cdmafw_xpt_capture_msg_t                     xpt_capture;
  
  cdmafw_stop_txagc_update_msg_t               stop_txagc_update;

  /* HDET reset MSG */
  cfw_tx_hdet_reset_msg_t                      hdet_reset;

  cfw_tx_dac_cal_config_msg_t                  dac_cal_config;

  /* EPT Capture MSG  */
  cdmafw_ept_capture_msg_t                     ept_capture;

  cdmafw_ept_override_msg_t                    ept_override;

  cfw_xpt_config_msg_t                         xpt_config;

  cfw_ept_config_msg_t                         ept_config;

  cfw_ept_sample_buffer_cfg_msg_t              ept_sample_buffer_cfg;
  
  /* RX IQ samples capture MSG*/
  cdmafw_rx_iq_samples_logging_msg_t           rx_iq_logging;

  /*!< QXDM diag logging configuration */
  cdmafw_diag_log_config_msg_t                 diag_log_config;
  
  cdmafw_jd_threshold_update_msg_t             jd_threshold_update;
  cdmafw_jd_mode_msg_t                         jd_mode;
  
  cfw_xpt_delay_update_msg_t                   xpt_delay_update;
  cdmafw_qxdm_diag_data_msg_t                  qxdm_diag_data;

  cfw_iq_capture_msg_t                         iq_capture;

  cdmafw_dsdx_config_msg_t                     dsdx_config; 

  /* FL-DRX Messages */
  cfw_rx_exercise_milli_micro_sleep_msg_t      exercise_milli_sleep;

  cdmafw_1x2x_qta_start_msg_t                  qta_1x2x_start;
  cdmafw_1x2x_qta_stop_msg_t                   qta_1x2x_stop;
  cdmafw_1x2x_qta_status_query_msg_t           qta_1x2x_status_query;

#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY
  cfw_rf_execute_ant_switch_msg_t              rf_execute_ant_switch_msg;
#endif          
 } cdmafw_msg_u;


/*! \brief Union of all indication messages recevied by software  */
typedef union {
  /*! \brief Message header */
  msgr_hdr_struct_type                         hdr;
  cdmafw_rxtx_frame_stats_ind_msg_t            rxtx_frame_stats;       
  cdmafw_qlic_frame_stats_ind_msg_t            qlic_frame_stats;    
  cdmafw_rc11_fch_decoder_done_ind_msg_t       rc11_fch_decoder_done;
  cdmafw_adv1x_log_buffer_rdy_ind_msg_t        adv1x_log_buffer_rdy;
  cdmafw_vi_ctl_ind_msg_t                      vi_ctl;
  cdmafw_qp_done_ind_msg_t                     qp_done;
  cdmafw_reset_req_ind_msg_t                   reset_req;
  cdmafw_ept_proc_done_msg_t                   ept_proc_done;
  cdmafw_ept_expansion_done_msg_t              ept_expansion_done;
  cdmafw_qta_rxagc_acq_done_ind_msg_t          qta_rxagc_acq_done;
  cdmafw_demback_sw_config_rsp_msg_t           demback_sw_config_rsp;
} cdmafw_msg_ind_u;


/*! \brief Union of all response messages recevied by software  */
typedef union {
  /*! \brief Message header */
  msgr_hdr_struct_type                         hdr;
  cdmafw_app_cfg_rsp_msg_t                     app_cfg;
  cdmafw_qlic_walsh_pwr_logging_rdy_rsp_msg_t  qlic_walsh_pwr_logging_rdy; 
  cdmafw_state_cfg_rsp_msg_t                   state_cfg;
  cdmafw_tx_high_pwr_det_rsp_msg_t             tx_high_pwr_det;     
  cdmafw_pilot_meas_cfg_rsp_msg_t              pilot_meas_cfg;
  cdmafw_pilot_meas_stop_stream_rsp_msg_t      pilot_meas_stop_stream_rsp;
  cfw_rx_start_rsp_msg_t                       rx_start_rsp;
  cfw_dac_cal_config_rsp_msg_t                 dac_cal_rsp;
  cdmafw_ept_capture_rsp_msg_t                 ept_capture_rsp;
  cdmafw_xpt_capture_rsp_msg_t                 xpt_capture_rsp;
  cfw_pa_params_rsp_msg_t                      pa_params_rsp;
  cfw_xpt_trans_compl_rsp_msg_t                xpt_trans_compl_rsp;
  cfw_xpt_config_rsp_msg_t                     xpt_rsp;
  cdmafw_rl_txfe_trigger_rsp_msg_t             rl_txfe_trigger_rsp;
  cfw_iq_capture_rsp_msg_t                     iq_capture;
  cdmafw_rx_combiner_slam_done_rsp_msg_t       combiner_slam;
  cdmafw_rx_combiner_slew_roll_done_rsp_msg_t           combiner_slew_roll;
  cdmafw_rx_combiner_slew_done_rsp_msg_t                combiner_slew;
  cdmafw_1x2x_qta_start_rsp_msg_t              qta_1x2x_start_rsp;
  cdmafw_1x2x_qta_stop_rsp_msg_t               qta_1x2x_stop_rsp;
  cdmafw_1x2x_qta_status_query_rsp_msg_t       qta_1x2x_status_query_rsp;
#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY
  cdmafw_asdiv_pri_change_rsp_msg_t            asdiv_pri_change_rsp;
#endif

} cdmafw_msg_rsp_u;


#define CDMAFW_MAX_MSG_SIZE       sizeof(cdmafw_msg_u)
#define CDMAFW_MAX_IND_MSG_SIZE   sizeof(cdmafw_msg_ind_u)
#define CDMAFW_MAX_RSP_MSG_SIZE   sizeof(cdmafw_msg_rsp_u)

#define CDMAFW_NUM_FL_CMD_MSG     (CDMA_FW_FL_CMD_LAST - CDMA_FW_FL_CMD_FIRST + 1)
#define CDMAFW_NUM_RL_CMD_MSG     (CDMA_FW_RL_CMD_LAST - CDMA_FW_RL_CMD_FIRST + 1)

#define CDMAFW_NUM_CMD_MSG        (CDMAFW_NUM_FL_CMD_MSG + CDMAFW_NUM_RL_CMD_MSG)
#define CDMAFW_NUM_IND_MSG        (CDMA_FW_IND_LAST - CDMA_FW_IND_FIRST + 1)
#define CDMAFW_NUM_RSP_MSG        (CDMA_FW_RSP_LAST - CDMA_FW_RSP_FIRST + 1)

/**@}*/

/*============================================================================
 *
 * SHARED MEMORY INTERFACE
 *
 * ==========================================================================*/
/**
 * \addtogroup cshared
 * @{
 */


typedef struct {
  uint32 sampleSrvL1SAddr[CDMAFW_NUM_ANTENNAS]; /** 0 => Invalid (SRAM not setup yet) */
} cdmafw_fl_samp_srv_info_t;

/** Finger Status Interface.  Content in this shared memory
    buffer is updated every 64 chips. */
typedef struct ALIGN(8)  { 
    int16   ComPilotRawQ;
    int16   ComPilotRawI; 
    int16   DivPilotRawQ;
    int16   DivPilotRawI;
    int32   ComFiltRSSI;
    int32   DivFiltRSSI;
    int16   ComPilotFiltQ;
    int16   ComPilotFiltI;
    int16   DivPilotFiltQ;
    int16   DivPilotFiltI;
    int16   MmseFiltA11I;
    int16   MmseFiltA01I;
    int16   MmseFiltA01Q;
    int16   MmseFiltA00I;
    boolean ComFingLock;
    boolean DivFingLock;
    boolean ComCombLock;
    boolean DivCombLock;
} cdmafw_finger_stats_interface_t;

/** AFC Status Interface.  Content in this shared memory
    buffer is updated every 64 chips. */
typedef struct {
    /*! VCO Frequency Accum */
    int32 afcFreqAccVco;
    /*! ROT Frequency Accum */
    int32 afcFreqAccRot;
    /*! Filtered AFC Frequency Error */
    int32 afcFreqErrorFilt;
    /*! Combined Pilot Ec/Io over locked fingers */
    int32 afcCmbPilotEcio;
} cdmafw_afc_stats_interface_t;

/** QPCH Status Interface. */
typedef struct {
    int16 qpIndI;
    int16 qpIndQ;
    int16 qpThresh1;
    int16 qpThresh2;
} cdmafw_online_qpch_stats_interface_t;

/*!< RPC Status Interface. 
 *
 * Contents of this SMEM location are updated every Frame 
 *
 */
typedef struct {
    /*!< RPC history 
     *
     * RPC history for the previous frame (frame offset aligned). The format is:
     * 
     * BIT(15-i) = 1 => RPC down command received in PCG i.
     * BIT(15-i) = 0 => Either RPC up command was received @ PCG i, or RPC bit is not
     *                  valid for this PCG (due to 1xA reduced rate RPC, RPC erasures etc.,)
     */
    uint16 rpcHistory;
    /*!< RPC History Valididy.
     *
     * RPC validity for the previous frame (frame offset aligned). The format is:
     *
     * BIT(15-i) = 1 => RPC command received in PCG(i) is valid.
     * BIT(15-i) = 0 => RPC command received in PCG(i) is invalid (due to 1xA reduced rate RPC,
     *                  RPC erasures etc.,)
     */
    uint16 rpcHistoryValid;
    /*!< FPC History 
     *
     * Stores the past 16 FPC decisions.
     *
     * BIT(i) = 1 => FPC down command was transmitted.
     * BIT(i) = 0 => Either FPC up command was transmitted, or no FPC command was transmitted.
     * 
     */
    uint16 fpcHistory;
    /*!< FPC History Validity
     *
     * Stores the validity for the past 16 FPC decisions.
     *
     * BIT(i) = 1 => FPC history for BIT(i) is valid.
     * BIT(i) = 0 => FPC hisotry for BIT(i) is invalid.
     */
    uint16 fpcHistoryValid;
    /*!< RPC History per Cell
     *
     * Same as rpcHistory, but has information for each PC cell.
     *
     */
    uint16 rpcHistoryCell[CDMAFW_NUM_PWR_CTL_CELLS];
    /*!< Frame Syncronized Closed Loop Adjust
     *
     * This gain adjust parameter is latched once per frame @ the frame boundary. This is expected
     * to be synchronous with RPC history.
     *
     * Units are -1/512 dB.
     *
     */
    int16  cTxGainAdjVal;
} cdmafw_power_control_stats_interface_t;

/** MUX Interface.  Content in this shared memory buffer 
    is updated every frame. */
typedef struct {
  
  /* DO NOT change order of any variables in this structure */
  int16 ch0AccFrame;                  
  int16 pcBitAccFrame;
  int16 fundWeightedPilotAccFrame;
  
  int16 ch1AccFrame;                  
  int16 ch2AccFrame;
  int16 suppWeightedPilotAccFrame;
  int16 suppWeightedNtMagAccFrame;
  int16 fundWeightedNtMagAccFrame;

  /*!< Output of the SVDO HE filter (-1/640 dBm units). */
  int16 rliHEFilterOutput;

} cdmafw_mux_read_interface_t;

/*! @brief RC11 FCH Decoder Done Info.
 *
 * This is the SMEM structure that contains RC11 FCH decoder information. This
 * structure is populated sometime within PCG0 of a frame for the decode that
 * happened in the previous frame.
 *   
 */
typedef struct {

  uint32 fliRC11FchRateInfo:5;
  /*!< RC 11 F-FCH rate determination result
    @verbatim
    0: Erasure
    1: Eighth rate
    2: Quarter rate
    3: Half rate
    4: Full rate
    @endverbatim
  */
  uint32 fliValidFPCFrame:1;
  /*!< Valid FPC frame flag
    @verbatim
    0: SW should not use this frame for channel supervision or adjusting FPC set point.
    1: SW should use this frame for channel supervision and adjusting FPC set point. 
    @endverbatim
	FW does not determine whether its erasure or blank frame. Based on the frame counter, 
	frame offset and the configuration of fliBlankingPeriod, FW determines whether its a 
	guaranteed non-null-rate frame or not.
  */
  
  uint32 fliFrameIndex:3;
  /*!< Frame Counter
   * A 3-bit frame count for the decoded frame.
   */
  
  uint32 fliFchDecodePCGIndexMinusOne:4;
  /*!< Decoded PCG Indicator
     SW shall use (fliFchDecodePCGIndexMinusOne+1) for PCG index in MAC counter.

     If (fliFchDecodePCGIndexMinusOne != 14),  this means:
     - Early decoding happens at symbol 16 of Rx PCG (fliFchDecodePCGIndexMinusOne +1) for the current frame. 
       There's no early (partial) decoding at symbol 16 of Rx PCG0, PCG1, PCG14, PCG15.
     - The number of symbols for decoding = 24*( fliFchDecodePCGIndexMinusOne +1)+16
     If (fliFchDecodePCGIndexMinusOne == 14), this means:
     - Full frame decoding happens at symbol 0 of Rx PCG 0 for the previous frame;
     - The number of symbols for decoding = 24*( fliFchDecodePCGIndexMinusOne +1)+24=384.
  */
  
} cdmafw_rc11_fch_decode_done_info_t;

/*! @brief FPC Freeze Info */
typedef struct {

  uint16   flFchFpcUpCnt:16;
  /*!< Count of FPC up decisions */

  uint16   fliFchFpcFreezeFlag:16;
  /*!< FPC Freeze Flag */

} cdmafw_fl_fpc_freeze_info_t;

/*! @brief RC11 ACK log buffer */
typedef struct {

  /*!< Index (0/1) of ACK log buffer presently updated by FW.
   * 
   * SW must use (ackLogBufIndex^1) to index the ackLogBuf buffer as FW is using ackLogBufIndex
   * to index ackLogBuf.
   *
   */
  uint16 ackLogBufIndex;

  /** Log Buffer
   * 
   * Each element below is a 16-bit quantity. There are two 32-bit words per cell in the ACK log
   * buffer.
   *
   * \verbatim
   *  +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
   *  |                               scaledFchAckSym (celln)                         |
   *  +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
   *  |                               scaledPwrTh1  (cell n)                          |
   *  +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
   *  |                 cellNAckDecision (cell n)  (0 => no ACK, 1 => ACK)            |    
   *  +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
   *  |                                  NtFilt (cell n)                              |
   *  +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
	  \endverbatim
  */

  uint32 ackLogBuf[CDMAFW_NUM_ACK_LOG_BUFFERS][CDMAFW_ACK_FRAME_LOG_SIZE];

} cdmafw_fl_rc11_ack_log_buffer_t;


/*! @brief RDA data Tables
 *
 * SMEM interface definition for RDA data tables provided to L1 SW. These will
 * be latched whenever cdmafw_rda_config_msg_t message is recieved with the
 * loadRdaDataTables field set to TRUE. 
 *
 * serFETThreshold table is special. This table is different between legacy
 * RDA and the newer MTVA-RDA. Therefore, if mtvaEnable field of
 * cdmafw_rda_config_msg_t is TRUE, then this table should contain valuse
 * specifc for MTVA-RDA. Otherwise, it should contain values for legacy RDA.
 *
 * @note MUST be cache-aligned.
 * 
 */
typedef struct ALIGN(32) {

  int16 slopeThreshold[CDMAFW_NUM_RATE_HYPOTHESES*CDMAFW_NUM_DECODE_ATTEMPTS][CDMAFW_NUM_RATE_HYPOTHESES] ; 
  int16 interceptThreshold[CDMAFW_NUM_RATE_HYPOTHESES*CDMAFW_NUM_DECODE_ATTEMPTS][CDMAFW_NUM_RATE_HYPOTHESES] ; 
  int16 energySelfThreshold[CDMAFW_NUM_DECODE_ATTEMPTS][CDMAFW_NUM_RATE_HYPOTHESES] ; 
  int16 serThreshold[CDMAFW_NUM_DECODE_ATTEMPTS][CDMAFW_NUM_RATE_HYPOTHESES] ; 
  int16 serFETThreshold[CDMAFW_NUM_DECODE_ATTEMPTS][CDMAFW_NUM_RATE_HYPOTHESES] ; 
  int16 serFrameEndThreshold[CDMAFW_NUM_RATE_HYPOTHESES] ; 

} cdmafw_rda_data_tables_t; 

/*! @brief CDMA TX SMEM Read Interface 
 *
 * Read Interface for CDMA TxAGC.
 *
 */
typedef struct ALIGN(32) {
  
  /* TxAGC Logical Updates */
  
  /*!< TX Open Loop (-1/640 dB units) */
  int16 cTxOpenLoopVal;  
  /*!< TX Rate Adjust  (-1/640 dB units) */
  int16 cTxRateAdjVal;
  /*!< TX Gain Adjust (-1/512 dB units) */
  int16 cTxGainAdjVal;
  /*!< TX total (-1/640 dB units) */
  int16 cTxTotalGainVal;
  /*!< Filtered TX total for SV headroom estimation (-1/640 dB units). 
   *
   *  The HE filter must be configured first using RL_PWR_FILTER_CONFIG message.
   *
   *  \sa cfw_rl_pwr_est_filter_cfg_msg_t
   */
  int16 rliHEFilterOutput;

  /*!< Filtered TX total for C+G Coex TX filter (-1/640 dB units). 
   *  See cfw_rl_pwr_est_filter_cfg_msg_t to configure this filter. See the same
   *  for FW's default filter time constant.
   */
  int16 CoexTxFiltOutput;

  /*!< TX MAX limiting flag */
  boolean cTxLimitingMin;
  /*!< TX MIN limiting flag */
  boolean cTxLimitingMax;

  /*!< Current PA State */
  int16 cPaStateValue __attribute__((deprecated));
  /*!< TX RFA LUT index */
  int16 cTxLutIndex __attribute__((deprecated));
  /*!< Analog Power */
  int16 cTxAnalogPwr __attribute__((deprecated));
  /*!< BetaP Gain (-1/640 dB) */
  int16 cTxBetapGain __attribute__((deprecated));

  /* TxAGC Physical Updates */

  /*!< PA Range */
  int16 cTxPaRangeValue __attribute__((deprecated)); 
  /*!< AVGA Gain Word written to SSBI interface */
  int32 cTxAgcAdjVal __attribute__((deprecated)); 
  /*!< BetaP Linear Multiplier value written to HW register*/
  int16 cTxBetapVal __attribute__((deprecated));
  /*!< TX SMPS APT PDM value written to HW register */
  int32 cTxAptValue __attribute__((deprecated));
  /*!< Filtered RX AGC (1/640 dBm units).
   *  Configure the filter gain in cfw_rx_agc_cfg_msg_t::agc_filt_gain.
   * DEPRICATED NOW, DONT READ THIS VARIABLE, USE rxAgcFiltPwr instead.
   */
  int16 rxFiltPwr;

  /*!< Filtered RX AGC per antenna (1/640 dBm units).
   *  Configure the filter gain in cfw_rx_agc_cfg_msg_t::agc_filt_gain.
   */
  int16 rxAgcFiltPwr[CDMAFW_NUM_ANTENNAS];
 
} cdmafw_rf_read_interface_t;

/*! @brief Walsh Power Logging SMEM Interface 
 */
typedef struct ALIGN(32)  {
    uint32 walshPower[WALSH_LOG_BUF_LEN/4];
} cdmafw_qlic_walsh_pwr_read_interface_t;

/*! @brief Rx AGC Report interface
 *
 *   This is the message definition for the cdmafw Rx AGC
 *   report interface.
 */
typedef struct ALIGN(32) {

  /*!< RxAGC(dBm) = -63.8 + Rx_AGC_Accum*102.4/65536 */
  uint16                  agcValue; 
  /*!< LNA decision from LNA controller FSM */ 
  uint16                  lnaDecision; 
  /*!< AGC acquisition down counter in units of 64 chips
   * The AGC moves to track mode when counter =0 */ 
  int32                   acqTimeDnCntr; 


} cdmafw_rx_agc_report_interface_t;

/*!
 * @brief Difference between STMR time and Sample count.
 */
typedef struct {

  /** Frame component */
  uint32 frameCount;
  /** PCG component */
  uint32 pcgCount;
  /** Chip component */
  uint32 chipCount;
  
} cdmafw_tx_stmr_sample_cnt_diff_t;


/*!
 * @brief Real-time TX timing info.
 *
 * This structure disseminates useful real-time TX timing information.
 *
 */
typedef struct {

  /** TX Advance Counter */
  uint32 advanceCount;

  /** TX Retard Counter */
  uint32 retardCount;

  /** Difference between STMR and TX Sample Counter */
  cdmafw_tx_stmr_sample_cnt_diff_t stmrSampleCntDiff;

} cdmafw_tx_timing_info_t;

/*! \brief RX_STOP timed trigger status */
typedef enum {
  OQPCH_STOP_SUCCESS,
  OQPCH_STOP_FAILURE
} cmdafw_timed_rx_stop_status_e;

/*!
 * @brief Timed RX_STOP info
 */

typedef struct {
  /*! Validity of RX_STOP trigger time for timed trigger. Set to _FAILURE if RX_STOP trigger time 
      has expired or is >13.33ms in the future */
  cmdafw_timed_rx_stop_status_e timed_rx_stop_status;

  /*! RX_STOP trigger RTC minus RTC when FW received RX_STOP in Cx1. Valid only when
      timed_stop_status = _FAILURE */
  int16                         stop_time_delta_cx1;

} cdmafw_timed_rx_stop_info_t;


/*! @brief Demod Finger SMEM Interface (new on BOLT)
 */
typedef union {

  struct {
    uint32 enable : 1;
    /* CX8 resolution */
    uint32 pnPosition : 18;
    uint32 : 13;
  };
  uint32 value;
} cdmafw_demod_finger_field_t;

typedef struct ALIGN(32)  {
    /* same as DEM1X_FINGERn */
    cdmafw_demod_finger_field_t  finger[CDMAFW_NUM_FINGERS];
} cdmafw_demod_interface_t;

/*! @brief FL Timing SMEM Interface (new on BOLT)
*/
typedef struct ALIGN(32)  {
  /* latched at 80ms frame boundary */
  vstmr_1xscomb_t scombTime;
  uint32          rtcCnt;
  uint32          xoCnt;
  uint64          lcState;
} cdmafw_fl_timing_interface_t;

/*! @brief Demback ID SMEM Interface (new on BOLT)
*/
typedef struct ALIGN(32)  {
  /* ready when SW gets the state cfg response msg */
  uint32  index;
  /* TDEC Address is set to valid address when TDEC bridge is enabled
     by SW. Otherwise, it is 0 (can be used by SW for error check) */
  uint32  tdecAddr;
} cdmafw_demback_interface_t;

/*! @brief ENC IB/OB LMEM0 MP PTRS */
typedef union {
  struct {
    uint32 line_addr                      : 13;  /* W[0] [12: 0] */
    uint32 reserved0                      :  2;  /* W[0] [14:13] */
    uint32 start_addr                     : 17;  /* W[0] [31:15] */
  };
  uint32 word_arr[1];
  uint32 word_val;
}cdmafw_hwapi_enc_mp_addr_t;

typedef union {
  struct {
    uint32 vbuff_len                      : 16;     /* W[0] [15: 0] */
    uint32 init_bank                      :  3;     /* W[0] [18:16] */
    uint32 min_bank                       :  3;     /* W[0] [21:19] */
    uint32 max_bank                       :  3;     /* W[0] [24:22] */
    uint32 reserved1                      :  7;     /* W[0] [31:25] */
  };
  uint32 word_arr[1];
  uint32 word_val;
}cdmafw_hwapi_enc_mp_vbuf_param_t;

typedef union {
  struct {
    cdmafw_hwapi_enc_mp_addr_t mp_addr;              /* W[0]..W[0] */
    cdmafw_hwapi_enc_mp_vbuf_param_t  mp_vbuf_param; /* W[1]..W[1] */
  };
  uint32 word_arr[2];
}cdmafw_hwapi_enc_mp_ptr_t;

/** CH0 Symbol Buffer Info. 
  */
typedef struct {

  /** Ping-Pong Buffer of CH0 Symbols.
      
      Bits 5:0   => CH0 Q-Soft Decision.
      Bits 13:8  => CH0 I-Soft Decision.

    */
  uint16 symbolBuffer[2][CDMAFW_NUM_SYMBOLS_PER_FRAME];

  /** Current FW Ping-Pong Buffer Write Index (0, 1) */
  uint32 fwBufferWrIdx;

  /** Current FW Symbol Write Index (0, ..., CDMA1X_NUM_SYMBOLS_IN_FRAME-1) */
  uint32 fwSymbolWrIdx;

  /** Current FW Frame Index */
  uint32 fwFrameIdx;

} cdmafw_ch0_symbol_buffer_intf_t;

/** Software Readable Shared Memory Interface Structure. Firmware only writes to this structure. */
typedef struct ALIGN(32)  {

  /* SMEM parameters updated every 64 chips in combiner interrupt */
  cdmafw_finger_stats_interface_t         fingerStats[CDMAFW_NUM_FINGERS];
  cdmafw_afc_stats_interface_t            afcStats;
  cdmafw_rx_agc_report_interface_t        rxAgcReport[CDMAFW_NUM_ANTENNAS];

  /*! @brief RxAGC log information for L1  
   * rxAgcLog [0][0..15] - RxAGC Value for primary antenna - PCGs->0 to 15
   * rxAgcLog [1][0..15] - RxAGC Value for secondary antenna - PCGs->0 to 15
   * For a disabled chain, RxAGC is always frozen from the last updated value.
   * */
  int16                                   rxAgcLog[CDMAFW_NUM_ANTENNAS][CDMAFW_NUM_PCG_PER_FRAME]; 

  /* MUX parameters updated every frame */
  cdmafw_mux_read_interface_t             muxRead;

  /* RPC information updated every frame */
  cdmafw_power_control_stats_interface_t  powerControlStats;

  /* FL FPC freeze information update very FPC interval */
  cdmafw_fl_fpc_freeze_info_t             flFpcFreezeInfo;

  /* SMEM parameters updated every 192 chips in TX interrupt */
  cdmafw_rf_read_interface_t              rf_rd;

  /* Walsh power data updated every time a walsh power measurement is requested */
  cdmafw_qlic_walsh_pwr_read_interface_t  walshPowerData;

  /* QPCH stats updated every time a QPCH bit is received */
  cdmafw_online_qpch_stats_interface_t    qpchStats;

  /* Decode done parameters updated at PCG0 for previous frame */
  cdmafw_rc11_fch_decode_done_info_t      adv1xDecodeDoneInfo;

  /* RC11 FCH ACK log buffer - updated every frame */
  cdmafw_fl_rc11_ack_log_buffer_t         ackLogBuffer;

  /* Difference in Cx8k between STMR & VSRC (updated every frame) */
  uint32                                  stmrVsrcDiffCx8k[CDMAFW_NUM_ANTENNAS];

  /*! \brief Number of HDET requests successfully serviced by FW from the
   * start of time. Will wrap-around to 0 after 2^31 -1 . SW should read this
   * value before sending a HDET request to FW and expect the incremented
   * value when FW finishes servicing the request. FW never resets this. So
   * the value is non-volatile between TX reinits. The only time this is
   * cleared is when the shared mem is cleared.*/
  uint32                                  numHdetServiced;

  /*! @brief Indicates if a HDET request is sucessfully serviced. SW should
   * first wait for the numHdetServiced to be incremented and then read this
   * variable.
   */
  boolean                                 hdetSuccess;

  /*! @brief The "frozen" tx total power value during the HDET measurement
   * process.
   */
  int16                                   txHdetTotalPwr;

  /*! @brief Log Buffer. 
   *
   * FW stores 8 PCG worth of data in a single log buffer. There are two log buffers
   * that are used in a ping-pong fashion (FW writes to one log buffer while SW reads
   * from the other. The log buffer structure is shown below.
   *
   * \verbatim
   * Time Stamp and TxTotal.
   *
   *    15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
   *  +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
   *  |                                  txTotal                                      |
   *  +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
   *  +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
   *  | frame count  |   PCG count       |              4-chip count                  |
   *  +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
   *
   * RL digital gains (incl BetaPGain), and Open Loop.
   * 
   *   15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
   * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
   * |                         Pilot Channel Gain                                    |
   * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
   * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
   * |                         ACK Channel Gain                                      |
   * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
   * 
   *   15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
   * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
   * |                         FCH Channel Gain                                      |
   * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
   * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
   * |                         SCH Channel Gain                                      |
   * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
   *
   *   15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
   * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
   * |                   TX Open Loop Power (1/640 dBm)                              |
   * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
   * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
   * |                         txBetaPGain (log)                                     |
   * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
   *
   * txRateAdj & Closed Loop Power.
   * 
   *   15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
   * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
   * |                   TX Closed Loop Power (1/512 dB)                             |
   * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
   * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
   * |                             TxRateAdj                                         |
   * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
   *
   * "The Others"
   * 
   *    15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
   *  +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
   *  |///////////////////////////////////////////////////////////| flRC11FchRateInfo |
   *  +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
   *  +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
   *  | VF |   PA    | RPCmsk  | RPCraw  | RPCrat  | SA | FA |   FPC   | LN | LX | AS |
   *  +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
   *
   *  AS - ACK suppressed ?
   *  LX - txLimitingMax ?
   *  LN - txLimitingMin ?
   *  FPC - fpcDecision
   *  FA - FCH ACK received ?
   *  SA - SCH ACK received ?
   *  RPCrat - RPC ratcheted
   *  RPCraw - RPC raw
  *  RPCmsk - RPC masked
    *  PA - PA state
    *  VF - flValidFPCFrame
    *  \endverbatim
    */
    uint32 adv1xHalfPcgLogBuffer[CDMAFW_LOG_NUM_BUFFERS][CDMAFW_LOG_BUFFER_SIZE];

  /*!
   * @brief TX timing info when DAC_START message is received.
   *
   * This structure contains useful TX timing information when TX DAC_START message is
   * received.
   */
  cfw_tx_dac_start_timing_info_t dacStartTimingInfo;

  /*!
   * @brief Real-time TX timing info.
   *
   * This structure disseminates useful real-time TX timing information.
   *
   */
  cdmafw_tx_timing_info_t txTimingInfo;

  /*! @brief Timed RX_STOP info (status: _SUCCESS/_FAILURE)
  */
  cdmafw_timed_rx_stop_info_t timed_rx_stop_info;

  // status of of FW TxAGC . TRUE if TxAGC is stopped. FALSE if TxAGC is
  // running. This flag is set when the cdmafw_stop_txagc_update_msg_t from
  // RFSW takes affect in FW. SW can poll on this flag to wait on FW TxAGC
  // status change after sending a cdmafw_stop_txagc_update_msg_t MSG.
  boolean stopTxAgcUpdates __attribute__((deprecated));

  /*! @brief Demod Finger SMEM Interface (new on BOLT)
  */  
  cdmafw_demod_interface_t          demod;

  /*! @brief Demod Timing SMEM Interface (new on BOLT)
  */
  cdmafw_fl_timing_interface_t      flTiming;   

  int16 fwRfJDAlgoStatus;   /* Jammer Detection HW status */
  int16 jamDetectedFlag;    /* Jammer Dtected Flag to be read by RFSW */

  /*! @brief Demback ID SMEM Interface (new on BOLT)
  */
  cdmafw_demback_interface_t     dembackId;    

  /** Encoder Input VPE0 LMEM address for each encoder channel 
   * This field holds valid value at the end of TX Start processing, so it
   * can be accessed after the receipt of TX START response */
  cdmafw_hwapi_enc_mp_ptr_t encInLMEM0Addr[CDMAFW_NUM_ENCODER_CHANNELS];

  /** Interleaver Output VPE0 LMEM address for each encoder channel -
   * ping and pong */
  cdmafw_hwapi_enc_mp_ptr_t chintlvOutLMEM0Addr[2][CDMAFW_NUM_ENCODER_CHANNELS];

  /** Encoder Input Buffer VPE0 LMEM AXI Ptrs for CH1,CH2,CH3
   * This field holds valid value at the end of TX Start processing, so it
   * can be accessed after the receipt of TX START response.
   * */
  uint32 *encInLMEM0AXIAddr[CDMAFW_NUM_ENCODER_CHANNELS];

//#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY
  /** Per frame counter that counts in units of 2 symbols (128 Cx1) to report
   * the duration when (txTotalPwr > txLimitingMaxPower). This is updated
   * every frame @ (PCG15,SYM22). */
  uint16 txLimitingMaxCount;
//#endif

  /** Sample ram address for 1X sample rams (AXI addr).
   * NULL => sample ram not valid
   */
  cdmafw_fl_samp_srv_info_t sampleSrvL1SInfo;


  /** CH0 Symbol Buffer SMEM - see struct for more details */
  cdmafw_ch0_symbol_buffer_intf_t ch0SymBufferInfo;
  
} cdmafw_smem_read_intf_t;

/** Software Writable Shared Memory Interface Structure. Firmware only reads from this structure. */
typedef struct ALIGN(32)  {

  /** Unused interface (generally used for FW standalone debugging) */
  uint16 channelConfig;

  /** Enable/Disable FW standalone debug logging interface */
  ALIGN(32)  uint8     logEnable;

  /** LNA state machine interface */
  cfw_rf_LNA_interface_t                  cfw_lna_cfg[CDMAFW_NUM_ANTENNAS];

  /** RDA tables used by FW for 1xA rate determination */
  cdmafw_rda_data_tables_t                rda_data;

  /** Buffer to hold data received from QXDM diag ("send data") commands */
  uint64 qxdmDiagDataBuffer[CDMAFW_QXDM_DIAG_DATA_BUF_SIZE_BYTES/8];

} cdmafw_smem_write_intf_t;

/** Structures to peek and poke FW internal variables for
 *  debug purposes */
typedef struct {
  uint32 var_update;
  uint32 var;
} cfw_dbg_poke_format_t;

typedef struct ALIGN(32) {
  cfw_dbg_poke_format_t poke_var[CFW_POKE_VAR_SIZE];
} cfw_dbg_poke_intf_t;

typedef struct ALIGN(32) {
  uint32 var[CFW_PEEK_VAR_SIZE];
} cfw_dbg_peek_intf_t;

/** Shared Memory Interface Structure. The shared memory should be accessed using 
  a pointer of this structure type. */
typedef struct {
  cdmafw_smem_read_intf_t                 read;
  cdmafw_smem_write_intf_t                write;
  cfw_dbg_peek_intf_t                     peek; 
  cfw_dbg_poke_intf_t                     poke;
  fw_c2k_version_t                        version;
} cdmafw_shared_memory_interface_t;
/**@}*/

/*-----------------------------------------------------------------------*/
/*     REMOVED INTERFACE VARIABLES                                       */
/*-----------------------------------------------------------------------*/
/**
  \defgroup riv Removed Interface Variables
  CDMA1x FW removed interface variables */
/*@{*/

/*! @brief Removed Interface Variables
 *
 *   This is the list of removed interface variables.
 */
typedef struct {
  uint16 TdShift[CDMAFW_NUM_FINGERS];
  /*!< obsolete */
  uint16 LockInit[CDMAFW_NUM_FINGERS];
  /*!< un-used */
  uint16 mmseRyyFeedFwdGain;
  /*!< un-used */
  uint16 mmseRyyFeedBckGain;
  /*!< un-used */
  uint16 ttInit[CDMAFW_NUM_FINGERS];
  /*!< one configuration only */
  uint16 rssiFiltGain;
  /*!< one configuration only */
  uint16 fingUpperLockThresh;
  /*!< one configuration only */
  uint16 fingRelativeLockThresh;
  /*!< one configuration only */
  uint16 fingLockAlgorithm;
  /*!< one configuration only */
  uint16 cdma1xPassThruEnable;
  /*!< obsolete */
  uint16 txRateAdjLatchOffset;
  /*!< obsolete */
  uint16 ntIoFiltGain;
  /*!< one configuration only */
  uint16 pcFiltGain;
  /*!< one configuration only */
} cdmafw_removed_variables_t;
/*@}*/

#endif /* CDMAFW_MSG_H */

