/*!
  @file
  cp_mutex.h

  @brief
  Declarations for control-plane mutexes.

  @detail
  A "control-plane mutex" wraps the platform's mutex/critical section but
  enhances it by tracking the order of entry of mutexes on a given thread.
  Each control-plane mutex has a defined order - a thread should never
  enter a mutex with a lower order than those it already holds or it risks
  deadlock.  The control-plane mutex logs an error message if a mutex is
  entered out of order, allowing the detection of potential deadlocks
  without having to have the timing just so to actually encounter the
  deadlock.

  -----------------------------------------------------------------------------
  -------------------------------- WARNING -------------------------------------
  -----------------------------------------------------------------------------
    These mutexes are only ever to be used in control-plane code paths (hence
    the name)!  They add performance overhead that is unacceptable in data-plane
    code paths!
  -----------------------------------------------------------------------------
  -----------------------------------------------------------------------------
  -----------------------------------------------------------------------------

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/cp_mutex.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#ifndef _CP_MUTEX_H_
#define _CP_MUTEX_H_

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#if !defined(WIN32)
#include <comdef.h>
#endif

#include "cp_mutex_order.h"

/*=============================================================================
===============================================================================

   This section is definitions that should not be used in modem code - these
   are used in defining the macros that should be used instead of rex_crit_sect
   or qurt_mutex calls.

===============================================================================
=============================================================================*/

/*-----------------------------------------------------------------------------
  Map the various Windows defines to the single WIN32.
-----------------------------------------------------------------------------*/
#if !defined(WIN32) && (defined(_WIN32) || defined(WINNT))
  #define WIN32
#endif

/*-----------------------------------------------------------------------------
   Define what a control-plane mutex is based on the architecture of the build.
-----------------------------------------------------------------------------*/
#if defined(WIN32)

  /*---------------------------------------------------------------------------
     Windows uses a standard critical section.
  ---------------------------------------------------------------------------*/
  #include "Windows.h"
  #define _CP_MUTEX_T CRITICAL_SECTION

#elif defined(__QDSP6_ARCH__) || defined(__HEXAGON_ARCH__)

  /*---------------------------------------------------------------------------
     QDSP6 uses a recursive mutex, meaning it can be entered multiple times
     on the same thread.  This is the desired behavior, and is the same
     as the Windows CRITICAL_SECTION.
  ---------------------------------------------------------------------------*/
  #include "qurt.h"
  #define _CP_MUTEX_T qurt_mutex_t

#else

  #error Architecture must be WIN32 or HEXAGON_ARCH to specify mutex type

#endif

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/* Define the control-plane mutex type.  This contains a pointer to an
   architecture-specific mutex, a member variable that specifies
   the order of the mutex, and the name of the mutex (to make logging
   messages more meaningful). */
typedef struct
{
  _CP_MUTEX_T * const     pcs;
  _cp_mutex_order_t order;
  const char *      name;
} cp_mutex_t;


/* Macro to get a pointer to a control-plane mutex.  This is used by the
   macros that modem code should use. */
#define GET_CP_MUTEX(name)  _cp_mutex_get_##name()

/*=============================================================================
===============================================================================

   These are the macros that should be used when writing code that uses
   modem critical sections.

===============================================================================
=============================================================================*/

/*-----------------------------------------------------------------------------
  DECLARE_CP_MUTEX(name)
  
  You should include this for a mutex in either the C file that uses
  that mutex or in an H file that gets included in the files that
  use the mutex.
-----------------------------------------------------------------------------*/
#define DECLARE_CP_MUTEX(name)                 \
  const cp_mutex_t *_cp_mutex_get_##name(void)

/*-----------------------------------------------------------------------------
  DEFINE_CP_MUTEX(name)

  This actually defines the control-plane mutex with the given name.  It
  should only be used once, in the C file in which the mutex is to be defined.
-----------------------------------------------------------------------------*/
#define DEFINE_CP_MUTEX(name)                  \
  static _CP_MUTEX_T     _real_mutex##name;    \
  static const cp_mutex_t  _cp_mutex_##name =  \
          {                                    \
            &_real_mutex##name,                \
            CP_MUTEX_ORDER(name),              \
            #name                              \
          };                                   \
                                               \
  const cp_mutex_t* _cp_mutex_get_##name(void) \
  {                                            \
    return &_cp_mutex_##name;                  \
  }

/*-----------------------------------------------------------------------------
  INIT_CP_MUTEX(name)

  Use this to initialize the mutex with the given name.
-----------------------------------------------------------------------------*/
#define INIT_CP_MUTEX(name)    _cp_mutex_init(GET_CP_MUTEX(name))

/*-----------------------------------------------------------------------------
  ENTER_CP_MUTEX(name)

  Use this to enter the named mutex.  The thread will block if it is unable to
  acquire the mutex.

  INIT_CP_MUTEX must have been called for the name.
-----------------------------------------------------------------------------*/
#define ENTER_CP_MUTEX(name)   _cp_mutex_enter(GET_CP_MUTEX(name))

/*-----------------------------------------------------------------------------
  TRY_ENTER_CP_MUTEX(name)

  Use this to attempt to enter the named mutex.  If the thread is able to enter
  the mutex it will do so and TRY_ENTER_CP_MUTEX will return TRUE.  If the
  mutex is held by another thread, TRY_ENTER_CP_MUTEX will return FALSE.

  INIT_CP_MUTEX must have been called for the name.
-----------------------------------------------------------------------------*/
#define TRY_ENTER_CP_MUTEX(name)  _cp_mutex_try_enter(GET_CP_MUTEX(name))

/*-----------------------------------------------------------------------------
  LEAVE_CP_MUTEX(name)

  Use this to leave the named mutex.

  INIT_CP_MUTEX must have been called for the name.  Behavior is undefined if
  either ENTER_CP_MUTEX or TRY_ENTER_CP_MUTEX was not previously called.
-----------------------------------------------------------------------------*/
#define LEAVE_CP_MUTEX(name)   _cp_mutex_leave(GET_CP_MUTEX(name))

/*-----------------------------------------------------------------------------
  DELETE_CP_MUTEX(name)

  Use this to delete the named mutex.

  INIT_CP_MUTEX must have been called for the name.
-----------------------------------------------------------------------------*/
#define DELETE_CP_MUTEX(name)  _cp_mutex_delete(GET_CP_MUTEX(name))

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/* Externs for the functions that actually make the mutex calls. */
void    _cp_mutex_init(const cp_mutex_t * pmcs);
void    _cp_mutex_enter(const cp_mutex_t * pmcs);
boolean _cp_mutex_try_enter(const cp_mutex_t * pmcs);
void    _cp_mutex_leave(const cp_mutex_t * pmcs);
void    _cp_mutex_delete(const cp_mutex_t * pmcs);

/* Call this to free the tracked list for a given thread. */
void  cp_mutex_free_tracked_list(void);

#endif   /* #ifndef _CP_MUTEX_H_ */

