
/*!
  @file
  intf_sys_msg.h

  @brief
  This file contains LTE LL system command id's and parameters structures

  @detail
  description of functions in this file
 
*/

/*===========================================================================

  Copyright (c) 2008 - 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/intf_sys_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/31/11   isalman Don't use q6 headers when compiling for UMPLT.
03/03/11   nbb     Added RF script + RXLM config message
10/12/09   pa      Fixed message payload type
===========================================================================*/
#ifndef INTF_SYS_MSG_H
#define INTF_SYS_MSG_H

#include "msgr_lte.h"
#include "msgr.h"
#include "msgr_types.h"
#include "lte_app.h"
#include "intf_sys.h"

/*===========================================================================

      Constants

===========================================================================*/


/*===========================================================================

      Macros

===========================================================================*/


/*===========================================================================

      Typedefs

===========================================================================*/

/*!
 * @addtogroup sys_umids System Messages
 * @{
 * @section Requests
 * <table border>
 *   <tr><td><b>UMID</b></td><td><b>Structure</b></td></tr>
 *   <tr><td>LTE_LL1_SYS_REQ_CONFIG_SERVING_CELL</td><td>lte_LL1_sys_config_serving_cell_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_REQ_CONFIG_UE</td><td>lte_LL1_sys_config_ue_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_REQ_SLAM_TIMING</td><td>lte_LL1_sys_slam_timing_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_REQ_ADJ_TIMING</td><td>lte_LL1_sys_adj_timing_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_REQ_TA_ADJ</td><td>lte_LL1_sys_ta_adj_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_REQ_PARAM_CONFIG</td><td>lte_LL1_sys_param_config_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_REQ_STOP_SYS_TIME</td><td>lte_LL1_sys_stop_sys_time_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_REQ_LOG_MASK_CONFIG</td><td>lte_LL1_sys_log_mask_config_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_REQ_CONFIG_APP</td><td>lte_LL1_sys_config_app_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_REQ_SLEEP</td><td>lte_LL1_sys_sleep_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_REQ_RUN_RF_SCRIPT</td><td>lte_LL1_sys_run_rf_script_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_REQ_SCELL_STAT_CONFIG</td><td>lte_LL1_sys_scell_stat_config_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_REQ_SCELL_SEMI_STAT_CONFIG</td><td>lte_LL1_sys_scell_semi_stat_config_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_REQ_GPS_TIMETAG</td><td>lte_LL1_sys_gps_timetag_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_REQ_SYNC_STMR_DUMP</td><td>lte_LL1_sys_sync_stmr_dump_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_REQ_RUN_RXLM_RF_SCRIPT</td><td>lte_LL1_sys_run_rxlm_rf_script_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_REQ_IQMC_UPDATE</td><td>lte_LL1_sys_iqmc_update_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_REQ_RX_ANT_MODE_CHG</td><td>lte_LL1_sys_rx_ant_mode_chg_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_REQ_EIB_ADDR</td><td> lte_LL1_sys_eib_addr_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_REQ_FREEZE_UPDATE</td><td> lte_LL1_sys_freeze_update_req_struct</td></tr>
 * </table>
 * @section Confirmations
 * <table border>
 *   <tr><td><b>UMID</b></td><td><b>Structure</b></td></tr>
 *   <tr><td>LTE_LL1_SYS_CNF_CONFIG_SERVING_CELL</td><td>lte_LL1_sys_config_serving_cell_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_CNF_CONFIG_UE</td><td>lte_LL1_sys_config_ue_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_CNF_SLAM_TIMING</td><td>lte_LL1_sys_slam_timing_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_CNF_ADJ_TIMING</td><td>lte_LL1_sys_adj_timing_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_CNF_TA_ADJ</td><td>lte_LL1_sys_ta_adj_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_CNF_PARAM_CONFIG</td><td>lte_LL1_sys_param_config_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_CNF_STOP_SYS_TIME</td><td>lte_LL1_sys_stop_sys_time_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_CNF_CONFIG_APP</td><td>lte_LL1_sys_config_app_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_CNF_SLEEP</td><td>lte_LL1_sys_sleep_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_CNF_RUN_RF_SCRIPT</td><td>lte_LL1_sys_run_rf_script_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_CNF_SCELL_STAT_CONFIG</td><td>lte_LL1_sys_scell_stat_config_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_CNF_SCELL_SEMI_STAT_CONFIG</td><td>lte_LL1_sys_scell_semi_stat_config_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_CNF_GPS_TIMETAG</td><td>lte_LL1_sys_gps_timetag_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_CNF_SYNC_STMR_DUMP</td><td>lte_LL1_sys_sync_stmr_dump_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_CNF_RUN_RXLM_RF_SCRIPT</td><td>lte_LL1_sys_run_rxlm_rf_script_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_CNF_EIB_ADDR</td><td> lte_LL1_sys_eib_addr_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_CNF_RX_ANT_MODE_CHG</td><td>lte_LL1_sys_rx_ant_mode_chg_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_CNF_DUAL_DEMOD_MAP_CHG</td><td>lte_LL1_sys_dual_demod_map_chg_cnf_struct</td></tr> 
 *   <tr><td>LTE_LL1_SYS_CNF_FREEZE_UPDATE</td><td> lte_LL1_sys_freeze_update_cnf_struct</td></tr>
 * </table>
 * @section Indications
 * <table border>
 *   <tr><td><b>UMID</b></td><td><b>Structure</b></td></tr>
 *   <tr><td>LTE_LL1_SYS_IND_SUBFRAME</td><td>lte_LL1_sys_subframe_ind_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_IND_RECOVERY</td><td>lte_LL1_sys_recovery_ind_struct</td></tr>
 *   <tr><td>LTE_LL1_SYS_IND_SAMPLE_REC_DONE</td><td>lte_LL1_sys_sample_rec_done_ind_msg_struct</td></tr>
 * </table>
 * @}
 */

/*! Enum for the external saps used to interface with L1 */
// Note: new messages must be registered with message router 
//       in cmd_proc_thread.c   

typedef enum
{
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, CONFIG_SERVING_CELL,      0x0, lte_LL1_sys_config_serving_cell_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, CONFIG_UE,                0x1, lte_LL1_sys_config_ue_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, SLAM_TIMING,              0x2, lte_LL1_sys_slam_timing_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, ADJ_TIMING,               0x3, lte_LL1_sys_adj_timing_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, TA_ADJ,                   0x4, lte_LL1_sys_ta_adj_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, PARAM_CONFIG,             0x5, lte_LL1_sys_param_config_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, STOP_SYS_TIME,            0x6, lte_LL1_sys_stop_sys_time_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, LOG_MASK_CONFIG,          0x7, lte_LL1_sys_log_mask_config_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, CONFIG_APP,               0x8, lte_LL1_sys_config_app_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, SLEEP,                    0x9, lte_LL1_sys_sleep_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, RUN_RF_SCRIPT,            0xA, lte_LL1_sys_run_rf_script_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, SCELL_STAT_CONFIG,        0xB, lte_LL1_sys_scell_stat_config_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, SCELL_SEMI_STAT_CONFIG,   0xC, lte_LL1_sys_scell_semi_stat_config_req_msg_struct),  
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, GPS_TIMETAG,              0xD, lte_LL1_sys_gps_timetag_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, SYNC_STMR_DUMP,           0xE, lte_LL1_sys_sync_stmr_dump_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, RUN_RXLM_RF_SCRIPT,       0xF, lte_LL1_sys_run_rxlm_rf_script_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, NB_CARRIER_SWITCH,       0x10, lte_LL1_sys_nb_carrier_switch_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, MEMPOOL_BUF_DETAILS,       0x11, lte_LL1_sys_mempool_buf_details_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, SCELL_STAT_DECONFIG,   0x12, lte_LL1_sys_scell_stat_deconfig_req_msg_struct),  
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, CXM_BOOT_PARAMS,          0x13, lte_LL1_sys_cxm_boot_params_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, CXM_ACTIVE,               0x14, lte_LL1_sys_cxm_active_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, LIGHT_SLEEP_MOD_CTRL,     0x15, lte_LL1_sys_light_sleep_mod_ctrl_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, RX_TOOL_CHAIN_CONFIG,   	0x16, lte_LL1_sys_rx_tool_chain_config_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, IQMC_UPDATE,              0x17, lte_LL1_sys_iqmc_update_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, EIB_ADDR,                 0x18, lte_LL1_sys_eib_addr_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, DRX_ON_DURATION,          0x19, lte_LL1_sys_drx_on_duration_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, CONFLICT_CHECK,           0x1a, lte_LL1_sys_conflict_check_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, DSDA_INTF_TEST,           0x1b, lte_LL1_sys_dsda_intf_test_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, MULTI_RAT_CHANGE_CXM_PARAMS, 0x1c, lte_LL1_sys_multi_rat_change_cxm_params_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, RX_ANT_MODE_CHG,          0x1d, lte_LL1_sys_rx_ant_mode_chg_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, DUAL_DEMOD_MAP_CHG,       0x1e, lte_LL1_sys_dual_demod_map_chg_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, RES_REL_ACQ,              0x1f, lte_LL1_sys_res_rel_acq_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, MACROSLEEP_SLEEP,         0x20, lte_LL1_sys_macrosleep_sleep_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, MACROSLEEP_WAKEUP,        0x21, lte_LL1_sys_macrosleep_wakeup_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, DTR_BRDG_CFG_CHG,         0x22, lte_LL1_sys_dtr_brdg_cfg_chg_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, FREEZE_UPDATE,            0x23, lte_LL1_sys_freeze_update_req_msg_struct),
#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY
  MSGR_DEFINE_UMID(LTE, LL1_SYS, REQ, ANT_SWITCH,               0x24, lte_LL1_sys_ant_switch_req_msg_struct),
  LTE_LL1_SYS_REQ_COUNT=0x25,
#else
  LTE_LL1_SYS_REQ_COUNT=0x24,
#endif
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, CONFIG_SERVING_CELL,      0x0, lte_LL1_sys_config_serving_cell_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, CONFIG_UE,                0x1, lte_LL1_sys_config_ue_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, SLAM_TIMING,              0x2, lte_LL1_sys_slam_timing_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, ADJ_TIMING,               0x3, lte_LL1_sys_adj_timing_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, TA_ADJ,                   0x4, lte_LL1_sys_ta_adj_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, PARAM_CONFIG,             0x5, lte_LL1_sys_param_config_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, STOP_SYS_TIME,            0x6, lte_LL1_sys_stop_sys_time_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, CONFIG_APP,               0x7, lte_LL1_sys_config_app_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, SLEEP,                    0x8, lte_LL1_sys_sleep_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, RUN_RF_SCRIPT,            0x9, lte_LL1_sys_run_rf_script_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, SCELL_STAT_CONFIG,        0xA, lte_LL1_sys_scell_stat_config_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, SCELL_SEMI_STAT_CONFIG,   0xB, lte_LL1_sys_scell_semi_stat_config_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, GPS_TIMETAG,              0xC, lte_LL1_sys_gps_timetag_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, SYNC_STMR_DUMP,           0xD, lte_LL1_sys_sync_stmr_dump_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, RUN_RXLM_RF_SCRIPT,       0xE, lte_LL1_sys_run_rxlm_rf_script_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, NB_CARRIER_SWITCH,        0xF, lte_LL1_sys_nb_carrier_switch_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, MEMPOOL_BUF_DETAILS,        0x10, lte_LL1_sys_mempool_buf_details_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, SCELL_STAT_DECONFIG,        0x11, lte_LL1_sys_scell_stat_deconfig_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, CXM_BOOT_PARAMS,          0x12, lte_LL1_sys_cxm_boot_params_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, CXM_ACTIVE,               0x13, lte_LL1_sys_cxm_active_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, LIGHT_SLEEP_MOD_CTRL,     0x14, lte_LL1_sys_light_sleep_mod_ctrl_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, RX_TOOL_CHAIN_CONFIG,	0x15, lte_LL1_sys_rx_tool_chain_config_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, EIB_ADDR,                 0x16, lte_LL1_sys_eib_addr_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, CONFLICT_CHECK,           0x17, lte_LL1_sys_conflict_check_cnf_msg_struct), 
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, RX_ANT_MODE_CHG,          0x18, lte_LL1_sys_rx_ant_mode_chg_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, DUAL_DEMOD_MAP_CHG,       0x19, lte_LL1_sys_dual_demod_map_chg_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, RES_REL_ACQ,              0x1a, lte_LL1_sys_res_rel_acq_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, MACROSLEEP_SLEEP,         0x1b, lte_LL1_sys_macrosleep_sleep_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, MACROSLEEP_WAKEUP,        0x1c, lte_LL1_sys_macrosleep_wakeup_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, FREEZE_UPDATE,            0x1d, lte_LL1_sys_freeze_update_cnf_msg_struct),
#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY
  MSGR_DEFINE_UMID(LTE, LL1_SYS, CNF, ANT_SWITCH,               0x1e, lte_LL1_sys_ant_switch_cnf_msg_struct),
  LTE_LL1_SYS_CNF_COUNT=0x1f,
#else
  LTE_LL1_SYS_CNF_COUNT=0x1e,
#endif

  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, SUBFRAME, 0x0, lte_LL1_sys_subframe_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_SRCH_PSS_RESULT_LOG_C, 0x1, lte_LL1_log_srch_pss_results_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_SRCH_SSS_RESULT_LOG_C, 0x2, lte_LL1_log_srch_sss_results_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_UL_PUSCH_TX_REPORT_LOG_C, 0x3, lte_LL1_log_ul_pusch_tx_report_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_UL_PUCCH_TX_REPORT_LOG_C, 0x4, lte_LL1_log_ul_pucch_tx_report_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_UL_SRS_TX_REPORT_LOG_C, 0x5, lte_LL1_log_ul_srs_tx_report_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_UL_PRACH_TX_REPORT_LOG_C, 0x6, lte_LL1_log_ul_rach_tx_report_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_DEMFRONT_SERVING_CELL_CER_LOG_C, 0x7, lte_LL1_log_demfront_serving_cell_cer_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_DEMFRONT_PDSCH_DEMAPPER_CONFIGURATION_LOG_C, 0x8, log_demfront_pdsch_demapper_configuration_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_CSF_SE_TXMODE127_3R1_RESULT_LOG_C,   0x9, (lte_LL1_log_csf_spectral_efficiency_for_txmode_1_2_7_ind_msg_struct*)),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_CSF_SE_TXMODE456_R1_RESULT_LOG_C,   0xa, (lte_LL1_log_csf_spectral_efficiency_for_txmode_4_5_6_rank_1_ind_msg_struct*)),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_CSF_SE_TXMODE4_R2_RESULT_LOG_C,   0xb, (lte_LL1_log_csf_spectral_efficiency_for_txmode_4_rank_2_ind_msg_struct*)),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_CSF_WHITENED_MATRICES_LOG_C,   0xc, (lte_LL1_log_csf_whitened_matrices_ind_msg_struct*)),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_CSF_PUCCH_PERIODIC_REPORT_LOG_C,   0xd, (lte_LL1_log_csf_pucch_report_ind_msg_struct*)),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_CSF_PUSCH_APERIODIC_REPORT_LOG_C,   0xe, (lte_LL1_log_csf_pusch_report_ind_msg_struct*)),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_SRCH_SERVING_CELL_MEASUREMENT_RESULT_INT_LOG_C, 0xf, lte_LL1_log_srch_serving_cell_measurement_results_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_UL_AGC_TX_REPORT_LOG_C, 0x10, lte_LL1_log_ul_agc_tx_report_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_RXFE_AGC_LOG_C, 0x11, lte_LL1_log_rxfe_agc_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_RXFE_AGC_INT_LOG_C, 0x12, lte_LL1_log_rxfe_agc_int_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_SRCH_SERVING_CELL_FTL_RESULT_INT_LOG_C, 0x13, lte_LL1_log_srch_serving_cell_ftl_results_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_SRCH_SERVING_CELL_TTL_RESULT_INT_LOG_C, 0x14, lte_LL1_log_srch_serving_cell_ttl_results_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_RLM_RESULT_INT_LOG_C, 0x15, lte_LL1_log_rlm_results_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_SRCH_NCELL_MEASUREMENT_AND_TRACKING_INT_LOG_C, 0x16, lte_LL1_log_srch_neighbor_cell_measurements_and_tracking_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_ANT_CORR_RESULT_LOG_C, 0x17, lte_LL1_log_demfront_antenna_correlation_results_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_DEMFRONT_SERVING_CELL_RS_LOG_C, 0x18, lte_LL1_log_demfront_serving_cell_rs_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, RECOVERY, 0x19, lte_LL1_sys_recovery_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_UERS_TONES_LOG_C, 0x1A, lte_LL1_log_uers_tones_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, SAMPLE_REC_DONE, 0x1B, lte_LL1_sys_sample_rec_done_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_DEMFRONT_NEIGHBOR_CELL_CER_LOG_C, 0x1C, lte_LL1_log_demfront_neighbor_cell_cer_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_PRS_FW_LOG_C, 0x1D, lte_LL1_log_prs_fw_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_MBSFN_EMBMS_CER_LOG_C, 0x1E, lte_LL1_log_mbsfn_embms_cer_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_MBSFN_WHITENED_MATRICES_LOG_C, 0x1F, lte_LL1_log_mbsfn_whitened_matrices_ind_msg_struct),  
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_SERVING_CELL_COM_LOOP_LOG_C, 0x20, lte_LL1_log_serving_cell_com_loop_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LTE_LL1_CXM_WCN_THRESHOLD_EXCEEDED, 0x21, lte_LL1_sys_cxm_wcn_threshold_exceeded_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_CSF_SE_TXMODE9_RESULT_LOG_C,   0x22, (lte_LL1_log_csf_spectral_efficiency_for_txmode_9_ind_msg_struct*)),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_CSF_CSIRS_CHEST_TXMODE9_LOG_C, 0x23, (lte_LL1_log_csf_csirs_chest_for_txmode_9_ind_msg_struct*)),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, CONFLICT_CHECK, 0x24, (lte_LL1_sys_conflict_check_ind_msg_struct)),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_MULTI_RAT_DL_REPORT_LOG_C, 0x25, lte_LL1_log_multi_rat_conflict_dl_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_MULTI_RAT_UL_REPORT_LOG_C, 0x26, lte_LL1_log_multi_rat_conflict_ul_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, LOG_LTE_LL1_CSF_CSIRS_CHEST_LOG_C, 0x27, (lte_LL1_log_csf_csirs_chest_ind_msg_struct*)),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, IND, FTL_FREEZE, 0x28, lte_LL1_sys_ftl_freeze_ind_msg_struct),
  LTE_LL1_SYS_IND_COUNT=0x29

} lte_LL1_sys_msg_enum;

/*
 ** Supervisory Messages
*/
enum
{
  MSGR_DEFINE_UMID(LTE, LL1_SYS, SPR, LOOPBACK, MSGR_ID_LOOPBACK, 
                   msgr_spr_loopback_s),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, SPR, LOOPBACK_REPLY, MSGR_ID_LOOPBACK_REPLY, 
                   msgr_spr_loopback_reply_s),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, SPR, THREAD_READY, LTE_APP_ID_THREAD_READY, 
                   void),
  MSGR_DEFINE_UMID(LTE, LL1_SYS, SPR, THREAD_KILL, LTE_APP_ID_THREAD_KILL, 
                   void),
};


// XXX remove later
#define LTE_LL1_SYS_CONFIG_RX_REQ  LTE_LL1_SYS_CONFIG_APP_REQ
#define LTE_LL1_SYS_CONFIG_RX_CNF  LTE_LL1_SYS_CONFIG_APP_CNF

#define LTE_LL1_SYS_REQ_CONFIG_RX  LTE_LL1_SYS_CONFIG_APP_REQ
#define LTE_LL1_SYS_CNF_CONFIG_RX  LTE_LL1_SYS_CONFIG_APP_CNF


#endif /* INTF_SYS_MSG_H */
