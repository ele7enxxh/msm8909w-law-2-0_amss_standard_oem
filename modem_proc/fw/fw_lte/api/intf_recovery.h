/*!
  @file
  intf_recovery.h

  @brief
  This file contains data structures for recovery and wakeup requests
  and confirmations.
  
 
*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/intf_recovery.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
#ifndef INTF_RECOVERY_H
#define INTF_RECOVERY_H

#include "intf_common.h"
#include "intf_config_app.h"

#include "lte_l1_types.h"
#include "lte_LL1_ue_types.h"

/*===========================================================================

      Constants

===========================================================================*/


/*===========================================================================

      Macros

===========================================================================*/

   

/*===========================================================================

      Typedefs

===========================================================================*/

/*! @brief
 *  indicate to ML a task stall condition has been detected
 *  and LL is in recovery mode
 */
typedef enum
{
  LTE_LL1_SYS_RECOV_IND_CAUSE_NONE                              = 0, 
  LTE_LL1_SYS_RECOV_IND_CAUSE_DLCCH                             = 1,
  LTE_LL1_SYS_RECOV_IND_CAUSE_DF_SCHD_STALL                     = 2,
  LTE_LL1_SYS_RECOV_IND_CAUSE_TX_STALL                          = 3,
  LTE_LL1_SYS_RECOV_IND_CAUSE_MIPS_OVERUN                       = 4, 
  LTE_LL1_SYS_RECOV_IND_CAUSE_VPE_NRML_SYNC_PT_LATE             = 5, 
  LTE_LL1_SYS_RECOV_IND_CAUSE_VPE_WKUP_SYNC_PT_LATE             = 6, 
  LTE_LL1_SYS_RECOV_IND_CAUSE_DL_PROC_TIME_GEQ_1PT5_MS          = 7,
  LTE_LL1_SYS_RECOV_IND_CAUSE_BUSY_GT_VPE_MAX_BUSY_COUNT        = 8, 
  LTE_LL1_SYS_RECOV_IND_CAUSE_CMD_VALID_FAIL_SYNC_PT_READ       = 9,  
  LTE_LL1_SYS_RECOV_IND_CAUSE_VPE_TIMELINE_CHECK_FAIL           = 10, 
  LTE_LL1_SYS_RECOV_IND_CAUSE_VPE_ERROR_CALLBACK_RCVD           = 11, 
  LTE_LL1_SYS_RECOV_IND_CAUSE_VPE_BUSY_DURING_STOP              = 12,
  LTE_LL1_SYS_RECOV_IND_CAUSE_GENERIC_ASSERT                    = 13,
  LTE_LL1_SYS_RECOV_IND_CAUSE_BBRX_HIB_EXIT_MISS                = 14,
  LTE_LL1_SYS_RECOV_IND_CAUSE_ODRX_SYNC_PT_MISS                 = 15,  
  LTE_LL1_SYS_RECOV_IND_CAUSE_X2L_VPE_IMAGE_LOAD_LATE           = 16,
  LTE_LL1_SYS_RECOV_IND_CAUSE_TDEC_STALL                        = 17,

  //Have this gap for error code expansion
  LTE_LL1_SYS_RECOV_IND_CAUSE_TEST_MODE                         = 25,
  LTE_LL1_SYS_RECOV_IND_CAUSE_MAX_CNT
} lte_LL1_sys_recovery_ind_cause_e;


// --------------------------------------------------------------------------
//
// Recovery/Stall indication
//
// --------------------------------------------------------------------------

/*! @brief
 *  indicate to ML a task stall condition has been detected
 *  and LL is in recovery mode
 */
typedef struct {
  uint16 frame;
  uint8 subframe;
  uint8 recovery_state;  //XXX- rename to "recovery_cause" once ML intf updated
} lte_LL1_sys_recovery_ind_struct;

typedef struct {
  msgr_hdr_struct_type              msg_hdr;      ///< message router header
  lte_LL1_sys_recovery_ind_struct   msg_payload;
} lte_LL1_sys_recovery_ind_msg_struct;

#endif

