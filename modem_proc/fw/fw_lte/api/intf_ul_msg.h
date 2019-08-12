
/*!
  @file
  intf_ul_msg.h

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/intf_ul_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/12/09   pa      Fixed message payload type
===========================================================================*/
#ifndef INTF_UL_MSG_H
#define INTF_UL_MSG_H

#include "msgr_lte.h"
#include "msgr.h"
#include "msgr_types.h"
#include "intf_ul.h"

/*===========================================================================

      Constants

===========================================================================*/


/*===========================================================================

      Macros

===========================================================================*/


/*===========================================================================

      Typedefs

===========================================================================*/

/*! @addtogroup ul_umids Uplink Messages
 * @{
 * @section Requests
 * <table border>
 *   <tr><td><b>UMID</b></td><td><b>Structure</b></td></tr>
 *   <tr><td>LTE_LL1_UL_REQ_PHYCHAN_CONFIG</td><td>lte_LL1_ul_phychan_config_req_struct</td></tr>
 *   <tr><td>LTE_LL1_UL_REQ_PHYCHAN_DECONFIG</td><td>lte_LL1_ul_phychan_deconfig_req_struct</td></tr>
 *   <tr><td>LTE_LL1_UL_REQ_PHYCHAN_SCHEDULE</td><td>lte_LL1_ul_phychan_schedule_req_struct</td></tr>
 *   <tr><td>LTE_LL1_UL_REQ_IFFT_SCALING_CHANGE</td><td>lte_LL1_ul_ifft_scaling_change_req_struct</td></tr>
 *   <tr><td>LTE_LL1_UL_REQ_TX_WINDOW_LENGTH_CHANGE</td><td>lte_LL1_ul_tx_window_length_change_req_struct</td></tr>
 *   <tr><td>LTE_LL1_UL_REQ_CNTL_INFO_TEST</td><td>lte_LL1_ul_cntl_info_test_req_struct</td></tr>
 *   <tr><td>LTE_LL1_UL_REQ_CTRL_INFO_BUF_TEST</td><td>lte_LL1_ul_ctrl_info_buf_test_req_struct</td></tr>
 *   <tr><td>LTE_LL1_UL_REQ_CSF_CTRL_INFO_TEST</td><td>lte_LL1_ul_csf_ctrl_info_test_req_struct</td></tr>
 *   <tr><td>LTE_LL1_UL_REQ_SIM_UL_DL_TEST</td><td>lte_LL1_ul_sim_ul_dl_test_req_struct</td></tr>
 *   <tr><td>LTE_LL1_UL_REQ_CAL_CONT_TX</td><td>lte_LL1_ul_cal_cont_tx_req_struct</td></tr>
 *   <tr><td>LTE_LL1_UL_REQ_TX_LM_CONFIG</td><td>lte_LL1_ul_tx_lm_config_req_struct</td></tr>
 * </table>
 * </table>
 * @section Confirmations
 * <table border>
 *   <tr><td><b>UMID</b></td><td><b>Structure</b></td></tr>
 *   <tr><td>LTE_LL1_UL_CNF_PHYCHAN_CONFIG</td><td>lte_LL1_ul_phychan_config_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_UL_CNF_PHYCHAN_DECONFIG</td><td>lte_LL1_ul_phychan_deconfig_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_UL_CNF_IFFT_SCALING_CHANGE</td><td>lte_LL1_ul_ifft_scaling_change_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_UL_CNF_TX_WINDOW_LENGTH_CHANGE</td><td>lte_LL1_ul_tx_window_length_change_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_UL_CNF_TX_LM_CONFIG</td><td>lte_LL1_ul_tx_lm_config_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_UL_CNF_ASYNC_PHYCHAN_REQUEST</td><td>lte_LL1_ul_aysnc_phychan_request_cnf_struct</td></tr>
 * </table>
 * </table>
 * @section Indications
 * <table border>
 *   <tr><td><b>UMID</b></td><td><b>Structure</b></td></tr>
 *   <tr><td>LTE_LL1_UL_IND_HDET</td><td>lte_LL1_ul_hdet_ind_struct</td></tr>
 *   <tr><td>LTE_LL1_UL_IND_HDET_FW_RD</td><td>lte_LL1_ul_hdet_fw_rd_ind_struct</td></tr>
 *   <tr><td>LTE_LL1_UL_IND_THERM_FW_RD</td><td>lte_LL1_ul_therm_fw_rd_ind_struct</td></tr>
 *   <tr><td>LTE_LL1_UL_IND_PUSCH_TX_STATUS</td><td>lte_LL1_ul_pusch_tx_status_ind_struct</td></tr>
 * </table>
 * @}
 */

/*! Enum for the external saps used to interface with L1 */
// Note: new messages must be registered with message router 
//       in cmd_proc_thread.c   
typedef enum
{
  MSGR_DEFINE_UMID(LTE, LL1_UL, REQ, PHYCHAN_CONFIG, 0x0, lte_LL1_ul_phychan_config_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_UL, REQ, PHYCHAN_DECONFIG, 0x1, lte_LL1_ul_phychan_deconfig_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_UL, REQ, PHYCHAN_SCHEDULE, 0x2, lte_LL1_ul_phychan_schedule_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_UL, REQ, IFFT_SCALING_CHANGE, 0x3, lte_LL1_ul_ifft_scaling_change_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_UL, REQ, TX_WINDOW_LENGTH_CHANGE, 0x4, lte_LL1_ul_tx_window_length_change_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_UL, REQ, CNTL_INFO_TEST, 0x5, lte_LL1_ul_cntl_info_test_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_UL, REQ, CTRL_INFO_BUF_TEST, 0x6, lte_LL1_ul_ctrl_info_buf_test_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_UL, REQ, CSF_CTRL_INFO_TEST, 0x7, lte_LL1_ul_csf_ctrl_info_test_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_UL, REQ, SIM_UL_DL_TEST, 0x8, lte_LL1_ul_sim_ul_dl_test_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_UL, REQ, CAL_CONT_TX, 0x9, lte_LL1_ul_cal_cont_tx_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_UL, REQ, TX_LM_CONFIG, 0xA, lte_LL1_ul_tx_lm_config_req_msg_struct),  
  MSGR_DEFINE_UMID(LTE, LL1_UL, REQ, IQ_SAMP_CAPTURE, 0xB, lte_LL1_ul_iq_samp_capture_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_UL, REQ, ET_RESOURCE_ENABLE, 0xC, lte_LL1_ul_et_resource_enable_req_msg_struct),
  LTE_LL1_UL_REQ_COUNT=13,

  MSGR_DEFINE_UMID(LTE, LL1_UL, CNF, PHYCHAN_CONFIG, 0x0, lte_LL1_ul_phychan_config_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_UL, CNF, PHYCHAN_DECONFIG, 0x1, lte_LL1_ul_phychan_deconfig_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_UL, CNF, IFFT_SCALING_CHANGE, 0x2, lte_LL1_ul_ifft_scaling_change_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_UL, CNF, TX_WINDOW_LENGTH_CHANGE, 0x3, lte_LL1_ul_tx_window_length_change_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_UL, CNF, TX_LM_CONFIG, 0x4, lte_LL1_ul_tx_lm_config_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_UL, CNF, ASYNC_PHYCHAN_REQUEST, 0x5, lte_LL1_ul_aysnc_phychan_request_cnf_msg_struct),  
  MSGR_DEFINE_UMID(LTE, LL1_UL, CNF, IQ_SAMP_CAPTURE, 0x6, lte_LL1_ul_iq_samp_capture_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_UL, CNF, ET_RESOURCE_ENABLE, 0x7, lte_LL1_ul_et_resource_enable_cnf_msg_struct),
  LTE_LL1_UL_CNF_COUNT=8,

  MSGR_DEFINE_UMID(LTE, LL1_UL, IND, HDET, 0x0, lte_LL1_ul_hdet_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_UL, IND, HDET_FW_RD, 0x1, lte_LL1_ul_hdet_fw_rd_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_UL, IND, THERM_FW_RD, 0x2, lte_LL1_ul_therm_fw_rd_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_UL, IND, PUSCH_TX_STATUS, 0x3, lte_LL1_ul_pusch_tx_status_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_UL, IND, TDET_TEMP_COMP_FW_RD, 0x4, lte_LL1_ul_tdet_temp_comp_fw_rd_ind_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_UL, IND, PHYCHAN, 0x5, lte_LL1_ul_phychan_ind_msg_struct),
  LTE_LL1_UL_IND_COUNT=6,

} lte_LL1_ul_msg_enum;



#endif /* INTF_UL_MSG_H */

