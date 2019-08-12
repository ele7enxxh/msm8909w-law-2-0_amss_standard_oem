/*=============================================================================

                  DALSYS Event Object Implementation for QURT

GENERAL DESCRIPTION
    This file provides an implementation of the DALSYS Event object

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
    None

  Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "DALSys.h"
#include "DALStdDef.h"
#include "DALSysCmn.h"
#include "qurt.h"
#include "qurt_timer.h"
#include "string.h"


/*=============================================================================
                    DEFINITIONS AND DECLARATIONS
=============================================================================*/

/*=============================================================================
      Constants and Macros
=============================================================================*/

/*=============================================================================
      Structures
=============================================================================*/
typedef struct _DALSYSq6TimerObj
{
    qurt_timer_t      timer;
    qurt_timer_attr_t attr;
    DALBOOL           active;
} _DALSYSq6TimerObj;

/*=============================================================================
                       FUNCTION DECLARATIONS FOR MODULE
=============================================================================*/
DALResult DALSYS_TimerStart(DALSYSEventHandle hEvent, uint32 time) 
{
    _DALSYSEventObj * pEventObj = (_DALSYSEventObj *)hEvent;
    _DALSYSq6TimerObj * pTimerObj;
    uint32 dwObjInfo;
    qurt_timer_type_t type;
    int i = 0;

    if (!hEvent)
        return DAL_ERROR;

    // Qurt timer does not support workloop or callback event since it can 
    // only use Qurt signals to notify timer expiry
    dwObjInfo = pEventObj->dwObjInfo;
    if (dwObjInfo & 
        (DALSYS_EVENT_ATTR_WORKLOOP_EVENT | DALSYS_EVENT_ATTR_CALLBACK_EVENT))
    {
        return DAL_ERROR;
    }

    // get timer type
    if (DALSYSCMN_IS_EVENT_A_TIMER(dwObjInfo))
    {
        type = QURT_TIMER_PERIODIC; // set period for periodic timer
    }
    else
    {
        type = QURT_TIMER_ONESHOT;
    }


    // get pointer to underlying timer
    pTimerObj = (_DALSYSq6TimerObj *)pEventObj->timeout;

    if (!pTimerObj && !time) //trying to cancel non-exiting timer
        return DAL_ERROR;

    if (pTimerObj)
    {
        if (pTimerObj->active)
        {
            // cancel existing timer first
            i = qurt_timer_delete(pTimerObj->timer);
            if (QURT_EOK != i)
                return DAL_ERROR;
            pTimerObj->active = FALSE;
        }

        if (!time) // no timer to set
            return DAL_SUCCESS;
    }
    else
    {
        if (DAL_SUCCESS != DALSYS_Malloc(sizeof(_DALSYSq6TimerObj), 
                                         (void**)&pTimerObj))
            return DAL_ERROR;

        DALSYS_memset(pTimerObj, 0, sizeof(_DALSYSq6TimerObj));
        pEventObj->timeout = pTimerObj;
    }

    // setup timer attribute
    qurt_timer_attr_init( &pTimerObj->attr );
    qurt_timer_attr_set_duration( &pTimerObj->attr, time );
    qurt_timer_attr_set_type( &pTimerObj->attr, type );

    i = qurt_timer_create(&pTimerObj->timer, &pTimerObj->attr, 
          &pEventObj->p.sig.anySignal, EVENT_DEFAULT_SIGNAL);

    if (QURT_EOK == i)
    {
        pTimerObj->active = TRUE;
        return DAL_SUCCESS;
    }
    else
    {
        DALSYS_LOG_ERROR_EVENT("qurt_timer_create failed- hEvent:0x%x", hEvent);
   return DAL_ERROR;
}
}

DALResult DALSYS_TimerStop(DALSYSEventHandle  hEvent) 
{
    _DALSYSq6TimerObj * pTimerObj = NULL;
    _DALSYSEventObj *pEventObj = (_DALSYSEventObj *) hEvent;
    int i = 0;

    if (!hEvent)
        return DAL_ERROR;

    // get pointer to underlying timer
    pTimerObj = pEventObj->timeout;

    // free underlying timer
    if (pTimerObj)
    {
        if (pTimerObj->active)
        {
            i = qurt_timer_delete( pTimerObj->timer );

            if (QURT_EOK != i)
                return DAL_ERROR;
        }

        pEventObj->timeout = NULL;
        DALSYS_Free(pTimerObj);
    }

    return DAL_SUCCESS;
}
