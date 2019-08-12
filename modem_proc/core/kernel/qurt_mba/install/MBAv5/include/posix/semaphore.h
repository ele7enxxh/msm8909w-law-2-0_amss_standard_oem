#ifndef SEMAPHORE_H
#define SEMAPHORE_H

/*==========================================================================
 * FILE:         semaphore.h
 *
 * SERVICES:     POSIX semaphore API interface
 *
 * DESCRIPTION:  POSIX semaphore API interface based upon POSIX 1003.1-2004
 *
 *               Copyright (c) 2008-2009 Qualcomm Technologies Incorporated.
 *               All Rights Reserved. QUALCOMM Proprietary and Confidential.
 *==========================================================================
 *
 *                          EDIT HISTORY FOR MODULE
 *
 *  This section contains comments describing changes made to the module.
 *  Notice that changes are listed in reverse chronological order.
 *
 *  $Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/posix/semaphore.h#1 $
 *
 *  when       who     what, where, why
 *  --------   ---     -------------------------------------------------------
 *  10/13/08   cz      Initial version.
 *  03/04/11   op      Fic warning msgs when running Doxygen
 *==========================================================================*/
#include <sys/types.h>     // Get all C sys types - includes POSIX specific
#include <sys/errno.h>     // error values

/*=============================================================================
                        TYPEDEFS
=============================================================================*/
/* sem_t definition */
typedef unsigned int   sem_t;
#define _SEM_T

/*=============================================================================
                        CONSTANTS AND MACROS
=============================================================================*/
/* constant definitions */
#define SEM_FAILED       ((sem_t*) 0)
#define SEM_VALUE_MAX    ((unsigned int) 30) // If need be increase this

/*=============================================================================
                        FUNCTIONS
=============================================================================*/

/** \details
 * POSIX standard comes with two kinds of semaphores: named and unnamed
 * semaphores.
 *
 * This implementation of POSIX kernel API only provide unnamed semaphore.
 *
 * Named semaphore functions, including sem_open(), sem_close(), sem_unlink()
 * are not supported.
 *
 * sem_timedwait() is not provided.
 */

/** \defgroup semaphore POSIX Semaphore API */

/** \ingroup semaphore */
/** @{ */

/** Initialize an unnamed semaphore.
 * Please refer to POSIX standard for details.
 * @param pshared [in] This implementation does not support non-zero value, 
 * i.e., semaphore cannot be shared between processes in this implementation. 
 */                 
int sem_init(sem_t *sem, int pshared, unsigned int value);

/** Lock a semaphore.
 * Please refer to POSIX standard for details.
 */
int sem_wait(sem_t *sem);

/** Lock a semaphore.
 * Please refer to POSIX standard for details.
 */
int sem_trywait(sem_t *sem);

/** Unlock a semaphore.
 * Please refer to POSIX standard for details.
 */
int sem_post(sem_t *sem);

/** Get the value of a semaphore.
 * Please refer to POSIX standard for details.
 */
int sem_getvalue(sem_t *sem, int *value);

/** Destroy an unnamed semaphore.
 * Please refer to POSIX standard for details.
 */
int sem_destroy(sem_t *sem);

/** @} */

#if 0
// UNSUPPORTED API declarations
int    sem_timedwait(sem_t *__sem, const struct timespec *__abstime);
sem_t *sem_open(const char *__name, int __oflag, ...);
int    sem_close(sem_t *__sem);
int    sem_unlink(const char *__name);
#endif

#endif  /* SEMAPHORE_H */

