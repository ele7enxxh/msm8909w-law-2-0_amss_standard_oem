/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 R U N   T I M E   E N V I R O N M E N T
                             
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2001-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/kernel/rte/src/rte.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
18-May-04  rwh     Changed FEATURE_REX_IPC to FEATURE_RTE_PROCESS
10-May-04  rwh     Feature-ized with FEATURE_REX_IPC
03/10/13   et      Created module.

===========================================================================*/


/*===========================================================================
   INCLUDES AND VARIABLE DEFINITIONS
===========================================================================*/

/*-------------------------------------------------------------------
   Include Files
-------------------------------------------------------------------*/
#include "comdef.h"   /*lint -e766 needed to remove compiler warning*/

#ifdef FEATURE_RTE_PROCESS

#include "msg.h"
#include "err.h"
#include "rex.h"
#ifdef FEATURE_L4
#include "rexl4.h"
#endif /* FEATURE_L4 */

#include "rte.h"
#include "rteprocess.h"
#include "pq.h"
#include "rte_ipc.h"
#include "ipcns.h"

/*-------------------------------------------------------------------
   Defines
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
   Type Declarations
-------------------------------------------------------------------*/
/* Critical section for ipc_info */
extern rex_crit_sect_type ipc_info_crit_sect;

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

DESCRIPTION                                                                */
   /** Create a new process.\\

@param name              Name of the process
@param priority          Priority of the thread in the enclosing process
@param entry_point       Function that represent the thread
@param stack_size        Size of the stack for this thread
@param private_pool_size Size of the process private message pool

@return {\bf The id of the created process. 0 if error.}
*/
/*=========================================================================*/
rte_process_id_type rte_create_process
(
   char*                    name,
   uint32                   priority,
   rte_process_fptr_type    entry_point,
   uint32                   stack_size,
   uint32                   private_pool_size,
   void*                    internal_ptr   
)
{
   struct rteprocess_process_type* piptr =
      (struct rteprocess_process_type*)internal_ptr;
   rte_thread_id_type      tid = 0;
   int                     dog_report_val = 0;
   rex_ipc_info_type *p_ipc_info_type;
   
   #ifdef ARCH_QDSP6
   rex_enter_crit_sect(&ipc_info_crit_sect);
   #else
   INTLOCK();
   #endif /* #ifdef ARCH_QDSP6 */

   p_ipc_info_type = rte_rex_get_ipc_info(piptr->os_info.tcb_ptr);
   memset( p_ipc_info_type,
           0,
           sizeof(rex_ipc_info_type));

   /* Create the thread representing the process */
   rex_def_task_ext2( piptr->os_info.tcb_ptr,
                          (unsigned char*)piptr->os_info.stack_ptr,
                          stack_size,
                          priority,
                          entry_point,
                          (dword)piptr,/*parameter is process info pointer*/
                          name,
                          FALSE,/*not suspended*/
                          dog_report_val );

   #ifdef ARCH_QDSP6
   rex_leave_crit_sect(&ipc_info_crit_sect);
   #else
   INTFREE();
   #endif /* #ifdef ARCH_QDSP6 */

   return tid;
}


/*===========================================================================
FUNCTION RTE_GET_PROCESS_ID()

DESCRIPTION                                                                */
/**  Get the current process id.\\

@param None

@return {\bf The id of the enclosing process.}
*/
/*=========================================================================*/
rte_process_id_type rte_get_process_id
(
   void
)
{
   rte_process_id_type ret_id_val;

   #ifdef ARCH_QDSP6
   rex_enter_crit_sect(&ipc_info_crit_sect);
   #else
   INTLOCK();
   #endif
   
   ret_id_val = (rte_process_id_type)rex_self()->ipc_info.node_id;

   #ifdef ARCH_QDSP6
   rex_leave_crit_sect(&ipc_info_crit_sect);
   #else
   INTFREE();
   #endif /* #ifdef ARCH_QDSP6 */

   return ret_id_val;
}


/*===========================================================================
FUNCTION RTE_GET_PROCESS_PRIORITY()

DESCRIPTION                                                                */
/** Get the current process priority

@param process_id   The id of the process.

@return {\bf None}
*/
/*=========================================================================*/
uint32 rte_get_process_priority
(
   void
)
{
   return rex_self()->pri;
}


/*===========================================================================
FUNCTION RTE_SET_PROCESS_PRIORITY()

DESCRIPTION                                                                */
/** Change the process priority of the current or a child process.

@param child_pid    The child id of the process.
                    0 changes the current process priority
@param priority     The new process priority

@return {\bf None}
*/
/*=========================================================================*/
void rte_set_process_priority
(
   rte_process_id_type child_pid,
   uint32              priority
)
{
}


/*===========================================================================
FUNCTION RTE_EXIT_THREAD()

DESCRIPTION                                                                */
/** Exit the current process gracefully

@param None

@return {\bf None}
*/
/*=========================================================================*/
void rte_exit_process
(
   void
)
{
}


/*===========================================================================
FUNCTION RTE_KILL_CHILD_PROCESS()

DESCRIPTION                                                                */
/** Kill a child process of the current process

@param pid   ID of the process to kill

@return {\bf None}
*/
/*=========================================================================*/
void rte_kill_child_process
(
   rte_process_id_type child_pid
)
{
}


/*===========================================================================
FUNCTION RTE_CREATE_SERVICE()

DESCRIPTION                                                                */
/** Create a thread within the containing process, providing a service.

@param name         Name of the thread
@param service      The service provided
@param priority     Priority of the thread in the enclosing process
@param entry_point  Function that represent the thread
@param stack_size   Size of the stack for this thread

@return {\bf The thread id of the created thread.  0 if error.}
*/
/*=========================================================================*/
rte_thread_id_type rte_create_service
(
   char*                    name,
   uint32                   service_id,
   uint32                   priority,
   rte_thread_fptr_type     entry_point,
   uint32                   stack_size,
   void*                    internal_ptr
)
{
   struct rteprocess_thread_type* tiptr =
     (struct rteprocess_thread_type*)internal_ptr;
   int                     dog_report_val = 0;
   rte_thread_id_type      ret_id_val;
   rex_ipc_info_type *p_ipc_info_type;

   #ifdef ARCH_QDSP6
   rex_enter_crit_sect(&ipc_info_crit_sect);
   #else
   INTLOCK();
   #endif /* #ifdef ARCH_QDSP6 */

   p_ipc_info_type = rte_rex_get_ipc_info(tiptr->os_info.tcb_ptr);
   
   memset( p_ipc_info_type,
           0,
           sizeof(rex_ipc_info_type));

   #ifdef ARCH_QDSP6
   rex_leave_crit_sect(&ipc_info_crit_sect);
   #else
   INTFREE();
   #endif /* #ifdef ARCH_QDSP6 */
  
   /* Create the thread representing the service */
   rex_def_task_ext2( tiptr->os_info.tcb_ptr,
                          (unsigned char*)tiptr->os_info.stack_ptr,
                          stack_size,
                          priority,
                          entry_point,
                          (dword)tiptr,/*thread info pointer*/
                          name,
                          FALSE,/*not suspended*/
                          dog_report_val );

   #ifdef ARCH_QDSP6
   rex_enter_crit_sect(&ipc_info_crit_sect);
   #else
   INTLOCK();
   #endif /* #ifdef ARCH_QDSP6 */

   if (ipcns_service_register(service_id, tiptr->os_info.tcb_ptr))
   {
      MSG_LOW( "Thread (%s) has Node ID (%d) and provides Service (%d)",
               name,
               p_ipc_info_type->node_id,
               service_id );
   }
   else
   {
      MSG_ERROR("Cannot create IPC Service for thread (%s)", name, 0, 0);
   }

   ret_id_val = p_ipc_info_type->node_id;

   #ifdef ARCH_QDSP6
   rex_leave_crit_sect(&ipc_info_crit_sect);
   #else
   INTFREE();
   #endif /* #ifdef ARCH_QDSP6 */

   return ret_id_val;
}


/*===========================================================================
FUNCTION RTE_CREATE_THREAD()

DESCRIPTION                                                                */
/** Create a thread within the containing process

@param name         Name of the thread
@param priority     Priority of the thread in the enclosing process
@param entry_point  Function that represent the thread
@param stack_size   Size of the stack for this thread

@return {\bf The thread id of the created thread.  0 if error.}
*/
/*=========================================================================*/
rte_thread_id_type rte_create_thread
(
   char*                name,
   uint32               priority,
   rte_thread_fptr_type entry_point,
   uint32               stack_size,
   void*                internal_ptr
)
{
   struct rteprocess_thread_type* tiptr =
      (struct rteprocess_thread_type*)internal_ptr;
   int dog_report_val = 0;
   rte_thread_id_type      ret_id_val;
   rex_ipc_info_type *p_ipc_info_type;

   #ifdef ARCH_QDSP6
   rex_enter_crit_sect(&ipc_info_crit_sect);
   #else
   INTLOCK();
   #endif /* #ifdef ARCH_QDSP6 */

   p_ipc_info_type = rte_rex_get_ipc_info(tiptr->os_info.tcb_ptr);
   
   memset( p_ipc_info_type,
           0,
           sizeof(rex_ipc_info_type));

   #ifdef ARCH_QDSP6
   rex_leave_crit_sect(&ipc_info_crit_sect);
   #else
   INTFREE();
   #endif /* #ifdef ARCH_QDSP6 */

   /* Create the thread */
   rex_def_task_ext2( tiptr->os_info.tcb_ptr,
                          (unsigned char*)tiptr->os_info.stack_ptr,
                          stack_size,
                          priority,
                          entry_point,
                          (dword)tiptr,/* thread info pointer*/
                          name,
                          FALSE,/*not suspended*/
                          dog_report_val );

   #ifdef ARCH_QDSP6
   rex_enter_crit_sect(&ipc_info_crit_sect);
   #else
   INTLOCK();
   #endif /* #ifdef ARCH_QDSP6 */
   
   ret_id_val = p_ipc_info_type->node_id;
   #ifdef ARCH_QDSP6
   rex_leave_crit_sect(&ipc_info_crit_sect);
   #else
   INTFREE();
   #endif /* #ifdef ARCH_QDSP6 */

   return ret_id_val;
}


/*===========================================================================
FUNCTION RTE_EXIT_THREAD()

DESCRIPTION                                                                */
/** Exit the calling thread gracefully

@param None

@return {\bf None}
*/
/*=========================================================================*/
void rte_exit_thread
(
   void
)
{
}


/*===========================================================================
FUNCTION RTE_KILL_THREAD()

DESCRIPTION                                                                */
/** Kill another thread in the same process.

@param tid   the id of the thread to kill

@return {\bf None}
*/
/*=========================================================================*/
void rte_kill_thread
(
   rte_thread_id_type tid
)
{
}


/*===========================================================================
FUNCTION RTE_GET_THREAD_PRIORITY()

DESCRIPTION                                                                */
/** Get the current priority for a thread within an existing process

@param tid   ID of the thread to get the priority from

@return {\bf The priority of the thread.  0 if tid is invalid.}
*/
/*=========================================================================*/
uint32 rte_get_thread_priority
(
   rte_thread_id_type tid
)
{
   return rex_get_pri();
}


/*===========================================================================
FUNCTION RTE_SET_THREAD_PRIORITY()

DESCRIPTION                                                                */
/** Set the new priority for a thread within an existing process.
    This will be an empty action if tid is invalid.

@param tid       Id of the thread having its priority changed
@param priority  New thread priority

@return {\bf None}
*/
/*=========================================================================*/
void rte_set_thread_priority
(
   rte_thread_id_type tid,
   uint32             priority
)
{
}


/*===========================================================================
FUNCTION RTE_THREAD_SELF()

DESCRIPTION                                                                */
/** Get the current thread id

@param None

@return {\bf The id of the current thread.}
*/
/*=========================================================================*/
rte_thread_id_type rte_thread_self
(
   void
)
{
   return (rte_thread_id_type)rex_self();
}


/*===========================================================================
FUNCTION RTE_SLEEP()

DESCRIPTION                                                                */
/** Suspend the current thread for the specified time length

@param duration   Time to sleep in milliseconds.

@return {\bf None}
*/
/*=========================================================================*/
void rte_sleep
(
   rte_timer_duration_type duration_ms
)
{
}


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

DESCRIPTION                                                                */
/** Create and initialize a new mutex

@param mutex   Mutex to initialize

@return {\bf None}
*/
/*=========================================================================*/
void rte_init_mutex
(
   rte_mutex_type* mutex_ptr
)
{
}


/*===========================================================================
FUNCTION RTE_MUTEX_LOCK()

DESCRIPTION                                                                */
/** Acquire the lock for the mutex. 
    The first thread to call lock on the mutex acquires it.
    Every subsequent thread attempting to lock the mutex will be suspended.
    When the owner of the mutex unlocks it, one of the suspended thread
    will be awakened and given to chance to lock the mutex.

@param mutex   Mutex to attempt to lock

@return {\bf None}
*/
/*=========================================================================*/
void rte_mutex_lock
(
   rte_mutex_type* mutex_ptr
)
{
}


/*===========================================================================
FUNCTION RTE_MUTEX_LOCK()

DESCRIPTION                                                                */
/** Release the lock associated with a mutex. 
    Calling unlock on a mutex that is not owned by the the caller,
    or on a mutex
    that is not locked, results in undefined behavior.
    The owner of the mutex The first thread to call lock on the mutex
    acquires it.  Every subsequent 
    thread attempting to lock the mutex will be suspended.
    When the owner of the mutex unlocks it, one of the suspended
    thread will be awakened and given to chance to lock the mutex.

@param mutex   Mutex to attempt to lock

@return {\bf None}
*/
/*=========================================================================*/
void rte_mutex_unlock
(
   rte_mutex_type* mutex_ptr
)
{
}

#endif /* FEATURE_RTE_PROCESS */

