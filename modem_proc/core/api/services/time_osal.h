 /***********************************************************************
 * time_osal.h
 * This file maps the timer osal types to underlying native kernel types.
 *
 * Copyright (C) 2015 Qualcomm Technologies, Inc.
 *
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/services/time_osal.h#1 $

when         who     what, where, why
--------   ---     ------------------------------------------------------------
05/20/14   abh     Added changes to support MultiPD on MPSS
05/01/14   abh     Changed the mutex handling macros to get the address as input params.
  
===========================================================================*/

#ifndef __TIME_OSAL_H__
#define __TIME_OSAL_H__

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "qurt.h"

/*===========================================================================
                           DATA DECLARATIONS
===========================================================================*/

/* Timer osal signal or rex tcb type object pointer */
typedef void *                           time_osal_notify_obj_ptr;

/* Signal mask for qurt_anysignal_t or rex tcb type object */
typedef uint32				time_osal_notify_data;

/* Signal mask for qurt_anysignal_t or rex tcb type object */
typedef qurt_thread_t					     time_osal_thread_id;

/* Mutex object to be used for time-services operation */
typedef qurt_mutex_t					     time_osal_mutex_t;

/* Macro for getting the thread ID */
#ifdef FEATURE_TIME_QDSP6
  #define TIME_OSAL_GET_THREAD_ID() qurt_thread_get_id()
#else
  #define TIME_OSAL_GET_THREAD_ID() 0xFF
#endif /* FEATURE_TIME_QDSP6 */

/* Time-services mutex lock / unlock macro's */
#define TIME_OSAL_MUTEX_INIT(X)  \
  qurt_pimutex_init(X); \

#define TIME_OSAL_MUTEX_LOCK(X)  \
  qurt_pimutex_lock(X); \

#define TIME_OSAL_MUTEX_UNLOCK(X)  \
  qurt_pimutex_unlock(X); \

#endif /* __TIME_OSAL_H__*/

