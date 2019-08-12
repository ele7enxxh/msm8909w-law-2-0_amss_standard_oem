/*!
  @file
  lte_mac_cfg.h

  @brief
  Compile time configuration for MAC.

  @detail
  MAC can be configured at compile time to turn on certain features. A
  feature is turned ON by uncommenting a #define in this file. Compile 
  time flags defined in this file shall meet these requirements:
  -# All flags shall start with FEATURE_LTE_MAC_.
  -# A Flag shall not cause another flag to be defined or undefined. 
  -# Flags used only in test code shall not be defined in this file.


*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/mac/inc/lte_mac_cfg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/08/13   md      Reduced stack size
04/05/10   ax      add task mailbox maximum messages 
03/26/10   ax      removed #define FEATURE_LTE_MAC_TASKS_PROFILE_ON
05/15/08   bn      Initial version
===========================================================================*/

#ifndef LTE_MAC_CFG_H
#define LTE_MAC_CFG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <customer.h>
#include "lte_l2_profile.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief maximum pending message router messages allowed
*/
#define LTE_MAC_CFG_DL_TASK_MAX_MSG                                          50

/*! @brief maximum pending message router messages allowed
*/
#define LTE_MAC_CFG_UL_TASK_MAX_MSG                                          50

/*! @brief maximum pending message router messages allowed
*/
#define LTE_MAC_CFG_CTRL_TASK_MAX_MSG                                        50

#endif /* LTE_MAC_CFG_H */
