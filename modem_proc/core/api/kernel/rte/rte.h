#ifndef RTE_H
#define RTE_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 R U N   T I M E   E N V I R O N M E N T
                             
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/api/kernel/rte/rte.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/01/20   gr      Added C++ include guards.
03/10/13   et      Created module.

===========================================================================*/


/*===========================================================================
   INCLUDES AND VARIABLE DEFINITIONS
===========================================================================*/

/*-------------------------------------------------------------------
   Include Files
-------------------------------------------------------------------*/
#include "comdef.h"
#include "ipcrex.h"

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------
   Defines
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
   Type Declarations
-------------------------------------------------------------------*/
/* Identifies a process */
typedef uint32 rte_process_id_type;

/* Identifies a thread */
typedef uint32 rte_thread_id_type;

/* Identifies a mutex */
typedef uint32 rte_mutex_type;

/* Identifies a timer */
typedef uint32 rte_timer_handle_type;

/* Defines a duration in milliseconds */
typedef uint32 rte_timer_duration_type;

/*-------------------------------------------------------------------
   Global Constant Declarations
-------------------------------------------------------------------*/


/*===========================================================================
   FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
=============================================================================
=============================================================================
===                                                                       ===
===               P R O C E S S E S   A N D   T A S K S                   ===
===                                                                       ===
=============================================================================
=============================================================================
===========================================================================*/

/*===========================================================================
FUNCTION RTE_CREATE_PROCESS()

DESCRIPTION
   Create a new process.

PARAMETERS
   name              Name of the process
   priority          Priority of the thread in the enclosing process
   entry_point       Function that represent the thread
   stack_size        Size of the stack for this thread
   private_pool_size Size of the process private message pool

RETURN
   The id of the created process. 0 if error.
===========================================================================*/
rte_process_id_type rte_create_process
(
   char*                    name,
   uint32                   priority,
   rte_process_fptr_type    entry_point,
   uint32                   stack_size,
   uint32                   private_pool_size,
   void*                    internal_ptr
);

/*===========================================================================
FUNCTION RTE_GET_PROCESS_ID()

DESCRIPTION
  Get the current process id.

PARAMETERS
  None

RETURN
  The id of the enclosing process.
===========================================================================*/
rte_process_id_type rte_get_process_id
(
   void
);

/*===========================================================================
FUNCTION RTE_GET_PROCESS_PRIORITY()

DESCRIPTION
  Get the current process priority

PARAMETERS
  process_id   The id of the process.

RETURN
  None
===========================================================================*/
uint32 rte_get_process_priority
(
   void
);

/*===========================================================================
FUNCTION RTE_SET_PROCESS_PRIORITY()

DESCRIPTION
  Change the process priority of the current or a child process.

PARAMETERS
  child_pid    The child id of the process.
               0 changes the current process priority
  priority     The new process priority

RETURN
  None.
===========================================================================*/
void rte_set_process_priority
(
   rte_process_id_type child_pid,
   uint32              priority
);

/*===========================================================================
FUNCTION RTE_EXIT_THREAD()

DESCRIPTION
  Exit the current process gracefully

PARAMETERS
  None

RETURN
  None.
===========================================================================*/
void rte_exit_process
(
   void
);

/*===========================================================================
FUNCTION RTE_KILL_CHILD_PROCESS()

DESCRIPTION
  Kill a child process of the current process

PARAMETERS
  pid   ID of the process to kill

RETURN
  None
===========================================================================*/
void rte_kill_child_process
(
   rte_process_id_type child_pid
);


/*===========================================================================
FUNCTION RTE_CREATE_THREAD()

DESCRIPTION
  Create a thread within the containing process

PARAMETERS
  name         Name of the thread
  priority     Priority of the thread in the enclosing process
  entry_point  Function that represent the thread
  stack_size   Size of the stack for this thread

RETURN
  The thread id of the created thread.  0 if error.
===========================================================================*/
rte_thread_id_type rte_create_thread
(
   char*                    name,
   uint32                   priority,
   rte_thread_fptr_type     entry_point,
   uint32                   stack_size,
   void*                    internal_ptr
);

/*===========================================================================
FUNCTION RTE_CREATE_SERVICE()

DESCRIPTION
  Create a thread providing a particular service

PARAMETERS
  name         Name of the thread
  priority     Priority of the thread in the enclosing process
  service_id   The service provided
  entry_point  Function that represent the thread
  stack_size   Size of the stack for this thread

RETURN
  The thread id of the created thread.  0 if error.
===========================================================================*/
rte_thread_id_type rte_create_service
(
   char*                    name,
   uint32                   priority,
   uint32                   service_id,
   rte_thread_fptr_type     entry_point,
   uint32                   stack_size,
   void*                    internal_ptr
);

/*===========================================================================
FUNCTION RTE_EXIT_THREAD()

DESCRIPTION
  Exit the calling thread gracefully

PARAMETERS
  None

RETURN
  None.
===========================================================================*/
void rte_exit_thread
(
   void
);

/*===========================================================================
FUNCTION RTE_KILL_THREAD()

DESCRIPTION
  Kill another thread in the same process.

PARAMETERS
  tid   the id of the thread to kill

RETURN
  None.
===========================================================================*/
void rte_kill_thread
(
   rte_thread_id_type tid
);


/*===========================================================================
FUNCTION RTE_GET_THREAD_PRIORITY()

DESCRIPTION
  Get the current priority for a thread within an existing process

PARAMETERS
  tid   ID of the thread to get the priority from

RETURN
  The priority of the thread.  0 if tid is invalid.
===========================================================================*/
uint32 rte_get_thread_priority
(
   rte_thread_id_type tid
);

/*===========================================================================
FUNCTION RTE_SET_THREAD_PRIORITY()

DESCRIPTION
   Set the new priority for a thread within an existing process.
   This will be an empty action if tid is invalid.

PARAMETERS
   tid       Id of the thread having its priority changed
   priority  New thread priority

RETURN
   None
===========================================================================*/
void rte_set_thread_priority
(
   rte_thread_id_type tid,
   uint32             priority
);

/*===========================================================================
FUNCTION RTE_THREAD_SELF()

DESCRIPTION
  Get the current thread id

PARAMETERS
  None

RETURN
  The id of the current thread.
===========================================================================*/
rte_thread_id_type rte_thread_self
(
   void
);

/*===========================================================================
FUNCTION RTE_SLEEP()

DESCRIPTION
  Suspend the current thread for the specified time length

PARAMETERS
  duration   Time to sleep in milliseconds.

RETURN
  None.
===========================================================================*/
void rte_sleep
(
   rte_timer_duration_type duration_ms
);

/*===========================================================================
=============================================================================
=============================================================================
===                                                                       ===
===                          M U T E X E S                                ===
===                                                                       ===
=============================================================================
=============================================================================
===========================================================================*/

/*===========================================================================
FUNCTION RTE_MUTEX_INIT()

DESCRIPTION
  Create and initialize a new mutex

PARAMETERS
  mutex   Mutex to initialize

RETURN
  None.
===========================================================================*/
void rte_init_mutex
(
   rte_mutex_type* mutex_ptr
);


/*===========================================================================
FUNCTION RTE_MUTEX_LOCK()

DESCRIPTION
  Acquire the lock for the mutex. 
  The first thread to call lock on the mutex acquires it.
  Every subsequent thread attempting to lock the mutex will be suspended.
  When the owner of the mutex unlocks it, one of the suspended thread
  will be awakened and given to chance to lock the mutex.

PARAMETERS
  mutex   Mutex to attempt to lock

RETURN
  None.
===========================================================================*/
void rte_mutex_lock
(
   rte_mutex_type* mutex_ptr
);


/*===========================================================================
FUNCTION RTE_MUTEX_LOCK()

DESCRIPTION
  Release the lock associated with a mutex. 
  Calling unlock on a mutex that is not owned by the the caller,
  or on a mutex
  that is not locked, results in undefined behavior.
  The owner of the mutex The first thread to call lock on the mutex
  acquires it.  Every subsequent 
  thread attempting to lock the mutex will be suspended.
  When the owner of the mutex unlocks it, one of the suspended
  thread will be awakened and given to chance to lock the mutex.

PARAMETERS
  mutex   Mutex to attempt to lock

RETURN
  None.
===========================================================================*/
void rte_mutex_unlock
(
   rte_mutex_type* mutex_ptr
);

/*===========================================================================
=============================================================================
=============================================================================
===                                                                       ===
===                 E R R O R   H A N D L E R S                           ===
===                                                                       ===
=============================================================================
=============================================================================
===========================================================================*/

/*===========================================================================
MACRO RTE_PROCESS_ERROR()

DESCRIPTION
  Causes the current to exit and restart.
  NOTE: Depends on the System Monitor for recovery

PARAMETERS
  __msg    A string in printf() format to log
  __arg0   Argument 0 to __msg
  __arg1   Argument 1 to __msg
  __arg2   Argument 2 to __msg

RETURN
  None
===========================================================================*/
#define rte_process_error(__msg, __arg0, __arg1, __arg2)

/*===========================================================================
MACRO RTE_SYSTEM_ERROR()

DESCRIPTION
  Causes all DMSS processes to exit and restart.
  NOTE: Depends on the System Monitor for recovery

PARAMETERS
  __msg    A string in printf() format to log
  __arg0   Argument 0 to __msg
  __arg1   Argument 1 to __msg
  __arg2   Argument 2 to __msg

RETURN
  None.
===========================================================================*/
#define rte_system_error(__msg, __arg0, __arg1, __arg2)

#ifdef __cplusplus
}
#endif

#endif /* RTE_H */
