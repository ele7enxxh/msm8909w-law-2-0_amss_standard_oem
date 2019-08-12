/*!
  @file
  lte_l2_profile.h

  @brief
  This file contains profile utilitiy functions interfaces for LTE L2 layer.

  @author
  baon, axiao


*/

/*===========================================================================

  Copyright (c) 2015 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/common/inc/lte_l2_profile.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/15/15   sb      CR 803880: Add Task_ID for new "PDCP COMP"task
12/16/11   ax      feature clean up FEATURE_LTE_L2_PROFILE_ON
09/22/10   bn      Added in feature time test
03/04/10   ax      incorporate profile code into target build
06/18/09   ax      pass UMID to LTE_L2_PROFILE_TASK_END
10/21/08   bn      Initial version
===========================================================================*/

#ifndef LTE_L2_PROFILE_H
#define LTE_L2_PROFILE_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "lte_variation.h"
#include <comdef.h>
#include <lte_assert.h>
#include <msgcfg.h>
#include <msg.h>
#include <msgr.h>
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*! @brief keep it to 2 to the power of n */
#define LTE_L2_PROFILE_HISTORY_BUF_SZ               32
#define LTE_L2_PROFILE_HISTORY_BUF_INDEX_MASK (LTE_L2_PROFILE_HISTORY_BUF_SZ - 1)

/*! @brief emumeration all the instance ID for profiling */
typedef enum
{
  LTE_L2_PROFILE_MACDL_TASK_ID,
  LTE_L2_PROFILE_MACUL_TASK_ID,
  LTE_L2_PROFILE_MACCTRL_TASK_ID,
  LTE_L2_PROFILE_RLCDL_TASK_ID,
  LTE_L2_PROFILE_RLCUL_TASK_ID,
  LTE_L2_PROFILE_PDCPDL_TASK_ID,
  LTE_L2_PROFILE_PDCPUL_TASK_ID,
  LTE_L2_PROFILE_PDCPOFFLOAD_TASK_ID,
  #ifdef FEATURE_LTE_UDC_ENABLED
  LTE_L2_PROFILE_PDCPCOMP_TASK_ID,
  #endif /*FEATURE_LTE_UDC_ENABLED*/
  LTE_L2_PROFILE_QTF_TASK_ID,
  LTE_L2_PROFILE_RLCDL_PDU_PROC_ID,
  LTE_L2_PROFILE_TASK_MAX_ID,
} lte_l2_profile_instance_id_e;

#ifdef TIMETEST
#error code not present
#endif

#define LTE_L2_PROFILE_TASK_INIT(task_id, print_periodic)

#define LTE_L2_PROFILE_TASK_START(task_id) 

#define LTE_L2_PROFILE_TASK_END(task_id, umid) 

#define LTE_L2_PROFILE_TASK_END_W_SFN(task_id, umid, sfn)

#endif /* LTE_L2_PROFILE_H */

