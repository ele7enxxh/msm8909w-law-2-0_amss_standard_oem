/*==============================================================================
@file  DALSYSTimer_ats.c

DALSYS timer implementation using ATS timer

        Copyright © 2013 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary
==============================================================================*/
/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "DALSys.h"
#include "DALStdDef.h"
#include "DALSysCmn.h"
#include "timer.h"

/*=============================================================================
                    DEFINITIONS AND DECLARATIONS
=============================================================================*/
static timer_group_type dalTimerGroup;

/*=============================================================================
      Constants and Macros
=============================================================================*/

/*=============================================================================
      Structures
=============================================================================*/

/*=============================================================================
                       FUNCTION DECLARATIONS FOR MODULE
=============================================================================*/
static void _TimeoutEventInternalCallback(timer_cb_data_type data)
{
    int hEventValue = (int)data; // needed to avoid compiler warning
    DALSYSEventHandle hEvent = (DALSYSEventHandle)hEventValue;

    if(hEvent)
        DALSYS_EventCtrl(hEvent, DALSYS_EVENT_CTRL_TRIGGER);
}

DALResult DALSYS_TimerStart(DALSYSEventHandle hEvent, uint32 time) 
{
    _DALSYSEventObj * pEventObj = (_DALSYSEventObj *)hEvent;
   uint32 period = 0;
   void * timeout;
   uint32 dwObjInfo;

   if(!hEvent)
       return DAL_ERROR;

   timeout = pEventObj->timeout;
   dwObjInfo = pEventObj->dwObjInfo;

   // need to allocate underlying timer?
   if(!timeout && time)
   {
       int hEventValue;

       if(DAL_SUCCESS != DALSYS_Malloc(sizeof(timer_type),
                                       (void**)&timeout))
           return DAL_ERROR;

       pEventObj->timeout = timeout;

       hEventValue = (int)hEvent; // used to avoid compiler warnings
       if(DALSYSCMN_IS_EVENT_NON_DEFERRED(dwObjInfo))
       {
          timer_def( timeout, &dalTimerGroup, rex_self(), 0,
                (timer_t1_cb_type)_TimeoutEventInternalCallback,
                (timer_cb_data_type)hEventValue);
       }
       else
       {
          timer_def( timeout, NULL, rex_self(), 0,
                (timer_t1_cb_type)_TimeoutEventInternalCallback,
                (timer_cb_data_type)hEventValue);
       }
   }

   if(!time) // clear underlying timer
   {
      if(timeout)
        timer_clr( timeout, T_USEC );
   }
   else // register underlying timer
   {
      if(DALSYSCMN_IS_EVENT_A_TIMER(dwObjInfo))
         period = time; // set period for periodic timer

      // time & period in milliseconds
      timer_set(timeout, time, period, T_USEC);
   }
   return DAL_SUCCESS;
}

// TODO: should support a periodic timer API

DALResult DALSYS_TimerStop(DALSYSEventHandle  hEvent) 
{
   _DALSYSEventObj * pEventObj = (_DALSYSEventObj *)hEvent;

   if(!hEvent)
      return DAL_ERROR;

   // free underlying timer
   if(pEventObj->timeout)
   {
      timer_clr( pEventObj->timeout, T_USEC );
      DALSYS_Free(pEventObj->timeout);
      pEventObj->timeout = NULL;
   }
   return DAL_SUCCESS;
}
