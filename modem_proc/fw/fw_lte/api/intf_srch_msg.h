
/*!
  @file
  intf_srch_msg.h

  @brief
  This file contains LTE LL search command id's and parameters structures

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/intf_srch_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/12/09   pa      Fixed message payload type
===========================================================================*/
#ifndef INTF_SRCH_MSG_H
#define INTF_SRCH_MSG_H

#include "msgr_lte.h"
#include "msgr.h"
#include "msgr_types.h"
#include "intf_srch.h"

/*===========================================================================

      Constants

===========================================================================*/


/*===========================================================================

      Macros

===========================================================================*/


/*===========================================================================

      Typedefs

===========================================================================*/

/*! @addtogroup srch_umids Searcher Messages
 * @{
 * @section Requests
 * <table border>
 *   <tr><td><b>UMID</b></td><td><b>Structure</b></td></tr>
 *   <tr><td>LTE_LL1_SRCH_REQ_FRONT_END_CONFIG</td><td>lte_LL1_srch_front_end_config_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_REQ_INIT_ACQ</td><td>lte_LL1_srch_init_acq_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_REQ_SERVING_CELL_SRCH</td><td>lte_LL1_srch_serving_cell_srch_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_REQ_NEIGHBOR_CELL_SRCH</td><td>lte_LL1_srch_neighbor_cell_srch_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_REQ_PBCH_DECODE</td><td>lte_LL1_srch_pbch_decode_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_REQ_STOP_PBCH_DECODE</td><td>msgr_hdr_struct_type</td></tr>
 *   <tr><td>LTE_LL1_SRCH_REQ_ABORT</td><td>lte_LL1_srch_init_acq_abort_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_REQ_TEST_PARAM_CFG</td><td>lte_LL1_srch_test_param_cfg_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_REQ_ABORT_NCELL_SRCH</td><td>lte_LL1_srch_ncell_srch_abort_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_REQ_PROC_CONFIG</td><td>lte_LL1_srch_proc_config_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_REQ_SRCH_BLACKLIST</td><td>lte_LL1_srch_black_list_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_REQ_LIST_FREQ_SCAN</td><td>lte_LL1_srch_list_freq_scan_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_REQ_FULL_FREQ_SCAN</td><td>lte_LL1_srch_full_freq_scan_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_REQ_ABORT_FREQ_SCAN</td><td>lte_LL1_srch_freq_scan_abort_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_REQ_MEAS_TTL_FTL_NCELL_CONN</td><td>lte_LL1_meas_ttl_ftl_ncell_conn_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_REQ_MEAS_TTL_FTL_NCELL_DRX</td><td>lte_LL1_meas_ttl_ftl_ncell_drx_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_REQ_MEAS_SCELL</td><td>lte_LL1_meas_scell_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_REQ_MEAS_TTL_FTL_NCELL_ABORT</td><td>lte_LL1_meas_ttl_ftl_ncell_abort_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_REQ_MEAS_TRACKING_NCELL</td><td>lte_LL1_meas_tracking_ncell_req_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_REQ_MEAS_TRACKING_NCELL_ABORT</td><td>lte_LL1_meas_tracking_ncell_abort_req_struct</td></tr>
 * </table>
 * @section Confirmations
 * <table border>
 *   <tr><td><b>UMID</b></td><td><b>Structure</b></td></tr>
 *   <tr><td>LTE_LL1_SRCH_CNF_FRONT_END_CONFIG</td><td>lte_LL1_srch_front_end_config_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_CNF_INIT_ACQ</td><td>lte_LL1_srch_init_acq_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_CNF_SERVING_CELL_SRCH</td><td>lte_LL1_srch_serving_cell_srch_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_CNF_NEIGHBOR_CELL_SRCH</td><td>lte_LL1_srch_neighbor_cell_srch_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_CNF_PBCH_DECODE</td><td>lte_LL1_srch_pbch_decode_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_CNF_STOP_PBCH_DECODE</td><td>msgr_hdr_struct_type</td></tr>
 *   <tr><td>LTE_LL1_SRCH_CNF_ABORT</td><td>lte_LL1_srch_init_acq_abort_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_CNF_ABORT_NCELL_SRCH</td><td>lte_LL1_srch_ncell_abort_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_CNF_PROC_CONFIG</td><td>lte_LL1_srch_proc_config_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_CNF_SRCH_BLACKLIST</td><td>lte_LL1_srch_black_list_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_CNF_LIST_FREQ_SCAN</td><td>lte_LL1_srch_list_freq_scan_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_CNF_FULL_FREQ_SCAN</td><td>lte_LL1_srch_full_freq_scan_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_CNF_ABORT_FREQ_SCAN</td><td>lte_LL1_srch_freq_scan_abort_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_SRCH_CNF_NEIGHBOR_CELL_SRCH</td><td>lte_LL1_srch_neighbor_cell_srch_ic_cnf_struct</td></tr>
 * </table>
 * @section Indications
 * <table border>
 *   <tr><td><b>UMID</b></td><td><b>Structure</b></td></tr>
 *   <tr><td>LTE_LL1_SRCH_IND_SRCH_INIT_ACQ_FIRST_CELL</td><td>lte_LL1_srch_init_acq_first_cell_ind_struct</td></tr>
 * </table>
 * @}
 */

/*! Enum for the external saps used to interface with L1 */
// Note: new messages must be registered with message router 
//       in cmd_proc_thread.c                            
typedef enum
{
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, REQ, FRONT_END_CONFIG, 0x0, lte_LL1_srch_front_end_config_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, REQ, INIT_ACQ, 0x1, lte_LL1_srch_init_acq_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, REQ, SERVING_CELL_SRCH, 0x2, lte_LL1_srch_serving_cell_srch_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, REQ, NEIGHBOR_CELL_SRCH, 0x3, lte_LL1_srch_neighbor_cell_srch_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, REQ, PBCH_DECODE, 0x4, lte_LL1_srch_pbch_decode_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, REQ, STOP_PBCH_DECODE, 0x5, msgr_hdr_struct_type),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, REQ, ABORT, 0x6, lte_LL1_srch_init_acq_abort_req_msg_struct),  
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, REQ, TEST_PARAM_CFG, 0x7, lte_LL1_srch_test_param_cfg_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, REQ, ABORT_NCELL_SRCH, 0x8, lte_LL1_srch_ncell_srch_abort_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, REQ, PROC_CONFIG, 0x9, lte_LL1_srch_proc_config_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, REQ, SRCH_BLACKLIST, 0xA, lte_LL1_srch_black_list_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, REQ, LIST_FREQ_SCAN, 0xB, lte_LL1_srch_list_freq_scan_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, REQ, FULL_FREQ_SCAN, 0xC, lte_LL1_srch_full_freq_scan_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, REQ, ABORT_FREQ_SCAN, 0xD, lte_LL1_srch_freq_scan_abort_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, REQ, MEAS_TTL_FTL_NCELL_CONN, 0xE, lte_LL1_meas_ttl_ftl_ncell_conn_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, REQ, MEAS_TTL_FTL_NCELL_DRX, 0xF, lte_LL1_meas_ttl_ftl_ncell_drx_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, REQ, MEAS_SCELL, 0x10, lte_LL1_meas_scell_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, REQ, MEAS_TTL_FTL_NCELL_ABORT, 0x11, lte_LL1_meas_ttl_ftl_ncell_abort_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, REQ, MEAS_TRACKING_NCELL, 0x12, lte_LL1_meas_tracking_ncell_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, REQ, MEAS_TRACKING_NCELL_ABORT, 0x13, lte_LL1_meas_tracking_ncell_abort_req_msg_struct),
  LTE_LL1_SRCH_REQ_COUNT=20,

  MSGR_DEFINE_UMID(LTE, LL1_SRCH, CNF, FRONT_END_CONFIG, 0x0, lte_LL1_srch_front_end_config_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, CNF, INIT_ACQ, 0x1, lte_LL1_srch_init_acq_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, CNF, SERVING_CELL_SRCH, 0x2, lte_LL1_srch_serving_cell_srch_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, CNF, NEIGHBOR_CELL_SRCH, 0x3, lte_LL1_srch_neighbor_cell_srch_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, CNF, PBCH_DECODE, 0x4, lte_LL1_srch_pbch_decode_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, CNF, STOP_PBCH_DECODE, 0x5, msgr_hdr_struct_type),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, CNF, ABORT, 0x6, lte_LL1_srch_init_acq_abort_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, CNF, ABORT_NCELL_SRCH, 0x7, lte_LL1_srch_ncell_abort_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, CNF, PROC_CONFIG, 0x8, lte_LL1_srch_proc_config_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, CNF, SRCH_BLACKLIST, 0x9, lte_LL1_srch_black_list_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, CNF, LIST_FREQ_SCAN, 0xA, lte_LL1_srch_list_freq_scan_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, CNF, FULL_FREQ_SCAN, 0xB, lte_LL1_srch_full_freq_scan_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, CNF, ABORT_FREQ_SCAN, 0xC, lte_LL1_srch_freq_scan_abort_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_SRCH, CNF, NEIGHBOR_CELL_SRCH_IC, 0xD, lte_LL1_srch_neighbor_cell_srch_ic_cnf_msg_struct),
  LTE_LL1_SRCH_CNF_COUNT=14,

} lte_LL1_srch_msg_enum;



#endif /* INTF_SRCH_MSG_H */





