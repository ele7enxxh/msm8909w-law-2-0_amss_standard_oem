#ifndef DSBCDB_TASK_H
#define DSBCDB_TASK_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           D S B C D B _ T A S K . H

GENERAL DESCRIPTION
  This file contains the task definition for the BCDB task.

EXTERNALIZED FUNCTIONS
  DSBCDB_TASK
    This is the entry point into the BCDBAPP task

INITIALIZATION AND SEQUENCING REQUIREMENTS
  dsbcdb_task() must be called first from TMC to start the BCDBAPP task.

 Copyright (c) 2006 - 2009 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/dsbcdb_task.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

-----------------------------------------------------------------------------
when        who    what, where, why
--------    ---    -------------------------------------------------------
03/04/09    sa     AU level CMI modifications.
08/23/04    vr     Initial revision.
===========================================================================*/

/*===========================================================================
                      INCLUDE FILES
===========================================================================*/
#include "comdef.h"

/*===========================================================================
                        TYPEDEFS AND VARIABLES
===========================================================================*/


/*===========================================================================
                   EXTERNAL FUNCTION PROTOTYPES 
===========================================================================*/
/*===========================================================================
FUNCTION      DSBCDB_TASK

DESCRIPTION  
  This is the entry point for the BCDBAPP Services Task. This function 
  contains the main processing loop that waits for events (signals or 
  commands) and dispatches each event to the appropriate entity for further 
  processing.

DEPENDENCIES  None.

RETURN VALUE  This function does not return.

SIDE EFFECTS  None.
===========================================================================*/
extern void dsbcdb_task
(
  dword ignored
);


#endif /* DSBCDB_TASK_H */
