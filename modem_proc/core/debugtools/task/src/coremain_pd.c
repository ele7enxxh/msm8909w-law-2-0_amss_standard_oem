/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M A I N   M O D U L E

GENERAL DESCRIPTION
  This module contains the Main procedure.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/debugtools/task/src/coremain_pd.c#1 $

===========================================================================*/

#include "rcinit_dal.h"
#include "qurt.h"

void bootstrap_rcinit_task(void); // forward reference
void bootstrap_err_exception_handler(void); // forward reference

/*===========================================================================
FUNCTION COREMAIN_MAIN

DESCRIPTION
  Bootstrap the context for error processing
  Bootstrap the context for rcinit processing

DEPENDENCIES
  None

RETURN VALUE
  None, this routine returns at the completion of rcinit startup processing
  to facilitate other images being able to initialize corebsp and then regain
  control of the context following the rcinit startup process.

SIDE EFFECTS
  None

===========================================================================*/
void coremain_main(void)
{
   DALSYS_InitMod(NULL);               // no error return

   // bootstrap_err_exception_handler();  // creates context for error processing

   bootstrap_rcinit_task();     // creates context for rcinit processing
}

/*===========================================================================
FUNCTION MAIN

DESCRIPTION
  Bootstrap the context for core_main through call to rex_init

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#if 0
int main(void)
{
   DALSYSEventHandle hEvent;

   coremain_main(); // when main is not delivered here, this is the entry to start cbsp

   // coremain_pd_spawn(); // qurt image spawning, compile time registered, not a supported interface.

   // Block Forever

   if ((DAL_SUCCESS == DALSYS_EventCreate(DALSYS_EVENT_ATTR_NORMAL, &hEvent, NULL)) &&
       (DAL_SUCCESS == DALSYS_EventWait(hEvent)))
   {
      for (;;) /* NULL */ ;
   }

   else
   {
      for (;;) /* NULL */ ;
   }

   /* NOTREACHED */

   return (0);
}
#endif
