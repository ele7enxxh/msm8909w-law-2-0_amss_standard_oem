#ifndef UIMQMI_TASK_H
#define UIMQMI_TASK_H
/*===========================================================================


            U I M Q M I    S U B S Y S T E M    T A S K


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2013 - 2014 QUALCOMM Technologies, Incorporated and its licensors.
All Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimqmi/src/uimqmi_task.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
07/14/14   tl      Remove always ON feature FEATURE_MODEM_RCINIT
01/10/14   df      Remove unnecessary include of msg.h
12/11/13   vs      Addition of UIMQMI task and QMI UIM REMOTE
=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "task.h"
#include "uim_variation.h"
#include "dog_hb_rex.h"
#include "rcinit.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/* Need this as we are referring to UIM task handle */
TASK_EXTERN(uim);
#define UIM_UIMQMI_TASK_TCB rcinit_lookup_rextask("uimqmi")

/*=============================================================================

                       DATA TYPE, CONSTANT and MACRO DECLARATIONS

=============================================================================*/

/*--------------------------------------------------------------------------
                             Signal Masks
--------------------------------------------------------------------------*/
#define UIMQMI_TASK_DOG_RPT_TIMER_SIG                 0x00000001
#define UIMQMI_TASK_QMI_MODEM_TASK_READY_SIG          0x00000002
#define UIMQMI_TASK_QMI_UIM_REMOTE_UIM_CMD_Q_SIG      0x00000004
#define UIMQMI_TASK_QMI_UIM_REMOTE_QCSI_SIG           0x00000008

#define UIMQMI_CLASH_WITH_TASK_OFFLINE_SIG    0x00002000 /* RESERVED SIGNAL */
#define UIMQMI_CLASH_WITH_TASK_STOP_SIG       0x00004000 /* RESERVED SIGNAL */
#define UIMQMI_CLASH_WITH_TASK_START_SIG      0x00008000 /* RESERVED SIGNAL */
#define UIMQMI_CLASH_WITH_ONCRPC_REPLY_SIG    0x00800000 /* RESERVED SIGNAL */
#define UIMQMI_CLASH_WITH_CAMERA_SIG          0x04000000 /* RESERVED SIGNAL */
#define UIMQMI_CLASH_WITH_GRP_ACK_SIG         0x08000000 /* RESERVED SIGNAL */
#define UIMQMI_CLASH_WITH_SEC_OP_COMPLETE_SIG 0x10000000 /* RESERVED SIGNAL */
#define UIMQMI_CLASH_WITH_VS_OP_COMPLETE_SIG  0x20000000 /* RESERVED SIGNAL */
#define UIMQMI_CLASH_WITH_FS_OP_COMPLETE_SIG  0x40000000 /* RESERVED SIGNAL */
#define UIMQMI_CLASH_WITH_REX_SLEEP_SIG       0x80000000 /* RESERVED SIGNAL */
/************************************************/

/* ----------------------------------------------------------------------------

   ENUM:      UIMQMI_TASKS_TYPE

   DESCRIPTION:
     The enum for various modem tasks that UIMQMI interacts with
-------------------------------------------------------------------------------*/
typedef enum {
  UIMQMI_TASK_TYPE_QMI_MODEM  = 0,
  UIMQMI_TASK_TYPE_MAX        = 0x7FFFFFFF
} uimqmi_tasks_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      UIMQMI_TASK_GENERIC_DATA_TYPE

   DESCRIPTION:
     The structure contains general global data for UIMQMI
-------------------------------------------------------------------------------*/
typedef struct {
  boolean              task_stopped;
  uint8                ready_tasks;
  dog_report_type      dog_rpt_var;
} uimqmi_task_generic_data_type;

/*===========================================================================
FUNCTION UIMQMI_TASK_IS_TASK_OPERATIONAL

DESCRIPTION
  Function to determine if UIMQMI task is operational

DEPENDENCIES
  None

RETURN VALUE
  Boolean:
  TRUE if task is running
  FALSE if task is stopped or global state pointer is not init

SIDE EFFECTS
  None
===========================================================================*/
boolean uimqmi_task_is_task_operational (
  void
);

/*===========================================================================
FUNCTION UIMQMI_TASK

DESCRIPTION
  Entry point for the UIMQMI task.  This function performs task initialization,
  then sits in an infinite loop, waiting on an input queue, and responding
  to messages received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void uimqmi_task (
  dword  dummy
);

/*===========================================================================
FUNCTION UIMQMI_TASK_INIT

DESCRIPTION
  Function called to initialize UIMQMI task queues and global variables.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.  Function should only be called once during task initialization.
  On targets with RCInit enabled in place of TMC, this function is called
  in RCInit context. Therefore, it should not include references to TCBs, NV
  operations, etc.
===========================================================================*/
extern void uimqmi_task_init (
  void
);
#endif /* UIMQMI_TASK_H */

