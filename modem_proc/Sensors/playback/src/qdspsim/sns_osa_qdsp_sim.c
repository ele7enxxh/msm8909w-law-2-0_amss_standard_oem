/*============================================================================
  @file sns_osa_qdsp_sim.c

  @brief
  Implements the OS abstraction layer for QDSP SIM on windows.


  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2013-06-11  ae   Initial version

============================================================================*/


/*=====================================================================
                              INCLUDES
=======================================================================*/
#include "sensor1.h"
#include "sns_osa.h"
#include "types.h"
#include <Windows.h>
#include <oal.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>

/*=====================================================================
                    INTERNAL DEFINITIONS AND TYPES
=======================================================================*/


/*=====================================================================
                    EXERNAL GLOBALS
=======================================================================*/

/*=====================================================================
                    STATIC GLOBALS
=======================================================================*/

/*=====================================================================
                    INTERNAL FUNCTIONS
=======================================================================*/

/*=====================================================================
                     EXTERNAL FUNCTIONS
=======================================================================*/
/* Task */
/*===========================================================================

  FUNCTION: sns_os_task_create

  ===========================================================================*/
uint8_t sns_os_task_create (void           (*task)(void *p_arg),
                            void            *p_arg,
                            OS_STK          *ptos,
                            uint8_t          prio)
{
  uint8_t error;
  error = _beginthread(task, 0, p_arg);

  if(error == -1) {
     return errno;
  } else {
     return OS_ERR_NONE;
  }
}

/*===========================================================================

  FUNCTION: sns_os_task_del

  ===========================================================================*/
uint8_t sns_os_task_del     (uint8_t prio)
{
  // TODO: Implement
  return 0;
}

/*===========================================================================

  FUNCTION: sns_os_task_del_req

  ===========================================================================*/
uint8_t sns_os_task_del_req (uint8_t prio)
{
  // TODO: Implement
  return 1;
}


/* Mutex */


/*===========================================================================

  FUNCTION: sns_os_mutex_create

  ===========================================================================*/
OS_EVENT *sns_os_mutex_create (uint8_t          prio,
                               uint8_t         *perr)
{
  OS_EVENT *event=NULL;

  event = (OS_EVENT*)malloc(sizeof(OS_EVENT));
  if(!event) {
      *perr = -1;
      return NULL;
  }

  *perr = OS_ERR_NONE;
  InitializeCriticalSection( &event->mutex );

  return event;
}


/*===========================================================================

  FUNCTION: sns_os_mutex_del

  ===========================================================================*/
OS_EVENT *sns_os_mutex_del (OS_EVENT        *pevent,
                            uint8_t          opt,
                            uint8_t         *perr)
{
  DeleteCriticalSection(&pevent->mutex);
  free(pevent);
  *perr = OS_ERR_NONE;
 
  return pevent;
}


/*===========================================================================

  FUNCTION: sns_os_mutex_pend

  ===========================================================================*/
void sns_os_mutex_pend (OS_EVENT        *pevent,
                        uint32_t         timeout,
                        uint8_t         *perr)
{
  EnterCriticalSection(&pevent->mutex);
  *perr = OS_ERR_NONE;
}

/*===========================================================================

  FUNCTION: sns_os_mutex_post

  ===========================================================================*/
uint8_t sns_os_mutex_post (OS_EVENT        *pevent)
{
    LeaveCriticalSection(&pevent->mutex);
    return OS_ERR_NONE;
}


/*===========================================================================

  FUNCTION: sns_os_sigs_create

  ===========================================================================*/
OS_FLAG_GRP  *sns_os_sigs_create (OS_FLAGS         flags,
                                  uint8_t         *perr)
{
  OS_FLAG_GRP *grp;

  grp = (OS_FLAG_GRP*)malloc(sizeof(OS_FLAG_GRP));
  if( NULL == grp ) {
    *perr = OS_ERR_MEM_FULL;
    return NULL;
  }
  grp->flags = flags;

  InitializeCriticalSection(&grp->mutex);
  InitializeConditionVariable(&grp->cond);

  *perr = OS_ERR_NONE;
  return grp;
}

/*===========================================================================

  FUNCTION: sns_os_sigs_del

  ===========================================================================*/
OS_FLAG_GRP  *sns_os_sigs_del (OS_FLAG_GRP     *pgrp,
                               uint8_t          opt,
                               uint8_t         *perr)
{
  DeleteCriticalSection(&pgrp->mutex);
  free(pgrp);
  pgrp = NULL;
  *perr = OS_ERR_NONE;
  return pgrp;
}

/*===========================================================================

  FUNCTION: sns_os_sigs_ready

  ===========================================================================*/
static int8_t sns_os_sigs_ready (OS_FLAGS set_flags,
                                 OS_FLAGS pend_flags,
                                 uint8_t  wait_type )
{
  int8_t rv = FALSE;
  switch( (wait_type & 0xF) ) {
    case OS_FLAG_WAIT_CLR_ALL:
      if( (~set_flags & pend_flags) == pend_flags ) {
        rv = TRUE;
      }
      break;
    case OS_FLAG_WAIT_CLR_ANY:
      if( ~set_flags & pend_flags ) {
        rv = TRUE;
      }
      break;
    case OS_FLAG_WAIT_SET_ALL:
      if( (set_flags & pend_flags) == pend_flags ) {
        rv = TRUE;
      }
      break;
    default:
    case OS_FLAG_WAIT_SET_ANY:
      if( set_flags & pend_flags ) {
        rv = TRUE;
      }
      break;
  }
  return rv;
}

/*===========================================================================

  FUNCTION: sns_os_sigs_pend

  ===========================================================================*/
OS_FLAGS  sns_os_sigs_pend (OS_FLAG_GRP     *pgrp,
                            OS_FLAGS         flags,
                            uint8_t          wait_type,
                            uint32_t         timeout,
                            uint8_t         *perr)
{
  uint32_t set_flags;
  int error = 0;

  EnterCriticalSection(&pgrp->mutex);
  set_flags = pgrp->flags & flags;
  if( !sns_os_sigs_ready(set_flags,flags,wait_type) ) {
    do {
        if( 0 == timeout ) {
            error =
                SleepConditionVariableCS(&pgrp->cond, &pgrp->mutex, INFINITE);
        } else {
            error =
                SleepConditionVariableCS(&pgrp->cond, &pgrp->mutex, timeout);
        }
        set_flags = pgrp->flags & flags;
        
        if( 0 == error ) {
            break;
        }
    } while( !sns_os_sigs_ready(set_flags,flags,wait_type) );
  }

  if( OS_FLAG_CONSUME & wait_type ) {
    pgrp->flags &= ~flags;
  }
  LeaveCriticalSection(&pgrp->mutex);

  if( 0 == error ) {
     // TODO: set a proper error
    *perr = OS_ERR_TIMEOUT;
  } else {
    *perr = OS_ERR_NONE;
  }
  return set_flags;
}


/*===========================================================================

  FUNCTION: sns_os_sigs_post

  ===========================================================================*/
OS_FLAGS sns_os_sigs_post (OS_FLAG_GRP     *pgrp,
                           OS_FLAGS         flags,
                           uint8_t          opt,
                           uint8_t         *perr)
{
  EnterCriticalSection(&pgrp->mutex);

  if( OS_FLAG_CLR == opt ) {
    pgrp->flags &= ~flags;
  } else {
    pgrp->flags |= flags;
  }

  flags = pgrp->flags;
  WakeConditionVariable(&pgrp->cond);
  LeaveCriticalSection(&pgrp->mutex);

  *perr = OS_ERR_NONE;
  return flags;
}

/*===========================================================================

  FUNCTION: sns_os_sigs_add

  ===========================================================================*/
void *sns_os_sigs_add (OS_FLAG_GRP     *pgrp,
                       OS_FLAGS         flags)
{
  // TODO: Implement
  return NULL;
}

