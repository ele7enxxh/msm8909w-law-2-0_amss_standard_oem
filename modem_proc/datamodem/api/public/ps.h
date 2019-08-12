#ifndef PS_H
#define PS_H
/*===========================================================================

                                P S . H

DESCRIPTION
  This is the header file for the Data protocol Task. Contained herein are
  the functions needed to initialize all the modules that execute in PS task
  context and the main task processing loop.

EXTERNALIZED FUNCTIONS
  ps_task()
    Data Protocol Task entry point and main processing loop.

Copyright (c) 1993-2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps.h_v   1.0   08 Aug 2002 11:19:56   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps.h#1 $ 
  $DateTime: 2005/12/23 17:19:09

===========================================================================*/

#include "comdef.h"
#include "rex.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
  Watchdog timer for PS task
---------------------------------------------------------------------------*/
extern rex_timer_type ps_dog_rpt_timer;          /* for watchdog reporting */


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION PS_TASK()

DESCRIPTION
  This function is the entry point and main processing loop for the Data
  Service(s) task.

DEPENDENCIES
  None.

RETURN VALUE
  Does not Return.

SIDE EFFECTS
  None.

===========================================================================*/
extern void ps_task
(
  uint32 dummy
);

#endif /* PS_H */
