#ifndef QMI_TASK_SIG_H
#define QMI_TASK_SIG_H
/*===========================================================================

                           Q M I   T A S K    S I G

                            H E A D E R   F I L E

DESCRIPTION
  This is the external header file for the Data Services QMI Task. This file
  contains all the functions, definitions and data types needed for other
  tasks to interface to the Data Services Task.

  Signal and command definitions for the QMI Task, and functions that
  should be used by other tasks to send commands to the QMI Task are
  defined here.

Copyright (c) 2009 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/inc/ds_qmimsglib_task_sig.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/02/09   pp      Created file as part of CMI: Public/Private split.
                   (Split from qmi_task.h)

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "task.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Task Signal Definitions
---------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Task top-level signals
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#define QMI_TASK_START_SIG       TASK_START_SIG         /* 0x8000          */
#define QMI_TASK_STOP_SIG        TASK_STOP_SIG          /* 0x4000          */
#define QMI_TASK_OFFLINE_SIG     TASK_OFFLINE_SIG       /* 0x2000          */
#define QMI_CMD_Q_SIG            0x1000   /* Command Queue signal          */
#define QMI_DOG_RPT_TIMER_SIG    0x0800   /* Watchdog Report timeout signal*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  QMI control messaging signal
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#define QMI_QMUX_RX_SIG          0x0080   /* Rx control data on any Rmnet  */


/*---------------------------------------------------------------------------
  QMI Task Command Definitions
---------------------------------------------------------------------------*/
/* NOT SUPPORTED AT THIS TIME */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/* NONE AT THIS TIME */

#endif /* QMI_TASK_SIG_H */
