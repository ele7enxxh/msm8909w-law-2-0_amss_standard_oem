/*!
  @file   for_apex.h

  @brief  Message Router structure to be imported by APEX

  @detail
  Defines Message Router related types and interfaces.

*/

/*===========================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/
#ifndef LTE_LL1_LOG_DEBUG_TYPES_H
#define LTE_LL1_LOG_DEBUG_TYPES_H

/* debug-log id */
typedef enum
{
  LTE_LL1_LOG_DEBUG_RESERVED = 0,
  // add id's below

  LTE_LL1_LOG_DEBUG_SRCH_FFE_RESULTS,
  LTE_LL1_LOG_DEBUG_SRCH_SSS_POST_RESULTS

} lte_LL1_log_debug_id_e;

typedef struct
{
  lte_LL1_log_debug_id_e    id;       ///< which debug log
  lte_LL1_sys_time_struct   sys_time; ///< sys-time that this debug log corresponds to
  uint32                    label;    ///< to send more information about the debug log
                                      ///< eg: symbol-number
} lte_LL1_log_debug_hdr_s;

#endif
