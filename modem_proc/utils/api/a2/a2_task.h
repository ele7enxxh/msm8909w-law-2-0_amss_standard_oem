#ifndef A2_TASK_H
#define A2_TASK_H
/*!
  @file a2_task.h

  @brief
   Header for the A2 task

  Details...
  
*/

/*==============================================================================

  Copyright (c) 2013 QUALCOMM Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/a2/a2_task.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/02/15   am      CR945970 Remove compiler warnings
09/13/13   vd      CR469984: Support for mount/unmount feature
07/19/13   ars     QMAP FC Feature
04/17/12   yjz     Move to a2_timetick_t
06/12/12   rp      Added NV EFS read/write a2_dbg profiling.
02/13/12   ar      added support for A2 PC + SPS RESET feature
10/06/11   ar      added a2 power collapse support
10/30/09   ar      removed A2_STACK_SIZ definition
07/15/09   sah     Remove dependancy of a2_util.h header file in a2_task.h
03/20/09   sah     Add continue signal.
03/09/09   sm      Extern a2_task_init()
02/25/09   sah     Switch to Rex/A2 OS layer.
10/10/08   sah     Initial Checkin

==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <rex.h>
#include <a2_common.h>

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief A2 task rex control block
 * a2 task stack and tcb are declared in hw sim for off-target environment.
 * On target these declarations reside in service/task/task.c */
extern rex_tcb_type a2_tcb;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*===========================================================================

  FUNCTION:  a2_task_main

===========================================================================*/
/*!
    @brief
    Rex main function for a2 task

    @return
    None
*/
/*=========================================================================*/
void a2_task_main
(
  dword dummy
);

/*===========================================================================

  FUNCTION:  a2_task_init

===========================================================================*/
/*!
    @brief
    call inits of general modules needed for a2 task and also sets the
    cache line registers

    @return
    None
*/
/*=========================================================================*/
void a2_task_init(void);

/*===========================================================================

  FUNCTION:  a2_task_signal_continue

===========================================================================*/
/*!
    @brief
    Signals the a2 task to continue processing.

    @return
    None
*/
/*=========================================================================*/
void a2_task_signal_continue(void);

/*===========================================================================

  FUNCTION:  a2_task_start_periodic_timer

===========================================================================*/
/*!
    @brief
    starts the DL PER periodic timer used for tracking DL PER inactivity

    @return
    none
*/
/*=========================================================================*/
extern void a2_task_start_periodic_timer
(
  void
);

/*===========================================================================

  FUNCTION:  a2_task_stop_periodic_timer

===========================================================================*/
/*!
    @brief
    stops the DL PER periodic timer

    @return
    none
*/
/*=========================================================================*/
extern void a2_task_stop_periodic_timer
(
  void
);

/*===========================================================================

  FUNCTION:  a2_task_pending_q_empty

===========================================================================*/
/*!
    @brief
    checks if PER pending queue is empty

    @return
    TRUE if pending q is empty, FALSE otherwise

*/
/*=========================================================================*/
extern boolean a2_task_pending_q_empty
(
  void
);

/*===========================================================================

  FUNCTION:  a2_task_profiling_efs_start_set_time

===========================================================================*/
/*!
    @brief
    Set the a2 efs start time in task profiling structure
*/
/*=========================================================================*/
extern void a2_task_profiling_efs_start_set_time
(
  a2_timetick_t value
);

/*===========================================================================

  FUNCTION:  a2_task_profiling_efs_conf_write_set_time

===========================================================================*/
/*!
    @brief
    Set the a2 efs conf file write time in task profiling structure
*/
/*=========================================================================*/
extern void a2_task_profiling_efs_conf_write_set_time
(
  a2_timetick_t value
);

/*===========================================================================

  FUNCTION:  a2_task_profiling_efs_get_set_time

===========================================================================*/
/*!
    @brief
    Set the a2 efs get time in task profiling structure
*/
/*=========================================================================*/
extern void a2_task_profiling_efs_get_set_time
(
  a2_timetick_t value
);

/*===========================================================================

  FUNCTION:  a2_task_add_pending_item_ext

===========================================================================*/
/*!
    @brief
    Adds the pending item to pending Q. Called outside a2_task
*/
/*=========================================================================*/
void  a2_task_add_pending_item_ext
(
  uint8 sio_index, 
  uint8 mux_id
);

/*===========================================================================

  FUNCTION:  a2_task_clear_dl_bam

===========================================================================*/
/*!
    @brief
    function clears DL BAM 

*/
/*=========================================================================*/
extern void a2_task_clear_dl_bam
(
  void
);

/*==============================================================================

  FUNCTION:  a2_task_msg_q_empty

==============================================================================*/
/*!
    @brief
    checks if DL PER message queue is empty

    @return
    TRUE if message queue is empty, FALSE otherwise

*/
/*============================================================================*/
boolean a2_task_msg_q_empty
(
  void
);

#endif /* A2_TASK_H */

