/*!
  @file 
  lte_rlcdl_task.h

  @brief
  This file contains all the common define structures/function prototypes 
  for LTE RLC DL Task.

  @author
  axiao
*/

/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/rlc/src/lte_rlcdl_task.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/04/15   sb      CR897982: Added support for sub_id in RLCDL
02/11/09   ax      move lte_rlcdl_task_init() into \inc\lte_rlc.h
10/21/08   ax      remove enable flag & reponse to startup complete indicaiton
08/21/08   ax      initial version                    
===========================================================================*/

#ifndef LTE_RLCDL_TASK_H
#define LTE_RLCDL_TASK_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <lte_l2_common.h>
#include <lte_l2_timer.h>


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

extern boolean lte_rlcdl_task_is_active(void);

extern boolean lte_rlcdl_task_is_started(void);

extern uint8 lte_rlcdl_task_get_sub_id(void);

#endif /* LTE_RLCDL_TASK_H */
