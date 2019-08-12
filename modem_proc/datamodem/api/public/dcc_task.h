#ifndef DCC_TASK_H
#define DCC_TASK_H
/*===========================================================================

                          D C C _ T A S K . H

DESCRIPTION
  This is the header file for the Data Common Control Task. Contained 
  herein are the functions needed to initialize all the modules that execute
  in DCC task context and the main task processing loop.

  DataCommon Control (DCC) task is the control counterpart of the Protocol
  Services (PS) task.

EXTERNALIZED FUNCTIONS
  dcc_task()
    DataCommon Control Task entry point and main processing loop.

Copyright (c) 2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps.h_v   1.0   08 Aug 2002 11:19:56   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/dcc_task.h#1 $ 
  $DateTime: 2005/12/23 17:19:09

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/19/09    am     Created module.
===========================================================================*/

#include "comdef.h"

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION DCC_TASK()

DESCRIPTION
  This function is the entry point and main processing loop for the Data
  Common Control task.

DEPENDENCIES
  None.

RETURN VALUE
  Does not Return.

SIDE EFFECTS
  None.

===========================================================================*/

void
dcc_task
(
  uint32 dummy
);

#endif /* DCC_TASK_H */
