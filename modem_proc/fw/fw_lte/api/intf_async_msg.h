
/*!
  @file
  intf_async_msg.h

  @brief
  This file contains LTE LL async command id's and parameters structures

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/intf_async_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/12/09   pa      Fixed message payload type
===========================================================================*/
#ifndef INTF_ASYNC_MSG_H
#define INTF_ASYNC_MSG_H

#include "msgr_lte.h"
#include "msgr.h"
#include "msgr_types.h"
#include "intf_async.h"

/*===========================================================================

      Constants

===========================================================================*/


/*===========================================================================

      Macros

===========================================================================*/


/*===========================================================================

      Typedefs

===========================================================================*/

/*! @addtogroup async_umids Asynchronous Messages
 * Additional information about Asynchronous messages
 * @{
 * @section Requests
 * <table border>
 *   <tr><td><b>UMID</b></td><td><b>Structure</b></td></tr>
 *   <tr><td>LTE_LL1_ASYNC_REQ_START_SYS_TIME</td><td>lte_LL1_async_start_sys_time_req_struct</td></tr>
 *   <tr><td>LTE_LL1_ASYNC_REQ_READ_HW_MEM</td><td>lte_LL1_async_read_hw_mem_req_struct</td></tr>
 *   <tr><td>LTE_LL1_ASYNC_REQ_WRITE_HW_MEM</td><td>lte_LL1_async_write_hw_mem_req_struct</td></tr>
 *   <tr><td>LTE_LL1_ASYNC_REQ_RESET</td><td>lte_LL1_async_reset_req_struct</td></tr>
 *   <tr><td>LTE_LL1_ASYNC_REQ_CONFIG_APP</td><td>lte_LL1_async_config_app_req_struct</td></tr>
 *   <tr><td>LTE_LL1_ASYNC_REQ_WAKEUP</td><td>lte_LL1_async_config_app_req_struct</td></tr>
 *   <tr><td>LTE_LL1_ASYNC_REQ_DOG_TIMEOUT</td><td>lte_LL1_async_dog_timeout_req_struct</td></tr>
 *   <tr><td>LTE_LL1_ASYNC_REQ_TTRANS_WAKEUP</td><td>lte_LL1_async_ttrans_wakeup_req_struct</td></tr>
 *   <tr><td>LTE_LL1_ASYNC_REQ_STOP</td><td>lte_LL1_async_stop_req_msg_struct</td></tr>
 *   <tr><td>LTE_LL1_ASYNC_REQ_PHYCHAN_SCHEDULE</td><td>lte_LL1_ul_phychan_schedule_req_struct</td></tr>
 *   <tr><td>LTE_LL1_ASYNC_REQ_DAC_CAL</td><td>lte_LL1_async_dac_cal_req_struct</td></tr>
 * </table>
 * @section Confirmations
 * <table border>
 *   <tr><td><b>UMID</b></td><td><b>Structure</b></td></tr>
 *   <tr><td>LTE_LL1_ASYNC_CNF_START_SYS_TIME</td><td>lte_LL1_async_start_sys_time_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_ASYNC_CNF_READ_HW_MEM</td><td>lte_LL1_async_read_hw_mem_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_ASYNC_CNF_WRITE_HW_MEM</td><td>lte_LL1_async_write_hw_mem_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_ASYNC_CNF_RESET</td><td>lte_LL1_async_reset_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_ASYNC_CNF_CONFIG_APP</td><td>lte_LL1_async_config_app_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_ASYNC_CNF_WAKEUP</td><td>lte_LL1_async_config_app_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_ASYNC_CNF_TTRANS_WAKEUP</td><td>lte_LL1_async_ttrans_wakeup_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_ASYNC_CNF_DAC_CAL</td><td>lte_LL1_async_dac_cal_cnf_struct</td></tr>
 *   <tr><td>LTE_LL1_ASYNC_CNF_STOP</td><td>lte_LL1_async_stop_cnf_msg_struct</td></tr>
 * </table>
 * @}
 */

/*! Enum for the external saps used to interface with L1 */
// Note: new messages must be registered with message router 
//       in cmd_proc_thread.c   

typedef enum
{
  MSGR_DEFINE_UMID(LTE, LL1_ASYNC, REQ, START_SYS_TIME,  0x0, lte_LL1_async_start_sys_time_req_msg_struct),   
  MSGR_DEFINE_UMID(LTE, LL1_ASYNC, REQ, READ_HW_MEM,     0x1, lte_LL1_async_read_hw_mem_req_msg_struct),   
  MSGR_DEFINE_UMID(LTE, LL1_ASYNC, REQ, WRITE_HW_MEM,    0x2, lte_LL1_async_write_hw_mem_req_msg_struct),   
  MSGR_DEFINE_UMID(LTE, LL1_ASYNC, REQ, RESET,           0x3, lte_LL1_async_reset_req_msg_struct),   
  MSGR_DEFINE_UMID(LTE, LL1_ASYNC, REQ, CONFIG_APP,      0x5, lte_LL1_async_config_app_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_ASYNC, REQ, WAKEUP,          0x6, lte_LL1_async_config_app_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_ASYNC, REQ, DOG_TIMEOUT,     0x7, lte_LL1_async_dog_timeout_req_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_ASYNC, REQ, TTRANS_WAKEUP,   0x8, lte_LL1_async_ttrans_wakeup_req_msg_struct),

  MSGR_DEFINE_UMID(LTE, LL1_ASYNC, REQ, STOP, 0x4, lte_LL1_async_stop_req_msg_struct),  
  MSGR_DEFINE_UMID(LTE, LL1_ASYNC, REQ, PHYCHAN_SCHEDULE, 0x9, lte_LL1_ul_phychan_schedule_req_msg_struct),  
  MSGR_DEFINE_UMID(LTE, LL1_ASYNC, REQ, DAC_CAL, 0xa, lte_LL1_async_dac_cal_req_msg_struct),  
  MSGR_DEFINE_UMID(LTE, LL1_ASYNC, REQ, MULTI_RAT_RAISE_CXM_PRIO, 0xb, lte_LL1_async_multi_rat_raise_cxm_prio_req_msg_struct), 
  LTE_LL1_ASYNC_REQ_COUNT=12,

  MSGR_DEFINE_UMID(LTE, LL1_ASYNC, CNF, START_SYS_TIME,  0x0, lte_LL1_async_start_sys_time_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_ASYNC, CNF, READ_HW_MEM,     0x1, lte_LL1_async_read_hw_mem_cnf_msg_struct),   
  MSGR_DEFINE_UMID(LTE, LL1_ASYNC, CNF, WRITE_HW_MEM,    0x2, lte_LL1_async_write_hw_mem_cnf_msg_struct),   
  MSGR_DEFINE_UMID(LTE, LL1_ASYNC, CNF, RESET,           0x3, lte_LL1_async_reset_cnf_msg_struct),   
  MSGR_DEFINE_UMID(LTE, LL1_ASYNC, CNF, CONFIG_APP,      0x5, lte_LL1_async_config_app_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_ASYNC, CNF, WAKEUP,          0x6, lte_LL1_async_config_app_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_ASYNC, CNF, TTRANS_WAKEUP,   0x7, lte_LL1_async_ttrans_wakeup_cnf_msg_struct),
  MSGR_DEFINE_UMID(LTE, LL1_ASYNC, CNF, DAC_CAL,         0x8, lte_LL1_async_dac_cal_cnf_msg_struct),

  // will remove the following 3  
  MSGR_DEFINE_UMID(LTE, LL1_ASYNC, CNF, STOP, 0x4, lte_LL1_async_stop_cnf_msg_struct),
  LTE_LL1_ASYNC_CNF_COUNT=9,

  LTE_LL1_ASYNC_IND_COUNT=0
    
} lte_LL1_async_msg_enum;



#endif /* INTF_ASYNC_MSG_H */





