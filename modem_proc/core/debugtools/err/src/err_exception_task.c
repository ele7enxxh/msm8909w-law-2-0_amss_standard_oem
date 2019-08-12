/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        ERR EXCEPTION TASK MODULE

GENERAL DESCRIPTION
  This module contains the task definition for err exception handler, when not running in main().

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2011-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/debugtools/err/src/err_exception_task.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/18/14   din     Added changes for ADSP island mode.
===========================================================================*/
#include "DALSys.h"
#include "err.h"

#define dwMaxNumEvents                          2                                // DALSys Worker Loop Event Queue Depth for DALTASK
#define wlPriority                              190                              // SHARED_DRIVER_SERVICE_PRI_ORDER

void err_exception_handler(void);                                                // forward declaration; exception handler

extern void err_emergency_error_recovery(void);

/*===========================================================================
FUNCTION ERR_TASK

DESCRIPTION
  Create a context for err_exception_handler when main() is not available.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

DALResult err_exception_task(DALSYSEventHandle hEvent, void* tid)
{

   err_exception_handler(); // does not return

   return(DAL_SUCCESS);
}

void bootstrap_exception_handler(void)
{
   const unsigned long wlStackSize = 4096;   // minimal (KB)
   static DALSYSEventHandle hEventStart;     // context for the error exception handler
   static DALSYSWorkLoopHandle hWorkLoop;    // context for the error exception handler

   if (DAL_SUCCESS != DALSYS_EventCreate(DALSYS_EVENT_ATTR_WORKLOOP_EVENT, &hEventStart, NULL))
   {
      /* Exception Handler initialization failure */
      err_emergency_error_recovery();
   }
   if (DAL_SUCCESS != DALSYS_RegisterWorkLoopEx("err_ex", wlStackSize, wlPriority, dwMaxNumEvents, &hWorkLoop, NULL))
   {
      /* Exception Handler initialization failure */
      err_emergency_error_recovery();
   }
   if (DAL_SUCCESS != DALSYS_AddEventToWorkLoop(hWorkLoop, err_exception_task, NULL, hEventStart, NULL))
   {
      /* Exception Handler initialization failure */
      err_emergency_error_recovery();
   }
   if (DAL_SUCCESS != DALSYS_EventCtrl(hEventStart, DALSYS_EVENT_CTRL_TRIGGER))
   {
      /* Exception Handler initialization failure */
      err_emergency_error_recovery();
   }

}

