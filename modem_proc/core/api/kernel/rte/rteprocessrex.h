#ifndef RTEPROCESSREX_H
#define RTEPROCESSREX_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/api/kernel/rte/rteprocessrex.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/13   et      Created module.

===========================================================================*/


/*===========================================================================
   INCLUDES AND VARIABLE DEFINITIONS
===========================================================================*/

/*-------------------------------------------------------------------
   Include Files
-------------------------------------------------------------------*/
#include "comdef.h"
#include "rex.h"

/*-------------------------------------------------------------------
   Defines
-------------------------------------------------------------------*/
#define __RTEPROCESS_DECLARE_PROCESS( pname )                           \
  extern struct rteprocess_process_type pname##_pobject

#define __RTEPROCESS_DEFINE_THREAD( tname, sid, pri, ssize, prehook )   \
   void tname##_task(uint32 dummy);                                     \
   void prehook##(void);                                                  \
   rex_stack_word_type    tname##_stack[ssize];                         \
   rex_tcb_type           tname##_tcb;                                  \
   struct rteprocess_thread_type tname##_tobject =                      \
   {                                                                    \
      NULL, NULL,                                                       \
      #tname, sid,                                                      \
      pri, tname##_task, ssize, prehook,                                \
      {&tname##_tcb, tname##_stack}                                     \
   }

#define __RTEPROCESS_DEFINE_PROCESS(pname, pri, ssize, psize, prehook)  \
   void rteprocess_process_func(uint32 dummy);                          \
   void prehook##(void);                                                \
   rex_stack_word_type    pname##_stack[ssize];                         \
   rex_tcb_type           pname##_tcb;                                  \
   struct rteprocess_process_type pname##_pobject =                     \
   {                                                                    \
      RTEPROCESS_PROCESS_STATE_IDLE,                                    \
      #pname, pri, rteprocess_process_func, ssize, psize, prehook,      \
      {0}, &pname##_pobject.thread_list, 0,                             \
      {&pname##_tcb, pname##_stack}                                     \
   }

#define __RTEPROCESS_REGISTER_THREAD(pname, tname)                      \
   {                                                                    \
      tname##_tobject.process_ptr               = &pname##_pobject;     \
      pname##_pobject.last_thread_ptr->next_ptr = &tname##_tobject;     \
      pname##_pobject.last_thread_ptr           = &tname##_tobject;     \
      pname##_pobject.threads_count++;                                  \
   }                                                                    \

#define __RTEPROCESS_PROCESS(pname) \
   void pname##_process(uint32 dummy) /*lint -esym(715,dummy)*/
#define __RTEPROCESS_THREAD(tname)  \
   void tname##_task(uint32 dummy) /*lint -esym(715,dummy)*/

/*-------------------------------------------------------------------
   Type Declarations
-------------------------------------------------------------------*/
typedef struct
{
   rex_tcb_type*        tcb_ptr;
   rex_stack_word_type* stack_ptr;
} rte_thread_os_info_type;

//typedef rte_thread_fptr_type rte_process_fptr_type;
//typedef rex_task_func_type rte_thread_fptr_type;
/*-------------------------------------------------------------------
   Global Constant Declarations
-------------------------------------------------------------------*/


/*===========================================================================
   FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION 

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
void __rteprocess_os_prehook
(
   void* ptr
);

/*===========================================================================
FUNCTION 

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
void __rteprocess_run
(
   void* ptr
);

/*===========================================================================
FUNCTION 

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
void __rteprocess_wait_for_process
(
   void* ptr
);

/*===========================================================================
FUNCTION 
  Process is waiting for a thread to send a signal.

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
void __rteprocess_wait_for_thread
(
   void* ptr
);

/*===========================================================================
FUNCTION 
  Process send a signal to the thread ordering it to run.
DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
void __rteprocess_run_thread
(
   void* ptr
);

/*===========================================================================
FUNCTION 
  A thread signal its enclosing process that it is ready.

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
void __rteprocess_notify_thread_ready
(
   void* ptr
);


/*===========================================================================
FUNCTION 
  A thread is waiting to be started

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
void __rteprocess_wait_for_run_order
(
   void* ptr
);


/*===========================================================================
FUNCTION 

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
void __rteprocess_verify_process
(
   void* ptr
);


/*===========================================================================
FUNCTION 

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
void __rteprocess_verify_thread
(
   void* ptr
);


/*===========================================================================
FUNCTION 
  The process is waiting for the System Monitor

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
void __rteprocess_wait_for_system_monitor
(
   void* ptr
);

/*===========================================================================
FUNCTION 
  A thread is waiting to be started

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
void __rteprocess_notify_system_monitor_process_ready
(
   void* ptr
);

#endif /* RTEPROCESSREX_H */
