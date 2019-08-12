#ifndef PS_RM_TASK_H
#define PS_RM_TASK_H
/*===========================================================================

                          P S _ RM _ T A S K . H

DESCRIPTION
  This is the header file for the PS RM Data processing Task. Contained 
  herein are the functions needed to initialize all the modules that execute
  in PS RM task context and the main task processing loop.

EXTERNALIZED FUNCTIONS
  ps_rm_task()
    PS RM Task entry point and main processing loop.

Copyright (c) 2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_rm_task.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $
  $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/16/15    ds     Added prototype of ps_rm_task_is_running().
06/10/10    dm     Created module

===========================================================================*/


/*===========================================================================

                                INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "dog.h"
#include "rex.h"


/*===========================================================================

                             MACROS & DATA DECLARATIONS

===========================================================================*/

extern dog_report_type   ps_rm_dog_rpt_var;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION PS_RM_TASK()

DESCRIPTION
  This function is the entry point and main processing loop for the RM Data
  processing task.

DEPENDENCIES
  None.

RETURN VALUE
  Does not Return.

SIDE EFFECTS
  None.

===========================================================================*/

void
ps_rm_task
(
  uint32 dummy
);

/*===========================================================================
FUNCTION PS_RM_TASK_IS_RUNNING()

DESCRIPTION
  This api checks whether ps rm task is initialized or not and return TRUE if 
  it is running otherwise FALSE.

DEPENDENCIES
  RM task should have been started.

RETURN VALUE
  TRUE/FALSE.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ps_rm_task_is_running
(
  void
);
#endif /* PS_RM_TASK_H */

