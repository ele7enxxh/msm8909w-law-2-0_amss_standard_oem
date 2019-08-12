#ifndef A2_UL_PER_TASK_H
#define A2_UL_PER_TASK_H
/*!
  @file a2_ul_per_task.h

  @brief
   Header for the A2 UL PER task

  Details...
  
*/

/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/a2/a2_ul_per_task.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
10/30/09   ar      updated a2_ul_per_tcb comments
05/28/09   yuw     Initial Checkin

==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <rex.h>

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief A2 UL PER task rex control block
 * a2 task stack and tcb are declared in hw sim for off-target environment.
 * On target these declarations reside in service/task/task.c */
extern rex_tcb_type a2_ul_per_tcb;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*===========================================================================

  FUNCTION:  a2_ul_per_task_main

===========================================================================*/
/*!
    @brief
    Rex main function for a2 UL PER task

*/

/*=========================================================================*/
void a2_ul_per_task_main
(
  dword dummy
);

/*===========================================================================

  FUNCTION:  a2_ul_per_task_init

===========================================================================*/
/*!
    @brief
    call inits of general modules needed for a2 UL PER task and also sets the
    cache line registers

    @return
    None
*/
/*=========================================================================*/
void a2_ul_per_task_init(void);

/*==============================================================================

  FUNCTION:  a2_ul_per_taskq_crit_sect_enter

==============================================================================*/
/*!
  @brief
  Start MUTEX for UL PER taskq.

*/
/*============================================================================*/
void a2_ul_per_taskq_crit_sect_enter(void);

/*==============================================================================

  FUNCTION:  a2_ul_per_taskq_crit_sect_leave

==============================================================================*/
/*!
  @brief
  Stop MUTEX for UL PER taskq.

*/
/*============================================================================*/
void a2_ul_per_taskq_crit_sect_leave(void);

#endif /* A2_UL_PER_TASK_H */

