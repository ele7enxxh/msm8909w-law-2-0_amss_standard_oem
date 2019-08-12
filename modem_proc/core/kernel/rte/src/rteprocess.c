/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2001-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/kernel/rte/src/rteprocess.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
18-May-04  rwh     Changed FEATURE_REX_IPC to FEATURE_RTE_PROCESS
12-May-04  rwh     Reordered some functions to avoid 6050 object file problem.
10-May-04  rwh     Feature-ized with FEATURE_REX_IPC. 
03/10/13   et      Created module.

===========================================================================*/


/*===========================================================================
   INCLUDES AND VARIABLE DEFINITIONS
===========================================================================*/

/*-------------------------------------------------------------------
   Include Files
-------------------------------------------------------------------*/
#include "comdef.h" /*lint -e766 needed to remove compiler warning*/

#ifdef FEATURE_RTE_PROCESS

#include "msg.h"
#include "rte.h"
#include "rte_ipc.h"

#include "rteprocess.h"

/*-------------------------------------------------------------------
   Defines
-------------------------------------------------------------------*/

      
/*-------------------------------------------------------------------
   Type Declarations
-------------------------------------------------------------------*/

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
void rteprocess_process_msg
(
   union ipc_msg_type* msg_ptr
)
{
   MSG_LOW("Got message ID 0x%04x", *((uint32*)msg_ptr), 0, 0 );  
}


/*===========================================================================
FUNCTION 

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
void rteprocess_thread_func
(
   dword dummy
)
{
   struct rteprocess_thread_type* tptr = (struct rteprocess_thread_type*)dummy;

   /*------------------------------------------------------------------------
   Do power-up initialization before task starts.
   -------------------------------------------------------------------------*/
   if (tptr->pre_hook != NULL)
   {
      tptr->pre_hook();
   }

   /*------------------------------------------------------------------------
   Let the process know we are done running our hook code
   -------------------------------------------------------------------------*/
   __rteprocess_notify_thread_ready(tptr);

   /*------------------------------------------------------------------------
   Wait for the process to give us the permission to run
   -------------------------------------------------------------------------*/
   __rteprocess_wait_for_run_order(tptr);

   /*------------------------------------------------------------------------
   Acknowledge the run order
   -------------------------------------------------------------------------*/
   __rteprocess_notify_thread_ready(tptr);

   /*------------------------------------------------------------------------
   Last checks to run before letting go...
   -------------------------------------------------------------------------*/
   __rteprocess_verify_thread(tptr);

   /*------------------------------------------------------------------------
   Run the user main loop code
   -------------------------------------------------------------------------*/
   tptr->entry_point(0);
}


/*===========================================================================
FUNCTION 

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
void rteprocess_create_threads
(
   struct rteprocess_process_type* process_ptr
)
{
   struct rteprocess_thread_type* thread_ptr;
   
   /*
   ** Create the threads in this process,
   ** and run the configured pre-hooks for each of them
   **/
   thread_ptr = process_ptr->thread_list.next_ptr;
   while (thread_ptr != NULL)
   {
      if (thread_ptr->service_id == 0)
      {
         (void)rte_create_thread( thread_ptr->name,
                                  thread_ptr->priority,
                                  rteprocess_thread_func,
                                  thread_ptr->stack_size,
                                  thread_ptr );
      }
      else
      {
         (void)rte_create_service( thread_ptr->name,
                                   thread_ptr->service_id,
                                   thread_ptr->priority,
                                   rteprocess_thread_func,
                                   thread_ptr->stack_size,
                                   thread_ptr );
      }

      /* Wait for the thread to run its pre-hook and get back to us */
      __rteprocess_wait_for_thread(thread_ptr);
      thread_ptr = thread_ptr->next_ptr;
   }
}


/*===========================================================================
FUNCTION 

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
void rteprocess_run_threads
(
   struct rteprocess_process_type* process_ptr
)
{
   struct rteprocess_thread_type* thread_ptr;
   /*
   ** Run the threads
   */
   thread_ptr = process_ptr->thread_list.next_ptr;
   while (thread_ptr != NULL)
   {
      __rteprocess_run_thread(thread_ptr);
      thread_ptr = thread_ptr->next_ptr;
   }
}


/*===========================================================================
FUNCTION 

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
void rteprocess_run
(
   struct rteprocess_process_type* process_ptr
)
{
   /*
   ** Run the process - All tasks are running
   */
   __rteprocess_run(process_ptr);
   __rteprocess_wait_for_process(process_ptr);
}


/*===========================================================================
FUNCTION 

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
void rteprocess_process_func
(
   dword dummy
)
{
   struct rteprocess_process_type* process_ptr;

   process_ptr = (struct rteprocess_process_type*)dummy;

   /*------------------------------------------------------------------------
   Do OS dependent initialization
   -------------------------------------------------------------------------*/
   __rteprocess_os_prehook(process_ptr);
   
   /*------------------------------------------------------------------------
   Do power-up initialization before task starts.
   -------------------------------------------------------------------------*/
   if (process_ptr->pre_hook != NULL)
   {
      process_ptr->pre_hook();
   }

   /*------------------------------------------------------------------------
   Run the process initialization state machine
   -------------------------------------------------------------------------*/
   while(process_ptr->state != RTEPROCESS_PROCESS_STATE_UP)
   {
      switch(process_ptr->state)
      {
         case RTEPROCESS_PROCESS_STATE_IDLE:
         {
            process_ptr->state = RTEPROCESS_PROCESS_STATE_INIT;
            break;
         }
         case RTEPROCESS_PROCESS_STATE_INIT:
         {
            rteprocess_create_threads(process_ptr);
            __rteprocess_notify_system_monitor_process_ready(process_ptr);
            process_ptr->state = RTEPROCESS_PROCESS_STATE_READY;
            break;
         }
         case RTEPROCESS_PROCESS_STATE_READY:
         {
            __rteprocess_wait_for_system_monitor(process_ptr);
            process_ptr->state = RTEPROCESS_PROCESS_STATE_COMING_UP;
            break;
         }
         case RTEPROCESS_PROCESS_STATE_COMING_UP:
         {
            rteprocess_run_threads(process_ptr);
            __rteprocess_notify_system_monitor_process_ready(process_ptr);
            process_ptr->state = RTEPROCESS_PROCESS_STATE_UP;
            break;
         }
         case RTEPROCESS_PROCESS_STATE_UP:
         case RTEPROCESS_PROCESS_STATE_COUNT:
         default:
         {
            MSG_MED("Bad rteprocess state (%d)", process_ptr->state,0,0);
         }
      }
   }
   
   /*------------------------------------------------------------------------
   Last checks to run before letting go...
   -------------------------------------------------------------------------*/
   __rteprocess_verify_process(process_ptr);

   /*
   ** Process is running normally.
   ** Handles process level messages that may come
   **/
   for(;;)
   {
      union ipc_msg_type* msg_ptr;

      msg_ptr = ipc_receive(IPC_MSG_ANY);
      rteprocess_process_msg(msg_ptr);
      ipc_delete(&msg_ptr);
   }
}


/*===========================================================================
FUNCTION 

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
void rteprocess_init
(
   struct rteprocess_process_type* process_ptr
)
{
   /*
   ** Create the thread impersonating this process.
   **/
   (void)rte_create_process( process_ptr->name,
                             process_ptr->priority,
                             rteprocess_process_func,
                             process_ptr->stack_size,
                             process_ptr->pool_size,
                             process_ptr );

   /* Wait for the process to kick-off all of its threads */
   __rteprocess_wait_for_process(process_ptr);
}

#endif /* FEATURE_RTE_PROCESS */

