#ifndef IXOSLOCK_H
#define IXOSLOCK_H

/**
   @file IxOSLock.h

   @brief
   This module is an operating system abstraction layer that 
   provides OS locking mechanisms for secureMSM applications.
   The calls are routed to Rex or Blast native API's. 

   Externalized Functions:
   IxOSLock_Create
   IxOSLock_Release
   IxOSLock_Enter
   IxOSLock_Leave
   IxOSLock_TryEnter.

   Initialization and sequencing requirements:
   None.
*/

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         H E A D E R   F O R
                       I X O S L O C K  M O D U L E

Copyright (c) 2010 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                       EDIT HISTORY FOR MODULE

$Header: //components/rel/core.mpss/3.5.c12.3/securemsm/smecom/ixosal/shared/inc/IxOSLock.h#1 $

when         who     what, where, why
--------     ---     --------------------------------------------------------
12/01/10     nk      Initial Revision.
04/11/11     nk      Doxygenated, added null check and other review
                     modifications
===========================================================================*/
/** @addtogroup SMECOMUTIL_IPC 
@{ 
*/

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
/*
* Include files
*/
#include "comdef.h"
#include "IxErrno.h"
#include "rtedef.h"

#ifdef __cplusplus
__CPLUSPLUS_GUARD_BEGIN
#endif /* #ifdef __cplusplus */


/**
@name OS ABSTRACTION LOCK MECHANISM
@{

   @brief
   The module APIs provide an operating system abstracted services
   to secureMSM components. The implementation provides an abstraction 
   layer over REX and Blast native lock mechanisms.

   @detail
   This module provides support for critical sections and interrupt lock
   for REX and only critical section for BLAST OS. IxOSLockEnumType defines 
   critical section and isr lock types. An object registry implementation 
   converts the normal unsigned int handle to corresponding native structures,
   This also keeps track of no of locks generated and type of locks for native 
   invocation.

   Mutexes:
   Mutexes are used to allow only one thread to enter the critical section
   of code that is accessing shared data. Thus, a given data structure which
   needs mutual exclusion will need to create a mutex to go with it. There
   will be three different types of mutexes created; one which is accessed from
   threads only, and another which is accessed between threads and the ISR, and
   another which defines critical sections using Rex.

   For the mutex accessed from threads only, context switching will be disabled
   if shared data is accessed between different processes - where one may
   not necessarily be the owner of (and thus not have full control of) the
   shared data structure. This is different from critical sections, where 
   context switching is still enabled (note that if a swapped in thread
   tries to access the shared resource critical section, it will suspend). 

   Description of Types of Locks
   1. OSLOCK_CRITICALSECTION_ONLY (REX, BLAST)
      Critical sections are used. Used to attempt to enter the critical section
      If successful, proceed to access the critical section. If not, the calling
      task will be suspended since the critical section is unavailable (already
      locked out), since context switching is still allowed. Interrupts can
      still occur. More efficient (much faster as far as lock times); use this
      mutex for all inter-thread synchronization. Critical sections are limited,
      however, since it cannot be shared across processes.

   2. OSLOCK_ISR_ONLY (REX only)
      Interrupt_Lock() is used to disable interrupts until after its subsequent call
      to os_MutexUnlock(). Context switching is still allowed. Used for mutex
      between thread and ISR.

   3. OSLOCK_TASK_ONLY  (REX only)
      Tasklock() is used to disable context switching until its subsequent
      call to os_MutexUnlock(). Processing stays within the current task while
      interrupts can still occur. Good to use for inter-process synchronization.
      Least efficient; use as a last resort.
@}
*/


/**
   Enum type defination for IxOSLockEnumType
   IxOSLockEnumType defines the time critical, task and isr only, 
   task and isr time critical lock types.
*/
typedef enum
{
   OSLOCK_CRITICALSECTION_ONLY = 0x0001,   /**< -- lock between Critical Section */
   OSLOCK_ISR_ONLY             = 0x0002,   /**< -- lock between ISR */
   OSLOCK_TASK_ONLY            = 0x0004,   /**< -- lock between Task */
} IxOSLockEnumType;

/**
   LOCKHANDLE is an unsigned int type used 
   as an handle for lock mechanism calls 
*/
typedef unsigned int LOCKHANDLE;

/**
   DEBUGFLAG is an unsigned int type used 
   as an type to print lock diagnostic messages. 
*/
typedef unsigned char DEBUGFLAG;

/**
   IX_HANDLE is an void * type used 
   as an handle for lock mechanism calls 
*/
typedef void* IX_HANDLE;

/**
   strLockInfo defines the rex lock variable and the lock type enum.
   IxOSLockEnumType and rex_crit_sect_type are its members.
*/
typedef struct
{
   IxOSLockEnumType e_LockType;    /**< -- Mutex Type: one of three types */
   IX_HANDLE lockhandle;         /**< -- Lock Handle: void handle*/
} sLockInfo;


/**
@name Macro declarations of LOCK API's
@{
   Macros created to handle LOCK API's 
   Usage IXOSLOCK_CREATE(g_sslcslock, LOCK_CRITICALSECTION_ONLY);
   IXOSLOCK_ENTER(g_sslcslock);
*/
/**
   creates lock with locktype and handle as param.
*/
#define IXOSLOCK_CREATE(A, B) \
        LOCKHANDLE B; \
        IxOSLock_Create(&B, A );

/**
   Releases lock with handle as param.
*/
#define IXOSLOCK_RELEASE(B) \
        IxErrnoType IxOSLock_Release(B);

/**
   Enter or lock with handle as param.
*/
#define IXOSLOCK_ENTER(B) \
        IxErrnoType IxOSLock_Enter(B);

/**
   Leave  or unlock with handle as param.
*/
#define IXOSLOCK_LEAVE(B) \
        IxErrnoType IxOSLock_Leave(B);

/**
   Try enter or lock with handle as param.
*/
#define IXOSLOCK_TRYENTER(B) \
IxErrnoType IxOSLock_TryEnter(B);

/** @} */ /** end_name Macro declarations of LOCK API's */
/**
   Creates the mutex handle and registers it into the object registry.
   The lockhandle should be intialised to 0 which is 
   treated as an error condition. Lock type should passed as param. 
   The function routes calls to REX or Blast native calls. 

   @param[in] p_iHandle Pointer to unsigned interger handle.
   @param[in] eLockType Enum for lock type
                        Values: OSLOCK_TASK_ONLY, 
                                OSLOCK_ISR_ONLY,
                                OSLOCK_CRITICALSECTION_ONLY, 

   @retval
   Returns the error code,
   E_SUCCESS  for success case.
   E_FAILURE for failure case.
   E_NOT_SUPPORTED when native calls are not supported.

   @dependencies 
   None.
*/
IxErrnoType IxOSLock_Create( LOCKHANDLE *p_iHandle, IxOSLockEnumType eLockType );

/**
   Release an previously created lock handle, 
   de-registers in object registry and cleans the internal 
   resources. This function routes calls to REX or Blast native API calls.

   NOTE: In case of Blast Mutexes must be destroyed when they are no longer in use. 
   Failure to do this will cause resource leaks in the BLAST kernel.
   Mutexes must not be destroyed while they are still in use. 
   If this happens the behavior of BLAST is undefined.

   @param[in] iHandle Unsigned int handle to the lock.

   @retval
   Returns the error code,
   E_SUCCESS  for success case.
   E_FAILURE for failure case.
   E_NOT_SUPPORTED when native calls are not supported.

   @dependencies 
   None.
*/
IxErrnoType IxOSLock_Release( LOCKHANDLE iHandle );

/**
   Enters the critical section or int lock routine.
   This function routes calls to REX or Blast native API calls.

   @param[in] iHandle Unsigned int handle to the lock.

   @retval
   Returns the error code,
   E_SUCCESS  for success case.
   E_FAILURE for failure case.
   E_NOT_SUPPORTED when native calls are not supported.

   @dependencies
   None.
*/
IxErrnoType IxOSLock_Enter( LOCKHANDLE iHandle );

/**
   Leaves the critical section or int lock routine.
   also frees the lock for mutex of type p_Lock.
   This function routes calls to REX or Blast native API calls.

   Note: The blast calls are called when feature blast is enabled. 
   If there is a thread waiting on the mutex then the mutex gets assigned to that
   waiting thread which is at the head of the queue.

   @param[in] iHandle Unsigned int handle to the lock.

   @retval
   Returns the error code,
   E_SUCCESS  for success case.
   E_FAILURE for failure case.
   E_NOT_SUPPORTED when native calls are not supported.

   @dependencies
   None.
*/
IxErrnoType IxOSLock_Leave( LOCKHANDLE iHandle );

/**

   Trys to enter the critical section or obtains a lock on interrupt.
   This function routes calls to REX or Blast native API calls.

   @param[in] iHandle Unsigned int handle to the lock.

   @retval
   Returns the error code,
   E_SUCCESS  for success case.
   E_FAILURE for failure case.
   E_NOT_SUPPORTED when native calls are not supported.

   @dependencies
   None.
*/
IxErrnoType IxOSLock_TryEnter( LOCKHANDLE iHandle );

#ifdef __cplusplus
__CPLUSPLUS_GUARD_END
#endif /* #ifdef __cplusplus */

/** @} */  /** end_addtogroup SMECOMUTIL_IPC */

#endif /* IxOSLOCK_H */


