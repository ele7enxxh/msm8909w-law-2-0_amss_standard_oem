#ifndef QMI_TASK_H
#define QMI_TASK_H
/*===========================================================================

                              Q M I   T A S K

                            H E A D E R   F I L E

DESCRIPTION
  This is the external header file for the Data Services QMI Task. This file
  contains all the functions, definitions and data types needed for other
  tasks to interface to the Data Services Task.

  Signal and command definitions for the QMI Task, and functions that
  should be used by other tasks to send commands to the QMI Task are
  defined here.

Copyright (c) 2008-2009 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ds_qmimsglib_task.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/02/09   pp      CMI: Moved this file to API folder. All signal macros
                   moved to new file "qmi_task_sig.h".
07/28/08   ar      Initial version.

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES

===========================================================================*/
#include "comdef.h"

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION QMI_TASK

DESCRIPTION
  This is the entry point for the QMI Task. This function contains
  the main processing loop that waits for events (signals or commands) and
  dispatches each event to the appropriate entity for further processing.

DEPENDENCIES
  None

RETURN VALUE
  This function does not return.

SIDE EFFECTS
  None

===========================================================================*/
extern void  qmi_task
(
  dword ignored
    /* lint -esym(715,ignored)
    ** Have lint not complain about the ignored parameter 'ignored' which is
    ** specified to make this routine match the template for rex_def_task().
    */
);

#endif /* QMI_TASK_H */
