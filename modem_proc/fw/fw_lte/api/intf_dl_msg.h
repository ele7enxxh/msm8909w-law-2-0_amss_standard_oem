/*!
  @file
  intf_dl_msg.h

  @brief
  This file contains LTE LL1 DL command id's and parameters structures

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/intf_dl_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/01/12   cj      Removing PRS CONFIG msg. 
10/12/09   pa      Fixed message payload type
07/15/09   anm     Added PDSCH stat message to replace PDSCH result ind
02-26-09   anm     Added SIB start and stop collect umids 
===========================================================================*/
#ifndef INTF_DL_MSG_H
#define INTF_DL_MSG_H

#include "msgr.h"
#include "msgr_types.h"
#include "msgr_lte.h"
#include "intf_dl.h"

/*===========================================================================

      Constants

===========================================================================*/


/*===========================================================================

      Macros

===========================================================================*/


/*===========================================================================

      Typedefs

===========================================================================*/


/*! @addtogroup dl_umids Downlink Messages
 * @{
 * @section Requests
 * <table border>
 *   <tr><td><b>UMID</b></td><td><b>Structure</b></td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_CH_CONFIG</td><td>lte_LL1_dl_ch_config_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_CH_STATE_CHANGE</td><td>lte_LL1_dl_ch_state_change_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_PDCCH_RNTI_CHANGE</td><td>lte_LL1_dl_pdcch_rnti_change_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_PBCH_STATE_CHANGE</td><td>lte_LL1_dl_pbch_state_change_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_ENABLE_DISABLE</td><td>lte_LL1_dl_enable_disable_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_TEST_DDE</td><td>lte_LL1_dl_test_dde_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_TEST_DDE_SF</td><td>lte_LL1_dl_test_dde_sf_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_TEST_CHEST</td><td>lte_LL1_dl_test_chest_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_TEST_FTL</td><td>lte_LL1_dl_test_ftl_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_TEST_TTL</td><td>lte_LL1_dl_test_ttl_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_TTL_CONFIG</td><td>void</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_FTL_CONFIG</td><td>void</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_MEAS_CONFIG</td><td>void</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_MEAS_SCELL</td><td>lte_LL1_meas_scell_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_MEAS_TRACKING_NCELL</td><td>lte_LL1_meas_tracking_ncell_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_MEAS_TRACKING_NCELL_ABORT</td><td>lte_LL1_meas_tracking_ncell_abort_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_SI_START_COLLECT</td><td>lte_LL1_dl_si_start_collect_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_SI_STOP_COLLECT</td><td>lte_LL1_dl_si_stop_collect_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_RLM</td><td>lte_LL1_dl_rlm_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_CONFIG_CE_GENERAL</td><td>lte_LL1_dl_config_ce_general_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_CONFIG_CE_IIR</td><td>lte_LL1_dl_config_ce_iir_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_PDSCH_TEST</td><td>lte_LL1_dl_pdsch_test_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_PDSCH_CONFIG</td><td>lte_LL1_dl_pdsch_config_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_PDCCH_TEST</td><td>lte_LL1_dl_pdcch_test_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_MEAS_SCELL_CONFIG</td><td>lte_LL1_meas_scell_config_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_MEAS_SCELL_ODYSSEY</td><td>void</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_TTL_SCELL_CONFIG</td><td>lte_LL1_ttl_scell_config_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_FTL_SCELL_CONFIG</td><td>lte_LL1_ftl_scell_config_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_CSF_CONFIG </td><td>lte_LL1_dl_csf_config_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_CSF_SCHED </td><td>lte_LL1_dl_csf_sched_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_CSF_DECONFIG</td><td>lte_LL1_dl_csf_deconfig_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_PBCH_DECODE</td><td>lte_LL1_dl_pbch_decode_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_PBCH_STOP_DECODE</td><td>lte_LL1_dl_pbch_stop_decode_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_RXAGC_CFG_PARAM</td><td>lte_LL1_rxagc_cfg_param_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_MEAS_TTL_FTL_NCELL_CONN</td><td>lte_LL1_meas_ttl_ftl_ncell_conn_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_MEAS_TTL_FTL_NCELL_DRX</td><td>lte_LL1_meas_ttl_ftl_ncell_drx_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_MEAS_TTL_FTL_NCELL_ABORT</td><td>lte_LL1_meas_ttl_ftl_ncell_abort_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_RVAFC_GET_FREQ_CORR</td><td>lte_LL1_rvafc_get_freq_corr_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_SPS_DEACTIVATE</td><td>lte_LL1_dl_sps_deactivate_req_msg_struct</td></tr>
 * </table>
 * <table border>
 *   <tr><td><b>UMID</b></td><td><b>Structure</b></td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_MEAS_TTL_FTL_NCELL_CDRX_SCHED</td><td>lte_LL1_meas_ttl_ftl_ncell_cdrx_sched_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_MEAS_TTL_FTL_NCELL_CDRX_CONT</td><td>lte_LL1_meas_ttl_ftl_ncell_cdrx_cont_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_ANT_CORR_ENABLE_DISABLE</td><td>lte_LL1_ant_corr_enable_disable_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_AP_CSF_SCHED </td><td>lte_LL1_dl_csf_sched_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_PRS_MEAS</td><td>lte_LL1_dl_prs_meas_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_PRS_ABORT</td><td>lte_LL1_dl_prs_abort_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_PMCH_DECODE</td><td>lte_LL1_dl_pmch_decode_req_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_PMCH_DECODE_STOP</td><td>lte_LL1_dl_pmch_decode_stop_req_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_REQ_PDCCH_MONITOR_CANCEL</td><td>lte_LL1_dl_pdcch_monitor_cancel_req_msg_struct</td></tr>
 * </table>
 * @section Confirmations
 * <table border>
 *   <tr><td><b>UMID</b></td><td><b>Structure</b></td></tr>
 *   <tr><td>LTE_LL1_DL_CNF_CH_CONFIG</td><td>lte_LL1_dl_ch_config_cnf_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_CNF_CH_STATE_CHANGE</td><td>lte_LL1_dl_ch_state_change_cnf_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_CNF_PDCCH_RNTI_CHANGE</td><td>lte_LL1_dl_pdcch_rnti_change_cnf_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_CNF_PBCH_STATE_CHANGE</td><td>lte_LL1_dl_pbch_state_change_cnf_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_CNF_ENABLE_DISABLE</td><td>lte_LL1_dl_enable_disable_cnf_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_CNF_MEAS_SCELL</td><td>lte_LL1_meas_scell_cnf_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_CNF_MEAS_TRACKING_NCELL</td><td>lte_LL1_meas_tracking_ncell_cnf_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_CNF_MEAS_TRACKING_NCELL_ABORT</td><td>lte_LL1_meas_tracking_ncell_abort_cnf_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_CNF_SI_START_COLLECT</td><td>lte_LL1_dl_si_start_collect_cnf_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_CNF_SI_STOP_COLLECT</td><td>lte_LL1_dl_si_stop_collect_cnf_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_CNF_PDSCH_TEST</td><td>lte_LL1_dl_pdsch_test_cnf_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_CNF_PDSCH_CONFIG</td><td>lte_LL1_dl_pdsch_config_cnf_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_CNF_MEAS_SCELL_ODYSSEY</td><td>void</td></tr>
 *   <tr><td>LTE_LL1_DL_CNF_CSF_CONFIG</td><td>lte_LL1_dl_csf_config_cnf_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_CNF_CSF_DECONFIG</td><td>lte_LL1_dl_csf_deconfig_cnf_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_CNF_PBCH_DECODE</td><td>lte_LL1_dl_pbch_decode_cnf_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_CNF_PBCH_STOP_DECODE</td><td>lte_LL1_dl_pbch_stop_decode_cnf_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_CNF_RXAGC_CFG_PARAM</td><td>lte_LL1_rxagc_cfg_param_cnf_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_CNF_MEAS_TTL_FTL_NCELL</td><td>lte_LL1_meas_ttl_ftl_ncell_cnf_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_CNF_MEAS_TTL_FTL_NCELL_ABORT</td><td>lte_LL1_meas_ttl_ftl_ncell_abort_cnf_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_CNF_RLM</td><td>lte_LL1_rlm_req_cnf_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_CNF_RVAFC_FREQ_CORR</td><td>lte_LL1_rvafc_freq_corr_cnf_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_CNF_PRS_MEAS</td><td>lte_LL1_dl_prs_meas_cnf_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_CNF_PRS_ABORT</td><td>lte_LL1_dl_prs_abort_cnf_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_CNF_PMCH_DECODE_STOP</td><td>lte_LL1_dl_pmch_decode_stop_cnf_struct</td></tr>
 * </table>
 * @section Indications
 * <table border>
 *   <tr><td><b>UMID</b></td><td><b>Structure</b></td></tr>
 *   <tr><td>LTE_LL1_DL_IND_PDCCH_PHICH</td><td>lte_LL1_dl_pdcch_phich_ind_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_IND_PDSCH_RESULT</td><td>lte_LL1_dl_pdsch_result_ind_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_IND_PBCH_RESULT</td><td>lte_LL1_dl_pbch_result_ind_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_IND_RLM_BLER</td><td>lte_LL1_dl_rlm_bler_ind_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_IND_PDSCH_STAT</td><td>lte_LL1_dl_pdsch_stat_ind_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_DL_IND_ANT_CORR_RESULT</td><td>lte_LL1_ant_corr_result_ind_msg_struct</td></tr>
 * </table>
 * @}
 */


/*! Enum for the external saps used to interface with L1 */   
// Note: new messages must be registered with message router 
//       in cmd_proc_thread.c
typedef enum
{

  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, CH_CONFIG, 0x0, lte_LL1_dl_ch_config_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, CH_STATE_CHANGE, 0x1, lte_LL1_dl_ch_state_change_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, PDCCH_RNTI_CHANGE, 0x2, lte_LL1_dl_pdcch_rnti_change_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, PBCH_STATE_CHANGE, 0x3, lte_LL1_dl_pbch_state_change_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, ENABLE_DISABLE, 0x4, lte_LL1_dl_enable_disable_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, TEST_DDE, 0x5, lte_LL1_dl_test_dde_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, TEST_DDE_SF, 0x6, lte_LL1_dl_test_dde_sf_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, TEST_CHEST, 0x7, lte_LL1_dl_test_chest_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, TEST_FTL, 0x8, lte_LL1_dl_test_ftl_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, TEST_TTL, 0x9, lte_LL1_dl_test_ttl_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, TTL_CONFIG, 0xA, void),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, FTL_CONFIG, 0xB, void),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, MEAS_CONFIG, 0xC, void),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, MEAS_SCELL, 0xD, lte_LL1_meas_scell_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, MEAS_TRACKING_NCELL, 0xE, lte_LL1_meas_tracking_ncell_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, MEAS_TRACKING_NCELL_ABORT, 0xF, lte_LL1_meas_tracking_ncell_abort_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, SI_START_COLLECT, 0x10, lte_LL1_dl_si_start_collect_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, SI_STOP_COLLECT, 0x11, lte_LL1_dl_si_stop_collect_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, RLM, 0x12, lte_LL1_dl_rlm_req_msg_struct),
// Begin Odyssey only
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, CONFIG_CE_GENERAL, 0x13, lte_LL1_dl_config_ce_general_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, CONFIG_CE_IIR, 0x14, lte_LL1_dl_config_ce_iir_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, PDSCH_TEST, 0x15, lte_LL1_dl_pdsch_test_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, PDSCH_CONFIG, 0x16, lte_LL1_dl_pdsch_config_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, PDCCH_TEST, 0x17, lte_LL1_dl_pdcch_test_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, MEAS_SCELL_CONFIG, 0x18, lte_LL1_meas_scell_config_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, MEAS_SCELL_ODYSSEY, 0x19, void), ///< @todo Remove later
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, TTL_SCELL_CONFIG, 0x1A, lte_LL1_ttl_scell_config_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, FTL_SCELL_CONFIG, 0x1B, lte_LL1_ftl_scell_config_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, CSF_CONFIG , 0x1C, lte_LL1_dl_csf_config_req_msg_struct), ///< in espresso this is an uplink message
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, CSF_SCHED , 0x1D, lte_LL1_dl_csf_sched_req_msg_struct), ///< in espresso this is an uplink message
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, CSF_DECONFIG, 0x1E, lte_LL1_dl_csf_deconfig_req_msg_struct), ///< in espresso this is an uplink message
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, PBCH_DECODE, 0x1F, lte_LL1_dl_pbch_decode_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, PBCH_STOP_DECODE, 0x20, lte_LL1_dl_pbch_stop_decode_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, RXAGC_CFG_PARAM, 0x21, lte_LL1_rxagc_cfg_param_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, MEAS_TTL_FTL_NCELL_CONN, 0x22, lte_LL1_meas_ttl_ftl_ncell_conn_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, MEAS_TTL_FTL_NCELL_DRX, 0x23, lte_LL1_meas_ttl_ftl_ncell_drx_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, MEAS_TTL_FTL_NCELL_ABORT, 0x24, lte_LL1_meas_ttl_ftl_ncell_abort_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, RVAFC_GET_FREQ_CORR, 0x25, lte_LL1_rvafc_get_freq_corr_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, MEAS_TTL_FTL_NCELL_CDRX_SCHED, 0x26, lte_LL1_meas_ttl_ftl_ncell_cdrx_sched_req_msg_struct),  
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, MEAS_TTL_FTL_NCELL_CDRX_CONT, 0x27, lte_LL1_meas_ttl_ftl_ncell_cdrx_cont_req_msg_struct),  
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, ANT_CORR_ENABLE_DISABLE, 0x28, lte_LL1_ant_corr_enable_disable_req_msg_struct),  
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, AP_CSF_SCHED , 0x29, lte_LL1_dl_csf_sched_req_msg_struct), ///< aperiodic CSF request, deququq in a separate q
// End Odyssey only
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, PRS_MEAS, 0x2A, lte_LL1_dl_prs_meas_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, PRS_ABORT, 0x2B, lte_LL1_dl_prs_abort_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, PMCH_DECODE, 0x2C, lte_LL1_dl_pmch_decode_req_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, PMCH_DECODE_STOP, 0x2D, lte_LL1_dl_pmch_decode_stop_req_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, IQ_SAMPLE_CAPTURE, 0x2E, lte_LL1_dl_iq_sample_capture_req_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, CRSIC_LQICE, 0x2F, lte_LL1_dl_crsic_lqice_req_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, PDCCH_MONITOR_CANCEL, 0x30, lte_LL1_dl_pdcch_monitor_cancel_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, BUNDLE_CSF_SCHED, 0x31, lte_LL1_dl_bundle_csf_sched_req_msg_struct), ///< in espresso this is an uplink message
  MSGR_DEFINE_UMID(LTE, LL1_DL, REQ, SPS_DEACTIVATE, 0x32, lte_LL1_dl_sps_deactivate_req_msg_struct),
  LTE_LL1_DL_REQ_COUNT=51,

  MSGR_DEFINE_UMID(LTE, LL1_DL, CNF, CH_CONFIG, 0x0, lte_LL1_dl_ch_config_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, CNF, CH_STATE_CHANGE, 0x1, lte_LL1_dl_ch_state_change_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, CNF, PDCCH_RNTI_CHANGE, 0x2, lte_LL1_dl_pdcch_rnti_change_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, CNF, PBCH_STATE_CHANGE, 0x3, lte_LL1_dl_pbch_state_change_cnf_msg_struct), // deprecated
  MSGR_DEFINE_UMID(LTE, LL1_DL, CNF, ENABLE_DISABLE, 0x4, lte_LL1_dl_enable_disable_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, CNF, MEAS_SCELL, 0x5, lte_LL1_meas_scell_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, CNF, MEAS_TRACKING_NCELL, 0x6, lte_LL1_meas_tracking_ncell_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, CNF, MEAS_TRACKING_NCELL_ABORT, 0x7, lte_LL1_meas_tracking_ncell_abort_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, CNF, SI_START_COLLECT, 0x8, lte_LL1_dl_si_start_collect_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, CNF, SI_STOP_COLLECT, 0x9, lte_LL1_dl_si_stop_collect_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, CNF, PDSCH_TEST, 0xA, lte_LL1_dl_pdsch_test_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, CNF, PDSCH_CONFIG, 0xB, lte_LL1_dl_pdsch_config_cnf_msg_struct),
// Begin Odyssey only
  MSGR_DEFINE_UMID(LTE, LL1_DL, CNF, MEAS_SCELL_ODYSSEY, 0xC, void),  ///< @todo To coexist with Espresso. Remove once Espresso message is discarded.
  MSGR_DEFINE_UMID(LTE, LL1_DL, CNF, CSF_CONFIG,   0xD, lte_LL1_dl_csf_config_cnf_msg_struct), ///< in espresso  this is an uplink message
  MSGR_DEFINE_UMID(LTE, LL1_DL, CNF, CSF_DECONFIG, 0xE, lte_LL1_dl_csf_deconfig_cnf_msg_struct), ///< in espresso this is an uplink message
  MSGR_DEFINE_UMID(LTE, LL1_DL, CNF, PBCH_DECODE, 0xF, lte_LL1_dl_pbch_decode_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, CNF, PBCH_STOP_DECODE, 0x10, lte_LL1_dl_pbch_stop_decode_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, CNF, RXAGC_CFG_PARAM, 0x11, lte_LL1_rxagc_cfg_param_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, CNF, MEAS_TTL_FTL_NCELL, 0x12, lte_LL1_meas_ttl_ftl_ncell_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, CNF, MEAS_TTL_FTL_NCELL_ABORT, 0x13, lte_LL1_meas_ttl_ftl_ncell_abort_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, CNF, RLM, 0x14, lte_LL1_rlm_req_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, CNF, RVAFC_FREQ_CORR, 0x15, lte_LL1_rvafc_freq_corr_cnf_msg_struct),  
// End Odyssey only
  MSGR_DEFINE_UMID(LTE, LL1_DL, CNF, PRS_MEAS, 0x16, lte_LL1_dl_prs_meas_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, CNF, PRS_ABORT, 0x17, lte_LL1_dl_prs_abort_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, CNF, PMCH_DECODE_STOP, 0x18, lte_LL1_dl_pmch_decode_stop_cnf_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, CNF, IQ_SAMPLE_CAPTURE, 0x19, lte_LL1_dl_iq_sample_capture_cnf_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, CNF, CRSIC_LQICE, 0x1A, lte_LL1_dl_crsic_lqice_cnf_struct),
  LTE_LL1_DL_CNF_COUNT=27,

  MSGR_DEFINE_UMID(LTE, LL1_DL, IND, PDCCH_PHICH, 0x0, lte_LL1_dl_pdcch_phich_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, IND, PBCH_RESULT, 0x1, lte_LL1_dl_pbch_result_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, IND, RLM_BLER, 0x2, lte_LL1_dl_rlm_bler_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, IND, PDSCH_STAT, 0x3, lte_LL1_dl_pdsch_stat_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, IND, ANT_CORR_RESULT, 0x4, lte_LL1_ant_corr_result_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, IND, EMBMS_WHITENED_MATRICES, 0x5, lte_LL1_dl_embms_whitened_matrices_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, IND, PRS_FD_DMA_READY, 0x6, lte_LL1_dl_prs_fd_dma_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_DL, IND, PRS_FD_BUFFER_LOGGING, 0x7, lte_LL1_dl_prs_fd_buffer_logging_ind_msg_struct),
  LTE_LL1_DL_IND_COUNT=8
  
} lte_LL1_DL_sap_enum;




#endif /* INTF_DL_MSG_H */





