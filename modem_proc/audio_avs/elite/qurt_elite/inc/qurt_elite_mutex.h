/*========================================================================
Copyright (c) 2010-2011, 2013 Qualcomm Technologies, Inc. All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================== */
/**
@file qurt_elite_mutex.h

@brief This file contains mutex utilites. Recursive mutexes are always used
for thread-safe programming.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      qurt_elite_mainpage.dox file.
===========================================================================*/

/*========================================================================
Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/elite/qurt_elite/inc/qurt_elite_mutex.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/25/13   sw      (Tech Pubs) Edited Doxygen comments/markup for 2.0.
05/03/11   leo     (Tech Pubs) Edited doxygen comments and markup.
03/08/10   brs      Edited for doxygen-to-latex process.
02/04/10   mwc      Created file.
========================================================================== */

#ifndef QURT_ELITE_MUTEX_H
#define QURT_ELITE_MUTEX_H

#include "qurt.h"
#include "qurt_pimutex.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** @addtogroup qurt_elite_mutex
@{ */

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

/** Mutex type. */
typedef qurt_mutex_t qurt_elite_mutex_t;

/****************************************************************************
** Mutexes
*****************************************************************************/

/**
  Initializes a mutex. Recursive mutexes are always used.

  @datatypes
  #qurt_elite_mutex_t

  @param[in] pqurt_elite_mutex   Pointer to the mutex to initialize.

  @return
  None.

  @dependencies
  None. @newpage
*/
static inline void qurt_elite_mutex_init(qurt_elite_mutex_t *pqurt_elite_mutex)
{
   qurt_pimutex_init((qurt_mutex_t*)pqurt_elite_mutex);
   //The desired attributes are the default attributes.
}

/**
  Locks a mutex. Recursive mutexes are always used.

  @datatypes
  #qurt_elite_mutex_t

  @param[in] pqurt_elite_mutex   Pointer to the mutex to lock.

  @return
  None.

  @dependencies
  The object must have been created and initialized before calling this
  function.
*/
static inline void qurt_elite_mutex_lock(qurt_elite_mutex_t *pqurt_elite_mutex)
{
   qurt_pimutex_lock((qurt_mutex_t*) pqurt_elite_mutex);
}

/**
  Attempts to lock a mutex. If the lock is already held, a failure is returned.

  @datatypes
  #qurt_elite_mutex_t

  @param[in] pqurt_elite_mutex   Pointer to the mutex to try locking.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  The object must have been created and initialized before calling this
  function. @newpage
*/
static inline int qurt_elite_mutex_try_lock(qurt_elite_mutex_t *pqurt_elite_mutex)
{
    return qurt_pimutex_try_lock((qurt_mutex_t*) pqurt_elite_mutex);
}

/**
  Unlocks a mutex. Recursive mutexes are always used.

  @datatypes
  #qurt_elite_mutex_t

  @param[in] pqurt_elite_mutex   Pointer to the mutex to unlock.

  @return
  None.

  @dependencies
  The object must have been created and initialized before calling this
  function.
*/
static inline void qurt_elite_mutex_unlock(qurt_elite_mutex_t *pqurt_elite_mutex)
{
   qurt_pimutex_unlock((qurt_mutex_t*) pqurt_elite_mutex);
}

/**
  Detroys a mutex. This function must be called for each corresponding
  qurt_elite_mutex_init() function to clean up all resources.

  @datatypes
  #qurt_elite_mutex_t

  @param[in] pqurt_elite_mutex   Pointer to the mutex to destroy.

  @return
  None.

  @dependencies
  The object must have been created and initialized before calling this
  function.
*/
static inline void qurt_elite_mutex_destroy(qurt_elite_mutex_t *pqurt_elite_mutex)
{
    qurt_pimutex_destroy((qurt_mutex_t*) pqurt_elite_mutex);
}

/** @} */ /* end_addtogroup qurt_elite_mutex */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_MUTEX_H

